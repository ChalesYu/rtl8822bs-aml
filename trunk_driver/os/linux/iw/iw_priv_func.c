/*
 * iw_priv_func.c
 *
 * used for iwpriv private command
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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"
#include "hif.h"
#include "iw_priv_func.h"
#include "mcu_cmd.h"
#include "nic_io.h"
#include "mp.h"

#define IW_PRIV_DBG(fmt, ...)        LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_PRIV_ARRAY(data, len)     log_array(data, len)
#define IW_PRIV_INFO(fmt, ...)      LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_PRIV_WARN(fmt, ...)      LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_PRIV_ERROR(fmt, ...)     LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)


int wf_iw_priv_data_xmit(nic_info_st * pnic_info);
int wf_iw_priv_phy_reg(nic_info_st *nic_info, wf_u32 time);
int wf_iw_priv_mac_reg(nic_info_st *nic_info, int time);


#define IW_PRV_DEBUG


#ifdef IW_PRV_DEBUG
static void usb_register_all_range(nic_info_st *nic_info, wf_u32 start, wf_u32 end)
{
    wf_u32 addr;
    for(addr=start; addr<=end; addr++)
    {
        if(addr%16 == 0)
        {
            printk("\n%08x ", addr);
            wf_msleep(10);
        }
        printk("%02x ", wf_io_read8(nic_info, addr, NULL));
    }
    printk("\n");
}

int wf_iw_priv_reg(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hif_node_st *hif_info  = pnic_info->hif_node;
    struct iw_point *wdata;
    wf_u8 *input;
    wf_u32 ret = 0;
    wf_u16 input_len = 0;
    wf_u32 type;
    wf_u32 addr = 0;
    wf_u32 data = 0;
    int bytes = 0;
    int virt;

    wdata = &wrqu->data;

    input_len = wdata->length;
    if(0 == input_len)
    {
        return -ENOMEM;
    }

    input = wf_kzalloc(input_len + 1);
    if (NULL == input)
    {
        ret = -ENOMEM;
        goto exit;
    }

    if (copy_from_user(input, wdata->pointer, input_len))
    {
        ret = -EFAULT;
        goto exit;
    }

    virt = sscanf(input, "%d,%x,%x", &type, &addr, &bytes);
    if(virt == 1 && type == 0)
    {
        LOG_D("******************************************************************");
        LOG_D("----------------------register help func--------------------------");
        LOG_D("------------------------------------------------------------------");
        LOG_D("cmd: iwpriv wlan0 reg cmd1,cmd2,cmd3");
        LOG_D("cmd1:");
        LOG_D("cmd : if cmd1 = 0 --------------------------------------------help");
        LOG_D("cmd : if cmd1 = 1 -------------------------------------------write");
        LOG_D("cmd : if cmd1 = 2 --------------------------------------------read");
        LOG_D("cmd : if cmd1 = 3 -------------------------------read all register");
        LOG_D("cmd2: Register address");
        LOG_D("cmd3: used for cmd1 == 2, bytes --Number of bytes read");
        LOG_D("cmd3: used for cmd1 == 1, data --The value written to the register");
        LOG_D("help  --------------eg: iwpriv wlan0 reg 0------------------------");
        LOG_D("write --------------eg: iwpriv wlan0 reg 1,123,1------------------");
        LOG_D("read  --------------eg: iwpriv wlan0 reg 2,123,1------------------");
        LOG_D("read all -----------eg: iwpriv wlan0 reg 3------------------------");
        LOG_D("******************************************************************");
        sprintf(extra, "--help");
        goto exit;
    }
    else if(type == 1 && virt == 3)
    {
        hif_io_write32(hif_info, addr, bytes);
        sprintf(extra, "%x --%02X", addr, bytes);
        goto exit;
    }
    else if(type == 2 && virt == 3)
    {
        switch(bytes)
        {
            case 1:
                data = hif_io_read8(hif_info, addr, NULL);
                snprintf(extra, 50, "%x --%02X", addr, data);
                break;
            case 2:
                data = hif_io_read16(hif_info, addr, NULL);
                sprintf(extra, "%x --%04X", addr, data);
                break;
            case 4:
                data = hif_io_read32(hif_info, addr, NULL);
                sprintf(extra, "%x --%08X", addr, data);
                break;
            default:
                break;
        }
        goto exit;
    }
    else if(type == 3 && virt == 1)
    {
        LOG_D("---------------------------------------------\n");
        usb_register_all_range(pnic_info, 0, 0x2ff);
        LOG_D("---------------------------------------------\n");
        usb_register_all_range(pnic_info, 0x300, 0x7ff);
        LOG_D("---------------------------------------------\n");
        usb_register_all_range(pnic_info, 0x800, 0xfff);
        LOG_D("---------------------------------------------\n");
        sprintf(extra, "--read all");
        goto exit;
    }
    else
    {
        LOG_D("help  --------------eg: iwpriv wlan0 reg 0------------------------");
        sprintf(extra, "--error");
        ret = -1;
        goto exit;
    }

exit:
    wf_kfree(input);
    return ret;
}
#endif





int wf_iw_reg_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
#ifdef MCU_CMD_TXD
    void *hif = pnic_info->hif_node;
#endif
    struct iw_point *wrqu;
    wf_u8 *pch;
    wf_u32 byte,addr,data;
    wf_u32 ret;
    wrqu = (struct iw_point *)wdata;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        LOG_D("copy_from_user fail");
        return WF_RETURN_FAIL;
    }

    pch = extra;
    LOG_D("in = %s",extra);

    sscanf(pch,"%d,%x,%x",&byte,&addr,&data);
    switch(byte)
    {

#ifdef MCU_CMD_TXD
        case 1:
            ret = wf_io_bulk_write8(hif,addr, data);
            break;
        case 2:
            ret = wf_io_bulk_write16(hif,addr, data);
            break;
        case 4:
            ret = wf_io_bulk_write32(hif,addr, data);
            break;
        default:
            LOG_E("byte error");
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
            LOG_D("byte error");
            ret = WF_RETURN_FAIL;
            break;
#endif
    }
    if(ret == WF_RETURN_OK)
    {
        sprintf(extra, "%s" "%s", extra, " ok");
    }
    else if(ret == WF_RETURN_FAIL)
    {
        sprintf(extra, "%s" "%s", extra, " ok");
    }
    wrqu->length = strlen(extra);
    return 0;
}


int wf_iw_ars(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
#ifdef CONFIG_ARS_DRIVER_SUPPORT
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info = pmlme_info->pwdn_info;
    wf_u8 *pch;
    int err = 0;
    wf_u32 reg_val;
    struct iw_point *wrqu;
    wf_u32 main_cmd, sub_cmd, param1, param2;
    wrqu = (struct iw_point *)wdata;
    ars_st *pars = (ars_st *)pnic_info->ars;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        LOG_D("copy_from_user fail");
        return WF_RETURN_FAIL;
    }
    pch = extra;

    sscanf(pch, "%d,%d,%x,%x", &main_cmd, &sub_cmd, &param1, &param2);

    LOG_D("cmd=%d, sub_cmd=%d, p1=%d, p2=%d", main_cmd, sub_cmd, param1, param2);

    switch(main_cmd)
    {
        case 0:
            wf_mcu_msg_body_set_ability(pnic_info, param1, param2);
            break;
        case 1:
            odm_FalseAlarmCounterStatistics(pars);
            for(reg_val=0; reg_val<sub_cmd; reg_val++)
            {
                wf_msleep(2000);
                ars_dump_info(pnic_info);
            }
            break;
        case 3:
            switch(sub_cmd)
            {
                case 0:
                    hw_write_bb_reg(pnic_info, ODM_REG_IGI_A_11N, ODM_BIT_IGI_11N, param1);
                    LOG_D("Config igi value %x", param1);
                    break;
                case 1:
                    if (wf_mcu_rate_table_update(pnic_info, pwdn_info))
                    {
                        LOG_E("wf_mcu_rate_table_update Failed");
                    }
                    break;
                case 2:
                    if (param1)
                    {
                        LOG_D("->Noisy Env. RA fallback value\n");
                        hw_write_bb_reg(pnic_info, 0x430, bMaskDWord, 0x0);
                        hw_write_bb_reg(pnic_info, 0x434, bMaskDWord, 0x04030201);
                    }
                    else
                    {
                        LOG_D("->Clean Env. RA fallback value\n");
                        hw_write_bb_reg(pnic_info, 0x430, bMaskDWord, 0x02010000);
                        hw_write_bb_reg(pnic_info, 0x434, bMaskDWord, 0x06050403);
                    }
                    break;
                case 3:
                    if(param1 == 0)
                    {
                        wf_io_write8(pnic_info, ODM_REG_CCK_CCA_11N, 0xcd);
                    }
                    else if(param1 == 1)
                    {
                        wf_io_write8(pnic_info, ODM_REG_CCK_CCA_11N, 0x83);
                    }
                    else if(param1 == 2)
                    {
                        wf_io_write8(pnic_info, ODM_REG_CCK_CCA_11N, 0x40);
                    }
                    break;
                case 4:
                    phydm_rssi_report(pars, pwdn_info);
                    break;
                case 5:
                    ODM_CfoTracking(pars);
                    break;
                case 254:
                    reg_val = wf_io_read32(pnic_info, param1,&err);
                    LOG_D("addr=%x, val=%x", param1, reg_val);
                    break;
                case 255:
                    wf_io_write32(pnic_info, param1, param2);
                    reg_val = wf_io_read32(pnic_info, param1,&err);
                    LOG_D("addr=%x, want=%x, actual=%x", param1, param2, reg_val);
                    break;
                default:
                    LOG_D("invalid sub cmd!!!");
                    break;
            }
            break;
        default:
            LOG_D("invalid main cmd!!!");
            break;
    }

#endif
    //sscanf(pch,"%d,%d",&in_ops,&in_ability);

    //wf_mcu_msg_body_set_ability(pnic_info,in_ops,in_ability);
    return 0;
}

int wf_iw_reg_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    wf_u8 *pch;
    wf_u32 byte,addr,data = 0;
    wf_u32 addr_reg = 0x0;
    wf_u32 start = 0x0,end = 0xff;
    wf_u32 ret=WF_RETURN_OK;
#ifdef MCU_CMD_TXD
    void *hif = pnic_info->hif_node;
#endif
    wrqu = (struct iw_point *)wdata;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        LOG_D("copy_from_user fail");
        return WF_RETURN_FAIL;
    }

    pch = extra;
    LOG_D("in = %s",extra);
    if(strncmp(pch, "all",3) == 0)
    {
        for(addr_reg = start; addr_reg<=end; addr_reg++)
        {
            LOG_D("%02x:",addr_reg);
            LOG_D("%08x ", data = wf_io_read32(pnic_info,addr_reg, NULL));
        }
        return 0;
    }

    sscanf(pch,"%d,%x",&byte,&addr);
    LOG_D("byte:%d addr:%x",byte,addr);
    switch(byte)
    {
#ifdef MCU_CMD_TXD
        case 1:
            data = wf_io_bulk_read8(hif,addr);
            break;
        case 2:
            data = wf_io_bulk_read16(hif,addr);
            break;
        case 4:
            data = wf_io_bulk_read32(hif,addr);
            break;
        default:
            LOG_E("byte error");
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
            LOG_D("byte error");
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

static wf_inline void generate_rand_data (wf_u32 *pdata, wf_u16 len)
{
    wf_u8 i;

    for (i = 0; i < len; i++)
    {
        pdata[i++] = wf_os_api_rand32();
    }
}

int isspace(int x)
{
    if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
        return 1;
    else
        return 0;
}
int isdigit(int x)
{
    if(x<='9'&&x>='0')
        return 1;
    else
        return 0;

}
int atoi(const char *nptr)
{
    int c;              /* current char */
    int total;         /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( isspace((int)(unsigned char)*nptr) )
        ++nptr;

    c = (int)(unsigned char)*nptr++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++;    /* skip sign */

    total = 0;

    while (isdigit(c))
    {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}


int wf_iw_cmddl(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;

    wf_u32 count;
    wf_u32 ret;
    char *ptr = NULL;
    wf_u8 data[40] = "cmdtest";
    wf_u32 data321[40];
    wf_u32 out_buf[40] = {0};
    wf_u32 failed_times=0;
    wf_u32 ok_times=0;
    wf_u32 data32 = 0;
    LOG_D("%s",__func__);

    ptr = extra;
    sscanf(ptr,"%d",&count);

    for(; count>0; count--)
    {
        int i = 0;
        int tmlen= strlen(data);
        memset(out_buf,0, sizeof(out_buf) /*  * sizeof(wf_u32)  */);
        for(i=0; i<tmlen; i++)
        {
            data321[i]= data[i];
        }
        //LOG_I("%s  tmlen:%d",__func__,tmlen);
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_CMD_TEST, (wf_u32 *)data321,tmlen, (wf_u32 *)out_buf, tmlen+1);
        if (WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s] UMSG_OPS_CMD_TEST failed", __func__);
            data32 = wf_io_read32(pnic_info, 0x9008, NULL);
            LOG_D("9008:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x9010, NULL);
            LOG_D("9010:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x904c, NULL);
            LOG_D("904c:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x60e0, NULL);
            LOG_D("0x60e0:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x60e4, NULL);
            LOG_D("0x60e4:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x60e8, NULL);
            LOG_D("0x60e8:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x60ec, NULL);
            LOG_D("0x60ec:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x60d4, NULL);
            LOG_D("0x60d4:%x",data32);
            data32 = wf_io_read32(pnic_info, 0x6024, NULL);
            LOG_D("0x6024:%x",data32);
            return ret;
        }
        //log_array(out_buf, (tmlen+1)*4);
        if(wf_memcmp(data321,out_buf,tmlen) == 0)
        {
            //LOG_D("OK");
            ok_times++;
        }
        else
        {
            LOG_D("FAIL");
            failed_times++;
        }
        //wf_msleep(500);
    }
    LOG_D("cmddl:%d ok:%d fail:%d",(ok_times+failed_times),ok_times,failed_times);
    sprintf(extra, "cmddl:%d ok:%d fail:%d",(ok_times+failed_times),ok_times,failed_times);
    return 0;

}
int wf_iw_txagg_timestart(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hif_node_st *hif_info   = pnic_info->hif_node;
    char *pch;
    char cmd[128];
    char *param1[3] = {0x00,0x00,0x00};
    char *token;
    int i = 0;
    struct iw_point *wrqu;

    wrqu = (struct iw_point *)wdata;
    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        LOG_D("copy_from_user fail");
        return WF_RETURN_FAIL;
    }
    sscanf(extra, "%s", cmd);
    pch = cmd;
    while ((token = strsep(&pch, ",")) != NULL)
    {
        if (i > 2)
            break;
        param1[i] = token;
        i++;
    }

    if(0 == strcmp(param1[0],"help"))
    {
        LOG_D("cmd :");
        return 0;
    }
    if(0 == strcmp(param1[0],"switch"))
    {
        LOG_D("txagg= %s",extra);
        hif_info->u.sdio.count_start = atoi(param1[1]);
        LOG_D("count_start= %d",hif_info->u.sdio.count_start);
        if(0 == hif_info->u.sdio.count_start)
        {
            hif_info->u.sdio.tx_agg_send_time = 0;
            hif_info->u.sdio.tx_flow_ctl_time = 0;
            hif_info->u.sdio.tx_all_time = 0;
            hif_info->u.sdio.count = 0;
            hif_info->u.sdio.tx_agg_num = 0;
            hif_info->u.sdio.tx_pkt_num = 0;
            hif_info->u.sdio.rx_time    = 0;
            hif_info->u.sdio.rx_count = 0;
            hif_info->u.sdio.rx_pkt_num = 0;
        }
    }
    else if( 0 == strcmp(param1[0],"config"))
    {
        LOG_D("txagg= %s",extra);
        pnic_info->agg_size = atoi(param1[1]);
        pnic_info->agg_time = atoi(param1[2]);
        LOG_D("agg_size= %x, agg_time=%x",pnic_info->agg_size,pnic_info->agg_time);
        wf_rx_config_agg(pnic_info);
    }


    return 0;
}


int wf_iw_priv_tx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *p;
    wf_u8 *tmp_buf;
    wf_u32 len;
    char cmd[512];
    char *test;
    char *param1[3] = {0x00,0x00,0x00};
    wf_u32 times,loopback;
    int i = 0;
    char *token;
    int param_times;
#ifdef IW_PRV_DEBUG
    LOG_D("[WLAN_IW_PRIV] : %s", __func__);
#endif
    p = &wrqu->data;
    len = p->length;
    if (0 == len)
        return -1;

    tmp_buf = wf_kzalloc(len+1);
    if(NULL == tmp_buf)
    {
        return -1;
    }
    if (copy_from_user(tmp_buf, p->pointer, len))
    {
        return -1;
    }

    sscanf(tmp_buf, "%s", cmd);
    test = cmd;
    while ((token = strsep(&test, ",")) != NULL)
    {
        LOG_D("token = %s", token);
        if (i > 2)
            break;
        param1[i] = token;
        i++;
    }

    if(strcmp(param1[0],"help") == 0)
    {
        LOG_D("\n\n\n");
        LOG_D("--------------------------------------------------------------------");
        LOG_D("-                 iwpriv tx help function                 -");
        LOG_D("--------------------------------------------------------------------");
        LOG_D("iwpriv wlan(X) tx mac                :mac loopback");
        LOG_D("iwpriv wlan(X) tx phy                :phy loopback");
        LOG_D("iwpriv wlan(X) tx data,(n)           :send data frame n times");
        LOG_D("iwpriv wlan(X) tx mgmt,(n)           :send mgmt frame n times");

        wf_kfree(tmp_buf);
        return 0;

    }

    if(strcmp(param1[0],"data") == 0)
    {
        times = atoi(param1[1]);
        param_times = times;
        for(; times > 0; times--)
        {
            wf_iw_priv_data_xmit(pnic_info);
            wf_msleep(2);
        }
        LOG_D("send null data frame times:%d",param_times);
        wf_kfree(tmp_buf);
        return 0;
    }
    if(strcmp(param1[0],"mgmt") == 0)
    {
        times = atoi(param1[1]);
        param_times = times;
        for(; times > 0; times--)
        {
            wf_scan_probe_send(pnic_info);
            wf_msleep(2);
        }
        LOG_D("send null probe request frame times:%d",param_times);
        wf_kfree(tmp_buf);
        return 0;
    }
    if(strcmp(param1[0],"mac") == 0)
    {
        loopback = wf_io_read32(pnic_info, 0x100,NULL);
        LOG_D("loopback:%x",loopback);
        loopback |= (BIT(24) | BIT(25) | BIT(27));
        LOG_D("loopback:%x",loopback);
        wf_io_write32(pnic_info, 0x100, loopback);
        wf_kfree(tmp_buf);
        return 0;
    }
    if(strcmp(param1[0],"phy") == 0)
    {
        loopback = wf_io_read32(pnic_info, 0xd00,NULL);
        LOG_D("loopback:%x",loopback);
        loopback |= 0x1000000;
        LOG_D("loopback:%x",loopback);
        wf_io_write32(pnic_info, 0xd00, loopback);
        wf_kfree(tmp_buf);
        return 0;
    }
    LOG_E("param error");
    wf_kfree(tmp_buf);
    return 0;
}

int wf_iw_priv_status(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    rx_info_t *prx_info;
    struct iw_point *p;
    wf_u8 *tmp_buf;
    wf_u32 len;
    char cmd[512];
    char *test;
    char *param1[3] = {0x00,0x00,0x00};
    wf_u32 time, i = 0;
    wf_u64 rx_pkt;
    char *token;
#ifdef IW_PRV_DEBUG
    LOG_D("[WLAN_IW_PRIV] : %s", __func__);
#endif
    prx_info = pnic_info->rx_info;
    p = &wrqu->data;
    len = p->length;
    if (0 == len)
        return -1;

    tmp_buf = wf_kzalloc(len+1);
    if(NULL == tmp_buf)
    {
        return -1;
    }
    if (copy_from_user(tmp_buf, p->pointer, len))
    {
        return -1;
    }

    sscanf(tmp_buf, "%s", cmd);
    test = cmd;
    while ((token = strsep(&test, ",")) != NULL)
    {
        if (i > 2)
            break;
        param1[i] = token;
        i++;
    }

    if(strcmp(param1[0],"help") == 0)
    {
        LOG_D("cmd :");
        LOG_D("iwpriv wlan0 tx cmd1,cmd2");
        LOG_D("cmd1:help/phy/rx");
        LOG_D("cmd2:phy interval time");
        wf_kfree(tmp_buf);
        return 0;
    }
    if(strcmp(param1[0],"rx") == 0)
    {
        if(param1[1] == NULL)
        {
            LOG_E("time not get");
            wf_kfree(tmp_buf);
            return 0;
        }
        time = atoi(param1[1]);
        LOG_D("time:%d",time);
        rx_pkt = prx_info->rx_total_pkts;
        wf_msleep(time*1000);
        LOG_D("time interval:%d s rx_total_pkts:%lld",time,(prx_info->rx_total_pkts - rx_pkt));

        wf_kfree(tmp_buf);
        return 0;
    }
    if(strcmp(param1[0],"phy") == 0)
    {
        time = atoi(param1[1]);
        if(param1[1] == NULL)
        {
            LOG_E("time not get");
            wf_kfree(tmp_buf);
            return 0;
        }
        wf_iw_priv_phy_reg(pnic_info,time);
        wf_kfree(tmp_buf);
        return 0;
    }
    LOG_E("param error");
    wf_kfree(tmp_buf);
    return 0;
}


int wf_iw_priv_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *ndev_priv = netdev_priv(dev);

    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);

#if 1
    wf_hw_info_set_channnel_bw(ndev_priv->nic, 1, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    wf_msleep(300);
    wf_scan_probe_send(ndev_priv->nic);
#endif

    return 0;
}

int wf_iw_priv_phy_reg(nic_info_st *nic_info, wf_u32 time)
{
    int ret = 0;
    int cnt_parity_fail1;//0x0da0
    int cnt_rate_fail1,cnt_crc8_fail1;//0x0da4
    int cnt_mcs_fail1;//0x0da8
    int cnt_fast_fsync1,cnt_sb_search_fail1;//0x0cf0
    int cnt_ofdm_fail1;

    int cnt_ofdm_ht_crc_ok1,cnt_ofdm_ht_crc_fail1;//0x0f90
    int cnt_ofdm_crc_ok1,cnt_ofdm_crc_fail1;//0x0f94
    int cnt_ofdm_crc_fail_all1;
    int cnt_ofdm_crc_ok_all1;

    int cnt_cck_crc_fail1;
    int cnt_cck_crc_ok1;

    int cnt_crc_fail_all1,cnt_crc_ok_all1;

    int cnt_cck_fail1;

    int ofdm_fail_probability1;
    int cck_fail_probability1;
    int cnt_parity_fail2;//0x0da0
    int cnt_rate_fail2,cnt_crc8_fail2;//0x0da4
    int cnt_mcs_fail2;//0x0da8
    int cnt_fast_fsync2,cnt_sb_search_fail2;//0x0cf0
    int cnt_ofdm_fail2;

    int cnt_ofdm_ht_crc_ok2,cnt_ofdm_ht_crc_fail2;//0x0f90
    int cnt_ofdm_crc_ok2,cnt_ofdm_crc_fail2;//0x0f94
    int cnt_ofdm_crc_fail_all2;
    int cnt_ofdm_crc_ok_all2;

    int cnt_cck_crc_fail2;
    int cnt_cck_crc_ok2;

    int cnt_crc_fail_all2,cnt_crc_ok_all2;

    int cnt_cck_fail2;

    int ofdm_fail_probability2;
    int cck_fail_probability2;

    int cnt_ofdm_fail_interval;
    int cnt_ofdm_crc_fail_all;
    int cnt_ofdm_crc_ok_all;

    int cnt_cck_crc_fail_all;
    int cnt_cck_crc_ok_all;
    int cnt_cck_fail;
    ret = wf_io_read32(nic_info,0x09cc,NULL);
    ret = wf_io_read32(nic_info,0x09d0,NULL);
    ret = wf_io_read32(nic_info,0x0da0,NULL);
    cnt_parity_fail1 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0da4,NULL);
    cnt_rate_fail1 = ret & 0xffff;
    cnt_crc8_fail1 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0da8,NULL);
    cnt_mcs_fail1 = ret & 0xffff;
    ret = wf_io_read32(nic_info,0x0cf0,NULL);
    cnt_fast_fsync1 = ret & 0xffff;
    cnt_sb_search_fail1 = (ret & 0xffff0000) >> 16;
    cnt_ofdm_fail1 = cnt_parity_fail1 + cnt_rate_fail1 + cnt_crc8_fail1 + cnt_mcs_fail1 + cnt_fast_fsync1 + cnt_sb_search_fail1;
    ret = wf_io_read32(nic_info,0x0f90,NULL);
    cnt_ofdm_ht_crc_ok1 = ret & 0xffff;
    cnt_ofdm_ht_crc_fail1 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0f94,NULL);
    cnt_ofdm_crc_ok1 = ret & 0xffff;
    cnt_ofdm_crc_fail1 = (ret & 0xffff0000) >> 16;
    cnt_ofdm_crc_fail_all1 = cnt_ofdm_crc_fail1 + cnt_ofdm_ht_crc_fail1;
    cnt_ofdm_crc_ok_all1 = cnt_ofdm_crc_ok1 + cnt_ofdm_ht_crc_ok1;
    ofdm_fail_probability1 = ((cnt_ofdm_crc_fail_all1*100)/(cnt_ofdm_crc_fail_all1 + cnt_ofdm_crc_ok_all1));
    ret = wf_io_read32(nic_info,0x0a5c,NULL);
    cnt_cck_fail1 = ret & 0xff;
    ret = wf_io_read32(nic_info,0x0a58,NULL);
    cnt_cck_fail1 += ((ret & 0xff000000) >> 16);
    ret = wf_io_read32(nic_info,0x0f84,NULL);
    cnt_cck_crc_fail1 = ret;
    ret = wf_io_read32(nic_info,0x0f88,NULL);
    cnt_cck_crc_ok1 = ret;
    cnt_crc_fail_all1 = cnt_cck_crc_fail1 + cnt_ofdm_crc_fail_all1;
    cnt_crc_ok_all1 = cnt_cck_crc_ok1 + cnt_ofdm_crc_ok_all1;
    cck_fail_probability1 = ((cnt_crc_fail_all1*100)/(cnt_crc_fail_all1 + cnt_crc_ok_all1));
    wf_msleep(time*1000);
    ret = wf_io_read32(nic_info,0x09cc,NULL);
    ret = wf_io_read32(nic_info,0x09d0,NULL);
    ret = wf_io_read32(nic_info,0x0da0,NULL);
    cnt_parity_fail2 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0da4,NULL);
    cnt_rate_fail2 = ret & 0xffff;
    cnt_crc8_fail2 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0da8,NULL);
    cnt_mcs_fail2 = ret & 0xffff;
    ret = wf_io_read32(nic_info,0x0cf0,NULL);
    cnt_fast_fsync2 = ret & 0xffff;
    cnt_sb_search_fail2 = (ret & 0xffff0000) >> 16;
    cnt_ofdm_fail2 = cnt_parity_fail2 + cnt_rate_fail2 + cnt_crc8_fail2 + cnt_mcs_fail2 + cnt_fast_fsync2 + cnt_sb_search_fail2;
    LOG_D("-----------------test------------------");
    LOG_D("cnt_parity_fail2:%d",(cnt_parity_fail2-cnt_parity_fail1));
    LOG_D("cnt_rate_fail2:%d",(cnt_rate_fail2-cnt_rate_fail1));
    LOG_D("cnt_crc8_fail2:%d",(cnt_crc8_fail2-cnt_crc8_fail1));
    LOG_D("cnt_mcs_fail2:%d",(cnt_mcs_fail2-cnt_mcs_fail1));
    LOG_D("cnt_fast_fsync2:%d",(cnt_fast_fsync2-cnt_fast_fsync1));
    LOG_D("cnt_sb_search_fail2:%d",(cnt_sb_search_fail2-cnt_sb_search_fail1));
    LOG_D("-----------------test------------------");
    ret = wf_io_read32(nic_info,0x0f90,NULL);
    cnt_ofdm_ht_crc_ok2 = ret & 0xffff;
    cnt_ofdm_ht_crc_fail2 = (ret & 0xffff0000) >> 16;
    ret = wf_io_read32(nic_info,0x0f94,NULL);
    cnt_ofdm_crc_ok2 = ret & 0xffff;
    cnt_ofdm_crc_fail2 = (ret & 0xffff0000) >> 16;
    cnt_ofdm_crc_fail_all2 = cnt_ofdm_crc_fail2 + cnt_ofdm_ht_crc_fail2;
    cnt_ofdm_crc_ok_all2 = cnt_ofdm_crc_ok2 + cnt_ofdm_ht_crc_ok2;
    ofdm_fail_probability2 = ((cnt_ofdm_crc_fail_all2*100)/(cnt_ofdm_crc_fail_all2 + cnt_ofdm_crc_ok_all2));
    ret = wf_io_read32(nic_info,0x0a5c,NULL);
    cnt_cck_fail2 = ret & 0xff;
    ret = wf_io_read32(nic_info,0x0a58,NULL);
    cnt_cck_fail2 += ((ret & 0xff000000) >> 16);
    ret = wf_io_read32(nic_info,0x0f84,NULL);
    cnt_cck_crc_fail2 = ret;
    ret = wf_io_read32(nic_info,0x0f88,NULL);
    cnt_cck_crc_ok2 = ret;
    cnt_crc_fail_all2 = cnt_cck_crc_fail2 + cnt_ofdm_crc_fail_all2;
    cnt_crc_ok_all2 = cnt_cck_crc_ok2 + cnt_ofdm_crc_ok_all2;
    cck_fail_probability2 = ((cnt_crc_fail_all2*100)/(cnt_crc_fail_all2 + cnt_crc_ok_all2));
    cnt_ofdm_fail_interval = cnt_ofdm_fail2 - cnt_ofdm_fail1;
    if(cnt_ofdm_fail_interval < 0)
    {
        cnt_ofdm_fail_interval = cnt_ofdm_fail1 - cnt_ofdm_fail2;
    }

    cnt_ofdm_crc_fail_all = cnt_ofdm_crc_fail_all2 - cnt_ofdm_crc_fail_all1;
    if(cnt_ofdm_crc_fail_all < 0)
    {
        cnt_ofdm_crc_fail_all = cnt_ofdm_crc_fail_all1 - cnt_ofdm_crc_fail_all2;
    }
    cnt_ofdm_crc_ok_all = cnt_ofdm_crc_ok_all2 - cnt_ofdm_crc_ok_all1;
    if(cnt_ofdm_crc_ok_all < 0)
    {
        cnt_ofdm_crc_ok_all = cnt_ofdm_crc_ok_all1 - cnt_ofdm_crc_ok_all2;
    }
    cnt_cck_crc_ok_all = cnt_cck_crc_ok2 - cnt_cck_crc_ok1;
    if(cnt_cck_crc_ok_all < 0)
    {
        cnt_cck_crc_ok_all = cnt_cck_crc_ok1 - cnt_cck_crc_ok2;
    }
    cnt_cck_crc_fail_all = cnt_cck_crc_fail2 - cnt_cck_crc_fail1;
    if(cnt_cck_crc_fail_all < 0)
    {
        cnt_cck_crc_fail_all = cnt_cck_crc_fail1 - cnt_cck_crc_fail2;
    }
    cnt_cck_fail = cnt_cck_fail2 - cnt_cck_fail1;
    if(cnt_cck_fail < 0)
    {
        cnt_cck_fail = cnt_cck_fail1 - cnt_cck_fail2;
    }
    LOG_D("ofdm fail interval %d s : %d",time,cnt_ofdm_fail_interval);
    LOG_D("ofdm crc fail interval %d s : %d",time,cnt_ofdm_crc_fail_all);
    LOG_D("ofdm crc ok interval %d s : %d",time,cnt_ofdm_crc_ok_all);

    LOG_D("cck fail interval %d s : %d",time,cnt_cck_fail);
    LOG_D("cck crc fail interval %d s : %d",time,cnt_cck_crc_fail_all);
    LOG_D("cck crc ok interval %d s : %d",time,cnt_cck_crc_ok_all);
    LOG_D("\n");
    LOG_D("\n");
    wf_iw_priv_mac_reg(nic_info, time);

    return 0;
}


int wf_iw_priv_mac_reg(nic_info_st *nic_info, int time)
{
    int rx_mac_num1,rx_mac_num2,rx_mac_num;
    int ret ;
    int val = 65535;
    ret = wf_io_read32(nic_info,0x04ec,NULL);
    rx_mac_num1 = ret & 0xffff;
    wf_msleep(time*1000);
    ret = wf_io_read32(nic_info,0x04ec,NULL);
    rx_mac_num2 = ret & 0xffff;
    rx_mac_num = rx_mac_num2 - rx_mac_num1;
    if(rx_mac_num < 0)
    {
        rx_mac_num = (rx_mac_num2 + val) - rx_mac_num1;
    }

    LOG_D("mac reg[0x04ec] interval %d s val:%d",time,rx_mac_num);
    return 0;
}


int wf_iw_priv_fw_dowlond_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *ndev_priv = netdev_priv(dev);
    int number = (int)*extra;
    int ret;
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    for(; number > 0; number--)
    {
        ret = wf_fw_download(ndev_priv->nic);
        if(ret != 0)
        {
            LOG_D("wf_fw_download fail!!!!");
            return -1;
        }
    }

    return 0;
}


int wf_iw_priv_send_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *ndev_priv = netdev_priv(dev);
    int cmd = (int)*extra;
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);

    switch(cmd)
    {
        case 0:
            LOG_D("tx mgmt frame");
            wf_scan_probe_send(ndev_priv->nic);
            break;
        case 1:
            LOG_D("tx data frame");
            wf_iw_priv_data_xmit(ndev_priv->nic);
            break;
        default:
            LOG_D("type error");
            break;
    }
    return 0;
}

int wf_iw_priv_write32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_read32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hif_node_st *hif_info  = pnic_info->hif_node;
    struct iw_point *p = NULL;
    wf_u8  *tmp_buf       = NULL;
    u16 len     = 0;
    int bytes    = 0;
    wf_u32 addr    = 0;
    wf_u32 data    = 0;

    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    p = &wrqu->data;
    len = p->length;

    if(0 == len)
    {
        return -EINVAL;
    }

    tmp_buf = wf_kzalloc(len+1);
    if(NULL == tmp_buf)
    {
        return -ENOMEM;
    }

    if(copy_from_user(tmp_buf,p->pointer,len))
    {
        wf_kfree(tmp_buf);
        return -EFAULT;
    }

    if(2 != sscanf(tmp_buf,"%d,%x",&bytes,&addr))
    {
        LOG_E("2 != sscanf");
        wf_kfree(tmp_buf);
        return -EFAULT;
    }

    switch (bytes)
    {
        case 1:
            data = hif_io_read8(hif_info, addr,NULL);
            sprintf(extra, "0x%x-----0x%02X", addr,data);
            break;
        case 2:
            data = hif_io_read16(hif_info, addr,NULL);
            sprintf(extra, "0x%x-----0x%04X", addr,data);
            break;
        case 4:
            LOG_W("wf_io_read32");
            data = hif_io_read32(hif_info, addr,NULL);
            sprintf(extra, "0x%x-----0x%08X", addr,data);
            break;

        default:
            LOG_I("%s: usage> read [bytes],[address(hex)]\n", __func__);
            break;
    }

    wf_kfree(tmp_buf);

    return 0;
}

int wf_iw_priv_set_rf(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_get_rf(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_set_p2p(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_get_p2p(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_get_ap_Info(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_get_sensitivity(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_set_pm(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}

int wf_iw_priv_set_pid(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    LOG_D("[WLAN_IW_PRIV] : %s\n", __func__);
    return 0;
}


inline wf_u32 wf_get_freq(nic_info_st * nic_info)
{
    local_info_st * local = (local_info_st *)nic_info->local_info;

    return (wf_u32)local->bw;
}



void null_data_wlan_hdr(nic_info_st * pnic_info, struct xmit_buf * pxmit_buf)
{
    wf_u8 * pframe;
    struct wl_ieee80211_hdr * pwlanhdr;

    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pwlanhdr->frame_ctl = 0;
    SetFrameType(pframe,WIFI_DATA_TYPE);
    SetFrameSubType(pframe, WIFI_DATA_NULL);  /* set subtype */
}


/*
tx data test frame
*/
int wf_iw_priv_data_xmit(nic_info_st * pnic_info)
{
    wf_u8 * pframe;
    wf_u8 * phead;
    struct wl_ieee80211_hdr * pwlanhdr;
    struct xmit_buf * pxmit_buf;
    wf_u16 pkt_len;
    tx_info_st *     ptx_info;
    int ret = 0;
    wf_pkt *pkt;
    wf_u8 bmc_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    wf_wlan_mgmt_info_t *wlan_mgmt_info = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    wf_u8 icmp[] = {0x14,0xe6,0xe4,0x38,0x6f,0x3c,0xb4,0x4,0x18,0xc6,0x75,0xf7,0x8,0x0,0x45,0x0,0x0,0x54,0xb8
                    ,0xcf,0x40,0x0,0x40,0x1,0xfe,0x13,0xc0,0xa8,0x1,0x74,0xc0,0xa8,0x1,0x1,0x8,0x0,0x4d,0xaf,0x26
                    ,0x27,0x0,0x10,0xf3,0xcf,0xf,0x60,0x0,0x0,0x0,0x0,0xbc,0x16,0x6,0x0,0x0,0x0,0x0,0x0,0x10,0x11,0x12,0x13
                    ,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27
                    ,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
                   };
    ptx_info =  (tx_info_st *)pnic_info->tx_info;
    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        LOG_E("[%s]: pxmit_buf is NULL",__func__);
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET);


    null_data_wlan_hdr(pnic_info, pxmit_buf);

    /* set txd at tx module */
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET; /* pframe point to wlan_hdr */
    phead = pframe;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len; /* point to iv or frame body */

    wf_memset(pwlanhdr->addr1, 0xff, MAC_ADDR_LEN);
    wf_memset(pwlanhdr->addr3, 0xff, MAC_ADDR_LEN);

    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);

    wf_memcpy(wlan_mgmt_info->cur_network.bssid,bmc_addr,MAC_ADDR_LEN);
#ifdef SCAN_DEBUG
    LOG_D("wlanhdr:addr1="WF_MAC_FMT,WF_MAC_ARG(pwlanhdr->addr1));
    LOG_D("wlanhdr:addr2="WF_MAC_FMT,WF_MAC_ARG(pwlanhdr->addr2));
    LOG_D("wlanhdr:addr3="WF_MAC_FMT,WF_MAC_ARG(pwlanhdr->addr3));
#endif
    pxmit_buf->pkt_len = pkt_len;

    pkt = wf_kzalloc(sizeof(wf_pkt));

    LOG_D("-----------------------------------------------------------");

    pkt->data = icmp;
    pkt->len = sizeof(icmp);
    if(wf_wdn_find_info(pnic_info,bmc_addr) == NULL)
    {
        wf_wdn_add(pnic_info,bmc_addr);
    }

    LOG_D("%s ret=%d",__func__,ret);


    if(pxmit_buf != NULL)
    {
        wf_xmit_extbuf_delete(ptx_info,pxmit_buf);
    }
    if(0 != ret)
    {
        LOG_E("[%s] failed, ret=%d",__func__,ret);
    }
    return 0;
}


void wf_iw_deauth_frame_wlan_hdr (nic_info_st *pnic_info, struct xmit_buf *pxmit_buf)
{
    wf_u8 *pframe;
    struct wl_ieee80211_hdr *pwlanhdr;
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pwlanhdr->frame_ctl = 0;
    SetFrameType(pframe, WIFI_MGT_TYPE);
    SetFrameSubType(pframe, WIFI_DEAUTH);  /* set subtype */
}

int wf_iw_deauth_ap (nic_info_st *pnic_info, wf_u16 reason)
{
    wf_u8 *pframe;
    struct wl_ieee80211_hdr *pwlanhdr;
    struct xmit_buf *pxmit_buf;
    wf_u16 pkt_len = 0;
    tx_info_st *ptx_info;
    wdn_net_info_st *wdn_info;
    mlme_info_t *mlme_info;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    wdn_info = wf_wdn_find_info(pnic_info, pcur_network->mac_addr);
    if (wdn_info == NULL)
    {
        return -1;
    }

    ptx_info    = (tx_info_st *)pnic_info->tx_info;
    mlme_info   = (mlme_info_t *)pnic_info->mlme_info;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        LOG_E("pxmit_buf is NULL");
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

    /* type of management is 0100 */
    wf_iw_deauth_frame_wlan_hdr(pnic_info, pxmit_buf);

    /* set txd at tx module */
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET; /* pframe point to wlan_hdr */
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    /* copy addr1/2/3 */
    wf_memcpy(pwlanhdr->addr1, pcur_network->mac_addr, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, wdn_info->bssid, MAC_ADDR_LEN);

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len; /* point to iv or frame body */

    reason = wf_cpu_to_le16(reason);
    pframe = set_fixed_ie(pframe, 2, (wf_u8 *)&reason, &pkt_len);
    wf_nic_mgmt_frame_xmit(pnic_info, wdn_info, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

int wf_iw_fw_debug(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_s32 ret = 0;
    wf_u32 inbuff[2] = {0xffffffff,0xffffffff};

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_DBGLOG_CONFIG, inbuff, 2, NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }
    else if(WF_RETURN_CMD_BUSY == ret)
    {
        LOG_W("[%s] cmd busy,try again if need!",__func__);
        return ret;
    }

    return 0;

    return 0;

}
