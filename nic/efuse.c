#include "common.h"
#include "wf_debug.h"

#define BOOT_FROM_EEPROM        WF_BIT(4)
#define EEPROMSEL               WF_BIT(4)
#define EEPROM_EN               WF_BIT(5)

/*************************************************
 * Function     : wf_get_efuse_type
 * Description  : get the type of eeprom
 * Input        : nic_info
 * Output       : None
 * Return       : NULL if failed; "E-FUSE" or "93C46"
 *************************************************/
char *wf_get_efuse_type(nic_info_st *nic_info)
{
    int ret = 0;
    int size = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_EFUSETYPE,NULL,0,&size,1);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_EEPORMTYPE,NULL,0,&size,1);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("get efuse type failed");
        return NULL;
    }

    size = (size & BOOT_FROM_EEPROM) ? 6 : 4;

    LOG_D("EEPROM type is %s\n", size == 4 ? "E-FUSE" : "93C46");

    return size == 4 ? "E-FUSE" : "93C46";
}

/*************************************************
* Function     : wf_get_efuse_len
* Description  :
* Input        : nic_info
* Output       :
* Return       : 0 if failed, >0
*************************************************/
wf_u32 wf_get_efuse_len(nic_info_st *nic_info)
{
    int ret = 0;
    wf_u32 len  = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_EFUSEMAP_LEN,NULL,0,&len,1);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_EEPORMMAP_LEN,NULL,0,&len,1);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("wf_get_efuse_len failed");
        return 0;
    }

    return len;
}

/*************************************************
* Function     : wf_get_phy_efuse_len
* Description  :
* Input        : nic_info
* Output       :
* Return       : 0 if failed, >0
*************************************************/
wf_u32 wf_get_phy_efuse_len(nic_info_st *nic_info)
{
    int ret = 0;
    wf_u32 len  = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_MP_EFUSE_GSize,NULL,0,&len,1);
    }
    else
    {
//        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_EEPORMMAP_LEN,NULL,0,&len,1);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("wf_get_efuse_len failed");
        return 0;
    }

    return len;
}


/*************************************************
 * Function     : wf_get_efuse_len
 * Description  :
 * Input        : nic_info, the info of nic;
                  efuse_len, the len of efuse data
 * Output       : outdata, the content of efuse
 * Return       : WF_RETURN_FAIL, WF_RETURN_OK
 *************************************************/
int  wf_get_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len )
{
    int ret                 = 0;
    wf_u32 word_len         = 0;
    wf_u32 intger           = 0;
    wf_u32 remainder        = 0;
    wf_u32 *pread           = 0;
    wf_u32  i               = 0;
    wf_u32 offset           = 0;

    //wf_u32 tmp_efuse_len = wf_get_efuse_len(nic_info);
    /* get efuse map */
    word_len = efuse_len / 4;
    //word_len = tmp_efuse_len / 4;
    intger = word_len / MAILBOX_MAX_RDLEN;
    remainder = word_len % MAILBOX_MAX_RDLEN;
    pread = (wf_u32 *)outdata;

    // LOG_I("%s,efuse_len=%d,intger=%d,remainder=%d",__FUNCTION__,efuse_len,intger,remainder);

    for (i = 0; i < intger; i++)
    {

        if(NIC_USB == nic_info->nic_type)
        {
            ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_HAL_EFUSEMAP,pread,MAILBOX_MAX_RDLEN, offset);
        }
        else
        {
            ret = mcu_cmd_communicate_special(nic_info,WLAN_OPS_DXX0_HAL_EEPORMMAP,pread,MAILBOX_MAX_RDLEN,offset);
        }

        if (WF_RETURN_FAIL == ret)
        {
            LOG_E("get efuse type failed");
            return WF_RETURN_FAIL;
        }
        LOG_I("[%s] offset:0x%x",__func__,offset);
        offset = offset + MAILBOX_MAX_RDLEN;//sdio need this
        pread  = pread + MAILBOX_MAX_RDLEN;

    }

    LOG_I("[%s] offset:0x%x",__func__,offset);

    if (remainder > 0)
    {

        if(NIC_USB == nic_info->nic_type)
        {
            ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_HAL_EFUSEMAP,pread,remainder,offset);
        }
        else
        {
            ret = mcu_cmd_communicate_special(nic_info,WLAN_OPS_DXX0_HAL_EEPORMMAP,pread,remainder,offset);
        }

        if (WF_RETURN_FAIL == ret)
        {
            LOG_E("get efuse type failed");
            return WF_RETURN_FAIL;
        }

        offset += remainder;
    }


#if 0
    log_array(outdata, 512);
#endif
    return WF_RETURN_OK;
}



/*************************************************
 * Function     : wf_get_phy_efuse_data
 * Description  :
 * Input        : nic_info, the info of nic;
                  efuse_len, the len of efuse data
 * Output       : outdata, the content of efuse
 * Return       : WF_RETURN_FAIL, WF_RETURN_OK
 *************************************************/
int  wf_get_phy_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len )
{
    int ret                 = 0;
    wf_u32 word_len         = 0;
    wf_u32 intger           = 0;
    wf_u32 remainder        = 0;
    wf_u32 *pread           = 0;
    wf_u32  i               = 0;
    wf_u32 offset           = 0;

    //wf_u32 tmp_efuse_len = wf_get_efuse_len(nic_info);
    /* get efuse map */
    word_len = efuse_len / 4;
    //word_len = tmp_efuse_len / 4;
    intger = word_len / MAILBOX_MAX_RDLEN;
    remainder = word_len % MAILBOX_MAX_RDLEN;
    pread = (wf_u32 *)outdata;

    // LOG_I("%s,efuse_len=%d,intger=%d,remainder=%d",__FUNCTION__,efuse_len,intger,remainder);

    for (i = 0; i < intger; i++)
    {

        if(NIC_USB == nic_info->nic_type)
        {
            ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_MP_EFUSE_ACCESS,pread,MAILBOX_MAX_RDLEN, offset);
        }
        else
        {
//            ret = mcu_cmd_communicate_special(nic_info,WLAN_OPS_DXX0_HAL_EEPORMMAP,pread,MAILBOX_MAX_RDLEN,offset);
        }

        if (WF_RETURN_FAIL == ret)
        {
            LOG_E("get efuse type failed");
            return WF_RETURN_FAIL;
        }
        LOG_I("[%s] offset:0x%x",__func__,offset);
        offset = offset + MAILBOX_MAX_RDLEN;//sdio need this
        pread  = pread + MAILBOX_MAX_RDLEN;

    }

    LOG_I("[%s] offset:0x%x",__func__,offset);

    if (remainder > 0)
    {

        if(NIC_USB == nic_info->nic_type)
        {
            ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_MP_EFUSE_ACCESS,pread,remainder,offset);
        }
        else
        {
//            ret = mcu_cmd_communicate_special(nic_info,WLAN_OPS_DXX0_HAL_EEPORMMAP,pread,remainder,offset);
        }

        if (WF_RETURN_FAIL == ret)
        {
            LOG_E("get efuse type failed");
            return WF_RETURN_FAIL;
        }

        offset += remainder;
    }


#if 0
    log_array(outdata, 512);
#endif
    return WF_RETURN_OK;
}


/*************************************************
* Function     : wf_efuse_select
* Description  :
* Input        : nic_info
* Output       :
* Return       : WF_RETURN_FAIL, WF_RETURN_OK
*************************************************/
int wf_efuse_select(nic_info_st *nic_info)
{
    int ret = 0;
    wf_u32 value32;
    wf_u8 eeValue;
    hw_info_st *hw_info = (hw_info_st *)nic_info->hw_info;

#if 0
    if( NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_EFSUSESEL,NULL,0,NULL,0);
    }
    else
    {
        //ret =  mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_HAL_EEPORM_SET_REG,NULL,0,NULL,0);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("set efuse select error");
        return WF_RETURN_FAIL;
    }

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_EFUSETYPE,NULL,0,&value32,1);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_EEPORMTYPE,NULL,0,&value32,1);
    }

    eeValue = (wf_u8)(value32&0xffff);

    hw_info->efuse_sel = (eeValue & EEPROMSEL) ? wf_true : wf_false;
    hw_info->bautoload_flag = (eeValue & EEPROM_EN) ? wf_false : wf_true;
#else
    hw_info->efuse_sel = wf_false;
    hw_info->bautoload_flag = wf_false;
#endif

    LOG_D("Boot from %s, Autoload %s !\n",
          (hw_info->efuse_sel ? "EEPROM" : "EFUSE"),
          (hw_info->bautoload_flag ? "Fail" : "OK"));

    return WF_RETURN_OK;
}

/*************************************************
* Function     : wf_get_efuse_load_mode
* Description  :
* Input        : nic_info
                 len, the output param 'bautoload_flag' length
* Output       : bautoload_flag
* Return       : WF_RETURN_FAIL, WF_RETURN_OK
*************************************************/
int wf_get_efuse_load_mode(nic_info_st *nic_info,  wf_u32 *bautoload_flag, wf_u32 len)
{
    int ret = 0;
    wf_u32 msg_box[2] = { 0 };
    hw_info_st *hw_info = (hw_info_st *)nic_info->hw_info;

    msg_box[1] = hw_info->bautoload_flag;

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG,msg_box,2,bautoload_flag,len);
    }
    else
    {
        msg_box[1] = 1;
        ret = mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_HAL_EEPORM_BAUTOLOAD_FLAG,msg_box,2,bautoload_flag,len);

    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("set efuse load mode failed");
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


/*************************************************
* Function     : wf_mcu_mp_efuse_get
* Description  :
* Input        : 1. nic_info
                 2. efuse_code
                 3. outdata_len
* Output       : outdata
* Return       : WF_RETURN_FAIL, WF_RETURN_OK
*************************************************/
int wf_mcu_mp_efuse_get(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *outdata, wf_u32 outdata_len)
{
    int ret = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        //ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_INIT_STEP0,&is_dw,1,NULL,0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_HAL_MP_EEPORM_GET,&efuse_code,1,outdata,outdata_len);
    }
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WLAN_OPS_DXX0_HAL_MP_EEPORM_SET failed", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}

/*************************************************
 * Function     : wf_mcu_mp_efuse_set
 * Description  :
 * Input        : 1. nic_info
                  2. efuse_code
                  3. indata
                  4. indata_len
                  5. out_len
 * Output       : 1. out
 * Return       : WF_RETURN_FAIL, WF_RETURN_OK
 *************************************************/
int wf_mcu_mp_efuse_set(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *indata, wf_u32 indata_len, wf_u32 *out, wf_u32 out_len)
{
    int ret = 0;
    wf_u32 msg[MAX_MAILBOX_LEN]= {0};

    msg[0] = efuse_code;
    wf_memcpy(&msg[1],indata,indata_len*4);
    if(NIC_USB == nic_info->nic_type)
    {
        //ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_INIT_STEP0,&is_dw,1,NULL,0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_HAL_MP_EEPORM_SET,&efuse_code,indata_len+1,out,out_len);
    }
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WLAN_OPS_DXX0_HAL_MP_EEPORM_SET failed", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}


int wf_mcu_efuse_read_1byte(nic_info_st *nic_info, wf_u16 Address, wf_u8 *data)
{
    int ret = 0;
    wf_u32 inbox = (wf_u32) Address;
    wf_u32 outbox = 0;

    if(NIC_USB == nic_info->nic_type)
    {

        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_EFUSE_1BYTE, &inbox,1,&outbox, 1);
    }
    else
    {
        LOG_I("[%s] to do for sdio",__func__);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] UMSG_OPS_EFUSE_1BYTE failed", __func__);
        return ret;
    }

    *data = (wf_u8) outbox;


    return WF_RETURN_OK;

}

