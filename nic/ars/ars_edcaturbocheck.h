#ifndef __ARS_EDCATURBOCHECK_H__
#define __ARS_EDCATURBOCHECK_H__

typedef enum _HT_IOT_PEER
{
    HT_IOT_PEER_UNKNOWN                 = 0,
    HT_IOT_PEER_REALTEK                 = 1,
    HT_IOT_PEER_REALTEK_92SE            = 2,
    HT_IOT_PEER_BROADCOM                = 3,
    HT_IOT_PEER_RALINK                  = 4,
    HT_IOT_PEER_ATHEROS                 = 5,
    HT_IOT_PEER_CISCO                   = 6,
    HT_IOT_PEER_MERU                    = 7,
    HT_IOT_PEER_MARVELL                 = 8,
    HT_IOT_PEER_REALTEK_SOFTAP          = 9,// peer is RealTek SOFT_AP, by Bohn, 2009.12.17
    HT_IOT_PEER_SELF_SOFTAP             = 10, // Self is SoftAP
    HT_IOT_PEER_AIRGO                   = 11,
    HT_IOT_PEER_INTEL                   = 12, 
    HT_IOT_PEER_RTK_APCLIENT            = 13, 
    HT_IOT_PEER_REALTEK_81XX            = 14,   
    HT_IOT_PEER_REALTEK_WOW             = 15,
    HT_IOT_PEER_REALTEK_JAGUAR_BCUTAP   = 16,
    HT_IOT_PEER_REALTEK_JAGUAR_CCUTAP   = 17,
    HT_IOT_PEER_MAX                     = 18
}HT_IOT_PEER_E, *PHTIOT_PEER_E;


typedef struct edcaturbo_info_st_
{
}ars_edcaturbo_info_st;

void odm_EdcaTurboCheck(void *ars);
#endif