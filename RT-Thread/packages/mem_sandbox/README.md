# RT-Thread 内存隔离沙箱

本沙箱改编自FreeRTOS heap4内存分配算法 (Dec 15, 2020)。



## 应用场景1 - 防止内存泄漏

例如，在向RT-Thread移植一些Linux的库时，由于Linux采用的是进程，在进程结束后，Linux会自动回收malloc出去的内存，因此一部分Linux软件库并不太注重内存的回收，此问题尤其表现在一些和终端相关的程序上。然而，RT-Thread并没有进程的概念，如果直接将Linux软件库直接移植到RT-Thread上，运行是可以运行，但是有可能存在内存泄漏的情况，而且泄漏位置很隐蔽，极难查找。因此本沙箱提供了一种比较快捷省事的解决内存泄漏的方案，即在运行Linux软件库之前，先创建一个沙箱，沙箱会申请一块内存，并接管这块内存。然后让Linux软件在沙箱中运行，由沙箱负责为给软件分配和释放内存，因此即便这个软件存在内存泄漏问题，也是在沙箱里边泄漏，等软件执行完，将沙箱一销毁，被沙箱接管的那一大块内存又交还给了RT-Thread，因此内存泄漏问题就直接解决了。

应用范例：[RT-Thread vi编辑器软件包](https://github.com/RT-Thread-packages/vi)  [RT-Thread ki编辑器软件包](https://github.com/mysterywolf/ki)



## 应用场景2 - 反内存碎片化

一些软件存在高频次，小内存分配释放，这种分配方式很容易导致RT-Thread管理的内存堆碎片化，为了解决这种问题，可以在一些反复申请释放内存的程序中使用本沙箱来进行反碎片化处理。先创建一个沙箱，将这类反复分配内存的代码交由沙箱分配内存，即便碎片化，也是在沙箱里边碎片化，等沙箱一销毁，又是一大块完整的内存交还给RT-Thread。因此内存碎片化问题可以得到有效遏制。



## API

```c
mem_sandbox_t mem_sandbox_create(rt_size_t sandbox_size); //创建一个沙箱
void mem_sandbox_delete(mem_sandbox_t p_sandbox); //销毁一个沙箱
void * mem_sandbox_malloc(mem_sandbox_t p_sandbox, rt_size_t xWantedSize); //从沙箱中分配内存 malloc
void *mem_sandbox_realloc(mem_sandbox_t p_sandbox, void * ptr, rt_size_t size); //从沙箱中分配内存 realloc
void *mem_sandbox_calloc(mem_sandbox_t p_sandbox, rt_size_t count, rt_size_t size); //从沙箱中分配内存 calloc
void mem_sandbox_free(mem_sandbox_t p_sandbox, void * pv); //从沙箱中释放内存 free
rt_size_t mem_sandbox_get_free_size(mem_sandbox_t p_sandbox); //获取沙箱当前剩余的内存大小(字节)
rt_size_t mem_sandbox_get_min_free_size(mem_sandbox_t p_sandbox); //获取沙箱历史最低剩余内存大小(字节)
void mem_sandbox_get_stats(mem_sandbox_t p_sandbox, HeapStats_t *pxHeapStats); //获取沙箱当前状态(一般不会用到)
char *mem_sandbox_strdup(mem_sandbox_t p_sandbox, const char *s); //沙箱版strdup函数
char *mem_sandbox_strndup(mem_sandbox_t p_sandbox, const char *s, size_t n); //沙箱版strndup函数
```



## 注意

1. 本沙箱可以初始化多个，互不干扰
2. 切记不要用沙箱的函数分配内存，然后用标准C库的free函数释放，一定要对应，用沙箱分配的内存就要用沙箱来回收。
3. 要特别注意`strdup()`和`strndup()`函数，这两个函数内部会调用标准C库函数来分配内存，因此在沙箱内运行的程序含有这两个函数的，要替换为`mem_sandbox_strdup()` 和 `mem_sandbox_strndup()`这两个函数。



## 示例

```c
#include <mem_sandbox.h>

sandbox = mem_sandbox_create(1024*10); //沙箱大小10KB

unsigned char *p = mem_sandbox_malloc(sandbox, 200); //从沙箱中分配200字节

mem_sandbox_free(sandbox, p); //将这块内存释放回沙箱中

mem_sandbox_delete(sandbox); //销毁这个沙箱，沙箱的10KB内存交回给RT-Thread进行管理
```



## 维护 & 联系

维护：[Meco Man](https://github.com/mysterywolf)

主页：https://github.com/mysterywolf/mem_sandbox
