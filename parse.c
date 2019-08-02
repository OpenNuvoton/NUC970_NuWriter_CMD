
#include "common.h"
#include <unistd.h> /* sleep */
#include <string.h>

int init_xusb(void)
{

	int ret=0;
	char DDR[256];
	char XUSB[256];
	int bResult,dlen,xlen;
	unsigned char *dbuf=NULL,*xbuf=NULL;

	ret=NUC_OpenUsb();
	if(ret<0) return -1;

	sprintf(DDR,"%s/sys_cfg/%s",Data_Path,DDR_fileName);
	dbuf=load_ddr(DDR,&dlen);
	if(dbuf==NULL) {
		ret=-1;
		goto EXIT;
	}
	bResult=DDRtoDevice(dbuf,dlen);
	if(bResult==RUN_ON_XUSB) {
		ret=bResult;
		goto EXIT;
	}
	if(bResult<0) {
		printf("init_xusb:Burn DDR to Device failed,bResult=%d\n",bResult);
		ret=bResult;
		goto EXIT;
	}
	if(DDR_fileName[strlen(DDR_fileName)-7]=='7') {
		MSG_DEBUG("load xusb128.bin\n");
		sprintf(XUSB,"%s/xusb128.bin",Data_Path);
	} else if(DDR_fileName[strlen(DDR_fileName)-7]=='6') {
		MSG_DEBUG("load xusb64.bin\n");
		sprintf(XUSB,"%s/xusb64.bin",Data_Path);
	} else  if(DDR_fileName[strlen(DDR_fileName)-7]=='5') {
		MSG_DEBUG("load xusb32.bin\n");
		sprintf(XUSB,"%s/xusb.bin",Data_Path);
	} else {
		MSG_DEBUG("load xusb16.bin\n");
		sprintf(XUSB,"%s/xusb16.bin",Data_Path);
	}
	xbuf=load_xusb(XUSB,&xlen);
	if(xbuf==NULL) {
		printf("Cannot find xusb.bin\n");
		ret=-1;
		goto EXIT;
	}
	if(XUSBtoDevice(xbuf,xlen)<0) {
		printf("Burn XUSB to Device failed\n");
		ret=-1;
		goto EXIT;
	}
	NUC_CloseUsb();
	sleep(1);
	while(get_device_num_with_vid_pid(ctx,USB_VENDOR_ID, USB_PRODUCT_ID)!=dev_count) {
		sleep(1);
	}
	ret=NUC_OpenUsb();
	if(ret<0) return -1;
EXIT:
	usleep(100);
	return ret;
}

int ParseFlashType(void)
{
	int ret;
	if((ret=init_xusb())<0) {
		printf("initail xusb failed %d\n",ret);
		return -1;
	}

	if(InfoFromDevice()<0) {
		printf(" Get information failed for Device\n");
		return -1;
	}
	switch(mode) {
	case SDRAM_M:
		MSG_DEBUG("ParseFlashType-SDRAM_M\n");
		if(UXmodem_SDRAM()<0) return -1;
		break;
	case NAND_M:
		MSG_DEBUG("ParseFlashType-NAND_M\n");
		if(UXmodem_NAND()<0) return -1;
		break;
	case SPI_M:
		MSG_DEBUG("ParseFlashType-SPI_M\n");
		if(UXmodem_SPI()<0) return -1;
		break;
	case EMMC_M:
		MSG_DEBUG("ParseFlashType-EMMC_M\n");
		if(UXmodem_EMMC()<0) return -1;
		break;
	}

	return 0;
}
