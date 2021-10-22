
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "wf_debug.h"
#include "iw_priv_func.h"
#include "mp.h"

#define MP_DBG(fmt, ...)        LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ARRAY(data, len)     log_array(data, len)
#define MP_INFO(fmt, ...)       LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_WARN(fmt, ...)       LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ERROR(fmt, ...)      LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#ifdef CONFIG_MP_MODE
#if defined(CONFIG_WEXT_PRIV)
extern wf_bool mp_test_enable;


//#define REG_CMD
#define     rFPGA0_XA_HSSIParameter1        0x820
#define     rFPGA0_XA_HSSIParameter2        0x824
#define     rFPGA0_XA_LSSIParameter     0x840
#define     rFPGA0_XA_LSSIReadBack      0x8a0
#define     TransceiverA_HSPI_Readback  0x8b8

#define     bLSSIReadAddress                0x7f800000
#define     bLSSIReadEdge                   0x80000000
#define     bLSSIReadBackData               0xfffff

#define     bMaskDWord      0xffffffff

wf_u32 mp_set_rf(nic_info_st *pnic_info)
{
    wf_u32 inbuff;
    wf_u32 ret = 0;
    inbuff = 1;
    if (NIC_USB == pnic_info->nic_type)
    {
        wf_io_write32(pnic_info, 0x92c, 0x1);
    }
    else
    {
        // ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_MP_SET_RF_PATH_SWICTH, &inbuff, 1, NULL, 0);
    }
    return ret;
}

static wf_u32 mp_bb_calculatebitshift(wf_u32 BitMask)
{
    wf_u32 i;
    for (i = 0; i <= 31; i++)
    {
        if (((BitMask >> i) & 0x1) == 1)
            break;
    }
    return (i);
}



static int mp_write_bb_reg(nic_info_st * pnic_info, wf_u32 RegAddr, wf_u32 BitMask, wf_u32 Data)
{
    wf_u32 OriginalValue, BitShift;
    int err;
    wf_u32 ret;
    OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
    BitShift = mp_bb_calculatebitshift(BitMask);
    Data = ((OriginalValue & (~BitMask)) | ((Data << BitShift) & BitMask));


    ret = wf_io_write32(pnic_info, RegAddr, Data);
    return ret;
}


static wf_u32 mp_read_bb_reg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask)
{
    wf_u32 ReturnValue = 0, OriginalValue, BitShift;
    int err;
    OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
    BitShift = mp_bb_calculatebitshift(BitMask);
    ReturnValue = (OriginalValue & BitMask) >> BitShift;

    return (ReturnValue);
}

static wf_u32 mp_read_rf_process(nic_info_st *pnic_info, wf_u32 eRFPath, wf_u32 Offset)
{
    wf_u32 retValue = 0;
    wf_u32 NewOffset;
    wf_u32 tmplong2;
    u8 RfPiEnable = 0;
    int MaskforPhySet = 0;
    int i = 0;


    Offset &= 0xff;

    NewOffset = Offset;

    tmplong2 =
        mp_read_bb_reg(pnic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet,
                       bMaskDWord);
    tmplong2 =
        (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;
    mp_write_bb_reg(pnic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
                    tmplong2 & (~bLSSIReadEdge));

    tmplong2 =
        mp_read_bb_reg(pnic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet,
                       bMaskDWord);
    mp_write_bb_reg(pnic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
                    tmplong2 & (~bLSSIReadEdge));
    mp_write_bb_reg(pnic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
                    tmplong2 | bLSSIReadEdge);

    wf_msleep(10);

    for (i = 0; i < 2; i++)
        wf_msleep(50);
    wf_msleep(10);

    RfPiEnable =
        (wf_u8) mp_read_bb_reg(pnic_info,
                               rFPGA0_XA_HSSIParameter1 | MaskforPhySet, BIT(8));

    if (RfPiEnable)
    {
        retValue =
            mp_read_bb_reg(pnic_info, 0x8b8 | MaskforPhySet,
                           bLSSIReadBackData);

    }
    else
    {
        retValue =
            mp_read_bb_reg(pnic_info, 0x8a0 | MaskforPhySet,
                           bLSSIReadBackData);

    }

    return retValue;

}


int mp_write_rf_process(nic_info_st *pnic_info,
                        wf_u32 eRFPath,  wf_u32 Offset,  wf_u32 Data)
{
    wf_u32 DataAndAddr = 0;
    wf_u32 NewOffset;
    wf_u32 ret;
    Offset &= 0xff;

    NewOffset = Offset;

    DataAndAddr = ((NewOffset << 20) | (Data & 0x000fffff)) & 0x0fffffff;

    ret = mp_write_bb_reg(pnic_info, 0x840, bMaskDWord, DataAndAddr);
    return ret;
}


static void mp_register_all_range(nic_info_st *nic_info, wf_u32 start, wf_u32 end)
{
    wf_u32 addr;
    LOG_D("%s ",__func__);
    for(addr=start; addr<=end; addr++)
    {
        printk("%02x:\n",addr);
        printk("%08x \n", mp_read_rf_process(nic_info, 0, addr));
    }
}


int wf_mp_reg_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    wf_u8 *pch;
    wf_u32 byte,addr;
    wf_u32 data=0;
    wf_u32 ret=WF_RETURN_OK;
    wrqu = (struct iw_point *)wdata;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        MP_WARN("copy_from_user fail");
        return WF_RETURN_FAIL;
    }
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG("in = %s",extra);

    sscanf(pch,"%d,%x",&byte,&addr);
    LOG_D("byte:%d addr:%x",byte,addr);
    switch(byte)
    {
#ifdef MCU_CMD_TXD
        case 1:
            data = wf_io_bulk_read8(pnic_info,addr);
            break;
        case 2:
            data = wf_io_bulk_read16(pnic_info,addr);
            break;
        case 4:
            data = wf_io_bulk_read32(pnic_info,addr);
            break;
        default:
            MP_WARN("byte error");
            ret = WF_RETURN_FAIL;
            break;
#else
        case 1:
            data = wf_io_read8(pnic_info,addr, NULL);
            break;
        case 2:
            data = wf_io_read16(pnic_info,addr, NULL);
            break;
        case 4:
            data = wf_io_read32(pnic_info,addr, NULL);
            break;
        default:
            MP_WARN("byte error");
            ret = WF_RETURN_FAIL;
            break;
#endif
    }
    LOG_D("reg:%x --%08X", addr, data);
    if(ret != WF_RETURN_FAIL)
    {
        sprintf(extra, "reg:%x --%08X", addr, data);
    }
    wrqu->length = strlen(extra);
    return 0;

}


int wf_mp_reg_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    wf_u8 *pch;
    wf_u32 byte,addr,data;
    wf_u32 ret;
    wrqu = (struct iw_point *)wdata;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        MP_WARN("copy_from_user fail");
        return WF_RETURN_FAIL;
    }
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG("in = %s",extra);

    sscanf(pch,"%d,%x,%x",&byte,&addr,&data);
    switch(byte)
    {
#ifdef  MCU_CMD_TXD
        case 1:
            ret = wf_io_bulk_write8(pnic_info,addr, data);
            break;
        case 2:
            ret = wf_io_bulk_write16(pnic_info,addr, data);
            break;
        case 4:
            ret = wf_io_bulk_write32(pnic_info,addr, data);
            break;
        default:
            MP_WARN("byte error");
            ret = WF_RETURN_FAIL;
            break;
#else
        case 1:
            ret = wf_io_write8(pnic_info,addr, data);
            break;
        case 2:
            ret = wf_io_write16(pnic_info,addr, data);
            break;
        case 4:
            ret = wf_io_write32(pnic_info,addr, data);
            break;
        default:
            MP_WARN("byte error");
            ret = WF_RETURN_FAIL;
            break;
#endif
    }
    if(ret == 0)
    {
        sprintf(extra,"write reg ok");
    }
    else
    {
        sprintf(extra,"write reg fail");
    }
    wrqu->length = strlen(extra);
    return 0;
}



int wf_mp_read_bb(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    char *pch;
    int err;
    wf_u32 bit,addr;
    wf_u32 data;
    wrqu = (struct iw_point *)wdata;
    err = 0;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG(": in=%s\n", extra);
    sscanf(extra,"%d,%x",&bit,&addr);
    LOG_D("bit:%d,addr:%x",bit,addr);
    switch(bit)
    {
        case 1:
            data = wf_io_read8(pnic_info, addr, NULL);
            snprintf(extra, 50, "%x --%02X", addr, data);
            break;
        case 2:
            data = wf_io_read16(pnic_info, addr, NULL);
            sprintf(extra, "%x --%04X", addr, data);
            break;
        case 4:
            data = wf_io_read32(pnic_info, addr, NULL);
            sprintf(extra, "%x --%08X", addr, data);
            break;
        default:
            MP_WARN("byte error");
            break;
    }
    wrqu->length = strlen(extra);
    return 0;

}


int wf_mp_write_bb(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    char *pch;
    int err;
    wf_u32 ret = -1;
    wf_u32 addr;
    wf_u32 data;
    wf_u32 bit;
    wrqu = (struct iw_point *)wdata;
    err = 0;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG(": in=%s\n", extra);
    sscanf(extra,"%d,%x,%x",&bit,&addr,&data);
    switch(bit)
    {
        case 1:
            ret = wf_io_write8(pnic_info, addr, data);
            break;
        case 2:
            ret = wf_io_write16(pnic_info, addr, data);
            break;
        case 4:
            ret = wf_io_write32(pnic_info, addr, data);
            break;
        default:
            MP_WARN("bit error");
            break;
    }
    sprintf(extra,"write bb %s", ret == 0 ? "ok" : "fail");
    wrqu->length = strlen(extra);
    return 0;
}


int wf_mp_read_rf(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    char *pch;
    int err;
    wf_u32 ant = 0,addr;
    wf_u32 data;
    wrqu = (struct iw_point *)wdata;
    err = 0;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;

    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG(": in=%s\n", extra);
    if(strcmp(pch, "all") == 0)
    {
        mp_register_all_range(pnic_info,0,0xff);
        return 0;
    }
    else
    {
        sscanf(extra,"%x",&addr);
        data = mp_read_rf_process(pnic_info, 0, addr);
        sprintf(extra, "ant:%d %x --%08X",ant, addr, data);
        wrqu->length = strlen(extra);
        return 0;
    }
    return 0;

}




int wf_mp_write_rf(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    char *pch;
    int err;
    wf_u32 addr;
    wf_u32 data;
    wf_u32 ret = -1;
    wrqu = (struct iw_point *)wdata;
    err = 0;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

    pch = extra;
    MP_DBG(": in=%s\n", extra);
    sscanf(extra,"%x,%x",&addr,&data);
    ret = mp_write_rf_process(pnic_info, 0, addr, data);

    if(ret == 0)
    {
        sprintf(extra,"write rf ok");
    }
    else
    {
        sprintf(extra,"write rf fail");
    }
    wrqu->length = strlen(extra);
    return 0;

}



int wf_mp_write_bbreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask,wf_u32 Data)
{
#ifdef REG_CMD
    wf_u32 ret;
    wf_u32 inbuff[3];
    inbuff[0] = RegAddr;
    inbuff[1] = BitMask;
    inbuff[2] = Data;
    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_BB_REG_SET, inbuff, 3, NULL, 0);
    }
    else
    {
//          ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_READ_VERSION, &efuse_code, 0, version, 1);
    }
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return WF_RETURN_FAIL;
    }
    return WF_RETURN_OK;
#else
    wf_u32 OriginalValue, BitShift;
    wf_u32 err=0;
    int ret;
    OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
    BitShift = mp_bb_calculatebitshift(BitMask);
    Data = ((OriginalValue & (~BitMask)) | ((Data << BitShift) & BitMask));
    ret = wf_io_write32(pnic_info, RegAddr, Data);
    return 0;
#endif
}

wf_u32 wf_mp_read_bbreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask)
{
    wf_u32 ret = 0;
    wf_u32 outbuff;

#ifdef REG_CMD
    inbuff[0] = RegAddr;
    inbuff[1] = BitMask;

    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_BB_REG_GET, inbuff, 2, &outbuff, 1);
    }
#else
    outbuff = mp_read_bb_reg(pnic_info, RegAddr, BitMask);
#endif

    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return -1;
    }
    return outbuff;
}


wf_u32 wf_mp_read_rfreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask)
{
    wf_u32 ret = 0;
    wf_u32 outbuff;

#ifdef REG_CMD
    inbuff[0] = RegAddr;
    inbuff[1] = BitMask;

    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_RF_REG_GET, inbuff, 2, &outbuff, 1);
    }

#else
    {
        wf_u32 Original_Value, BitShift;

        Original_Value = mp_read_rf_process(pnic_info, 0, RegAddr);

        BitShift = mp_bb_calculatebitshift(BitMask);
        outbuff = (Original_Value & BitMask) >> BitShift;
    }
#endif

    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return -1;
    }
    return outbuff;
}




int wf_mp_write_rfreg(nic_info_st *pnic_info, wf_u32 eRFPath, wf_u32 RegAddr,wf_u32 BitMask, wf_u32 Data)
{
#ifdef REG_CMD
    wf_u32 ret;
    wf_u32 inbuff[3];
    inbuff[0] = RegAddr;
    inbuff[1] = BitMask;
    inbuff[2] = Data;
    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_RF_REG_SET, inbuff, 3, NULL, 0);
    }
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }
#else
    wf_u32 Original_Value, BitShift;
    wf_u32 ret;
    Original_Value = mp_read_rf_process(pnic_info, eRFPath, RegAddr);
    BitShift = mp_bb_calculatebitshift(BitMask);
    Data = ((Original_Value & (~BitMask)) | (Data << BitShift));


    ret = mp_write_rf_process(pnic_info, eRFPath, RegAddr, Data);
    return ret;

#endif
}



#endif
#endif
