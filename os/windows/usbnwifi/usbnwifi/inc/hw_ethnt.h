#ifndef __INC_ETHERNET_H
#define __INC_ETHERNET_H

///////////////////////////////////////////
//copy from ethe.h
#define ETH_ALEN	6
#define ETH_HLEN	14

#define ETH_P_IP	0x0800
#define ETH_P_ARP	0x0806
#define ETH_P_ATALK	0x809B
#define ETH_P_AARP	0x80F3
#define ETH_P_8021Q	0x8100
#define ETH_P_IPX	0x8137
#define ETH_P_IPV6	0x86DD
#define ETH_P_PPP_DISC	0x8863
#define ETH_P_PPP_SES	0x8864
///////////////////////////////////////////

#define ETHERNET_ADDRESS_LENGTH         6
#define ETHERNET_HEADER_SIZE                14
#define LLC_HEADER_SIZE                     6
#define TYPE_LENGTH_FIELD_SIZE              2
#define MINIMUM_ETHERNET_PACKET_SIZE        60
#define MAXIMUM_ETHERNET_PACKET_SIZE        1514

#define RT_ETH_IS_MULTICAST(_pAddr) ((((PUCHAR)(_pAddr))[0]&0x01)!=0)
#define RT_ETH_IS_BROADCAST(_pAddr) (                                       \
                                            ((PUCHAR)(_pAddr))[0]==0xff    &&      \
                                            ((PUCHAR)(_pAddr))[1]==0xff    &&      \
                                            ((PUCHAR)(_pAddr))[2]==0xff    &&      \
                                            ((PUCHAR)(_pAddr))[3]==0xff    &&      \
                                            ((PUCHAR)(_pAddr))[4]==0xff    &&      \
                                            ((PUCHAR)(_pAddr))[5]==0xff        )


#endif // #ifndef __INC_ETHERNET_H

