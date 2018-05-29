
#include "common.h"
#include <string.h>
#include <unistd.h>  /* getopt */
#include <ctype.h>  /* isprint */

int DDRtoDevice(unsigned char *buf,unsigned int len)
{

	unsigned int ack=0;
	AUTOTYPEHEAD head;
	head.address=DDRADDRESS;
	head.filelen=len;
	MSG_DEBUG("head.address=0x%08x,head.filelen=%d\n",head.address,head.filelen);
	NUC_WritePipe(0,(unsigned char*)&head,sizeof(AUTOTYPEHEAD));
	NUC_WritePipe(0,(unsigned char *)buf,len);
	NUC_ReadPipe(0,(unsigned char *)&ack,(int)sizeof(unsigned int));
	if(ack==(BUF_SIZE+1))	return RUN_ON_XUSB;
	NUC_ReadPipe(0,(unsigned char *)&ack,(int)sizeof(unsigned int));
	if(ack!=0x55AA55AA) return -1;
	MSG_DEBUG("ack=0x%08x\n",ack);
	return 0;
}

int XUSBtoDevice(unsigned char *buf,unsigned int len)
{
	unsigned char SIGNATURE[]= {'W','B',0x5A,0xA5};
	AUTOTYPEHEAD fhead;
	XBINHEAD *xbinhead;
	unsigned int scnt,rcnt,file_len,ack,total;
	unsigned char *pbuf;
	int bResult,pos;
	xbinhead=(XBINHEAD *)buf;
	pbuf=buf+sizeof(XBINHEAD);
	file_len=len-sizeof(XBINHEAD);
	fhead.filelen = file_len;
	if(xbinhead->sign==*(unsigned int *)SIGNATURE) {
		MSG_DEBUG("passed xbinhead->address=%x\n",xbinhead->address);
		fhead.address = xbinhead->address;//0x8000;
	} else {
		fhead.address = 0xFFFFFFFF;
	}
	if(fhead.address==0xFFFFFFFF) {
		return -1;
	}
	NUC_WritePipe(0,(unsigned char*)&fhead,sizeof(AUTOTYPEHEAD));
	scnt=file_len/BUF_SIZE;
	rcnt=file_len%BUF_SIZE;
	total=0;
	while(scnt>0) {
		bResult=NUC_WritePipe(0,(unsigned char*)pbuf,BUF_SIZE);
		if(bResult<0)	return -1;
		pbuf+=BUF_SIZE;
		total+=BUF_SIZE;
		pos=(int)(((float)(((float)total/(float)file_len))*100));
		bResult=NUC_ReadPipe(0,(UCHAR *)&ack,4);
		if(bResult<0 || ack!=BUF_SIZE) {
			return -1;
		}
		scnt--;
	}
	if(rcnt>0) {
		bResult=NUC_WritePipe(0,(unsigned char*)pbuf,rcnt);
		if(bResult<0) return -1;
		total+=rcnt;
		bResult=NUC_ReadPipe(0,(UCHAR *)&ack,4);
		if(bResult<0 || ack!=rcnt) return -1;
	}
	return 0;

}


/*
typedef struct _INFO_T {
	DWORD	Nand_uPagePerBlock;
	DWORD	Nand_uPageSize;
	DWORD   Nand_uSectorPerBlock;
	DWORD   Nand_uBlockPerFlash;
	DWORD	Nand_uBadBlockCount;
	DWORD   Nand_uSpareSize;
	DWORD   SPI_ID;
	DWORD   EMMC_uBlock;
	DWORD   EMMC_uReserved;
	DWORD	MTP_uNumber;
} INFO_T,*PINFO_T;
*/
int InfoFromDevice(void)
{
	int bResult;
	if(NUC_OpenUsb()<0) return -1;
	NUC_SetType(0,INFO);

	m_info.Nand_uPagePerBlock =  0;//Nand_uPagePerBlock;
	m_info.Nand_uBlockPerFlash = 0;//Nand_uBlockPerFlash;

	m_info.Nand_uPagePerBlock =  0;
	m_info.Nand_uBlockPerFlash = 0;
	bResult=NUC_WritePipe(0,(UCHAR *)&m_info, sizeof(INFO_T));
	if(bResult<0) goto EXIT;
	bResult=NUC_ReadPipe(0,(UCHAR *)&m_info, sizeof(INFO_T));
	if(bResult<0) goto EXIT;

	MSG_DEBUG("Nand_uPagePerBlock=%d\n",m_info.Nand_uPagePerBlock);
	MSG_DEBUG("Nand_uPageSize=%d\n",m_info.Nand_uPageSize);
	MSG_DEBUG("Nand_uSectorPerBlock=%d\n",m_info.Nand_uSectorPerBlock);
	MSG_DEBUG("Nand_uBlockPerFlash=%d\n",m_info.Nand_uBlockPerFlash);
	MSG_DEBUG("Nand_uBadBlockCount=%d\n",m_info.Nand_uBadBlockCount);
	return 0;
EXIT:

	return -1;
}

#if 0
int main(int argc, char **argv)
{

	int r,dlen,xlen;
	int i;
	unsigned int *p32;
	unsigned char *dbuf,*xbuf;
	libusb_init(&ctx);

	//Open Device with VendorID and ProductID
	handle = libusb_open_device_with_vid_pid(ctx,
	         USB_VENDOR_ID, USB_PRODUCT_ID);
	if (!handle) {
		perror("device not found");
		return 1;
	}

	r = libusb_claim_interface(handle, 0);
	if (r < 0) {
		fprintf(stderr, "usb_claim_interface error %d\n", r);
		return 2;
	}

	dbuf=load_ddr("Release/sys_cfg/NUC972DF62Y.ini",&dlen);
	if(DDRtoDevice(dbuf,dlen)<0) {
		printf("Burn DDR to Device failed\n");
		goto EXIT;
	}
	xbuf=load_xusb("Release/xusb64.bin",&xlen);
	if(XUSBtoDevice(xbuf,xlen)<0) {
		printf("Burn XUSB to Device failed\n");
		goto EXIT;
	}

EXIT:
	//never reached
	libusb_close(handle);
	libusb_exit(NULL);
}
#endif
