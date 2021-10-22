
#include "wf_80211.h"

#define min(a, b)((a)> (b)?(b) : (a))  
#define max(a, b)((a)> (b)?(a) : (b)) 

struct key_params {
  wf_u8 *key;
  wf_u8 *seq;
  int key_len;
  int seq_len;
  wf_u32 cipher;
};


int wf_rt_key(nic_info_st *pnic_info,
              wf_u8 key_index, const wf_u8 * mac_addr,
              struct key_params *params);


int wf_rt_sta_set_encryption(nic_info_st *pnic_info,
                             ieee_param *param, wf_u32 param_len);

