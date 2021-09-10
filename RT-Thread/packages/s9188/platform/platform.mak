
HOST_PLAT ?= pc

ifeq ($(HOST_PLAT), pc)
CONFIG_PLATFORM_I386_PC = y
endif
ifeq ($(HOST_PLAT), goke)
CONFIG_PLATFORM_GOKE_IPC = y
endif
ifeq ($(HOST_PLAT), smart210)
CONFIG_PLATFORM_ARM_Smart210 = y
endif
ifeq ($(HOST_PLAT), mys6ul)
CONFIG_PLATFORM_ARM_mys6ul = y
endif
ifeq ($(HOST_PLAT), nuc980_iot)
CONFIG_PLATFORM_ARM_nuc980_iot = y
endif
ifeq ($(HOST_PLAT), tiny4412)
CONFIG_PLATFORM_ARM_tiny4412 = y
endif
ifeq ($(HOST_PLAT), stm32mp157c_dk2)
CONFIG_PLATFORM_ARM_STM32MP157C_DK2 = y
endif
ifeq ($(HOST_PLAT), t31)
CONFIG_PLATFORM_MIPS_T31 = y
endif
ifeq ($(HOST_PLAT), goke_7102)
CONFIG_PLATFORM_GOKE_7102 = y
endif
ifeq ($(HOST_PLAT), raspberry)
CONFIG_PLATFORM_RASP_PI = y
endif
ifeq ($(HOST_PLAT), rk3229)
CONFIG_PLATFORM_ARM_rk3229 = y
endif

ifeq ($(HOST_PLAT), h313)
CONFIG_PLATFORM_ARM_H313 = y
endif

ifeq ($(HOST_PLAT), hi3798mv310)
CONFIG_PLATFORM_ARM_HI3798MV310 = y
endif

ifeq ($(HOST_PLAT), amlogic905W2)
CONFIG_PLATFORM_ARM_Amlogic905W2 = y
endif

###################### Platform Related #######################
CONFIG_PLATFORM_I386_PC ?= n
CONFIG_PLATFORM_GOKE_IPC ?= n
CONFIG_PLATFORM_ARM_Smart210 ?= n
CONFIG_PLATFORM_ARM_mys6ul ?=n
CONFIG_PLATFORM_ARM_tiny4412 ?= n
CONFIG_PLATFORM_MIPS_T31 ?= n
CONFIG_PLATFORM_GOKE_7102 ?= n
CONFIG_PLATFORM_RASP_PI ?= n
CONFIG_PLATFORM_ARM_H313 ?= n
CONFIG_PLATFORM_ARM_rk3229 ?=n
CONFIG_PLATFORM_ARM_HI3798MV310 ?=n
CONFIG_PLATFORM_ARM_Amlogic905W2 ?=n
##################### Platform Configure ######################
ifeq ($(CONFIG_PLATFORM_I386_PC), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
CROSS_COMPILE ?=
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
endif


ifeq ($(CONFIG_PLATFORM_GOKE_IPC), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_IPC
#EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211
ARCH := arm
#CROSS_COMPILE ?= /home/hichard/renhaibo/gk720x/GK720X_LinuxSDK_v1.0.0/opt/goke/toolchain/uClibc_gcc4.8.5/arm-gk720x-linux/bin/arm-gk720x-linux-uclibcgnueabi-
#KSRC ?=/home/hichard/renhaibo/gk720x/GK720X_LinuxSDK_v1.0.0/linux/kernel/3.4.43
CROSS_COMPILE ?= arm-gk720x-linux-uclibcgnueabi-
KSRC ?= /home/syt/share/GK720X_LinuxSDK_v1.0.0/linux/kernel/3.4.43
#KSRC ?= /home/hichard/renhaibo/002.gk7202/GK720X_LinuxSDK_v1.0.0/linux/kernel/3.4.43
endif



ifeq ($(CONFIG_PLATFORM_ARM_Smart210), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 3.0.8
CROSS_COMPILE :=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
KSRC ?= /home/syt/share/smart210/linux/linux-3.0.8
endif

ifeq ($(CONFIG_PLATFORM_ARM_tiny4412), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_4412
ARCH := arm
KVER  := 3.0.86
CROSS_COMPILE :=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
KSRC := /home/syt/share/4412/linux/linux-3.0.86
#KSRC ?= /home/hichard/renhaibo/001.smart210/linux-3.0.86
endif

ifeq ($(CONFIG_PLATFORM_MIPS_T31), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := mips
KVER  := 3.10.14
CROSS_COMPILE :=/home/syt/share/t31/mips-gcc472-glibc216-64bit/bin/mips-linux-gnu-
KSRC ?= /home/syt/share/t31/kernel
endif

ifeq ($(CONFIG_PLATFORM_GOKE_7102), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH  := arm
KVER  := 3.4.43
CROSS_COMPILE :=/opt/goke/ct_uClibc/4.6.1/usr/bin/arm-goke-linux-uclibcgnueabi-
KSRC ?= /home/syt/share/7102C/GK710X_LinuxSDK_v2.1.0/linux/kernel/3.4.43
endif

ifeq ($(CONFIG_PLATFORM_RASP_PI), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
ARCH := arm
CROSS_COMPILE ?=
KVER  := $(shell uname -r)
#KVER  = 5.10.17+
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
endif

ifeq ($(CONFIG_PLATFORM_ARM_mys6ul), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 4.9
CROSS_COMPILE :=/opt/gcc-linaro-4.9-2014.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
KSRC ?= /home/syt/share/MYiR-iMX-Linux
endif

ifeq ($(CONFIG_PLATFORM_ARM_nuc980_iot), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 4.4
CROSS_COMPILE :=/opt/nuc-arm9-linux/arm_linux_4.8/bin/arm-linux-
KSRC ?= /home/hichard/renhaibo/003.NK-980IOT/nuc980bsp/NUC980-linux-4.4.y
endif

ifeq ($(CONFIG_PLATFORM_ARM_STM32MP157C_DK2), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 5.4
CROSS_COMPILE :=/opt/stm32mp157_sdk/sysroots/x86_64-ostl_sdk-linux/usr/bin/arm-ostl-linux-gnueabi/arm-ostl-linux-gnueabi-
KSRC ?= /home/hichard/renhaibo/002.stm32mp157c-dk2/stm32mp1-openstlinux-5-4-dunfell-mp1-20-11-12_SOURCES/sources/arm-ostl-linux-gnueabi/linux-stm32mp-5.4.56-r0/linux-5.4.56
endif

ifeq ($(CONFIG_PLATFORM_ARM_rk3229), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 3.10.104
CROSS_COMPILE ?=/mnt/Android/rk3229_71/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KSRC ?= /mnt/Android/rk3229_71/kernel
endif

ifeq ($(CONFIG_PLATFORM_ARM_H313), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm64
KVER  := 4.9
CROSS_COMPILE ?= /mnt/Android/H313/longan/out/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
KSRC ?= /mnt/Android/H313/longan/kernel/linux-4.9
endif

ifeq ($(CONFIG_PLATFORM_ARM_HI3798MV310), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
KVER  := 4.9
CROSS_COMPILE ?=/home/zenghua/sdc/patch_1_for_HiSTBAndroidV800R001C00SPC120_20200221/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
KSRC ?= /home/zenghua/sdc/patch_1_for_HiSTBAndroidV800R001C00SPC120_20200221/out/target/product/Hi3798MV110/obj/KERNEL_OBJ
endif

ifeq ($(CONFIG_PLATFORM_ARM_Amlogic905W2), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
KSRC ?= ${OUT_DIR}
endif
