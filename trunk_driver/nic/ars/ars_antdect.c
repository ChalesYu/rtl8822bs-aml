
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT
#define ARS_AD_DBG(fmt, ...)    LOG_D("ARS_ANTDECT[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_AD_PRT(fmt, ...)    LOG_D("ARS_ANTDECT-"fmt,##__VA_ARGS__)
#define ARS_AD_INFO(fmt, ...)   LOG_I("ARS_ANTDECT-"fmt,##__VA_ARGS__)
#define ARS_AD_ERR(fmt, ...)    LOG_E("ARS_ANTDECT-"fmt,##__VA_ARGS__)

#endif

