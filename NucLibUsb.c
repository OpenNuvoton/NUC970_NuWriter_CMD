
#include "common.h"

int NUC_SetType(int id,int type)
{
	unsigned int ack=0;
	do {

		libusb_control_transfer(handle,
		                        0x40, /* requesttype */
		                        BURN, /* request */
		                        BURN_TYPE+(unsigned int)type, /* wValue */
		                        0, /* wIndex */
		                        NULL,
		                        0, /* wLength */
		                        USB_TIMEOUT);
		libusb_control_transfer(handle,
		                        0xC0, /* requesttype */
		                        BURN, /* request */
		                        BURN_TYPE+(unsigned int)type, /* wValue */
		                        0, /* wIndex */
		                        (unsigned char *)&ack,
		                        (unsigned short)sizeof(unsigned int), /* wLength */
		                        USB_TIMEOUT);

	} while((unsigned char)(ack&0xFF)!=(BURN_TYPE+type));
	return 0;
}


int NUC_ReadPipe(int id,unsigned char *buf,int len)
{
	int nread, ret;

	ret = libusb_bulk_transfer(handle, USB_ENDPOINT_IN, buf, len,
	                           &nread, USB_TIMEOUT);
	if (ret) {
		MSG_DEBUG("ERROR in bulk read: %d\n", ret);
		return -1;
	} else {
		//MSG_DEBUG("receive %d bytes from device\n", nread);
		//printf("%s", receiveBuf);  //Use this for benchmarking purposes
		return 0;
	}
}
int NUC_WritePipe(int id,unsigned char *buf,int len)
{

	int ret;
	int nwrite;

	libusb_control_transfer(handle,
	                        0x40, /* requesttype */
	                        0xA0, /* request */
	                        0x12, /* wValue */
	                        len, /* wIndex */
	                        buf,
	                        0, /* wLength */
	                        USB_TIMEOUT);


	//write transfer
	//probably unsafe to use n twice...
	ret = libusb_bulk_transfer(handle, USB_ENDPOINT_OUT, buf, len, &nwrite, USB_TIMEOUT);
	//Error handling
	switch(ret) {
	case 0:
		//MSG_DEBUG("send %d bytes to device\n", len);
		return 0;
	case LIBUSB_ERROR_TIMEOUT:
		printf("ERROR in bulk write: %d Timeout\n", ret);
		break;
	case LIBUSB_ERROR_PIPE:
		printf("ERROR in bulk write: %d Pipe\n", ret);
		break;
	case LIBUSB_ERROR_OVERFLOW:
		printf("ERROR in bulk write: %d Overflow\n", ret);
		break;
	case LIBUSB_ERROR_NO_DEVICE:
		printf("ERROR in bulk write: %d No Device\n", ret);
		break;
	default:
		printf("ERROR in bulk write: %d\n", ret);
		break;

	}
	return 0;
}

int compare_port_path(uint8_t *port_numbers1,int port_numbers_len1,uint8_t *port_numbers2,int port_numbers_len2)
{
    int iRet;
	int i;

	if(port_numbers_len1 != port_numbers_len2)
    {
        iRet = port_numbers_len1-port_numbers_len1;
    }

	for(i=0;i< port_numbers_len1;i++)
	{
	    if(port_numbers1[i] != port_numbers2[i])
	    {
	        iRet = port_numbers1[i] - port_numbers2[i];
	        break;
	    }
	}

	return iRet;
	
}

void sort_dev_array(libusb_device **dev_arr,int count)
{
    int i,j;
	libusb_device *tmp;
	uint8_t port_numbers1[8];
	uint8_t port_numbers2[8];
	int port_numbers_len1;
	int port_numbers_len2;
	for(int i=0;i<count-1;i++)
	{
	    for(j=count -1;j>i;j--)
	    {
	        port_numbers_len1 = libusb_get_port_numbers(dev_arr[j-1],port_numbers1,sizeof(port_numbers1));
			port_numbers_len2 = libusb_get_port_numbers(dev_arr[j],port_numbers2,sizeof(port_numbers2));
	        if(compare_port_path(port_numbers1,port_numbers_len1,port_numbers2,port_numbers_len2) > 0 )
	        {
	            tmp = dev_arr[j-1];
				dev_arr[j-1] = dev_arr[j];
				dev_arr[j] = tmp;
	        }      
	    }
	}
    
}

void print_port_numbers(libusb_device *dev)
{
    uint8_t port_numbers[8];
	int port_numbers_len;
	int i;
	port_numbers_len = libusb_get_port_numbers(dev,port_numbers,sizeof(port_numbers));
	if(port_numbers_len > 0)
	{
	    printf(" %d", port_numbers[0]);
		for (i = 1; i <port_numbers_len; i++)
		printf(".%d", port_numbers[i]);
	}
}

libusb_device_handle * libusb_open_device_with_vid_pid_index
(
libusb_context *ctx,
unsigned int vendor_id,
unsigned int product_id,
int index
)
{
    libusb_device **devs;
	ssize_t cnt;
	libusb_device *dev;
	libusb_device *dev_arr[100];
	int i=0,j=0,count=0;
	libusb_device_handle *dev_handle;
	
	cnt = libusb_get_device_list(NULL,&devs);
	if(cnt < 0)
	{
	     printf("get device list failed\n");
             return NULL;
	}
	while(((dev = devs[i++]) != NULL)&&(count < 100))
	{
	     
	       struct libusb_device_descriptor desc; 
		   int r = libusb_get_device_descriptor(dev,&desc);
		   if(r < 0)
		   {
				fprintf(stderr,"failed to get device descriptor\r\n");
		        return NULL;
		   }
		   if((desc.idVendor == vendor_id)&&(desc.idProduct == product_id))
	       {  
				dev_arr[count] = dev;
				count++;
				//printf("devarr[%d] device =%d port_num=%d \r\n",count-1,libusb_get_device_address(dev),libusb_get_port_number(dev));
		   }
    }
	libusb_free_device_list(devs, 1);

	if(count == 0)
	{
            printf("count =0\n");
	    return NULL;
	}
    
	sort_dev_array(dev_arr,count);
	
    if(index > count)
    {
    	printf("index > count");
        return NULL;
    }
	printf("usb port is ");
	print_port_numbers(dev_arr[index-1]);
    printf("\r\n");
    int r;
	r = libusb_open(dev_arr[index-1],&dev_handle);
    if(r !=  0 )
    {
	     printf("r = %d\r\n",r);
         return NULL;
    }
    else
    {
	     return dev_handle;
    }
	return NULL;
}
int NUC_OpenUsb(void)
{
	int ret=0;
	//libusb_device_handle * handle2=NULL;
	int index = csg_usb_index;

	if(handle!=NULL) return 0;
	//Open Device with VendorID and ProductID
	//handle = libusb_open_device_with_vid_pid(ctx,
	//         USB_VENDOR_ID, USB_PRODUCT_ID);
	handle = libusb_open_device_with_vid_pid_index(ctx,
	         USB_VENDOR_ID, USB_PRODUCT_ID,index);
	if (!handle) {
		perror("device not found");
		ret=-1;
		libusb_exit(NULL);
		return -1;
	}
#ifdef _WIN32
	libusb_set_auto_detach_kernel_driver(handle, 1);
	ret = libusb_claim_interface(handle, 0);
	if (ret < 0) {
		fprintf(stderr, "usb_claim_interface error %d\n", ret);
		return -1;
	}
#endif
	return 0;
}

void NUC_CloseUsb(void)
{
#ifdef _WIN32
	libusb_release_interface(handle, 0);
#endif
	libusb_close(handle);
	handle=NULL;
}

