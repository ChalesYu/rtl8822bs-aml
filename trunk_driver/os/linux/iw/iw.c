/*
 * iw.c
 *
 * used for wext
 *
 * Author: houchuang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "wf_debug.h"
#include "ndev_linux.h"
#include "iw_func.h"
#include "iw_priv_func.h"
#include "mp.h"

#ifdef CONFIG_WIRELESS_EXT
static struct iw_statistics *wf_get_wireless_stats (struct net_device *ndev)
{
    struct iw_statistics *piw_stats;

    ndev_priv_st *ndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = ndev_priv->nic;
    wf_u8 qual,level;

    wf_wlan_get_signal_and_qual(pnic_info,&qual,&level);
    if (ndev_priv == NULL)
    {
        free_netdev(ndev);
        LOG_E("netdev_priv error");
        return NULL;
    }
    else
    {
        piw_stats = &ndev_priv->iw_stats;
    }


    piw_stats->qual.qual = qual;
    piw_stats->qual.level = signal_scale_mapping(level);
    piw_stats->qual.noise = 0;
    piw_stats->qual.updated = IW_QUAL_ALL_UPDATED ;/* |IW_QUAL_DBM; */

    //LOG_D("QUAL:%d      level:%d",piw_stats->qual.qual,piw_stats->qual.level);

    return piw_stats;
}
#endif

#ifdef CONFIG_WIRELESS_EXT
#ifndef CONFIG_MP_MODE
static iw_handler wl_handlers[] =
{
    NULL,//    wf_iw_setCommit,             /*0x8B00  SIOCSIWCOMMIT*/
    wf_iw_getName,               /*0x8B01  SIOCGIWNAME*/
    NULL,//    wf_iw_setNetworkId,          /*0x8B02  SIOCSIWNWID*/
    NULL,//    wf_iw_getNetworkId,          /*0x8B03  SIOCGIWNWID*/
    wf_iw_setFrequency,          /*0x8B04  SIOCSIWFREQ*/
    wf_iw_getFrequency,          /*0x8B05  SIOCGIWFREQ*/
    wf_iw_setOperationMode,      /*0x8B06  SIOCSIWMODE*/
    wf_iw_getOperationMode,      /*0x8B07  SIOCGIWMODE*/
    NULL,//    wf_iw_setSensitivity,        /*0x8B08  SIOCSIWSENS*/
    wf_iw_getSensitivity,        /*0x8B09  SIOCGIWSENS*/
    NULL,//    wf_iw_setRange,              /*0x8B0A  SIOCSIWRANGE*/
    wf_iw_getRange,              /*0x8B0B  SIOCGIWRANGE*/
    wf_iw_setPriv,               /*0x8B0C  SIOCSIWPRIV*/
    NULL,//    wf_iw_getPriv,               /*0x8B0D  SIOCGIWPRIV*/
    NULL,//    wf_iw_setWirelessStats,      /*0x8B0E  SIOCSIWSTATS*/
    wf_iw_getWirelessStats,      /*0x8B0F  SIOCGIWSTATS*/
    NULL,//    wf_iw_setSpyAddresses,       /*0x8B10  SIOCSIWSPY*/
    NULL,//    wf_iw_getSpyInfo,            /*0x8B11  SIOCGIWSPY*/
    NULL,//    wf_iw_setSpyThreshold,       /*0x8B12  SIOCGIWTHRSPY*/
    NULL,//    wf_iw_getSpyThreshold,       /*0x8B13  SIOCWIWTHRSPY*/
    wf_iw_setWap,                /*0x8B14  SIOCSIWAP*/
    wf_iw_getWap,                /*0x8B15  SIOCGIWAP*/
    wf_iw_setMlme,               /*0x8B16  SIOCSIWMLME*/
    NULL,//    wf_iw_getWapList,            /*0x8B17  SIOCGIWAPLIST*/
    wf_iw_setScan,               /*0x8B18  SIOCSIWSCAN*/
    wf_iw_getScan,               /*0x8B19  SIOCGIWSCAN*/
    wf_iw_setEssid,              /*0x8B1A  SIOCSIWESSID*/
    wf_iw_getEssid,              /*0x8B1B  SIOCGIWESSID*/
    NULL,//    wf_iw_setNick,               /*0x8B1C  SIOCSIWNICKN*/
    wf_iw_getNick,               /*0x8B1D  SIOCGIWNICKN*/
    NULL,//      NULL,                        /*0x8B1E  ---hole---*/
    NULL,//      NULL,                        /*0x8B1F  ---hole---*/
    wf_iw_setRate,               /*0x8B20  SIOCSIWRATE*/
    wf_iw_getRate,               /*0x8B21  SIOCGIWRATE*/
    wf_iw_setRts,                /*0x8B22  SIOCSIWRTS*/
    wf_iw_getRts,                /*0x8B23  SIOCGIWRTS*/
    wf_iw_setFragmentation,      /*0x8B24  SIOCSIWFRAG*/
    wf_iw_getFragmentation,      /*0x8B25  SIOCGIWFRAG*/
    NULL,//    wf_iw_setTransmitPower,      /*0x8B26  SIOCSIWTXPOW*/
    NULL,//    wf_iw_getTransmitPower,      /*0x8B27  SIOCGIWTXPOW*/
    NULL,//    wf_iw_setRetry,              /*0x8B28  SIOCSIWRETRY*/
    wf_iw_getRetry,              /*0x8B29  SIOCGIWRETRY*/
    wf_iw_setEnc,                /*0x8B2A  SIOCSIWENCODE*/
    wf_iw_getEnc,                /*0x8B2B  SIOCGIWENCODE*/
    NULL,//    wf_iw_setPower,              /*0x8B2C  SIOCSIWPOWER*/
    wf_iw_getPower,              /*0x8B2D  SIOCGIWPOWER*/
    NULL,//      NULL,                        /*0x8B2E  ---hole---*/
    NULL,//      NULL,                        /*0x8B2F  ---hole---*/
    wf_iw_setGenIe,              /*0x8B30  SIOCSIWGENIE*/
    NULL,//    wf_iw_getGenIe,              /*0x8B31  SIOCGWGENIE*/
    wf_iw_setAuth,               /*0x8B32  SIOCSIWAUTH*/
    wf_iw_getAuth,               /*0x8B33  SIOCGIWAUTH*/
    wf_iw_setEncExt,             /*0x8B34  SIOCSIWENCODEEXT*/
    wf_iw_getEncExt,             /*0x8B35  SIOCGIWENCODEEXT*/
    wf_iw_setPmkid,              /*0x8B36  SIOCSIWPMKSA*/
    NULL,//      NULL,                        /*---hole---*/
};
#endif
#endif


#if defined(CONFIG_WEXT_PRIV)
static iw_handler wl_private_handler[] =
{
    wf_iw_fw_debug,
    wf_iw_reg_read,
    wf_iw_reg_write,
    wf_iw_ars,
    wf_iw_cmddl,
    wf_iw_txagg_timestart,
#ifdef CONFIG_MP_MODE
    wf_mp,
#endif
};



static struct iw_priv_args wl_private_args[] =
{
    {IW_PRV_FW_DEBUG,              IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,               "fw_debug"},
    {IW_PRV_READ_REG_TEST,       IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "read_reg_test"},
    {IW_PRV_WRITE_REG_TEST,        IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,             "write_reg_test"},
    {IW_PRV_ARS,                       IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,             "ars"},
    {IW_PRV_TEST,                  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,             "cmd_download"},
    {IW_PRV_TXAGG,                  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "txagg"},
#ifdef CONFIG_MP_MODE
    {IW_PRV_MP_GET,                IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,               ""},
    {IW_PRV_SET,                   IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "set"},
    {IW_PRV_GET,                   IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "get"},
    {IW_PRV_EFUSE_SET_PHY,         IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "write_efuse"},
    {IW_PRV_EFUSE_GET_PHY,         IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "read_efuse"},
    {IW_PRV_READ_BB,               IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "read_bb"},
    {IW_PRV_READ_RF,               IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "read_rf"},
    {IW_PRV_WRITE_RF,              IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "write_rf"},
    {IW_PRV_WRITE_BB,              IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "write_bb"},
    {IW_PRV_EFUSE_SET_LOGIC,       IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "set_efuse"},
    {IW_PRV_EFUSE_GET_LOGIC,       IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "get_efuse"},
    {IW_PRV_WRITE_REG,             IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "write_reg"},
    {IW_PRV_READ_REG,              IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "read_reg"},
    {IW_PRV_WRITE_PHY_EFUSE,       IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "set_phy_efuse"},
    {IW_PRV_FW_INIT,               IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,              "fw_init"},
#endif
};
#endif

#ifdef CONFIG_WIRELESS_EXT
const struct iw_handler_def wl_handlers_def =
{
#   ifndef CONFIG_MP_MODE
    .standard = (iw_handler *)wl_handlers,
    .num_standard = ARRAY_SIZE(wl_handlers),
#   endif
#   if defined(CONFIG_WEXT_PRIV)
    .private = (iw_handler *)wl_private_handler,
    .num_private = ARRAY_SIZE(wl_private_handler),

    .private_args = (struct iw_priv_args *)wl_private_args,
    .num_private_args = ARRAY_SIZE(wl_private_args),
#   endif
#   if WIRELESS_EXT >= 17
    .get_wireless_stats = wf_get_wireless_stats,
#   endif
};
#endif







