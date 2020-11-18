#include "common.h"
#include <string.h>
#include <unistd.h>  /* getopt */
#include <ctype.h>  /* isprint */
#include <dirent.h>
#include "config.h"

char DDR_fileName[128];
char write_file[256];
char read_file[256];
char Data_Path[256];
int mode;
int type;
unsigned int exe_addr;
unsigned int dram_run;
unsigned int erase_read_len;
int erase_tag;
int read_tag;
int write_tag;
int verify_tag;
int dtb_tag;
unsigned int dtb_addr;

struct _INFO_T m_info;

libusb_context *ctx;
libusb_device_handle *handle;

unsigned int csg_usb_index;
libusb_device *dev_arr[MAX_DEV];
unsigned int dev_count;

int check_strlen(const struct dirent *dir)
{
	if(strlen(dir->d_name)>5)
		return 1;
	else
		return 0;
}

void print_using()
{
	fprintf(stderr, "Download:\n");
	fprintf(stderr, "	./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/kernel_970image\n");
	fprintf(stderr, "Downalod & run:\n");
	fprintf(stderr, "	./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/kernel_970image -n\n");
	fprintf(stderr, "Download dtb(device tree):\n");
	fprintf(stderr, "	./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x1E00000 -w $IMAGE_PATH/nuc970-evb.dtb\n");
	fprintf(stderr, "	./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/kernel_970image -n -i 0x1E00000\n");
	fprintf(stderr, "Burn u-boot-spl.bin to NAND:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -t uboot -a 0x200 -w $IMAGE_PATH/nand_uboot_spl.bin -v\n");
	fprintf(stderr, "Burn u-boot.bin to NAND:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -t data -a 0x100000 -w $IMAGE_PATH/nand_uboot.bin -v\n");
	fprintf(stderr, "Burn env.txt to NAND:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/nand_env.txt -v\n");
	fprintf(stderr, "Burn linux to NAND:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/kernel_970uimage -v\n");
	fprintf(stderr, "Burn Pack image to NAND\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/nand_pack.bin\n");
	fprintf(stderr, "Erase NAND (chip erase):\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -e 0xffffffff\n");
	fprintf(stderr, "Erase NAND 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -a 10 -e 20\n");
	fprintf(stderr, "Read NAND 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m nand -d NUC972DF62Y.ini -a 10 -e 1 -r $IMAGE_PATH/test.bin\n");
	fprintf(stderr, "Burn u-boot.bin to SPI:\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -t uboot -a 0xE00000 -w $IMAGE_PATH/spi_uboot.bin -v\n");
	fprintf(stderr, "Burn env.txt to SPI:\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/spi_env.txt -v\n");
	fprintf(stderr, "Burn linux to SPI:\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/kernel_970uimage -v\n");
	fprintf(stderr, "Burn Pack image to SPI\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/spi_pack.bin\n");
	fprintf(stderr, "Erase SPI (chip erase):\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -e 0xffffffff\n");
	fprintf(stderr, "Erase SPI 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -a 10 -e 20\n");
	fprintf(stderr, "Read SPI 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m spi -d NUC972DF62Y.ini -a 10 -e 20 -r $IMAGE_PATH/test.bin\n");
	fprintf(stderr, "Burn u-boot.bin to eMMC:\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -t uboot -a 0xE00000 -w $IMAGE_PATH/emmc_uboot.bin -v\n");
	fprintf(stderr, "Burn env.txt to eMMC:\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/emmc_env.txt -v\n");
	fprintf(stderr, "Burn linux to eMMC:\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/kernel_970uimage -v\n");
	fprintf(stderr, "Burn Pack image to eMMC\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/emmc_pack.bin\n");
	fprintf(stderr, "Erase eMMC 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -a 10 -e 20\n");
	fprintf(stderr, "Read eMMC 10th block to 30th block:\n");
	fprintf(stderr, "	./nuwriter -m emmc -d NUC972DF62Y.ini -a 10 -e 1 -r $IMAGE_PATH/test.bin\n");
}
int main(int argc, char **argv)
{

	/* Initial */
	char *path;
	type = -1;
	DDR_fileName[0]='\0';
	exe_addr=0xffffffff;
	dram_run = 0;
	erase_tag = 0;
	write_tag= 0;
	read_tag = 0;
	verify_tag = 0;
	dtb_tag = 0;
	int cmd_opt = 0;
    int n;

    csg_usb_index = 1;

#ifndef _WIN32

    n = readlink("/proc/self/exe", Data_Path, sizeof(Data_Path) - 1);
    if((n < 0) || (n > (sizeof(Data_Path) - 50))) {
        fprintf(stderr, "Link Error!\n");
        return -1;
    }

    Data_Path[n] = '\0';
    path = strrchr(Data_Path, '/');
    if(path == NULL) {
        fprintf(stderr, "Data Path Error!\n");
        return -2;
    }

    path[1] = '\0';
	strcat(Data_Path,"../share/nudata");
#else
    n = snprintf(Data_Path, sizeof(Data_Path), argv[0]));
    if(n > (sizeof(Data_Path) - 50)) {
        fprintf(stderr, "Data Path Too Long!\n");
        return -2;
    }

    path=strrchr(Data_Path,'/');
    if(path == NULL) {
        if(getcwd(Data_Path, sizeof(Data_Path) - 50)) == NULL) {
            fprintf(stderr, "Data Path Error!\n");
            return errno;
        }
    } else {
        *path='\0';
    }

	strcat(Data_Path, "/nudata");
#endif
	//fprintf(stderr,"Data_Path=%s\n",Data_Path);
	//fprintf(stderr, "argc:%d\n", argc);
	while(1) {
		//fprintf(stderr, "proces index:%d\n", optind);
		cmd_opt = getopt(argc, argv, "a:d:e:i:nvhw:r:t:m:z::c::");
		/* End condition always first */
		if (cmd_opt == -1) {
			break;
		}

		/* Lets parse */
		switch (cmd_opt) {
		/* No args */
		case 'h':
			fprintf(stderr, "============================================\n");
			fprintf(stderr, "==   Nuvoton NuWriter Command Tool V%s   ==\n",PACKAGE_VERSION);
			fprintf(stderr, "============================================\n");

			fprintf(stderr, "NuWriter [Options] [File/Value]\n\n");
			fprintf(stderr, "-d [File]      Set DDR initial file\n");
#if 0
			fprintf(stderr, "-c [id]        Set device number,default 1\n");
#endif
#ifndef _WIN32
			fprintf(stderr, "-d show        Print supported DDR model\n");
#endif
			fprintf(stderr, "\n");
			fprintf(stderr, "-m sdram       Set SDRAM Mode\n");
			fprintf(stderr, "-m emmc        Set eMMC Mode\n");
			fprintf(stderr, "-m nand        Set NAND Mode\n");
			fprintf(stderr, "-m spi         Set SPI Mode\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "-t data        Set DATA Type\n");
			fprintf(stderr, "-t env         Set Environemnt Type\n");
			fprintf(stderr, "-t uboot       Set uBoot Type\n");
			fprintf(stderr, "-t pack        Set PACK Type\n");
			fprintf(stderr, "SDRAM parameters:\n");
			fprintf(stderr, "-a [value]     Set execute address\n");
			fprintf(stderr, "-w [File]      Write image file to SDRAM\n");
			fprintf(stderr, "-i [value]     device tree address\n");
			fprintf(stderr, "-n             Download & Run\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "NAND/SPI/eMMC parameters:\n");
			fprintf(stderr, "-a [value]     Set start offset/execute address\n");
			fprintf(stderr, "               if erase, unit of block\n");
			fprintf(stderr, "-w [File]      Write image file to NAND\n");
			fprintf(stderr, "-r [File]      Read image file from NAND\n");
			fprintf(stderr, "-e [value]     Read/Erase length(unit of block)\n");
			fprintf(stderr, "-e 0xFFFFFFFF  Chip erase\n");
			fprintf(stderr, "-v             Verify image file after Write image \n");
			fprintf(stderr, "\n============================================\n");
			return 0;
		case 'n':
			dram_run = 1;
			break;

		/* Single arg */
		case 'a':
			exe_addr=strtoul(optarg,NULL,0);
			break;
		case 'd': {
			char dir_path[256];
			sprintf(dir_path,"%s%s",Data_Path,"/sys_cfg");
#ifndef _WIN32
			if(strcasecmp(optarg,"show")==0) {
				struct dirent **namelist;
				int i, total;
				total = scandir(dir_path,&namelist,check_strlen,0);
				for(i=0; i<total; i++)
					fprintf(stderr,"%s\n",namelist[i]->d_name);
				return 0;
			} else {
				strcpy(DDR_fileName,optarg);
				//fprintf(stderr,"DDR_fileName=%s\n",DDR_fileName);
			}
#else
			strcpy(DDR_fileName,optarg);
#endif
		}
		break;
		case 'i':
			dtb_tag=1;
			dtb_addr=strtoul(optarg,NULL,0);
			break;
		case 'w':
			write_tag=1;
			strcpy(write_file,optarg);
			break;
		case 'r':
			read_tag=1;
			strcpy(read_file,optarg);
			break;
		case 'v':
			verify_tag=1;
			break;
		case 'm':
			if(strcasecmp(optarg,"sdram")==0) {
				//fprintf(stderr,"SDRAM mode\n");
				mode=SDRAM_M;
			} else if(strcasecmp(optarg,"emmc")==0) {
				//fprintf(stderr,"eMMC mode\n");
				mode=EMMC_M;
			} else if(strcasecmp(optarg,"nand")==0) {
				//fprintf(stderr,"NAND mode\n");
				mode=NAND_M;
			} else if(strcasecmp(optarg,"spi")==0) {
				//fprintf(stderr,"SPI mode\n");
				mode=SPI_M;
			} else {
				fprintf(stderr,"Unknown mode\n");
			}
			break;

		case 't':
			if(strcasecmp(optarg,"data")==0) {
				//fprintf(stderr,"DATA type\n");
				type=DATA;
			} else if(strcasecmp(optarg,"env")==0) {
				//fprintf(stderr,"Environment type\n");
				type=ENV;
			} else if(strcasecmp(optarg,"uboot")==0) {
				//fprintf(stderr,"uBoot type\n");
				type=UBOOT;
			} else if(strcasecmp(optarg,"pack")==0) {
				//fprintf(stderr,"Pack type\n");
				type=PACK;
			} else {
				fprintf(stderr,"Unknown type\n");
			}
			break;
		case 'e':
			erase_tag = 1;
			erase_read_len=strtoul(optarg,NULL,0);
			break;
		/* Optional args */
		case 'z':
			print_using();
			return 0;
			break;
#if 0
		case 'c':

			csg_usb_index = atoi(argv[optind]);
			break;
#endif

		/* Error handle: Mainly missing arg or illegal option */
		case '?':
			fprintf(stderr, "Illegal option:-%c\n", isprint(optopt)?optopt:'#');
			break;
		default:
			fprintf(stderr, "Not supported option\n");
			fprintf(stderr, "Try 'nuwriter -h' for more information.\n");
			break;
		}
	}

	if(strlen(DDR_fileName)==0) {
		fprintf(stderr, "Not setting DDR file\n");
		return -1;
	}
	if(mode==-1) {
		fprintf(stderr, "Not setting mode\n");
		return -1;
	}

	if(mode==SDRAM_M) {
		if(exe_addr==0xffffffff) {
			fprintf(stderr, "Not setting execute/download address(-a [Address])\n");
			return -1;
		}
		if(strlen(write_file)==0) {
			fprintf(stderr, "Not setting DDR file(-m [DDR])\n");
			return -1;
		}

	}

	if((mode==NAND_M || mode==SPI_M || mode==EMMC_M) && type!=PACK) {
		if(write_tag==1 && read_tag==1) {
			fprintf(stderr, "Cannot write & read Nand flash at same time\n");
			return -1;
		}

		if(read_tag==1) {

			if(exe_addr==0xffffffff) {
				fprintf(stderr, "Not setting start address(-a [value])\n");
				return -1;
			}
			if(erase_tag!=1) {
				fprintf(stderr, "Not setting length(-e [value])\n");
				return -1;
			}
			erase_tag=0;
		}

		if(write_tag==1) {
			if(exe_addr==0xffffffff) {
				fprintf(stderr, "Not setting start address(-a [value])\n");
				return -1;
			}
			if(strlen(write_file)==0 && strlen(write_file)==0) {
				fprintf(stderr,"Not setting read/write file(-w [file])\n");
				return -1;
			}
			if(type==-1 && strlen(write_file)!=0) {
				fprintf(stderr, "Not setting type(-t [type])\n");
				return -1;
			}
		}
	}

	if(type==PACK) {
		if(verify_tag==1)
			fprintf(stderr, "Pack cannot supported verify (-v)\n");
	}

	libusb_init(NULL);
	if((dev_count=get_device_num_with_vid_pid(ctx,USB_VENDOR_ID, USB_PRODUCT_ID))==0) {
		printf("Device not found\n");
		libusb_exit(NULL);
		return -1;
	}

	if(ParseFlashType()< 0) {
		printf("Failed\n");
		NUC_CloseUsb();
		libusb_exit(NULL);
		return -1;
	}
	NUC_CloseUsb();
	libusb_exit(NULL);
	return 0;
}
