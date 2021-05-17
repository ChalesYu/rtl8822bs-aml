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


#include <stdio.h>

#include <ntddk.h>
#include "usbdi.h"
#include "usbdlib.h"

// For making calls into NDIS
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
                   
// Miscellanous globals to improve code readability etc.
#include "Globals.h"
#include "nic_attr.h"

// Various functions and data structures provided by WiFi
#include "Windot11.h"

#pragma warning(disable:4214)  // bit field types other than int

#include "common.h"
#include "80211hdr.h"

#include "hw_def.h"


#include "mp_def.h"
#include "mp_dbg.h"
#include "mp_dot11.h"

#include "power_on.h"

#include "mp_dev.h"

#include "tx_windows.h"
#include "rx_windows.h"

#include "usb_dev.h"
#include "usb_reg.h"

#include "wf_ioctrl.h"
#include "wf_oids.h"
#include "wf_oids_adapt.h"
#include "wf_oids_sec.h"

#pragma warning(default:4214)

