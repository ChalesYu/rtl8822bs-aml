/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_nictest,
    0xae6d6ce9,0x2664,0x4c8d,0x9d,0x92,0x21,0x92,0x1d,0xb1,0xae,0x00);
// {ae6d6ce9-2664-4c8d-9d92-21921db1ae00}
