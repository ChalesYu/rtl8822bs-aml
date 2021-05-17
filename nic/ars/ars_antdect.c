#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT
#if 0
#define ARS_AD_DBG(fmt, ...)      LOG_D("ARS_ANTDECT[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_AD_PRT(fmt, ...)      LOG_D("ARS_ANTDECT-"fmt,##__VA_ARGS__)

#else
#define ARS_AD_DBG(fmt, ...)
#define ARS_AD_PRT(fmt, ...) 

#endif

#define ARS_AD_INFO(fmt, ...)      LOG_I("ARS_ANTDECT-"fmt,##__VA_ARGS__)
#define ARS_AD_ERR(fmt, ...)      LOG_E("ARS_ANTDECT-"fmt,##__VA_ARGS__)


#endif

