#ifndef __NIC_ATTR_H__
#define __NIC_ATTR_H__

/**
* TODO: I am not sure about the exact data for RichV200. As a consequence, a series of 
*       default value are put here.
*		Adjust the datas in this file in the future.            2021/03/15 
*/


/** Maximum number of MAC addresses we support in the excluded list */
#define STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT      4
/** Max number of desired BSSIDs we can handle */
#define STA_DESIRED_BSSID_MAX_COUNT             32
/** Max number of desired PHYs we can handle */
#define STA_DESIRED_PHY_MAX_COUNT               5
/** Max number of PMKID we can handle */
#define STA_PMKID_MAX_COUNT                     3
/** Max number of enabled multicast cipher algorithms */
#define STA_MULTICAST_CIPHER_MAX_COUNT          6
#define STA_UNICAST_CIPHER_MAX_COUNT            6


/** Maximum number of BSSs we will store in our BSS entries list */
#define STA_BSS_ENTRY_MAX_ENTRIES_DEFAULT       128
#define STA_BSS_ENTRY_MAX_ENTRIES_MIN           16
#define STA_BSS_ENTRY_MAX_ENTRIES_MAX           512

/** Maximum number of Adhoc stations we will cache state about */
#define STA_ADHOC_STA_MAX_ENTRIES_DEFAULT       64
#define STA_ADHOC_STA_MAX_ENTRIES_MIN           16
#define STA_ADHOC_STA_MAX_ENTRIES_MAX           512

/** Maximum number of candidate BSS we will use for association */
#define STA_CANDIDATE_AP_MAX_COUNT              32

/**
* Time duration after it was created at which a BSS entry is
* considered to have expired
*/
#define STA_BSS_ENTRY_EXPIRE_TIME_DEFAULT       750000000       // 75 seconds
#define STA_BSS_ENTRY_EXPIRE_TIME_MIN           150000000       // 15 seconds
#define STA_BSS_ENTRY_EXPIRE_TIME_MAX           2000000000      // 200 seconds

/** Time to wait for infrastructure Join to complete (in number of beacon intervals) */
#define STA_JOIN_FAILURE_TIMEOUT                40 //10 

/** Time to wait for authentication handshakes to complete (in number of milliseconds) */
#define STA_AUTHENTICATE_FAILURE_TIMEOUT        1000 

/** Time to wait for association response (in number of milliseconds) */
#define STA_ASSOCIATE_FAILURE_TIMEOUT           1000 

/** Number to increate the association cost on failed association */
#define STA_ASSOC_COST_FAILED_CONNECT           2

/** Number of increment the association cost on remote disconnection */
#define STA_ASSOC_COST_REMOTE_DISCONNECT        2

/** Number of increae the association cost on disassociation due to roaming */
#define STA_ASSOC_COST_ROAM_DISCONNECT          1

/** Cost at which we will reject an association */
#define STA_ASSOC_COST_MAX_DONT_CONNECT         10

/**
* Number of beacon intervals after which if we havent
* received a beacon from the AP we assume we have
* lost connectivity
*/
#define STA_INFRA_ROAM_NO_BEACON_COUNT_DEFAULT  20
#define STA_INFRA_ROAM_NO_BEACON_COUNT_MIN      5
#define STA_INFRA_ROAM_NO_BEACON_COUNT_MAX      50

/**
* Minimum time we give before connectivity is partially lost. This
* is because we may not receive beacon when we are scanning and
* dont want to penalize the AP for that
*/
#define STA_MIN_INFRA_UNREACHABLE_TIMEOUT       30000000     // In 100s of nanoseconds = 3 second

/**
* Maximum time we give before assuming connectivity is lost.
* At this time if we dont find a new AP, we will drop connectivity
*
*/
#define STA_MAX_INFRA_UNREACHABLE_TIMEOUT       100000000     // In 100s of nanoseconds = 10 second


/**
* Link quality value (0-100 which if we go below for a sequence of beacons,
* would suggest that we need to roam
*/
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_DEFAULT   15
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MIN       5
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MAX       80

/**
* Number of beacon for which we have continuously received
* beacons with RSSI below above threshold, we will roam
*/
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_DEFAULT        15
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_MIN            5
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_MAX            20

/**
* The interval at which the periodic scan timer
* runs. It will not scan
*
*/
#define STA_DEFAULT_SCAN_TIMER_INTERVAL         1000

/**
* Periodic scan will happen every so many intervals
* if everything is okay. This is related to the
* above time
*/
#define STA_DEFAULT_SCAN_TICK_COUNT             60

/**
* Periodic scan will happen not be initiated if
* we had performed a scan these many SECONDs ago
* and we dont need to do a scan
*/
#define STA_PERIODIC_SCAN_NORMAL_GAP            70

/**
* Periodic scan will happen not be initiated if
* we had performed a scan these many SECONDs ago
* and we need to do a scan for connecting or something.
* This gets exponentially increased
*/
#define STA_PERIODIC_SCAN_SHORT_GAP             1

/**
* Number of milliseconds to wait for queuing the timer
* when we are not connected and want to
*/
#define STA_FORCED_PERIODIC_SCAN_INTERVAL       10

/**
* Time interval in seconds between two consecutive check
* of new PKMID candidates.
*/
#define STA_PMKID_CHECK_INTERVAL                120

/**
* Listen interval in default power save mode
*/
#define STA_LISTEN_INTERVAL_DEFAULT             3

/**
* Listen interval in low power save mode
*/
#define STA_LISTEN_INTERVAL_LOW_PS_MODE         1


#endif
