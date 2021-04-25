#include "pcomp.h"

#include "wf_mix.h"
#include "wf_typedef.h"

#include "wf_os_api.h"

// TODO: Add wf_mdelay after OS API is finished. 2021/03/02
// Temporarily do nothing with function wf_msleep().


int power_on(PADAPTER pAdapter)
{
	wf_bool initSuccess = wf_false;
	wf_u8  value8;
	wf_u16 value16;
	wf_u32 value32;


	//set 0x_00AC  bit 4 §Õ0
	value8 = HwPlatformIORead1Byte(pAdapter, 0xac, NULL);
	value8 &= 0xEF;
	HwPlatformIOWrite1Byte(pAdapter, 0xac, value8);

	//set 0x_00AC  bit 0 §Õ0
	value8 &= 0xFE;
	HwPlatformIOWrite1Byte(pAdapter, 0xac, value8);
	
	//set 0x_00AC  bit 0 §Õ1
	value8 |= 0x01;
	HwPlatformIOWrite1Byte(pAdapter, 0xac, value8);

	wf_msleep(10);
	// waiting for power on
	value16 = 0;

	while (1) {
		value8 = HwPlatformIORead1Byte(pAdapter, 0xac, NULL);
		if (value8 & 0x10) {
			initSuccess = wf_true;
			break;
		}
		value16++;
		if (value16 > 1000) {
			break;
		}
	}

	// enable mcu-bus clk
	HwPlatformIORead4Byte(pAdapter, 0x94, NULL);
	HwPlatformIOWrite4Byte(pAdapter, 0x94, 0x6);


	if (initSuccess == wf_false)
	{
		LOG_E(" failed!!!");
		return WF_RETURN_FAIL;
	}

	LOG_D(" success");

	return WF_RETURN_OK;

}