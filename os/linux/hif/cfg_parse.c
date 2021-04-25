#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/utsname.h>
#include <linux/list.h>
#include <linux/usb.h>
#include <linux/time.h>

#include "wf_debug.h"
#include "wf_list.h"
#include "hif.h"
#include "power.h"

struct cfg_parse_t {
   const char *key;
   int (*parse_handle)(nic_info_st *nic_info, const char *value);
};

int wf_isspace(int x);
int wf_atoi(const char *nptr);
static int ssid_parse_handle(nic_info_st *nic_info, const char *value);
static int channel_parse_handle(nic_info_st *nic_info, const char *value);
static int bw_parse_handle(nic_info_st *nic_info, const char *value);
static int work_mode_parse_handle(nic_info_st *nic_info, const char *value);
static int channelplan_parse_handle(nic_info_st *nic_info, const char *value);
static int ba_func_parse_handle(nic_info_st *nic_info, const char *value);

static const struct cfg_parse_t __gl_cfg_parse_st[] = 
{
   {"ssid", ssid_parse_handle},
   {"channel", channel_parse_handle},
   {"bw", bw_parse_handle},
   {"work_mode", work_mode_parse_handle},
   {"channelplan", channelplan_parse_handle},
   {"ba_func", ba_func_parse_handle}
};

static int ssid_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("ssid:%s", value);
    wf_memcpy(plocal->ssid, value,strlen(value));
    return 0;
}

static int channel_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("channel:%s", value);
    plocal->channel = wf_atoi(value);
    return 0;
}

static int bw_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("bw:%s", value);
    if(strncmp(value,"20M",strlen("20M")) == 0)
    {
        plocal->bw = 0;
    }
    else if(strncmp(value,"40M",strlen("40M")) == 0)
    {
        plocal->bw = 1;
    }
    else
    {
        LOG_E("cfg file format error for bw");
    }
    return 0;
}

static int work_mode_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("work_mode:%s", value);
    if(strncmp(value,"sta",strlen("sta")) == 0)
    {
        plocal->work_mode = WF_INFRA_MODE;
    }
    else if(strncmp(value,"ap",strlen("ap")) == 0)
    {
        plocal->work_mode = WF_MASTER_MODE;
    }
    else if(strncmp(value,"adhoc",strlen("adhoc")) == 0)
    {
        plocal->work_mode = WF_ADHOC_MODE;
    }
    else if(strncmp(value,"moniter",strlen("moniter")) == 0)
    {
        plocal->work_mode = WF_MONITOR_MODE;
    } else {
       LOG_E("cfg file format error for param work_mode"); 
    }
    return 0;
}

static int channelplan_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("channelplan:%s", value);
    plocal->channel_plan = wf_atoi(value);
    return 0;
}

static int ba_func_parse_handle(nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;
    LOG_I("ba_func:%s", value);
    plocal->ba_enable = wf_atoi(value);
    return 0;
}

static void cfg_buffer_handle(const char *in_buffer, char *out_buffer)
{
    int i, j;
    int len = strlen(in_buffer);

    for(i=0, j= 0; i<len; i++) {
        if(!wf_isspace(in_buffer[i])) {
            out_buffer[j] = in_buffer[i];
            j++;
        }
    }
    out_buffer[j] = '\0';
}

static void cfg_parse_handle(nic_info_st *nic_info, const char *buffer)
{
    int i;
    const char *key;
    char *pos;
    int num = sizeof(__gl_cfg_parse_st) / sizeof(struct cfg_parse_t);

    pos = strchr(buffer, '=');
    if(pos == NULL) {
        LOG_E("can't find sep for this param");
        return;
    }
    *pos++= '\0';
    key = (char *)buffer;
    for(i=0; i<num; i++)
    {
       if(strcmp(__gl_cfg_parse_st[i].key, key) == 0) {
           __gl_cfg_parse_st[i].parse_handle(nic_info, pos);
           return;
       }
    }
    LOG_W("[%s]:can't find handler for this key:%s, please register it!", __func__, key);
}

static int cfg_read_line(const char *cfg_content, size_t size, loff_t *pos, char *buffer, wf_u32 length)
{
    int ret;
    wf_u32 read_length;
    char *eol;
    loff_t offset = *pos;

    if(offset >= size) {
        return 0;
    }

    if((offset + length) > size) {
        read_length = size - offset;
    } else {
        read_length = length;
    }
    wf_memcpy(buffer, &cfg_content[offset], read_length);
    offset += read_length;
    eol = strstr(buffer, "\r\n");
    if (eol != NULL) {
        *eol++ = '\0';
        *eol++ = '\0';
        ret = (size_t)(eol - buffer);
        offset -= (read_length - ret);
        *pos = offset;
    } else {
        return -1;
    }

    return ret;
}

int wf_isspace(int x)
{
 if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
  return 1;
 else
  return 0;
}
int wf_isdigit(int x)
{
 if(x<='9'&&x>='0')
  return 1;
 else
  return 0;

}

int wf_atoi(const char *nptr)
{
    int c;              /* current char */
    int total;         /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( wf_isspace((int)(unsigned char)*nptr) )
        ++nptr;

    c = (int)(unsigned char)*nptr++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++;    /* skip sign */

    total = 0;

    while (wf_isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}

int wf_cfg_file_parse(void *pnic_info)
{
    char read_buffer[65] = {0};
    char handle_buffer[65] = {0};
    loff_t pos = 0;
	nic_info_st *nic_info = (nic_info_st *)pnic_info;
    hif_mngent_st *hif = hif_mngent_get();

    if(hif->cfg_size == 0) {
        LOG_E("cfg_size is 0, no need parse");
        return -1;
    }

    while(cfg_read_line(hif->cfg_content, hif->cfg_size, &pos, read_buffer, 64) > 0) {
         if(strlen(read_buffer) == 0) {
             continue;
         }
         cfg_buffer_handle((const char*)read_buffer,handle_buffer);
         if((handle_buffer[0] == '#') || (strlen(handle_buffer) == 0)) {
             continue;
         }
         cfg_parse_handle(nic_info, handle_buffer);
    }

    return 0;
}




