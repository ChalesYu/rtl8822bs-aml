/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    mp_inc.h

Abstract:
    MP layer includes
    
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
#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4200)   // non-standard extension used
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4054)   // cast of function pointer to PVOID
#pragma warning(disable:4206)   // translation unit is empty


//
// For making calls into NDIS
//
#include <ndis.h>

//
// Miscellanous globals to improve code readability etc.
//
#include "Globals.h"
#include "Windot11.h"
#include "80211hdr.h"

#include "mp_dbg.h"
#include "mp_def.h"
#include "mp_pktq.h"
#include "mp_dot11.h"
#include "mp_intf.h"  

