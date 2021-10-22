/*
 * cfg_parse.c
 *
 * used for cfg file parse.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "hif.h"
#include "wf_debug.h"

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
static int scan_ch_to_parse_handle(nic_info_st *nic_info, const char *value);
static int scan_prb_times_handle (nic_info_st *nic_info, const char *value);
static int scan_que_deep_handle (nic_info_st *nic_info, const char *value);
static int scan_que_node_ttl_handle (nic_info_st *nic_info, const char *value);

static const struct cfg_parse_t __gl_cfg_parse_st[] = 
{
  {"ssid", ssid_parse_handle},
  {"channel", channel_parse_handle},
  {"bw", bw_parse_handle},
  {"work_mode", work_mode_parse_handle},
  {"channelplan", channelplan_parse_handle},
  {"ba_func", ba_func_parse_handle},
  {"scan_ch_to", scan_ch_to_parse_handle},
  {"scan_prb_times", scan_prb_times_handle},
  {"scan_que_deep", scan_que_deep_handle},
  {"scan_que_node_ttl", scan_que_node_ttl_handle},
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
    plocal->bw = CHANNEL_WIDTH_20;
  }
  else if(strncmp(value,"40M",strlen("40M")) == 0)
  {
    plocal->bw = CHANNEL_WIDTH_40;
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

static int scan_ch_to_parse_handle (nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;

    LOG_I("scan_ch_to:%s", value);
    plocal->scan_ch_to = wf_atoi(value);
    return 0;
}

static int scan_prb_times_handle (nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;

    LOG_I("scan_prb_times:%s", value);
    plocal->scan_prb_times = wf_atoi(value);
    return 0;
}

static int scan_que_deep_handle (nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;

    LOG_I("scan_que_deep:%s", value);
    plocal->scan_que_deep = wf_atoi(value);
    return 0;
}

static int scan_que_node_ttl_handle (nic_info_st *nic_info, const char *value)
{
    local_info_st *plocal = (local_info_st *)nic_info->local_info;

    LOG_I("scan_que_node_ttl:%s", value);
    plocal->scan_que_node_ttl = wf_atoi(value);
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
  int c;
  int total;
  int sign;
  
  while ( wf_isspace((int)(unsigned char)*nptr) )
    ++nptr;
  
  c = (int)(unsigned char)*nptr++;
  sign = c;
  if (c == '-' || c == '+')
    c = (int)(unsigned char)*nptr++;
  
  total = 0;
  
  while (wf_isdigit(c)) {
    total = 10 * total + (c - '0');
    c = (int)(unsigned char)*nptr++;
  }
  
  if (sign == '-')
    return -total;
  else
    return total;
}

int wf_cfg_file_parse(void *pnic_info)
{
  char read_buffer[65] = {0};
  char handle_buffer[65] = {0};
  loff_t pos = 0;
  wf_file *file = NULL;
  nic_info_st *nic_info = (nic_info_st *)pnic_info;
  hif_mngent_st *hif = hif_mngent_get();
  
  file = wf_os_api_file_open(hif->cfg_dir);
  if(file == NULL) {
    LOG_E("can't open cfg file");
    return -1;
  }
  
  while(wf_os_api_file_readline(file, &pos, (unsigned char *)read_buffer, 64) > 0) {
    if(strlen(read_buffer) == 0) {
      continue;
    }
    cfg_buffer_handle((const char*)read_buffer,handle_buffer);
    if((handle_buffer[0] == '#') || (strlen(handle_buffer) == 0)) {
      continue;
    }
    cfg_parse_handle(nic_info, handle_buffer);
  }
  
  wf_os_api_file_close(file);
  
  return 0;
}

