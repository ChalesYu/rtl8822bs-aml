#include "hif.h"

// include msg_ctrl/bluk_ctrl
int hif_io_write()
{
	//Todo 
	#ifdef CONFIG_USB
	usb_write();
	#else
	sdio_write();	
	#endif
	
}


int hif_io_read()
{
		//Todo 
	#ifdef CONFIG_USB
	usb_read();
	#else
	sdio_read();	
	#endif
	
}