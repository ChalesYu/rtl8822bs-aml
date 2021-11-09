
/* include */
#include "global.h"
#include "wifi/wf_wifi.h"

/* macro */
#if 1
#define WIFI_DBG(fmt, ...)      OS_LOG_D("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define WIFI_DBG(fmt, ...)
#endif
#define WIFI_INFO(fmt, ...)     OS_LOG_I("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WIFI_WARN(fmt, ...)     OS_LOG_W("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WIFI_ERROR(fmt, ...)    OS_LOG_E("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* variable declaration */
static OS_U8 wdev_cnt = 0;

/* function declaration */

OS_STATIC_INLINE
err_t _netif_linkoutput (struct netif *netif, struct pbuf *p)
{
    struct pbuf *pbuf = p;
    wlan_dev_t *wdev = netif->state;
    wf_wifi_hd_t wifi_hd = wlan_dev_priv(wdev);
    int ret = ERR_OK;

    if (p->tot_len != p->len)
    {
        pbuf = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_POOL);
        if (!pbuf)
        {
            OS_LOG_E("malloc pbuf failed");
            ret = ERR_MEM;
            goto exit;
        }
        pbuf_copy_partial(p, pbuf->payload, pbuf->len, 0);
    }

    if (wf_wifi_data_xmit(wifi_hd, pbuf->payload, pbuf->len))
    {
        OS_LOG_E("wifi data send failed");
        ret = ERR_IF;
    }

    if (pbuf != p)
    {
        pbuf_free(pbuf);
    }

exit :
    return ret;
}

OS_STATIC_INLINE
err_t _netif_init (struct netif *netif)
{
    wlan_dev_t *wdev;

    wdev = netif->state;
    if (!wdev)
    {
        return ERR_IF;
    }

    /* set output */
#if LWIP_IPV4
    netif->output       = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6   = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput   = _netif_linkoutput;

    /* set name */
    netif->name[0] = 0x0;
    netif->name[1] = 0x0;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "ZTOP";
#endif /* LWIP_NETIF_HOSTNAME */

    /* copy device flags to netif flags */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
                   NETIF_FLAG_ETHERNET  | NETIF_FLAG_IGMP   |
                   NETIF_FLAG_MLD6;

    /* set hw address */
    {
        OS_U8 mac[6];
        wdev->ops->get_mac(wdev, mac);
        netif->hwaddr_len = OS_ARRAY_SIZE(mac);
        wf_memcpy(netif->hwaddr, mac, netif->hwaddr_len);
    }

    /* maximum transfer unit */
    netif->mtu = 1500;

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 0);

    /* set default netif */
    if (!netif_default)
    {
        netif_set_default(netif);
    }

    /* set interface up */
    netif_set_up(netif);

    return ERR_OK;
}

static int _netif_register (wf_wifi_hd_t wifi_hd)
{
    char name[] = "wlan0";
    wlan_dev_t *wdev;

    if (OS_WARN_ON(!wifi_hd))
    {
        return -1;
    }

    /* new wdev */
    os_sprintf(name, "wlan%d", wdev_cnt++);
    wdev = wlan_dev_new(name, &gwlan_dev_ops_reg, sizeof(wifi_hd));
    if (!wdev)
    {
        return -2;
    }
    wlan_dev_priv_set(wdev, wifi_hd);
    wf_wifi_priv_set(wifi_hd, wdev);

    /* register wdev */
    wlan_dev_register(wdev);
    /* wlan open */
    wlan_dev_open(wdev);

    /* lwip init */
    {
        struct ip4_addr ipaddr, netmask, gw;
        struct netif *netif;

        netif = osZMalloc(sizeof(struct netif));
        if (!netif)
        {
            OS_LOG_D("malloc netif failed");
            return -1;
        }
        wlan_dev_parent_set(wdev, netif);

        /* netif */
        IP4_ADDR(&ipaddr, 192, 168, 3, 66);
        IP4_ADDR(&netmask, 255, 255, 255, 0);
        IP4_ADDR(&gw, 192, 168, 3, 1);

        /* netif add */
        netif_add(netif,
#if LWIP_IPV4
                  &ipaddr, &netmask, &gw,
#endif
                  wdev, _netif_init, tcpip_input);
    }

    return 0;
}

static int _netif_unregister (wf_wifi_hd_t wifi_hd)
{
    wlan_dev_t *wdev;

    if (OS_WARN_ON(!wifi_hd))
    {
        return -1;
    }

    wdev = wf_wifi_priv(wifi_hd);
    wlan_dev_unregister(wdev);
    wf_wifi_priv_set(wifi_hd, NULL);

    return 0;
}


static int _scan_report (wf_wifi_hd_t wifi_hd,
                         wf_wifi_scan_report_t *scan_rpo)
{
    wlan_dev_t *wdev = wf_wifi_priv(wifi_hd);
    wlan_dev_info_t *wlan_dev_info;

    /* new wlan dev */
    wlan_dev_info = wlan_dev_info_alloc();
    if (!wlan_dev_info)
    {
        return -1;
    }

    /* make infomation */
    {
        os_memcpy(&wlan_dev_info->ssid, &scan_rpo->ssid,
                  sizeof(wlan_dev_info->ssid));
        wlan_dev_info->hidden = scan_rpo->hidden;
        os_memcpy(&wlan_dev_info->bssid, &scan_rpo->bssid,
                  sizeof(wlan_dev_info->bssid));
        wlan_dev_info->sec = (wlan_dev_security_t)scan_rpo->sec;
        wlan_dev_info->channel = scan_rpo->channel;
        wlan_dev_info->datarate = scan_rpo->datarate;
        wlan_dev_info->band = (wlan_dev_802_11_band_t)scan_rpo->band;
        wlan_dev_info->rssi = scan_rpo->rssi;
    }

    wlan_dev_info_push(wdev, wlan_dev_info);

    return 0;
}

static int _scan_done (wf_wifi_hd_t wifi_hd)
{
    wlan_dev_t *wdev = wf_wifi_priv(wifi_hd);

    return wlan_dev_event_post(wdev, WLAN_DEV_EVT_SCAN_DONE);
}

static int _conn_done (wf_wifi_hd_t wifi_hd, wf_wifi_conn_info_t *conn_info)
{
    wlan_dev_t *wdev = wf_wifi_priv(wifi_hd);
    struct netif *netif = wlan_dev_parent(wdev);
    wlan_dev_info_t *wlan_dev_info = NULL;

    if (conn_info->ssid.data == NULL)
    {
        wlan_dev_event_post(wdev, WLAN_DEV_EVT_CONNECT_FAIL);
        return -1;
    }

    wlan_dev_info = osZMalloc(sizeof(wlan_dev_info_t));
    wlan_dev_info->channel = conn_info->channel;
    wlan_dev_info->ssid.len = conn_info->ssid.len;
    os_memcpy(wlan_dev_info->ssid.val, conn_info->ssid.data,
              conn_info->ssid.len);
    os_memcpy(wlan_dev_info->bssid, conn_info->bssid, 6);
    wlan_dev_info_push(wdev, wlan_dev_info);
    wlan_dev_event_post(wdev, WLAN_DEV_EVT_CONNECT_SUCCESS);

    /* notify lwip wifi link up */
    netif_set_link_up(netif);

    return 0;
}

static int _disconn_evt (wf_wifi_hd_t wifi_hd)
{
    wlan_dev_t *wdev = wf_wifi_priv(wifi_hd);
    struct netif *netif = wlan_dev_parent(wdev);

    /* notify lwip wifi link break */
    netif_set_link_down(netif);

    wlan_dev_event_post(wdev, WLAN_DEV_EVT_DISCONNECT);

    return 0;
}

static int _assoc_evt (wdn_net_info_st *pwdn_info)
{
    WIFI_INFO("AID: %d MAC: %02x:%02x:%02x:%02x:%02x:%02x\n\r", pwdn_info->aid,
              pwdn_info->mac[0],
              pwdn_info->mac[1],
              pwdn_info->mac[2],
              pwdn_info->mac[3],
              pwdn_info->mac[4],
              pwdn_info->mac[5]);

    return 0;
}

static int _disassoc_evt (wdn_net_info_st *pwdn_info)
{
    WIFI_INFO("AID: %d MAC: %02x:%02x:%02x:%02x:%02x:%02x\n\r", pwdn_info->aid,
              pwdn_info->mac[0],
              pwdn_info->mac[1],
              pwdn_info->mac[2],
              pwdn_info->mac[3],
              pwdn_info->mac[4],
              pwdn_info->mac[5]);

    return 0;
}

static wf_wifi_skb_hd_t _skb_create (wf_wifi_hd_t wifi_hd,
                                     void *data, wf_u32 len,
                                     void **skb_data, wf_u32 *skb_len)
{
    struct pbuf *p;

    OS_UNUSED(wifi_hd);

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE;
#endif
    p = pbuf_alloc(PBUF_RAW, (wf_u16)len, PBUF_POOL);
    if (!p)
    {
        return NULL;
    }

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);
#endif
    pbuf_take(p, data, len);

    *skb_data   = p->payload;
    *skb_len    = p->tot_len;

    return p;
}

static int _skb_free (wf_wifi_hd_t wifi_hd, wf_wifi_skb_hd_t skb)
{
    OS_UNUSED(wifi_hd);

    return pbuf_free(skb) ? 0 : -1;
}

static int _skb_upload (wf_wifi_hd_t wifi_hd, wf_wifi_skb_hd_t skb)
{
    struct netif *netif;
    struct pbuf *p;
    wlan_dev_t *wdev;

    OS_UNUSED(wifi_hd);

    wdev = wf_wifi_priv(wifi_hd);
    netif  = wdev->parent;
    p = skb;

    if (netif->input(p, netif) != ERR_OK)
    {
        _skb_free(wifi_hd, skb);
    }

    return 0;
}


const wf_wifi_ops_t gwf_wifi_ops_reg =
{
    .netif_register     = _netif_register,
    .netif_unregister   = _netif_unregister,
    .scan_report        = _scan_report,
    .scan_done          = _scan_done,
    .conn_done          = _conn_done,
    .disconn_evt        = _disconn_evt,
    .assoc_evt          = _assoc_evt,
    .disassoc_evt       = _disassoc_evt,
    .skb_create         = _skb_create,
    .skb_upload         = _skb_upload,
    .skb_free           = _skb_free,
};


