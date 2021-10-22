
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "wf_debug.h"
#include "iw_priv_func.h"
#include "mp.h"


#ifdef CONFIG_MP_MODE
#if defined(CONFIG_WEXT_PRIV)
extern wf_bool mp_test_enable;


#define MP_DBG(fmt, ...)        LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ARRAY(data, len)     log_array(data, len)
#define MP_INFO(fmt, ...)       LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_WARN(fmt, ...)       LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ERROR(fmt, ...)      LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)


static wf_u8 mp_key_of_char2num_func(wf_u8 ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else
        return 0xff;
}

static wf_u8 mp_key_of_2char2num_func(wf_u8 hch, wf_u8 lch)
{
    return ((mp_key_of_char2num_func(hch) << 4) | mp_key_of_char2num_func(lch));
}


static int  wf_get_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len )
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
        ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_HAL_EFUSEMAP,pread,MAILBOX_MAX_RDLEN, offset);
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

        ret = mcu_cmd_communicate_special(nic_info,UMSG_OPS_HAL_EFUSEMAP,pread,remainder,offset);
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

static int mp_num0fstr(char *Mstr, char *substr)
{
    int number = 0;

    char *p;
    char *q;

    while (*Mstr != '\0')
    {
        p = Mstr;
        q = substr;

        while ((*p == *q) && (*p != '\0') && (*q != '\0'))
        {
            p++;
            q++;
        }
        if (*q == '\0')
        {
            number++;
        }
        Mstr++;
    }
    return number;
}

int wf_mp_logic_efuse_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_mp_info_st *mp_info = pnic_info->mp_info;
    struct iw_point *wrqu;
    int ret = 0,i,cnts = 0;
    char *pch, *ptmp, *token, *tmp[3] = { 0x00, 0x00, 0x00 };
    wf_u8 *outbuff = NULL;
    wf_u32 efuse_len;
    static wf_u8 order = 0;
    wf_u8 *efuse;
    wf_u32 shift, cnt;
    wf_u32 blksz = 0x200; /* The size of one time show, default 512 */
    wf_u32 j=0;
    wf_u32 inbuff[2];
    MP_DBG();

    wrqu = (struct iw_point *)wdata;
    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        MP_DBG("copy_from_user fail");
        return -EFAULT;
    }
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }
    pch = extra;

    i = 0;
    MP_DBG(": in=%s\n", extra);
    ptmp = strsep(&pch,"=");
    while ((token = strsep(&pch, ",")) != NULL)
    {
        if (i > 2)
            break;
        tmp[i] = token;
        i++;
    }

    if (strcmp(ptmp, "realmap") == 0)
    {

        /*get efuse len*/
        efuse_len = 512;
        LOG_I("[%s] efuse len: %d",__func__, efuse_len);

        if(mp_info->efuse_data_map == NULL)
        {
            LOG_E("Please iwpriv wlanx set test=start");
            return -1;
        }
        /* get efuse map */
        ret = wf_get_efuse_data(pnic_info, mp_info->efuse_data_map, efuse_len);
        efuse = mp_info->efuse_data_map;

//        *extra = 0;
        shift = blksz * order;
        efuse += shift;
        cnt = efuse_len - shift;
        if (cnt > blksz)
        {
            cnt = blksz;
            order++;
        }
        else
            order = 0;

        sprintf(extra, "\n");
        for (i = 0; i < cnt; i += 16)
        {
            sprintf(extra, "%s0x%02x\t", extra, shift + i);
            for (j = 0; j < 8; j++)
                sprintf(extra, "%s%02X ", extra, efuse[i + j]);
            sprintf(extra, "%s\t", extra);
            for (; j < 16; j++)
                sprintf(extra, "%s%02X ", extra, efuse[i + j]);
            sprintf(extra, "%s\n", extra);
        }
        if ((shift + cnt) < efuse_len)
            sprintf(extra, "%s\t...more (left:%d/%d)\n", extra, efuse_len-(shift + cnt), efuse_len);

        wrqu->length = strlen(extra);
        LOG_D("wrqu->length:%d",wrqu->length);

        return 0;
    }
    else if(strcmp(ptmp, "rmap") == 0)
    {
//        inbuff[0] = strtol(tmp[1]);
//        inbuff[1] = atoi(tmp[1]);
        sscanf(tmp[0],"%x",&inbuff[0]);
        sscanf(tmp[1],"%d",&inbuff[1]);

        outbuff = wf_kzalloc(MAILBOX_MAX_TXLEN * 4);
        if (outbuff == NULL)
        {
            MP_DBG("alloc recv buff fail");
            return  WF_RETURN_FAIL;
        }


        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_READ, inbuff, 1, (wf_u32 *) outbuff, inbuff[1]);


        if (WF_RETURN_FAIL == ret)
        {
            MP_WARN("[%s] read reg failed,check!!!", __func__);
            if (outbuff)
                wf_kfree(outbuff);
            return WF_RETURN_FAIL;
        }
        MP_DBG("inbuff[0] = 0x%x, cnts = %d",inbuff[0], outbuff[0]);

        *extra = 0;
        for (i = 0; i < inbuff[1]; i++)
        {
            sprintf(extra, "%s 0x%02X", extra, outbuff[i]);
            if (i != (cnts - 1))
            {
                sprintf(extra, "%s", extra);
            }
            LOG_D("%x ",outbuff[i]);
        }
        wrqu->length = strlen(extra);
        if (outbuff)
            wf_kfree(outbuff);
        return WF_RETURN_OK;
    }

    if (outbuff)
        wf_kfree(outbuff);
    return WF_RETURN_OK;

}


int wf_mp_logic_efuse_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    int set_len;
    wf_u32 i = 0;
    char *pch, *ptmp, *token, *tmp[2] = { 0x00, 0x00 };
    char *set_data;
    char cmd[8][20];
    char data[8][80];
    char str_data[100] = { 0 };
    wf_u32 addr = 0xFF;
    wf_u32 inbuff[32] = {0};
    char buf_test[16] = {0};
    wf_u32  outbuff = 0;
    int cmd_len;
    int ret = 0;
    int num = 0;
    int bit = 0;

    tmp[0] = str_data;

    memset(cmd, 0, sizeof(cmd));
    memset(data, 0, sizeof(data));
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

    set_len = mp_num0fstr(extra, "=");
    cmd_len = set_len;

    if (set_len > 7)
    {
        MP_WARN("set_len error");
        return WF_RETURN_FAIL;
    }

    token = strsep(&pch, ",");
    ptmp = strsep(&token, "=");
    sprintf(cmd[0], "%s", ptmp);
    sprintf(data[0], "%s", token);
//        MP_DBG("cmd = %s", cmd[0]);
//        MP_DBG("data = %s", data[0]);
//        MP_DBG("pch = %s", pch);
    if (strcmp(cmd[0], "wmap") == 0)
    {

        sscanf(data[0],"%x",&addr);
//            MP_DBG("test2 = 0x%x", addr);
        set_data = pch;
        inbuff[0] = addr;
        inbuff[1] = strlen(pch)/2;
        if(strlen(pch)%8 == 0)
        {
            num = strlen(pch)/8;
        }
        else
        {
            num = strlen(pch)/8 + 1;
        }
        for(i=0; i<num; i++)
        {
            MP_DBG("%s",set_data);
            sscanf(set_data,"%8s",buf_test);
            MP_DBG("data:0x%s ",buf_test);
            sscanf(buf_test,"%8x",&inbuff[i+2]);
            MP_DBG("inbuff:0x%x ",inbuff[i+2]);
            inbuff[i+2] = wf_cpu_to_be32(inbuff[i+2]);
            MP_DBG("data:0x%x ",inbuff[i+2]);
            if(num - i != 1)
                set_data=set_data+8;
        }
        bit = (((strlen(pch))/2)%4);
        if(bit != 0)
        {
            MP_DBG("inbuff[i+2]:%8x",inbuff[i+1]);
            inbuff[i+1] = inbuff[i+1] >> (4 - bit) * 8;
            MP_DBG("inbuff[i+2]:%8x",inbuff[i+1]);
        }
        MP_DBG("bit:%d",bit);
        MP_DBG("inbuff[0]:%x",inbuff[0]);
        MP_DBG("inbuff[1]:%x",inbuff[1]);
        MP_DBG("inbuff[2]:%x",inbuff[2]);
        MP_DBG("inbuff[3]:%x",inbuff[3]);
        MP_DBG("inbuff[4]:%x",inbuff[4]);
        MP_DBG("inbuff[5]:%x",inbuff[5]);
        MP_DBG("inbuff[6]:%x",inbuff[6]);


    }
    else
    {
        sprintf(extra, "Command not found!");
        inbuff[0] = -1;
        return WF_RETURN_FAIL;
    }


    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_WRITE, (wf_u32 *) inbuff, i+2, &outbuff, 1);


    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");

        return WF_RETURN_FAIL;
    }
    if (outbuff == 0)
    {
        sprintf(extra, "%s" "%s", extra, " fail");
    }
    else
    {
        sprintf(extra, "%s" "%s", extra, " ok");
    }
    wrqu->length = strlen(extra);

    return WF_RETURN_OK;

}


int wf_mp_efuse_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    int set_len;
    wf_u8 m_len;
    wf_u8 p_len;
    wf_u32 i = 0,jj, kk;
    char *pch, *ptmp, *token, *tmp[2] = { 0x00, 0x00 };
    char cmd[8][20];
    char data[8][80];
    char str_data[100] = { 0 };

    wf_u16  cnts = 0;
    wf_u8 *inbuff = NULL;
    wf_u32  outbuff = 0;
    int cmd_len;
    int ret;
    int test = 0;
    tmp[0] = str_data;

    memset(cmd, 0, sizeof(cmd));
    memset(data, 0, sizeof(data));
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


    inbuff = wf_kzalloc(MAILBOX_MAX_TXLEN * 4);
    if (inbuff == NULL)
    {
        MP_WARN("kmalloc fail");
        return WF_RETURN_FAIL;
    }
    pch = extra;
    MP_DBG("in = %s",extra);

    set_len = mp_num0fstr(extra, "=");
    cmd_len = set_len;

    if (set_len > 7)
    {
        MP_WARN("set_len error");
        if (inbuff)
            wf_kfree(inbuff);
        return WF_RETURN_FAIL;
    }

    while (set_len--)
    {
        token = strsep(&pch, ",");
        ptmp = strsep(&token, "=");
        sprintf(cmd[i], "%s", ptmp);
        sprintf(data[i], "%s", token);
        MP_DBG("cmd = %s", cmd[i]);
        i++;
    }
    if (cmd_len == 1)
    {

        if (strncmp(cmd[0], "mac", 3) == 0)
        {
            inbuff[0] = WLAN_EEPORM_MAC;
        }
        else if (strncmp(cmd[0], "vid", 3) == 0)
        {
            inbuff[0] = EFUSE_VID;
        }
        else if (strncmp(cmd[0], "pid", 3) == 0)
        {
            inbuff[0] = EFUSE_PID;
        }
        else if (strncmp(cmd[0], "manufacture", strlen("manufacture")) == 0)
        {
            inbuff[0] = EFUSE_MANU;
        }
        else if (strncmp(cmd[0], "product", strlen("product")) == 0)
        {
            inbuff[0] = EFUSE_PRODUCT;
        }
        else if (strncmp(cmd[0], "freqcal", strlen("freqcal")) == 0)
        {
            inbuff[0] = EFUSE_FREQCAL;
        }
        else if (strncmp(cmd[0], "tempcal", strlen("tempcal")) == 0)
        {
            inbuff[0] = EFUSE_TEMPCAL;
        }
        else if (strncmp(cmd[0], "channelplan", strlen("channelplan")) == 0)
        {
            inbuff[0] = EFUSE_CHANNELPLAN;
        }
        else if (strncmp(cmd[0], "powercal", strlen("powercal")) == 0)
        {
            inbuff[0] = EFUSE_POWERCAL;
        }
        else if (strncmp(data[0], "id", 2) == 0)
        {
            inbuff[0] = EFUSE_HEADERCHECK;
            cnts = 1;
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_SET, (wf_u32 *) inbuff, (cnts + 6) / 4, &outbuff, 1);
            if(ret == WF_RETURN_FAIL)
            {
                MP_WARN("set reg fail");
                if (inbuff)
                    wf_kfree(inbuff);
                return WF_RETURN_FAIL;
            }
            if (outbuff == 0)
            {
                sprintf(extra, "%s" "%s", extra, " fail");
            }
            else
            {
                sprintf(extra, "%s" "%s", extra, " ok");
            }
            if (inbuff)
            {
                wf_kfree(inbuff);
                return WF_RETURN_OK;
            }
            return WF_RETURN_OK;
        }
        else if (strncmp(data[0], "fix", 3) == 0)
        {
            inbuff[0] = EFUSE_FIXDVALUE;
            cnts = 1;
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_SET, (wf_u32 *) inbuff, (cnts + 6) / 4, &outbuff, 1);
            if(ret == WF_RETURN_FAIL)
            {
                MP_WARN("set reg fail");
                if (inbuff)
                    wf_kfree(inbuff);
                return WF_RETURN_FAIL;
            }
            if (outbuff == 0)
            {
                sprintf(extra, "%s" "%s", extra, " fail");
            }
            else
            {
                sprintf(extra, "%s" "%s", extra, " ok");
            }
            if (inbuff)
            {
                wf_kfree(inbuff);
                return WF_RETURN_OK;
            }
            return WF_RETURN_OK;
        }
        else if (strncmp(data[0], "phy", 3) == 0)
        {
            inbuff[0] = EFUSE_PHYCFGCHECK;
            cnts = 1;
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_SET, (wf_u32 *) inbuff, (cnts + 6) / 4, &outbuff, 1);
            if(ret == WF_RETURN_FAIL)
            {
                MP_WARN("set reg fail");
                if (inbuff)
                    wf_kfree(inbuff);
                return WF_RETURN_FAIL;
            }
            if (outbuff == 0)
            {
                sprintf(extra, "%s" "%s", extra, " fail");
            }
            else
            {
                sprintf(extra, "%s" "%s", extra, " ok");
            }
            if (inbuff)
            {
                wf_kfree(inbuff);
                return WF_RETURN_OK;
            }
            return WF_RETURN_OK;
        }
        else
        {
            sprintf(extra, "Command not found!");
            inbuff[0] = -1;
        }

        if (inbuff[0] < 0 || inbuff[0] > WLAN_EEPORM_CODE_MAX)
        {
            MP_WARN("inbuff error");
            return WF_RETURN_FAIL;
        }
        if (data[0] != 0)
        {
            cnts = strlen(data[0]);
            if (cnts % 2)
            {
                MP_WARN("cnts error");
                return WF_RETURN_FAIL;
            }

            cnts /= 2;
            MP_DBG("%s: cnts=%d", __func__, cnts);
            MP_DBG("%s: data=%s", __func__, data[0]);

            inbuff[1] = cnts;

            for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
            {
                inbuff[jj + 2] = mp_key_of_2char2num_func(data[0][kk], data[0][kk + 1]);
            }
        }
    }
    if (cmd_len > 1)
    {
        if ((strncmp(cmd[0], "freqcal", strlen("freqcal")) == 0)
            && (strncmp(cmd[1], "tempcal", strlen("tempcal")) == 0))
        {
            inbuff[0] = EFUSE_GROUP_0;
            sprintf(tmp[0], "%s" "%s", data[0], data[1]);
            sprintf(extra, "%s", "EFUSE_GROUP_0");
        }
        if (cmd_len == 7)
        {
            if ((strncmp(cmd[0], "channelplan", strlen("channelplan")) == 0)
                && (strncmp(cmd[1], "check", strlen("check")) == 0)
                && (strncmp(cmd[2], "vid", strlen("vid")) == 0)
                && (strncmp(cmd[3], "pid", strlen("pid")) == 0)
                && (strncmp(cmd[4], "mac", strlen("mac")) == 0)
                && (strncmp(cmd[5], "manufacture", strlen("manufacture")) == 0)
                && (strncmp(cmd[6], "product", strlen("product")) == 0)
               )
            {
                inbuff[0] = EFUSE_GROUP_2;
                m_len = strlen(data[5]) + 4;
                p_len = strlen(data[6]) + 4;

                sprintf(tmp[0],
                        "%s" "%s" "%s" "%s" "%02x" "%02x" "%s" "%02x" "%02x"
                        "%s", data[0], data[2], data[3], data[4], m_len / 2, 3,
                        data[5], p_len / 2, 3, data[6]);
                sprintf(extra, "%s", "EFUSE_GROUP_2");
            }
        }
        if (cmd_len == 4)
        {
            if ((strncmp(cmd[0], "channelplan", strlen("channelplan")) == 0)
                && (strncmp(cmd[1], "check", strlen("check")) == 0)
                && (strncmp(cmd[2], "vid", strlen("vid")) == 0)
                && (strncmp(cmd[3], "pid", strlen("pid")) == 0)
               )
            {
                inbuff[0] = EFUSE_GROUP_1;

                sprintf(tmp[0], "%s" "%s" "%s", data[0], data[2], data[3]);
                sprintf(extra, "%s", "EFUSE_GROUP_1");
            }
        }
        if ((strncmp(cmd[0], "vid", strlen("vid")) == 0)
            && (strncmp(cmd[1], "pid", strlen("pid")) == 0))
        {
            inbuff[0] = EFUSE_GROUP_4;

            sprintf(tmp[0], "%s" "%s", data[0], data[1]);
            sprintf(extra, "%s", "EFUSE_GROUP_3");

        }
        if ((strncmp(cmd[0], "manufacture", strlen("manufacture")) == 0)
            && (strncmp(cmd[1], "product", strlen("product")) == 0))
        {
            inbuff[0] = EFUSE_GROUP_3;
            m_len = strlen(data[0]) + 4;
            p_len = strlen(data[1]) + 4;
            sprintf(tmp[0], "%02x" "%02x" "%s" "%02x" "%02x" "%s", m_len / 2, 3,
                    data[0], p_len / 2, 3, data[1]);
            sprintf(extra, "%s", "EFUSE_GROUP_4");
        }

        if (tmp[0] != 0)
        {

            cnts = strlen(tmp[0]);
            if (cnts % 2)
            {
                MP_WARN("cnts error");
                return WF_RETURN_FAIL;
            }

            cnts /= 2;
            MP_DBG("%s: addr=0x%X\n", __func__, addr);
            MP_DBG("%s: cnts=%d\n", __func__, cnts);
            MP_DBG("%s: data=%s\n", __func__, tmp[0]);

            inbuff[1] = cnts;

            for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
            {
                inbuff[jj + 2] = mp_key_of_2char2num_func(tmp[0][kk], tmp[0][kk + 1]);
            }
        }

    }
    for(test=0; test<24; test++)
    {
        LOG_D("%x",inbuff[test]);
    }
    //return WF_RETURN_FAIL;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_SET, (wf_u32 *) inbuff, (cnts + 6) / 4, &outbuff, 1);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        if (inbuff)
            wf_kfree(inbuff);
        return WF_RETURN_FAIL;
    }
    if (outbuff == 0)
    {
        sprintf(extra, "%s" "%s", extra, " fail");
    }
    else
    {
        sprintf(extra, "%s" "%s", extra, " ok");
    }
    if (inbuff)
        wf_kfree(inbuff);
    return WF_RETURN_OK;

}

static int  wf_get_phy_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len )
{
    int ret                 = 0;
    wf_u32 word_len         = 0;
    wf_u32 intger           = 0;
    wf_u32 remainder        = 0;
    wf_u32 *pread           = 0;
    wf_u32  i               = 0;
    wf_u32 offset           = 0;
    wf_u32 inbuff           = 0;

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

        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_MP_EFUSE_ACCESS,&inbuff,1,pread,MAILBOX_MAX_RDLEN);


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
        inbuff = inbuff + MAILBOX_MAX_RDLEN * 4;
        LOG_I("[%s] offset:0x%x",__func__,offset);
        offset = offset + MAILBOX_MAX_RDLEN;//sdio need this
        pread  = pread + MAILBOX_MAX_RDLEN;

    }

    LOG_I("[%s] offset:0x%x",__func__,offset);

    if (remainder > 0)
    {

        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_MP_EFUSE_ACCESS,&inbuff,1,pread,remainder);

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

        offset += remainder;
    }


#if 0
    log_array(outdata, 512);
#endif
    return WF_RETURN_OK;
}


int wf_mp_efuse_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    int ret,i,cnts = 0;
    int efuse_code = 1000;
    char *pch,*token, *tmp[3] = { 0x00, 0x00, 0x00 };
    wf_u8 *outbuff = NULL;
    static wf_u8 raw_order = 0;
    wf_u32 shift, cnt;
    wf_u32 blksz = 0x200; /* The size of one time show, default 512 */
//    wf_u8 *rawdata = NULL;
    wf_u32 mapLen=256;
    wf_u32 j=0;
    wf_u8 *rawdata;
    wf_u8 efuse_data[256];
    MP_DBG();

    wrqu = (struct iw_point *)wdata;
    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        MP_DBG("copy_from_user fail");
        return -EFAULT;
    }

    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }
    MP_DBG("in = %s",extra);
    pch = extra;

    i = 0;
    while ((token = strsep(&pch, ",")) != NULL)
    {
        if (i > 2)
            break;
        tmp[i] = token;
        i++;
    }

    if (strcmp(tmp[0], "phy_freespace") == 0)
    {
        efuse_code = EFUSE_PHYSPACE;
    }
    else if (strcmp(tmp[0], "mac") == 0)
    {
        efuse_code = WLAN_EEPORM_MAC;
    }
    else if (strcmp(tmp[0], "vid") == 0)
    {
        efuse_code = EFUSE_VID;
    }
    else if (strcmp(tmp[0], "pid") == 0)
    {
        efuse_code = EFUSE_PID;
    }
    else if (strcmp(tmp[0], "manufacture") == 0)
    {
        efuse_code = EFUSE_MANU;
    }
    else if (strcmp(tmp[0], "product") == 0)
    {
        efuse_code = EFUSE_PRODUCT;
    }
    else if (strcmp(tmp[0], "freqcal") == 0)
    {
        efuse_code = EFUSE_FREQCAL;
    }
    else if (strcmp(tmp[0], "tempcal") == 0)
    {
        efuse_code = EFUSE_TEMPCAL;
    }
    else if (strcmp(tmp[0], "channelplan") == 0)
    {
        efuse_code = EFUSE_CHANNELPLAN;
    }
    else if (strcmp(tmp[0], "powercal") == 0)
    {
        efuse_code = EFUSE_POWERCAL;
    }
    else if (strcmp(tmp[0], "realraw") == 0)
    {
//        mapLen = wf_get_phy_efuse_len(pnic_info);
//        mapLen = 256;
        wf_get_phy_efuse_data(pnic_info,efuse_data,mapLen);
        rawdata = efuse_data;

        wf_memset(extra, '\0', strlen(extra));
        shift = blksz * raw_order;
        rawdata += shift;
        cnt = mapLen - shift;
        if (cnt > blksz)
        {
            cnt = blksz;
            raw_order++;
        }
        else
            raw_order = 0;
        if(rawdata == NULL)
        {
            LOG_E("rawdata NULL");
            return -1;
        }
        sprintf(extra, "\n");
        for (i = 0; i < cnt; i += 16)
        {
            sprintf(extra, "%s0x%02x\t", extra, shift + i);
            for (j = 0; j < 8; j++)
                sprintf(extra, "%s%02X ", extra, rawdata[i + j]);
            sprintf(extra, "%s\t", extra);
            for (; j < 16; j++)
                sprintf(extra, "%s%02X ", extra, rawdata[i + j]);
            sprintf(extra, "%s\n", extra);
        }
        if ((shift + cnt) < mapLen)
            sprintf(extra, "%s\t...more (left:%d/%d)\n", extra, mapLen-(shift + cnt), mapLen);

        if (outbuff)
            wf_kfree(outbuff);

        wrqu->length = strlen(extra);

        return 0;

    }
    else
    {
        sprintf(extra, "Command not found!");
    }


    if (efuse_code < 0 || efuse_code > WLAN_EEPORM_CODE_MAX)
    {
        MP_DBG("efuse_code fail");
        return WF_RETURN_FAIL;
    }

    outbuff = wf_kzalloc(MAILBOX_MAX_TXLEN * 4);
    if (outbuff == NULL)
    {
        MP_DBG("alloc recv buff fail");
        return  WF_RETURN_FAIL;
    }

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_GET, &efuse_code, 1, (wf_u32 *) outbuff, MAILBOX_MAX_TXLEN);
    if (WF_RETURN_FAIL == ret)
    {
        MP_WARN("[%s] read reg failed,check!!!", __func__);
        if (outbuff)
            wf_kfree(outbuff);
        return WF_RETURN_FAIL;
    }
    MP_DBG("efuse_code = %d, cnts = %d", efuse_code, outbuff[0]);

    *extra = 0;
    for (i = 0; i < outbuff[0]; i++)
    {
        sprintf(extra, "%s 0x%02X", extra, outbuff[i + 1]);
        if (i != (cnts - 1))
        {
            sprintf(extra, "%s", extra);
        }
        LOG_D("%x ",outbuff[i + 1]);
    }
    if (outbuff)
        wf_kfree(outbuff);

    wrqu->length = strlen(extra);

    return 0;

}

int wf_mp_fw_init(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *nic_info = pndev_priv->nic;
    int ret = -1;
    local_info_st *local_info = nic_info->local_info;
    hw_param_st *hw_param = NULL;
#ifndef CONFIG_RICHV100
    int i;
    hw_info_st *hwinfo = (hw_info_st *)nic_info->hw_info;
#endif
    struct iw_point *wrqu;
    wrqu = (struct iw_point *)wdata;

    if (copy_from_user(extra, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;
    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }
#ifdef CONFIG_RICHV100

    if (local_info->work_mode == WF_INFRA_MODE)
    {
        hw_param->send_msg[0] = WIFI_STATION_STATE;
    }
    else if (local_info->work_mode == WF_MASTER_MODE)
    {
        hw_param->send_msg[0] = WIFI_AP_STATE;
    }
    else if (local_info->work_mode == WF_MONITOR_MODE)
    {
        hw_param->send_msg[0] = WIFI_SITE_MONITOR;
    }
    // hardware init end
    ret = wf_mcu_init_hardware2(nic_info, hw_param);
#else
    if (local_info->work_mode == WF_INFRA_MODE)
    {
        hw_param->work_mode = WIFI_STATION_STATE;
    }
    else if (local_info->work_mode == WF_MASTER_MODE)
    {
        hw_param->work_mode = WIFI_AP_STATE;
    }
    else if (local_info->work_mode == WF_MONITOR_MODE)
    {
        hw_param->work_mode = WIFI_SITE_MONITOR;
    }
    for (i = 0; i < WF_ETH_ALEN; i++)
    {
        hw_param->mac_addr[i] = hwinfo->macAddr[i];
    }
    if (nic_info->buddy_nic)
    {
        hw_param->concurrent_mode = 1;//concurrent_mode
    }
#ifdef CONFIG_SOFT_RX_AGGREGATION
    hw_param->rx_agg_enable = 1;//rx_agg enable
#endif

    // hardware init end
    ret = wf_mcu_hw_init(nic_info, hw_param);
#endif
    if (ret == 0)
    {
        sprintf(extra, "%s", " ok");
    }
    else
    {
        sprintf(extra, "%s", " fail");
    }
    wrqu->length = strlen(extra);
    return 0;
}

int wf_mp_write_phy_efuse(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *wrqu;
    char *pch, *token;
    wf_u32 inbuff[32] = {0};
    wf_u32 addr = 0xFF;
    char *set_data;
    int num = 0,i=0;
    int ret = -1;
    char buf_test[16] = {0};
    int bit = 0;
    wrqu = (struct iw_point *)wdata;

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
    token = strsep(&pch, ",");
    LOG_D("pch:%s",pch);
    LOG_D("token:%s",token);

    sscanf(token,"%x",&addr);
    inbuff[0] = addr;

    set_data = pch;
    inbuff[1] = strlen(pch)/2;
    if(strlen(pch)%8 == 0)
    {
        num = strlen(pch)/8;
    }
    else
    {
        num = strlen(pch)/8 + 1;
    }
    for(i=0; i<num; i++)
    {
        MP_DBG("%s",set_data);
        sscanf(set_data,"%8s",buf_test);
        MP_DBG("data:0x%s ",buf_test);
        sscanf(buf_test,"%8x",&inbuff[i+2]);
        MP_DBG("inbuff:0x%x ",inbuff[i+2]);
        inbuff[i+2] = wf_cpu_to_be32(inbuff[i+2]);
        MP_DBG("data:0x%x ",inbuff[i+2]);
        if(num - i != 1)
            set_data=set_data+8;
    }
    bit = (((strlen(pch))/2)%4);
    if(bit != 0)
    {
        MP_DBG("inbuff[i+2]:%8x",inbuff[i+1]);
        inbuff[i+1] = inbuff[i+1] >> (4 - bit) * 8;
        MP_DBG("inbuff[i+2]:%8x",inbuff[i+1]);
    }
    MP_DBG("bit:%d",bit);
    inbuff[0] = inbuff[0] | BIT(31);
    MP_DBG("inbuff[0]:%x",inbuff[0]);
    MP_DBG("inbuff[1]:%x",inbuff[1]);
    MP_DBG("inbuff[2]:%x",inbuff[2]);
    MP_DBG("inbuff[3]:%x",inbuff[3]);
    MP_DBG("inbuff[4]:%x",inbuff[4]);
    MP_DBG("inbuff[5]:%x",inbuff[5]);
    MP_DBG("inbuff[6]:%x",inbuff[6]);

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_EFUSE_ACCESS, (wf_u32 *) inbuff, i+2, NULL, 0);

    if (ret == 0)
    {
        sprintf(extra, "%s", " ok");
    }
    else
    {
        sprintf(extra, "%s", " fail");
    }
    wrqu->length = strlen(extra);
    return 0;
}

#endif
#endif
