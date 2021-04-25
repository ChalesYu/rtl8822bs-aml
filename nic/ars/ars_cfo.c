#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_ARS_SUPPORT

void ODM_ParsingCFO(void *pars, void *pkt_info,wf_s8 *pcfotail )
{
    ars_st *ars                         = NULL;
    ODM_PACKET_INFO_T *pktinfo          = NULL;
    CFO_TRACKING            *pCfoTrack  = NULL;
    wf_u8 i = 0;

    if(NULL == pars || NULL == pkt_info)
    {
        ARS_DBG("input param is null");
        return;
    }
    
    ars = pars;
    pktinfo = pkt_info;
    pCfoTrack = &ars->cfo.cfo_tr;
    if(!(ars->SupportAbility & ODM_BB_CFO_TRACKING))
        return;

    if(pktinfo->bPacketMatchBSSID)
    {               
        //3 Update CFO report for path-A & path-B
        // Only paht-A and path-B have CFO tail and short CFO
        for(i = ODM_RF_PATH_A; i <= ODM_RF_PATH_B; i++)   
        {
            pCfoTrack->CFO_tail[i] = (int)pcfotail[i];
        }

        //3 Update packet counter
        if(pCfoTrack->packetCount == 0xffffffff)
            pCfoTrack->packetCount = 0;
        else
            pCfoTrack->packetCount++;
    }
}
    

#endif

