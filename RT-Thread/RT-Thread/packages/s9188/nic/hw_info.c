/*
* hw_info.c
*
* used for Hardware information
*
* Author: songqiang
*
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "wf_debug.h"

#if 0
#define HWINFO_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define HWINFO_INFO(fmt, ...)     LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define HWINFO_DBG(fmt, ...)
#define HWINFO_INFO(fmt, ...)
#endif
#define HWINFO_WARN(fmt, ...)     LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define WF_CCK_RATES_NUM              (4)
#define WF_OFDM_RATES_NUM             (8)


typedef struct
{
  wf_u8 channel[14];
  wf_u8 len;
} channel_plan_2g_t;

typedef struct
{
  wf_u8 index_2g;
  wf_u8 regd;
} channel_plan_map_t;



static channel_plan_map_t channel_plan_map[] =
{
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_MKK1, TXPWR_LMT_WW},
  {WF_RD_2G_WORLD, TXPWR_LMT_WW},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_WW},
  {WF_RD_2G_FCC1, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_ETSI},
  {WF_RD_2G_ETSI1, TXPWR_LMT_MKK},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_ETSI1, TXPWR_LMT_MKK},
  {WF_RD_2G_WORLD, TXPWR_LMT_WW},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_NULL, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  
  {WF_RD_2G_WORLD, TXPWR_LMT_WW},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_ETSI2, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_FCC},
  {WF_RD_2G_FCC2, TXPWR_LMT_FCC},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_WORLD, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_FCC},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_MKK1, TXPWR_LMT_MKK},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_GLOBAL, TXPWR_LMT_WW},
  {WF_RD_2G_ETSI1, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
  {WF_RD_2G_MKK2, TXPWR_LMT_MKK},
  {WF_RD_2G_WORLD, TXPWR_LMT_ETSI},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_NULL, TXPWR_LMT_WW},
  {WF_RD_2G_FCC1, TXPWR_LMT_FCC},
};

static channel_plan_map_t channel_plan_map_wk_wlan_define =
{
  WF_RD_2G_WORLD, TXPWR_LMT_FCC
};

static channel_plan_2g_t ch_plan_2g[] =
{
  {{0}, 0},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, 13},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, 13},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, 11},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, 14},
  {{10, 11, 12, 13}, 4},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, 14},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, 13},
  {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, 13},
};




static wf_u8 WIFI_CCKRATES[] =
{
  (IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK),
  (IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK),
  (IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK),
  (IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK)
};

static wf_u8 WIFI_OFDMRATES[] = { (IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_9MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_18MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_36MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_48MB | IEEE80211_BASIC_RATE_MASK),
(IEEE80211_OFDM_RATE_54MB | IEEE80211_BASIC_RATE_MASK)
};


static unsigned char supported_mcs_set[WF_MCS_NUM] =
{
  0xff, 0xff, 0xff, 0x00, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};


#define CHPLAN_MAP(_alpha2, _chplan) \
{ (_alpha2), (_chplan) }

struct country_chplan
{
  char alpha2[2];
  wf_u8 chplan;
} static const chplan_map[] =
{
  CHPLAN_MAP("AD", 0x26),
  CHPLAN_MAP("AE", 0x26),
  CHPLAN_MAP("AF", 0x42),
  CHPLAN_MAP("AG", 0x30),
  CHPLAN_MAP("AI", 0x26),
  CHPLAN_MAP("AL", 0x26),
  CHPLAN_MAP("AM", 0x26),
  CHPLAN_MAP("AO", 0x26),
  CHPLAN_MAP("AQ", 0x26),
  CHPLAN_MAP("AR", 0x57),
  CHPLAN_MAP("AS", 0x34),
  CHPLAN_MAP("AT", 0x26),
  CHPLAN_MAP("AU", 0x45),
  CHPLAN_MAP("AW", 0x34),
  CHPLAN_MAP("AZ", 0x26),
  CHPLAN_MAP("BA", 0x26),
  CHPLAN_MAP("BB", 0x34),
  CHPLAN_MAP("BD", 0x26),
  CHPLAN_MAP("BE", 0x26),
  CHPLAN_MAP("BF", 0x26),
  CHPLAN_MAP("BG", 0x26),
  CHPLAN_MAP("BH", 0x47),
  CHPLAN_MAP("BI", 0x26),
  CHPLAN_MAP("BJ", 0x26),
  CHPLAN_MAP("BN", 0x47),
  CHPLAN_MAP("BO", 0x30),
  CHPLAN_MAP("BR", 0x34),
  CHPLAN_MAP("BS", 0x34),
  CHPLAN_MAP("BW", 0x26),
  CHPLAN_MAP("BY", 0x26),
  CHPLAN_MAP("BZ", 0x34),
  CHPLAN_MAP("CA", 0x34),
  CHPLAN_MAP("CC", 0x26),
  CHPLAN_MAP("CD", 0x26),
  CHPLAN_MAP("CF", 0x26),
  CHPLAN_MAP("CG", 0x26),
  CHPLAN_MAP("CH", 0x26),
  CHPLAN_MAP("CI", 0x26),
  CHPLAN_MAP("CK", 0x26),
  CHPLAN_MAP("CL", 0x30),
  CHPLAN_MAP("CM", 0x26),
  CHPLAN_MAP("CN", 0x48),
  CHPLAN_MAP("CO", 0x34),
  CHPLAN_MAP("CR", 0x34),
  CHPLAN_MAP("CV", 0x26),
  CHPLAN_MAP("CX", 0x45),
  CHPLAN_MAP("CY", 0x26),
  CHPLAN_MAP("CZ", 0x26),
  CHPLAN_MAP("DE", 0x26),
  CHPLAN_MAP("DJ", 0x26),
  CHPLAN_MAP("DK", 0x26),
  CHPLAN_MAP("DM", 0x34),
  CHPLAN_MAP("DO", 0x34),
  CHPLAN_MAP("DZ", 0x26),
  CHPLAN_MAP("EC", 0x34),
  CHPLAN_MAP("EE", 0x26),
  CHPLAN_MAP("EG", 0x47),
  CHPLAN_MAP("EH", 0x47),
  CHPLAN_MAP("ER", 0x26),
  CHPLAN_MAP("ES", 0x26),
  CHPLAN_MAP("ET", 0x26),
  CHPLAN_MAP("FI", 0x26),
  CHPLAN_MAP("FJ", 0x34),
  CHPLAN_MAP("FK", 0x26),
  CHPLAN_MAP("FM", 0x34),
  CHPLAN_MAP("FO", 0x26),
  CHPLAN_MAP("FR", 0x26),
  CHPLAN_MAP("GA", 0x26),
  CHPLAN_MAP("GB", 0x26),
  CHPLAN_MAP("GD", 0x34),
  CHPLAN_MAP("GE", 0x26),
  CHPLAN_MAP("GF", 0x26),
  CHPLAN_MAP("GG", 0x26),
  CHPLAN_MAP("GH", 0x26),
  CHPLAN_MAP("GI", 0x26),
  CHPLAN_MAP("GL", 0x26),
  CHPLAN_MAP("GM", 0x26),
  CHPLAN_MAP("GN", 0x26),
  CHPLAN_MAP("GP", 0x26),
  CHPLAN_MAP("GQ", 0x26),
  CHPLAN_MAP("GR", 0x26),
  CHPLAN_MAP("GS", 0x26),
  CHPLAN_MAP("GT", 0x34),
  CHPLAN_MAP("GU", 0x34),
  CHPLAN_MAP("GW", 0x26),
  CHPLAN_MAP("GY", 0x44),
  CHPLAN_MAP("HK", 0x26),
  CHPLAN_MAP("HM", 0x45),
  CHPLAN_MAP("HN", 0x32),
  CHPLAN_MAP("HR", 0x26),
  CHPLAN_MAP("HT", 0x34),
  CHPLAN_MAP("HU", 0x26),
  CHPLAN_MAP("ID", 0x54),
  CHPLAN_MAP("IE", 0x26),
  CHPLAN_MAP("IL", 0x47),
  CHPLAN_MAP("IM", 0x26),
  CHPLAN_MAP("IN", 0x47),
  CHPLAN_MAP("IQ", 0x26),
  CHPLAN_MAP("IR", 0x26),
  CHPLAN_MAP("IS", 0x26),
  CHPLAN_MAP("IT", 0x26),
  CHPLAN_MAP("JE", 0x26),
  CHPLAN_MAP("JM", 0x51),
  CHPLAN_MAP("JO", 0x49),
  CHPLAN_MAP("JP", 0x27),
  CHPLAN_MAP("KE", 0x47),
  CHPLAN_MAP("KG", 0x26),
  CHPLAN_MAP("KH", 0x26),
  CHPLAN_MAP("KI", 0x26),
  CHPLAN_MAP("KN", 0x34),
  CHPLAN_MAP("KR", 0x28),
  CHPLAN_MAP("KW", 0x47),
  CHPLAN_MAP("KY", 0x34),
  CHPLAN_MAP("KZ", 0x26),
  CHPLAN_MAP("LA", 0x26),
  CHPLAN_MAP("LB", 0x26),
  CHPLAN_MAP("LC", 0x34),
  CHPLAN_MAP("LI", 0x26),
  CHPLAN_MAP("LK", 0x26),
  CHPLAN_MAP("LR", 0x26),
  CHPLAN_MAP("LS", 0x26),
  CHPLAN_MAP("LT", 0x26),
  CHPLAN_MAP("LU", 0x26),
  CHPLAN_MAP("LV", 0x26),
  CHPLAN_MAP("LY", 0x26),
  CHPLAN_MAP("MA", 0x47),
  CHPLAN_MAP("MC", 0x26),
  CHPLAN_MAP("MD", 0x26),
  CHPLAN_MAP("ME", 0x26),
  CHPLAN_MAP("MF", 0x34),
  CHPLAN_MAP("MG", 0x26),
  CHPLAN_MAP("MH", 0x34),
  CHPLAN_MAP("MK", 0x26),
  CHPLAN_MAP("ML", 0x26),
  CHPLAN_MAP("MM", 0x26),
  CHPLAN_MAP("MN", 0x26),
  CHPLAN_MAP("MO", 0x26),
  CHPLAN_MAP("MP", 0x34),
  CHPLAN_MAP("MQ", 0x26),
  CHPLAN_MAP("MR", 0x26),
  CHPLAN_MAP("MS", 0x26),
  CHPLAN_MAP("MT", 0x26),
  CHPLAN_MAP("MU", 0x26),
  CHPLAN_MAP("MV", 0x26),
  CHPLAN_MAP("MW", 0x26),
  CHPLAN_MAP("MX", 0x34),
  CHPLAN_MAP("MY", 0x47),
  CHPLAN_MAP("MZ", 0x26),
  CHPLAN_MAP("NA", 0x26),
  CHPLAN_MAP("NC", 0x26),
  CHPLAN_MAP("NE", 0x26),
  CHPLAN_MAP("NF", 0x45),
  CHPLAN_MAP("NG", 0x50),
  CHPLAN_MAP("NI", 0x34),
  CHPLAN_MAP("NL", 0x26),
  CHPLAN_MAP("NO", 0x26),
  CHPLAN_MAP("NP", 0x47),
  CHPLAN_MAP("NR", 0x26),
  CHPLAN_MAP("NU", 0x45),
  CHPLAN_MAP("NZ", 0x45),
  CHPLAN_MAP("OM", 0x26),
  CHPLAN_MAP("PA", 0x34),
  CHPLAN_MAP("PE", 0x34),
  CHPLAN_MAP("PF", 0x26),
  CHPLAN_MAP("PG", 0x26),
  CHPLAN_MAP("PH", 0x26),
  CHPLAN_MAP("PK", 0x51),
  CHPLAN_MAP("PL", 0x26),
  CHPLAN_MAP("PM", 0x26),
  CHPLAN_MAP("PR", 0x34),
  CHPLAN_MAP("PT", 0x26),
  CHPLAN_MAP("PW", 0x34),
  CHPLAN_MAP("PY", 0x34),
  CHPLAN_MAP("QA", 0x51),
  CHPLAN_MAP("RE", 0x26),
  CHPLAN_MAP("RO", 0x26),
  CHPLAN_MAP("RS", 0x26),
  CHPLAN_MAP("RU", 0x59),
  CHPLAN_MAP("RW", 0x26),
  CHPLAN_MAP("SA", 0x26),
  CHPLAN_MAP("SB", 0x26),
  CHPLAN_MAP("SC", 0x34),
  CHPLAN_MAP("SE", 0x26),
  CHPLAN_MAP("SG", 0x47),
  CHPLAN_MAP("SH", 0x26),
  CHPLAN_MAP("SI", 0x26),
  CHPLAN_MAP("SJ", 0x26),
  CHPLAN_MAP("SK", 0x26),
  CHPLAN_MAP("SL", 0x26),
  CHPLAN_MAP("SM", 0x26),
  CHPLAN_MAP("SN", 0x26),
  CHPLAN_MAP("SO", 0x26),
  CHPLAN_MAP("SR", 0x34),
  CHPLAN_MAP("ST", 0x34),
  CHPLAN_MAP("SV", 0x30),
  CHPLAN_MAP("SX", 0x34),
  CHPLAN_MAP("SZ", 0x26),
  CHPLAN_MAP("TC", 0x26),
  CHPLAN_MAP("TD", 0x26),
  CHPLAN_MAP("TF", 0x26),
  CHPLAN_MAP("TG", 0x26),
  CHPLAN_MAP("TH", 0x26),
  CHPLAN_MAP("TJ", 0x26),
  CHPLAN_MAP("TK", 0x45),
  CHPLAN_MAP("TM", 0x26),
  CHPLAN_MAP("TN", 0x47),
  CHPLAN_MAP("TO", 0x26),
  CHPLAN_MAP("TR", 0x26),
  CHPLAN_MAP("TT", 0x42),
  CHPLAN_MAP("TW", 0x39),
  CHPLAN_MAP("TZ", 0x26),
  CHPLAN_MAP("UA", 0x26),
  CHPLAN_MAP("UG", 0x26),
  CHPLAN_MAP("US", 0x34),
  CHPLAN_MAP("UY", 0x34),
  CHPLAN_MAP("UZ", 0x47),
  CHPLAN_MAP("VA", 0x26),
  CHPLAN_MAP("VC", 0x34),
  CHPLAN_MAP("VE", 0x30),
  CHPLAN_MAP("VI", 0x34),
  CHPLAN_MAP("VN", 0x26),
  CHPLAN_MAP("VU", 0x26),
  CHPLAN_MAP("WF", 0x26),
  CHPLAN_MAP("WS", 0x34),
  CHPLAN_MAP("YE", 0x26),
  CHPLAN_MAP("YT", 0x26),
  CHPLAN_MAP("ZA", 0x26),
  CHPLAN_MAP("ZM", 0x26),
  CHPLAN_MAP("ZW", 0x26),
};

static wf_bool is_channel_plan_empty (wf_u8 id)
{
  channel_plan_map_t *pent;
  
  pent = (id == WF_CHPLAN_WK_WLAN_DEFINE) ?
    &channel_plan_map_wk_wlan_define : &channel_plan_map[id];
    return (wf_bool)(pent->index_2g == WF_RD_2G_NULL);
}

static wf_bool is_channel_plan_valid (wf_u8 ch_plan)
{
  if (ch_plan < WF_CHPLAN_MAX || ch_plan == WF_CHPLAN_WK_WLAN_DEFINE)
  {
    if (!is_channel_plan_empty(ch_plan))
    {
      return wf_true;
    }
  }
  return wf_false;
}


int channel_init (nic_info_st *pnic_info)
{
  hw_info_st *phw_info = (hw_info_st *)pnic_info->hw_info;
  wf_u8 ch_plan = phw_info->channel_plan;
  wf_channel_info_t *pch_set;
  wf_u8 ch_set_size;
  wf_u8 i;
  wf_u8 index_2g;
  wf_u8 ch_num;
  
  if (!is_channel_plan_valid(ch_plan))
  {
    LOG_I("channel plan ID 0x%02X error !!!!!\n", ch_plan);
    return -1;
  }
  
  wf_memset(phw_info->channel_set, 0, sizeof(phw_info->channel_set));
  
  index_2g = WF_CHPLAN_WK_WLAN_DEFINE == ch_plan ?
    channel_plan_map_wk_wlan_define.index_2g :
    channel_plan_map[ch_plan].index_2g;
    
    ch_set_size = ch_plan_2g[index_2g].len;
    pch_set = phw_info->channel_set;
    for (i = 0; i < ch_set_size; i++)
    {
      ch_num = ch_plan_2g[index_2g].channel[i];
      pch_set[i].channel_num = ch_num;
      if (ch_plan == WF_CHPLAN_GLOBAL_DOAMIN ||
          ch_plan == WF_CHPLAN_GLOBAL_NULL)
      {
        pch_set[i].scan_type = (ch_num >= 1 && ch_num <= 11) ?
      SCAN_TYPE_ACTIVE : SCAN_TYPE_PASSIVE;
      }
      else if (ch_plan == WF_CHPLAN_WORLD_WIDE_13 ||
               ch_plan == WF_CHPLAN_WORLD_WIDE_5G ||
                 index_2g == WF_RD_2G_WORLD)
      {
        pch_set[i].scan_type = ch_num <= 11 ?
      SCAN_TYPE_ACTIVE : SCAN_TYPE_PASSIVE;
      }
      else
      {
        pch_set[i].scan_type = SCAN_TYPE_ACTIVE;
      }
    }
    
    phw_info->max_chan_nums = ch_set_size;
    phw_info->Regulation2_4G = ch_plan == WF_CHPLAN_WK_WLAN_DEFINE ?
      channel_plan_map_wk_wlan_define.regd :
      channel_plan_map[ch_plan].regd;
      
      LOG_I("ChannelPlan ID: 0x%02x, NUM: %d\n", ch_plan, ch_set_size);
      
      return 0;
}

static int data_rate_init(nic_info_st *nic_info)
{
  hw_info_st *hw_info =(hw_info_st *) nic_info->hw_info;
  
  wf_memcpy(hw_info->datarate, WIFI_CCKRATES, WF_CCK_RATES_NUM);
  wf_memcpy(hw_info->datarate+WF_CCK_RATES_NUM, WIFI_OFDMRATES, WF_OFDM_RATES_NUM);
  wf_memcpy(hw_info->default_supported_mcs_set, supported_mcs_set, WF_MCS_NUM);
  
  return 0;
}


int wf_hw_info_init(nic_info_st *pnic_info)
{
  hw_info_st *phw_info;
  phw_info = wf_kzalloc(sizeof(hw_info_st));
  if (phw_info == NULL)
  {
    LOG_E("[wf_hw_info_init] malloc hw_info failed");
    return -1;
  }
  
  pnic_info->hw_info = phw_info;
  
  data_rate_init(pnic_info);
  
  return 0;
}

int wf_hw_info_term(nic_info_st *nic_info)
{
  hw_info_st *hw_info = nic_info->hw_info;
  
  if (hw_info)
  {
    wf_kfree(hw_info);
    nic_info->hw_info = NULL;
  }
  
  return 0;
}


wf_u8 do_query_center_ch(wf_u8 chnl_bw, wf_u8 channel, wf_u8 chnl_offset)
{
  wf_u8 center_ch = channel;
  
  if (chnl_bw == CHANNEL_WIDTH_40)
  {
    if (chnl_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
    {
      center_ch = channel + 2;
    }
    else if (chnl_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
    {
      center_ch = channel - 2;
    }
    else
    {
      center_ch = channel;
    }
  }
  
  return center_ch;
}

int wf_hw_info_set_channnel_bw(nic_info_st *nic_info,
                               wf_u8 channel,
                               CHANNEL_WIDTH cw,
                               HAL_PRIME_CH_OFFSET offset)
{
  int ret = 0;
  wf_u8 center_ch;
  wf_u32 uarg[7]= {0};
  
  center_ch = do_query_center_ch(cw, channel, offset);
  
  if (NIC_USB == nic_info->nic_type)
  {
    uarg[0] =   0;
  }
  else
  {
    uarg[0] =   0;
  }
  uarg[1] =   center_ch;
  uarg[2] =   cw;
  uarg[3] =   offset;
  uarg[4] =   1;
  uarg[5] =   1;
  uarg[6] =   1;
  
  ret = wf_mcu_set_ch_bw(nic_info, uarg, 7);
  if(nic_info->hw_ch && nic_info->hw_bw && nic_info->hw_offset)
  {
     *nic_info->hw_bw = cw;
     *nic_info->hw_offset = offset;
     *nic_info->hw_ch = channel;
  }
  return ret ;
}

int wf_hw_info_get_ch_bw(nic_info_st *nic_info, wf_u8 *ch,CHANNEL_WIDTH *bw,HAL_PRIME_CH_OFFSET *offset)
{
    if(NULL == nic_info)
    {
        return -1;
    }
    
    if(NULL == nic_info->hw_ch || NULL == nic_info->hw_bw || NULL == nic_info->hw_offset)
    {
        return -2;
    }
    *ch = *nic_info->hw_ch;
    *bw = *nic_info->hw_bw;
    *offset = *nic_info->hw_offset;
    //LOG_I("[%s] channel:%d  bw:%d  offset:%d",__func__,*ch, *bw, *offset);
    return 0;
}
int wf_hw_info_get_channnel_bw(nic_info_st *nic_info,
                               wf_u8 *channel, CHANNEL_WIDTH *cw,
                               HAL_PRIME_CH_OFFSET *offset)
{
  int ret = 0;
  
  ret = wf_mcu_get_ch_bw(nic_info, channel, cw, offset);
  
  return ret ;
}

int wf_hw_info_get_default_cfg(nic_info_st *pnic_info)
{
  int ret;
  wf_u32 version = 0;
  hw_info_st *phw_info = pnic_info->hw_info;
  wf_u8 macAddr[WF_ETH_ALEN] = {0xb4, 0x04, 0x18, 0x00, 0x00, 0x12};
  wf_u8 bmc_macAddr[WF_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  
  LOG_D("--Get EFUSE Info--");
  
  ret = wf_mcu_get_chip_version(pnic_info, &version);
  LOG_D("chip version: 0x%x",version );
  
  ret = wf_mcu_efuse_get(pnic_info, EFUSE_VID, (wf_u32 *)&phw_info->efuse.vid, 2);
  LOG_D("vid:0x%x ", phw_info->efuse.vid);
  
  ret = wf_mcu_efuse_get(pnic_info, EFUSE_PID, (wf_u32 *)&phw_info->efuse.pid, 2);
  LOG_D("pid:0x%x ", phw_info->efuse.pid);
  
  if (!pnic_info->virNic)
  {
    ret = wf_mcu_efuse_get(pnic_info, WLAN_EEPORM_MAC, (wf_u32 *)phw_info->macAddr, 6);
    if (wf_memcmp(bmc_macAddr, phw_info->macAddr, WF_ETH_ALEN) == 0)
    {
      LOG_E("efuse read mac fail, use default addr");
      wf_memcpy(phw_info->macAddr, macAddr, WF_ETH_ALEN);
      phw_info->macAddr[4] = wf_os_api_rand32()%0xFF;
      phw_info->macAddr[5] = wf_os_api_rand32()%0xFF;
    }
  }
  else
  {
    nic_info_st *pnic_info_r = pnic_info->buddy_nic;
    hw_info_st *phw_info_r = pnic_info_r->hw_info;
    memcpy(phw_info->macAddr, phw_info_r->macAddr, WF_ETH_ALEN);
    phw_info->macAddr[0] |= 0x02;
    LOG_D("make virtual mac");
  }
  
    LOG_D("[%d] macaddr :"WF_MAC_FMT,pnic_info->ndev_id, WF_MAC_ARG(phw_info->macAddr));
  
  ret = wf_mcu_efuse_get(pnic_info, EFUSE_CHANNELPLAN, (wf_u32 *)&phw_info->channel_plan, 1);
  LOG_D("get channel plan:0x%x ", phw_info->channel_plan);
    if(0xff == phw_info->channel_plan)
    {
        phw_info->channel_plan = 0x20;
        LOG_D("default channel plan:0x%x ", phw_info->channel_plan);
    }
  
  LOG_D("\n--Get HW Defualt Setting Info--");
  phw_info->vcs_type = RTS_CTS;
  
  phw_info->use_fixRate = wf_false;
  phw_info->tx_data_rpt = wf_false;
  phw_info->ba_enable   = wf_true;
  
  phw_info->use_drv_odm = wf_false;
  
  phw_info->dot80211n_support = wf_true;
  phw_info->cbw40_support = wf_true;
  phw_info->sm_ps_support = wf_false;
  phw_info->max_rx_ampdu_factor = MAX_AMPDU_FACTOR_64K;
  phw_info->wdn_sleep_support = wf_true;
  phw_info->ldpc_support = wf_false;
  phw_info->rx_stbc_support = wf_true;
  phw_info->rx_stbc_num = 1;
  phw_info->tx_stbc_support = wf_false;
  phw_info->rx_packet_offset = RXDESC_SIZE + DRVINFO_SZ * 8;
  phw_info->max_recvbuf_sz = MAX_RECVBUF_SZ;
  phw_info->best_ampdu_density = AMPDU_DENSITY_VALUE_7;
  phw_info->rf_type = 3;
  phw_info->frag_thresh =2346U;
  
  LOG_D("\n");
  
  return WF_RETURN_OK;
}

int wf_hw_info_set_default_cfg(nic_info_st *nic_info)
{
  int i;
  int ret;
  hw_info_st *hw_info = nic_info->hw_info;
  local_info_st *local_info = nic_info->local_info;
  hw_param_st hw_param;
  hw_info_st *hwinfo = (hw_info_st *)nic_info->hw_info;
  
  LOG_D("[HW_CFG] channel_plan: 0x%x",hw_info->channel_plan);
  LOG_D("[HW_CFG] ba_func: %d",hw_info->ba_enable);
  wf_memset(&hw_param,0,sizeof(hw_param_st));
  
  ret = wf_mcu_set_hw_invalid_all(nic_info);
  if (ret != WF_RETURN_OK)
  {
    return WF_RETURN_FAIL;
  }
  ret = wf_mcu_ars_init(nic_info);
  if (ret != WF_RETURN_OK)
  {
    return WF_RETURN_FAIL;
  }
  if (local_info->work_mode == WF_INFRA_MODE)
  {
    hw_param.work_mode = WIFI_STATION_STATE;
  }
  else if (local_info->work_mode == WF_MASTER_MODE)
  {
    hw_param.work_mode = WIFI_AP_STATE;
  }
  else if (local_info->work_mode == WF_MONITOR_MODE)
  {
    hw_param.work_mode = WIFI_SITE_MONITOR;
  }
  for (i = 0; i < WF_ETH_ALEN; i++)
  {
    hw_param.mac_addr[i] = hwinfo->macAddr[i];
  }
  
#ifdef CONFIG_STA_AND_AP_MODE
  hw_param.concurrent_mode = 1;
#endif
  
#ifdef CONFIG_SOFT_RX_AGGREGATION
  hw_param.rx_agg_enable = 1;
#else
  hw_param.rx_agg_enable = 0;
#endif
  
  ret = wf_mcu_hw_init(nic_info, &hw_param);
  if (ret != WF_RETURN_OK)
  {
    return WF_RETURN_FAIL;
  }
  
  return WF_RETURN_OK;
}



