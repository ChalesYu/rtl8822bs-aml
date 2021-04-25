#include "common.h"
#include "wf_debug.h"


#define WF_CLK_ADDR           0x0094     // bit2  M0 rst; bit1 bus enable; bit0 fclk enable
#define WF_EN_ADDR	 	      0x00e4


int wf_hw_mcu_disable(nic_info_st *nic_info)
{
    int ret;

	LOG_I("[%s] begin",__func__);
#ifndef CONFIG_RICHV200_FPGA
    ret = wf_io_write32(nic_info,WF_EN_ADDR,0);
    if (WF_RETURN_FAIL == ret )
    {
    	LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#endif
    wf_io_read32(nic_info,WF_CLK_ADDR,NULL);

	ret = wf_io_write32(nic_info,WF_CLK_ADDR,0);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

int wf_hw_mcu_enable(nic_info_st *nic_info)
{
    int ret;

    LOG_I("[%s] start",__func__);
    wf_io_read32(nic_info,WF_CLK_ADDR,NULL);
    LOG_I("[%s] end",__func__);

   ret = wf_io_write32(nic_info,WF_CLK_ADDR,0x6);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

#ifndef CONFIG_RICHV200_FPGA
    wf_io_read32(nic_info,WF_EN_ADDR,NULL);

    ret = wf_io_write32(nic_info,WF_EN_ADDR,0x1);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#endif

    return WF_RETURN_OK;
}


int wf_hw_mcu_startup(nic_info_st *nic_info)
{
    int ret;

#ifndef CONFIG_RICHV200_FPGA
    ret = wf_io_write32(nic_info,WF_EN_ADDR,0x2);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    ret = wf_io_write32(nic_info,WF_CLK_ADDR,0x7);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#else
    ret = wf_io_write32(nic_info,WF_CLK_ADDR,0x1f);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#endif
    return WF_RETURN_OK;
}


