################################################################################
################################################################################
##
## Makefile -- project specific makefile to build SCI Wifi driver
##
## (C) Copyright 2018-2020 by Suzhou WF Chip Semiconductor Co., LTD
##
## Mandatory settings:
##
## o TOPDIR                        = the toplevel directory (using slashes as path separator)
## o SUBDIR                        = the make file folder
## o CT                            = display driver compile time(y)
## o CONFIG_DRIVER_VER             = null(use svn version), else this is version
## o CONFIG_OS_LINUX               = Linux OS define (y)
## o CONFIG_OS_ANDROID             = Android OS define (n)
## o CONFIG_OS_RTOS                = RTOS OS define (n)
## o CONFIG_DBG_LEVEL              = 0x0F    One bit represents one debug level
## o CONFIG_DBG_COLOR              = Debug info display font color(y)
## o CONFIG_DBG_MEM                = Memory allocation and free debug flag (y)
## o CONFIG_WIFI_INTERFACE_TWO     = Second WiFi interface (y)
## o CONFIG_TX_SOFT_AGG            = switch for tx soft agg(n)
## o CONFIG_RX_SOFT_AGG            = switch for rx soft agg(y)
## o CONFIG_CMD_METHOD             = method for mcu cmd (mailbox,txd)
## o CONFIG_WIFI_MODE              = all(sta/ap/adhoc/monitor), sta, ap, adhoc
## o CONFIG_WIFI_FRAMEWORK         = wext, nl80211, mp
## o CONFIG_HIF_PORT               = usb, sdio, both
## o CONFIG_CHIP                   = s9083,s9188
## o CONFIG_ARS_SUPPORT            = Adaptive rate system(y)
################################################################################
  export WDRV_DIR ?= $(shell pwd)
  SUBDIR = mak
  PLATDIR = platform
  CT                       ?= n
  CONFIG_DRIVER_VER         = null
  CONFIG_OS_LINUX           = y
  CONFIG_OS_ANDROID         = n
  CONFIG_OS_RTOS            = n
  CONFIG_DBG_LEVEL          = 0x0F
  CONFIG_DBG_COLOR          = y
  CONFIG_DBG_MEM            = n
  CONFIG_DMA_MEM            = n
  CONFIG_WIFI_INTERFACE_TWO = n
  CONFIG_TX_SOFT_AGG        = n
  CONFIG_RX_SOFT_AGG        = y
  CONFIG_CMD_METHOD         = mailbox
  CONFIG_WIFI_MODE          = all
  CONFIG_WIFI_FRAMEWORK     = nl80211
  CONFIG_HIF_PORT           = both
  CONFIG_CHIP               = s9083
  CONFIG_POWER_SAVING       = n
  CONFIG_ARS_SUPPORT        = n

include $(WDRV_DIR)/$(PLATDIR)/platform.mak

ifeq ($(CONFIG_OS_LINUX), y)
    include $(WDRV_DIR)/$(SUBDIR)/linux/Makefile
else ifeq ($(CONFIG_OS_ANDROID), y)
    include $(WDRV_DIR)/$(SUBDIR)/linux/Makefile
else ifeq ($(CONFIG_OS_RTOS), y)
    include $(WDRV_DIR)/$(SUBDIR)/rtos/Makefile
endif


