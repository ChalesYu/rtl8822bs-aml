#ifndef __ARS_DEFINE_H__
#define __ARS_DEFINE_H__

#define ODM_NUM_RATE_IDX (ODM_RATEMCS7+1)

typedef enum tag_Wireless_Mode_Definition
{
    ODM_WM_UNKNOW   = 0x0,
    ODM_WM_B        = BIT(0),
    ODM_WM_G        = BIT(1),
    ODM_WM_A        = BIT(2),
    ODM_WM_N24G     = BIT(3),
    ODM_WM_N5G      = BIT(4),
    ODM_WM_AUTO     = BIT(5),
    ODM_WM_AC       = BIT(6),
}ODM_WIRELESS_MODE_E;

#endif
