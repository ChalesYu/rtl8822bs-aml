/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Precomp.h

Abstract:
    Hw layer precompiled header
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
//
// Globally disabled warnings
// These are superfluous errors at warning level 4.
//
#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include <stdio.h>

#include <ntddk.h>
#include "usbdi.h"
#include "usbdlib.h"

//
// For making calls into NDIS
//
#include <ndis.h>


#include <wdf.h>
#include <wdfusb.h>
#include <wdfworkitem.h>
#include <wdfdevice.h>
#include <Wdfcore.h>
#include <wdfrequest.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include <WdfMiniport.h>
#include <wdftimer.h>
                   
//
// Miscellanous globals to improve code readability etc.
//
#include "Globals.h"
#include "nic_attr.h"

//
// Various functions and data structures provided by WiFi
//
#include "Windot11.h"
#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)



#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int
#include "common.h"
#include "80211hdr.h"

#include "hw_def.h"

#include "mp_def.h"
#include "mp_dbg.h"
#include "mp_intf.h"
#include "mp_pktq.h"
#include "mp_dot11.h"
#include "mp_elem.h"
#include "mp_evnts.h"
#include "mp_util.h"
#include "mp_ioctl.h"
#include "mp_main.h"
#include "mp_oids.h"
#include "mp_recv.h"
#include "mp_send.h"
#include "mp_cntl.h"

#include "usb_reg.h"
#include "usb_main.h"
#include "usb_recv.h"
#include "usb_xmit.h"
#include "usb_io.h"

#include "hw_sec.h"
#include "bus_intf.h"
#include "hw_oids.h"
#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
WDF_REQUEST_SEND_OPTIONS syncRequset;
typedef struct _TIMER_NIC_CONTEXT {
	PADAPTER        adapter;
}   TIMER_NIC_CONTEXT, *PTIMER_NIC_CONTEXT;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TIMER_NIC_CONTEXT, GetTimerContext)
