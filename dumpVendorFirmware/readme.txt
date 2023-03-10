This tool can be use to extract firmware from realtek vendor driver.

The guide will show how to extract firmware from rtl8822bs vendor driver, and make rtw88 driver load.
It also can be use on other realtek vendor driver for different realtek WiFi chip.

STEP1:
	Copy hal/rtl8822b/hal8822b_fw.c and rename it to tfw.c
	Copy hal/rtl8822b/hal8822b_fw.h and rename it to tfw.h

STEP2:
	Using command modify tfw.c and tfw.h:

	sed -i "/^#/d" tfw.h
	sed -i "/^#/d" tfw.c
	sed -i 's/u8/unsigned char/' tfw.h
	sed -i 's/u32/unsigned int/' tfw.h
	sed -i 's/u8/unsigned char/' tfw.c
	sed -i 's/u32/unsigned int/' tfw.c

STEP3:
	tfw.h  show the list of firmware.
	For rtl8822bs, it need array_mp_8822b_fw_nic and array_length_mp_8822b_fw_nic.
	So, need to edit string2bin.c ,modify some line:

	#define firmware_array  array_mp_8822b_fw_nic
	#define firmware_length  array_length_mp_8822b_fw_nic

STEP4:
	Then, compile string2bin ,run it.
	using command:

	make
	./string2bin

STEP5:
	It will output firmware.bin for rtl8822b
	Rename firmware.bin to rtw8822b_fw.bin
	Then place to /lib/firmware/rtw88/rtw8822b_fw.bin to make rtw88 driver load.

