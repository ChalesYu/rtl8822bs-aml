#pragma once

#define delay_us(t)                     NdisStallExecution(t)
#define delay_ms(t)                     NdisStallExecution((t)*1000)
#define delay(t)                        delay_ms(t)

NTSTATUS 
HwReadMacSieRegister(
    __in PADAPTER adapter,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __out PVOID Buffer
    );

NTSTATUS 
HwWriteMacSieRegister(
    __in PADAPTER adapter,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer ,
    __in USHORT Index   
    );

UCHAR
HwPlatformIORead1Byte(
    PADAPTER adapter,
    ULONG      offset
    );

USHORT
HwPlatformIORead2Byte(
    PADAPTER adapter,
    ULONG      offset
    );

ULONG
HwPlatformIORead4Byte(
    PADAPTER adapter,
    ULONG      offset
    );


VOID
HwPlatformIOWrite1Byte(
    __in PADAPTER adapter,
    ULONG       Offset,
    UCHAR       ParamData
    )  ;

VOID
HwPlatformIOWrite2Byte(
    __in PADAPTER adapter,
    ULONG       Offset,
    USHORT      ParamData
    ) ;

VOID
HwPlatformIOWrite4Byte(
    __in PADAPTER adapter,
    ULONG       Offset,
    ULONG       ParamData
    ) ;

//
//  write macros
//  
#define HwPlatformEFIOWrite1Byte(_a,_b,_c)  HwPlatformIOWrite1Byte(_a,_b,_c)
            
#define HwPlatformEFIOWrite2Byte(_a,_b,_c)  HwPlatformIOWrite2Byte(_a,_b,(_c))
            
#define HwPlatformEFIOWrite4Byte(_a,_b,_c)  HwPlatformIOWrite4Byte(_a,_b,(_c))


//
//  read macros
//  
#define HwPlatformEFIORead1Byte(_a,_b)      HwPlatformIORead1Byte(_a,  _b)
#define HwPlatformEFIORead2Byte(_a,_b)      HwPlatformIORead2Byte(_a,  _b) 
#define HwPlatformEFIORead4Byte(_a,_b)      HwPlatformIORead4Byte(_a,  _b)
          

