#include "hif.h"

int init hif_init(void)
{
    #ifdef CONFIG_USB
    usb_init();
	#else
	sdio_init();
    #endif

    return ret;
}

static void hif_exit(void)
{
	#ifdef CONFIG_USB
    usb_exit();
	#else
	sdio_exit();
    #endif
   
}

