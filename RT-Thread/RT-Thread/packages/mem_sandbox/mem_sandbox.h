/*
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     Meco Man     port to RT-Thread
 */

#ifndef __MEM_SANDBOX_H__
#define __MEM_SANDBOX_H__

#include <rtthread.h>

/* Define the linked list structure.  This is used to link free blocks in order
 * of their memory address. */
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK * pxNextFreeBlock; /*<< The next free block in the list. */
    rt_size_t xBlockSize;                     /*<< The size of the free block. */
} BlockLink_t;

struct mem_sandbox
{
    /* Create a couple of list links to mark the start and end of the list. */
    BlockLink_t xStart, * pxEnd;

    /* Keeps track of the number of calls to allocate and free memory as well as the
     * number of free bytes remaining, but says nothing about fragmentation. */
    rt_size_t xFreeBytesRemaining;
    rt_size_t xMinimumEverFreeBytesRemaining;
    rt_size_t xNumberOfSuccessfulAllocations;
    rt_size_t xNumberOfSuccessfulFrees;

    /* Gets set to the top bit of an rt_size_t type.  When this bit in the xBlockSize
     * member of an BlockLink_t structure is set then the block belongs to the
     * application.  When the bit is free the block is still part of the free heap
     * space. */
    rt_size_t xBlockAllocatedBit;

    /* Allocate the memory for the heap. */
    rt_uint8_t *ucHeap;

    rt_size_t xAvailableTotalHeapSize;
};
typedef struct mem_sandbox *mem_sandbox_t;

/* Used to pass information about the heap out of vPortGetHeapStats(). */
typedef struct xHeapStats
{
    rt_size_t xAvailableHeapSpaceInBytes;          /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    rt_size_t xSizeOfLargestFreeBlockInBytes;      /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    rt_size_t xSizeOfSmallestFreeBlockInBytes;     /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    rt_size_t xNumberOfFreeBlocks;                 /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    rt_size_t xMinimumEverFreeBytesRemaining;      /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    rt_size_t xNumberOfSuccessfulAllocations;      /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    rt_size_t xNumberOfSuccessfulFrees;            /* The number of calls to vPortFree() that has successfully freed a block of memory. */
} HeapStats_t;

mem_sandbox_t mem_sandbox_create(rt_size_t sandbox_size);
void mem_sandbox_delete(mem_sandbox_t p_sandbox);
void * mem_sandbox_malloc(mem_sandbox_t p_sandbox, rt_size_t xWantedSize);
void *mem_sandbox_realloc(mem_sandbox_t p_sandbox, void * ptr, rt_size_t size);
void *mem_sandbox_calloc(mem_sandbox_t p_sandbox, rt_size_t count, rt_size_t size);
void mem_sandbox_free(mem_sandbox_t p_sandbox, void * pv);
rt_size_t mem_sandbox_get_free_size(mem_sandbox_t p_sandbox);
rt_size_t mem_sandbox_get_min_free_size(mem_sandbox_t p_sandbox);
void mem_sandbox_get_stats(mem_sandbox_t p_sandbox, HeapStats_t *pxHeapStats);
char *mem_sandbox_strdup(mem_sandbox_t p_sandbox, const char *s);
char *mem_sandbox_strndup(mem_sandbox_t p_sandbox, const char *s, size_t n);

#endif
