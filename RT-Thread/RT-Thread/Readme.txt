软件主要基于RT Thread操作系统，进行开发。目录重组说明：

board：板级支持包，对应rt-thread目录下的bsp目录，存放一般板级支持驱动
components： 组件库，对应rt-thread目录components目录
hal：  硬件移植层，cortex-m库，具体芯片外设驱动库存放
include: 系统头文件，对应rt-thread目录的include目录
kernel：操作系统内核，对应rtt-thread目录的src目录
libcpu：cpu调度移植，对应特定cpu内核的调度移植代码
packages：在线软件包，这是rt-thread在线软件包的目录

当前RTT版本是：rt-thread-2020-06-23的github版本