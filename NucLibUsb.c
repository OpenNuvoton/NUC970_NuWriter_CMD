
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
libusb_device_handle * libusb_open_device_with_vid_pid_index( libusb_context *ctx,
                                                              unsigned int vendor_id,
                                                              unsigned int product_id,
                                                              int index
)
{
	libusb_device **devs;
	ssize_t cnt;
	libusb_device *dev;
	int i=0,j=0,count=0;
	libusb_device_handle *dev_handle;
	
	cnt = libusb_get_device_list(NULL,&devs);
	if(cnt < 0)
	{
        return NULL;
	}
	while((dev = devs[i++]) != NULL)
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
		    count++;
		    if(count == index)
		    {
                r = libusb_open(dev,&dev_handle);
		        if(r <  0 )
	            {
			        printf("r = %d\r\n",r);
	                return NULL;
		        }
		        else
		        {
			        return dev_handle;
		        }
		    }
	    }
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

