#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
  * @brief      Load ddr file
  * @param      FilePath	file path
  * @param 	len		ddr length
  * @return     ddr buffer address
  * @details    This function read ddr into buffer.
  */
char* load_ddr(char *FilePath,int *len)
{
	FILE *pf;
	char *buf;
	char tmp[512];
	int length,tmpbuf_size;
	int i;
	char *ptmpbuf,*ptmp,cvt[128];
	unsigned int * puint32_t;
	unsigned int val=0;
	buf=(char *)malloc(sizeof(char)*512);
	memset(buf,0,512);
	puint32_t=(unsigned int *)buf;
	pf=fopen(FilePath, "rb");
	if(pf==NULL) {
		printf("cannot open %s\n",FilePath);
		*len = 0;
		return NULL;
	}
	tmpbuf_size=0;
	while(fgets(tmp,512,pf) != NULL) {
		ptmp=strchr(tmp,'=');
		strncpy((char *)cvt,(char *)tmp,(unsigned long)ptmp-(unsigned long)tmp);
		cvt[(unsigned long)ptmp-(unsigned long)tmp]='\0';
		val=strtoul(cvt,NULL,0);
		*puint32_t=val;
		puint32_t++;
		tmpbuf_size+=4;
		strncpy(cvt,++ptmp,strlen(ptmp));
		cvt[strlen(ptmp)]='\0';
		val=strtoul(cvt,NULL,0);
		*puint32_t=val;
		puint32_t++;
		tmpbuf_size+=4;
	}
	*len=tmpbuf_size;
	fclose(pf);
	return buf;
}


/**
  * @brief      Load xusb file
  * @param      FilePath	file path
  * @param 	len		xusb length
  * @return     xusb buffer address
  * @details    This function read xusb into buffer.
  */
char * load_xusb(char *FilePath,int *len)
{
	FILE *fp;
	char *buf;
	fp=fopen(FilePath, "rb");
	if(fp==NULL) {
		printf("cannot open %s\n",FilePath);
		*len = 0;
		return NULL;
	}
	fseek(fp,0,SEEK_END);
	*len=ftell(fp);
	fseek(fp,0,SEEK_SET);
	buf=(char *)malloc(sizeof(char)*(*len));
	fread(buf,*len,1,fp);
	fclose(fp);
	return buf;

}

#if 0
int main(int argc, char **argv)
{
	unsigned char *dbuf,*buf;
	unsigned int *p32;
	int dlen,len;
	int i;

	dbuf=load_ddr("Release/sys_cfg/NUC972DF62Y.ini",&dlen);

	printf("len=%d\n",dlen);
	p32=(unsigned int *)dbuf;
	for(i=0; i<(dlen/4); i+=2) {
		printf("%08x=%08x \t",*(p32+i),*(p32+i+1));
		if((i+2)%8==0) printf("\n");
	}
	printf("\n");

	buf=load_xusb("Release/xusb64.bin",&len);
	printf("S:");
	for(i=0; i<16; i++)
		printf("%02x ",buf[i]);
	printf("\nE:");
	for(i=(len-16); i<len; i++)
		printf("%02x ",buf[i]);
	printf("\n");
}
#endif
