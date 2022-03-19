########### COMMON  #################################
ifeq ($(CONFIG_CHIP), s9083)
ifeq ($(CONFIG_HIF_PORT), usb)
MODULE_NAME := s9083u
else ifeq ($(CONFIG_HIF_PORT), sdio)
MODULE_NAME := s9083s
else
MODULE_NAME := s9083
endif
endif

ifeq ($(CONFIG_CHIP), s9188)
ifeq ($(CONFIG_HIF_PORT), usb)
MODULE_NAME := s9188u
else ifeq ($(CONFIG_HIF_PORT), sdio)
MODULE_NAME := s9188s
else
MODULE_NAME := s9188
endif
endif

ifeq ($(CONFIG_CHIP), richv300)
ifeq ($(CONFIG_HIF_PORT), usb)
MODULE_NAME := richv300u
else ifeq ($(CONFIG_HIF_PORT), sdio)
MODULE_NAME := richv300s
else
MODULE_NAME := richv300
endif
endif
#####################################################
ifeq ($(CONFIG_DRIVER_VER), null)
# get svn version
#EXTRA_CFLAGS += -D'WF_VERSION="[svn]$(shell svnversion -n $$WDRV_DIR)"'
EXTRA_CFLAGS += -D'WF_VERSION="[svn]$(shell svnversion -c $$WDRV_DIR | cut -d : -f 2)"'
else
# use Driver version
EXTRA_CFLAGS += -D'WF_VERSION="$(CONFIG_DRIVER_VER)"'
endif
#get compile time
ifeq ($(CT), y)
EXTRA_CFLAGS += -D'COMPILE_TIME="$(shell date +"%Y-%m-%d %H:%M:%S")"'
endif

EXTRA_CFLAGS += $(USER_EXTRA_CFLAGS)
EXTRA_CFLAGS += -O1
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/os_porting
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/hif
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/netdev
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/cfg80211
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/iw
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/pwr
EXTRA_CFLAGS += -I$(WDRV_DIR)/os/linux/mp
EXTRA_CFLAGS += -I$(WDRV_DIR)/nic
EXTRA_CFLAGS += -I$(WDRV_DIR)/nic/utility
EXTRA_CFLAGS += -I$(WDRV_DIR)/nic/p2p
EXTRA_CFLAGS += -I$(WDRV_DIR)/nic/ars
ifndef __linux__
EXTRA_CFLAGS += -D__linux__
endif
ifneq ($(CONFIG_DBG_LEVEL), 0)
EXTRA_CFLAGS += -DWF_DEBUG_LEVEL=$(CONFIG_DBG_LEVEL)
endif
ifeq ($(CONFIG_DBG_COLOR), y)
EXTRA_CFLAGS += -DWF_DEBUG_COLOR
endif

ifeq ($(CONFIG_TX_SOFT_AGG), y)
EXTRA_CFLAGS += -DCONFIG_SOFT_TX_AGGREGATION
endif

ifeq ($(CONFIG_RX_SOFT_AGG), y)
EXTRA_CFLAGS += -DCONFIG_SOFT_RX_AGGREGATION
endif

ifeq ($(CONFIG_HIF_PORT), usb)
EXTRA_CFLAGS += -DCONFIG_USB_FLAG
else ifeq ($(CONFIG_HIF_PORT), sdio)
EXTRA_CFLAGS += -DCONFIG_SDIO_FLAG
else
EXTRA_CFLAGS += -DCONFIG_BOTH_FLAG
endif

ifeq ($(CONFIG_WIFI_MODE), ap)
EXTRA_CFLAGS += -DCFG_ENABLE_AP_MODE
EXTRA_CFLAGS += -DWF_WEP_AUTH_ENABLE_WEXT
endif

ifeq ($(CONFIG_WIFI_MODE), monitor)
EXTRA_CFLAGS += -DCFG_ENABLE_MONITOR_MODE
endif

ifeq ($(CONFIG_WIFI_MODE), adhoc)
EXTRA_CFLAGS += -DCFG_ENABLE_ADHOC_MODE
endif

ifeq ($(CONFIG_WIFI_MODE), all)
EXTRA_CFLAGS += -DCFG_ENABLE_AP_MODE
EXTRA_CFLAGS += -DCFG_ENABLE_ADHOC_MODE
EXTRA_CFLAGS += -DCFG_ENABLE_MONITOR_MODE
EXTRA_CFLAGS += -DWF_WEP_AUTH_ENABLE_WEXT
endif

ifeq ($(CONFIG_WIFI_FRAMEWORK), nl80211)
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DWF_USE_CFG80211_STA_EVENT -DCONFIG_IOCTL_WEXT
else ifeq ($(CONFIG_WIFI_FRAMEWORK), wext)
EXTRA_CFLAGS += -DCONFIG_IOCTL_WEXT
else
EXTRA_CFLAGS += -DCONFIG_MP_MODE
endif

ifeq ($(CONFIG_STA_AND_AP_MODE), y)
EXTRA_CFLAGS += -DCONFIG_STA_AND_AP_MODE
endif

ifeq ($(CONFIG_POWER_SAVING), y)
EXTRA_CFLAGS += -DCONFIG_LPS
endif

ifeq ($(CONFIG_OS_ANDROID), y)
EXTRA_CFLAGS += -DCONFIG_OS_ANDROID
EXTRA_CFLAGS += -DCONFIG_ANDROID_HIF
endif

ifeq ($(CONFIG_ARS_SUPPORT), firmware)
EXTRA_CFLAGS += -DCONFIG_ARS_FIRMWARE_SUPPORT
else ifeq ($(CONFIG_ARS_SUPPORT), driver)
EXTRA_CFLAGS += -DCONFIG_ARS_DRIVER_SUPPORT
else
EXTRA_CFLAGS += -DCONFIG_ARS_DISABLE
endif

ifeq ($(CONFIG_DBG_MEM), y)
EXTRA_CFLAGS += -DMEMDBG_ENABLE
endif

EXTRA_LDFLAGS += --strip-debug

EXTRA_CFLAGS += -Wno-implicit-fallthrough

########################## Features ###########################

ifneq ($(KERNELRELEASE),)

ifeq ($(CONFIG_HIF_PORT), usb)
$(MODULE_NAME)-y += os/linux/hif/usb.o
else ifeq ($(CONFIG_HIF_PORT), sdio)
$(MODULE_NAME)-y += os/linux/hif/sdio.o
else
$(MODULE_NAME)-y += os/linux/hif/usb.o
$(MODULE_NAME)-y += os/linux/hif/sdio.o
endif

ifeq ($(CONFIG_DBG_MEM), y)
$(MODULE_NAME)-y += os/linux/memdbg.o
endif
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_mix.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_lock.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_sema.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_thread.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_timer.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_file.o
$(MODULE_NAME)-y += os/linux/os_porting/wf_os_api_workqueue.o
$(MODULE_NAME)-y += os/linux/hif/hif.o
$(MODULE_NAME)-y += os/linux/hif/hif_io.o
$(MODULE_NAME)-y += os/linux/hif/cfg_parse.o
$(MODULE_NAME)-y += os/linux/hif/hif_queue.o
$(MODULE_NAME)-y += os/linux/netdev/ndev_linux.o
$(MODULE_NAME)-y += os/linux/iw/iw.o
$(MODULE_NAME)-y += os/linux/iw/iw_priv_func.o

ifeq ($(CONFIG_WIFI_FRAMEWORK), nl80211)
$(MODULE_NAME)-y += os/linux/cfg80211/wf_cfg80211.o
$(MODULE_NAME)-y += os/linux/iw/iw_func.o
else ifeq ($(CONFIG_WIFI_FRAMEWORK), wext)
$(MODULE_NAME)-y += os/linux/iw/iw_func.o
else
$(MODULE_NAME)-y += os/linux/mp/mp.o
$(MODULE_NAME)-y += os/linux/mp/mp_efuse.o
$(MODULE_NAME)-y += os/linux/mp/mp_reg.o
$(MODULE_NAME)-y += os/linux/mp/mp_trx.o
endif


$(MODULE_NAME)-y += os/linux/rx_linux.o
$(MODULE_NAME)-y += os/linux/tx_linux.o
$(MODULE_NAME)-y += os/linux/proc.o
$(MODULE_NAME)-y += nic/p2p/p2p.o
$(MODULE_NAME)-y += nic/p2p/p2p_frame_mgt.o
$(MODULE_NAME)-y += nic/p2p/p2p_proto_mgt.o
$(MODULE_NAME)-y += nic/p2p/p2p_wfd.o
$(MODULE_NAME)-y += nic/p2p/p2p_wowlan.o
$(MODULE_NAME)-y += nic/nic.o
$(MODULE_NAME)-y += nic/nic_io.o
$(MODULE_NAME)-y += nic/rx.o
$(MODULE_NAME)-y += nic/tx.o
$(MODULE_NAME)-y += nic/sec.o
$(MODULE_NAME)-y += nic/sec_arc4.o
$(MODULE_NAME)-y += nic/sec_tkip.o
$(MODULE_NAME)-y += nic/sec_wep.o
$(MODULE_NAME)-y += nic/mlme.o
$(MODULE_NAME)-y += nic/wdn.o
$(MODULE_NAME)-y += nic/scan.o
$(MODULE_NAME)-y += nic/auth.o
$(MODULE_NAME)-y += nic/assoc.o
$(MODULE_NAME)-y += nic/action.o
$(MODULE_NAME)-y += nic/wlan_mgmt.o
$(MODULE_NAME)-y += nic/wf_80211.o
$(MODULE_NAME)-y += nic/ie.o
$(MODULE_NAME)-y += nic/local_config.o
$(MODULE_NAME)-y += nic/fw_download.o
$(MODULE_NAME)-y += nic/hw_ctrl.o
$(MODULE_NAME)-y += nic/hw_info.o
$(MODULE_NAME)-y += nic/mcu_cmd.o
$(MODULE_NAME)-y += nic/lps.o

$(MODULE_NAME)-y += nic/efuse.o
$(MODULE_NAME)-y += nic/utility/wf_que.o
$(MODULE_NAME)-y += nic/utility/wf_msg.o
$(MODULE_NAME)-y += nic/utility/wf_timer.o

ifeq ($(CONFIG_OS_ANDROID), y)
$(MODULE_NAME)-y += os/android/android_priv_cmd.o
endif

ifeq ($(CONFIG_WIFI_MODE), adhoc)
$(MODULE_NAME)-y += nic/adhoc.o
endif

ifeq ($(CONFIG_WIFI_MODE), ap)
$(MODULE_NAME)-y += nic/ap.o
endif

ifeq ($(CONFIG_WIFI_MODE), all)
$(MODULE_NAME)-y += nic/ap.o
$(MODULE_NAME)-y += nic/adhoc.o
endif


ifeq ($(CONFIG_CHIP), s9083)
$(MODULE_NAME)-y += os/linux/pwr/power_rich100.o
EXTRA_CFLAGS += -DCONFIG_RICHV100
endif

ifeq ($(CONFIG_CHIP), s9188)
$(MODULE_NAME)-y += os/linux/pwr/power_hw_rich200.o
EXTRA_CFLAGS += -DCONFIG_RICHV200
EXTRA_CFLAGS += -DCONFIG_FW_ENCRYPT
endif

ifeq ($(CONFIG_CHIP), richv300)
$(MODULE_NAME)-y += os/linux/pwr/power_hw_rich200.o
EXTRA_CFLAGS += -DCONFIG_RICHV300
EXTRA_CFLAGS += -DCONFIG_RICHV200
EXTRA_CFLAGS += -DCONFIG_FW_ENCRYPT
endif

ifeq ($(CONFIG_ARS_SUPPORT), driver)
$(MODULE_NAME)-y += nic/ars/ars_acs.o
$(MODULE_NAME)-y += nic/ars/ars_adaptivity.o
$(MODULE_NAME)-y += nic/ars/ars_antdect.o
$(MODULE_NAME)-y += nic/ars/ars_antdiv.o
$(MODULE_NAME)-y += nic/ars/ars_bbps.o
$(MODULE_NAME)-y += nic/ars/ars_beamforming.o
$(MODULE_NAME)-y += nic/ars/ars_cfo.o
$(MODULE_NAME)-y += nic/ars/ars_debug.o
$(MODULE_NAME)-y += nic/ars/ars_dig.o
$(MODULE_NAME)-y += nic/ars/ars_edcaturbocheck.o
$(MODULE_NAME)-y += nic/ars/ars_entry.o
$(MODULE_NAME)-y += nic/ars/ars_hwconfig.o
$(MODULE_NAME)-y += nic/ars/ars_noisemonitor.o
$(MODULE_NAME)-y += nic/ars/ars_pathdiv.o
$(MODULE_NAME)-y += nic/ars/ars_powertracking.o
$(MODULE_NAME)-y += nic/ars/ars_rainfo.o
$(MODULE_NAME)-y += nic/ars/ars_thread.o
$(MODULE_NAME)-y += nic/ars/ars_txpower.o
$(MODULE_NAME)-y += nic/ars/ars_phy_iq.o
endif

ifeq ($(CONFIG_CMD_METHOD), mailbox)
EXTRA_CFLAGS += -DMCU_CMD_MAILBOX
else
EXTRA_CFLAGS += -DMCU_CMD_TXD
endif

obj-$(CONFIG_SCI9083H) := $(MODULE_NAME).o

else

export CONFIG_SCI9083H = m

all: modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(WDRV_DIR)  modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

install:
	install -p -m 644 $(MODULE_NAME).ko  $(MODDESTDIR)
	/sbin/depmod -a ${KVER}

modules_install:
	@$(MAKE) INSTALL_MOD_STRIP=1 M=$(WDRV_DIR) -C $(KSRC) modules_install
	mkdir -p ${OUT_DIR}/../vendor_lib/modules
	cd ${OUT_DIR}/$(WDRV_DIR)/; find -name $(MODULE_NAME).ko -exec cp {} ${OUT_DIR}/../vendor_lib/modules/$(MODULE_NAME).ko \;

uninstall:
	rm -f $(MODDESTDIR)/$(MODULE_NAME).ko
	/sbin/depmod -a ${KVER}

config_r:
	@echo "make config"
	/bin/bash script/Configure script/config.in


.PHONY: modules clean

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(WDRV_DIR)  clean
endif

