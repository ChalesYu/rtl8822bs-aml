/*
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     Meco Man     port to RT-Thread
 */

/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that combines
 * (coalescences) adjacent memory blocks as they are freed, and in so doing
 * limits memory fragmentation.
 *
 * See heap_1.c, heap_2.c and heap_3.c for alternative implementations, and the
 * memory management pages of https://www.FreeRTOS.org for more information.
 */

#include "mem_sandbox.h"

#define portBYTE_ALIGNMENT          RT_ALIGN_SIZE

#if portBYTE_ALIGNMENT == 32
    #define portBYTE_ALIGNMENT_MASK    ( 0x001f )
#elif portBYTE_ALIGNMENT == 16
    #define portBYTE_ALIGNMENT_MASK    ( 0x000f )
#elif portBYTE_ALIGNMENT == 8
    #define portBYTE_ALIGNMENT_MASK    ( 0x0007 )
#elif portBYTE_ALIGNMENT == 4
    #define portBYTE_ALIGNMENT_MASK    ( 0x0003 )
#elif portBYTE_ALIGNMENT == 2
    #define portBYTE_ALIGNMENT_MASK    ( 0x0001 )
#elif portBYTE_ALIGNMENT == 1
    #define portBYTE_ALIGNMENT_MASK    ( 0x0000 )
#else
    #error "Invalid portBYTE_ALIGNMENT definition"
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE    ( ( rt_size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE         ( ( rt_size_t ) 8 )

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
 * block must by correctly byte aligned. */
static const rt_size_t xHeapStructSize = ( sizeof( BlockLink_t ) + ( ( rt_size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( rt_size_t ) portBYTE_ALIGNMENT_MASK );

/*-----------------------------------------------------------*/
/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList(mem_sandbox_t p_sandbox, BlockLink_t * pxBlockToInsert);

/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMalloc() is called.
 */
static void prvHeapInit(mem_sandbox_t p_sandbox) ;

/*-----------------------------------------------------------*/

void * mem_sandbox_malloc(mem_sandbox_t p_sandbox, rt_size_t xWantedSize) /*pvPortMalloc*/
{
    BlockLink_t * pxBlock, * pxPreviousBlock, * pxNewBlockLink;
    void * pvReturn = RT_NULL;

    rt_enter_critical();
    {
        /* If this is the first call to malloc then the heap will require
         * initialisation to setup the list of free blocks. */
        if(p_sandbox->pxEnd == RT_NULL)
        {
            prvHeapInit(p_sandbox);
        }

        /* Check the requested block size is not so large that the top bit is
         * set.  The top bit of the block size member of the BlockLink_t structure
         * is used to determine who owns the block - the application or the
         * kernel, so it must be free. */
        if((xWantedSize & p_sandbox->xBlockAllocatedBit) == 0)
        {
            /* The wanted size must be increased so it can contain a BlockLink_t
             * structure in addition to the requested amount of bytes. */
            if( ( xWantedSize > 0 ) &&
                ( ( xWantedSize + xHeapStructSize ) >  xWantedSize ) ) /* Overflow check */
            {
                xWantedSize += xHeapStructSize;

                /* Ensure that blocks are always aligned. */
                if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
                {
                    /* Byte alignment required. Check for overflow. */
                    if( ( xWantedSize + ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) ) )
                            > xWantedSize )
                    {
                        xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
                        RT_ASSERT( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 );
                    }
                    else
                    {
                        xWantedSize = 0;
                    }
                }
            }
            else
            {
                xWantedSize = 0;
            }

            if( ( xWantedSize > 0 ) && ( xWantedSize <= p_sandbox->xFreeBytesRemaining ) )
            {
                /* Traverse the list from the start (lowest address) block until
                 * one of adequate size is found. */
                pxPreviousBlock = &p_sandbox->xStart;
                pxBlock = p_sandbox->xStart.pxNextFreeBlock;

                while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != RT_NULL ) )
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }

                /* If the end marker was reached then a block of adequate size
                 * was not found. */
                if( pxBlock != p_sandbox->pxEnd )
                {
                    /* Return the memory space pointed to - jumping over the
                     * BlockLink_t structure at its start. */
                    pvReturn = ( void * ) ( ( ( rt_uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

                    /* This block is being returned for use so must be taken out
                     * of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                    /* If the block is larger than required it can be split into
                     * two. */
                    if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                    {
                        /* This block is to be split into two.  Create a new
                         * block following the number of bytes requested. The void
                         * cast is used to prevent byte alignment warnings from the
                         * compiler. */
                        pxNewBlockLink = ( void * ) ( ( ( rt_uint8_t * ) pxBlock ) + xWantedSize );
                        RT_ASSERT((((rt_size_t) pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

                        /* Calculate the sizes of two blocks split from the
                         * single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;

                        /* Insert the new block into the list of free blocks. */
                        prvInsertBlockIntoFreeList(p_sandbox, pxNewBlockLink);
                    }

                    p_sandbox->xFreeBytesRemaining -= pxBlock->xBlockSize;

                    if( p_sandbox->xFreeBytesRemaining < p_sandbox->xMinimumEverFreeBytesRemaining )
                    {
                        p_sandbox->xMinimumEverFreeBytesRemaining = p_sandbox->xFreeBytesRemaining;
                    }

                    /* The block is being returned - it is allocated and owned
                     * by the application and has no "next" block. */
                    pxBlock->xBlockSize |= p_sandbox->xBlockAllocatedBit;
                    pxBlock->pxNextFreeBlock = RT_NULL;
                    p_sandbox->xNumberOfSuccessfulAllocations++;
                }
            }
        }
    }
    rt_exit_critical();

    RT_ASSERT( ( ( ( rt_size_t ) pvReturn ) & ( rt_size_t ) portBYTE_ALIGNMENT_MASK ) == 0 );
    return pvReturn;
}
/*-----------------------------------------------------------*/

void mem_sandbox_free(mem_sandbox_t p_sandbox, void * pv) /*vPortFree*/
{
    rt_uint8_t * puc = ( rt_uint8_t * ) pv;
    BlockLink_t * pxLink;

    if( pv != RT_NULL )
    {
        /* The memory being freed will have an BlockLink_t structure immediately
         * before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = ( void * ) puc;

        /* Check the block is actually allocated. */
        RT_ASSERT((pxLink->xBlockSize & p_sandbox->xBlockAllocatedBit) != 0);
        RT_ASSERT(pxLink->pxNextFreeBlock == RT_NULL);

        if((pxLink->xBlockSize & p_sandbox->xBlockAllocatedBit) != 0)
        {
            if(pxLink->pxNextFreeBlock == RT_NULL)
            {
                /* The block is being returned to the heap - it is no longer
                 * allocated. */
                pxLink->xBlockSize &= ~p_sandbox->xBlockAllocatedBit;

                rt_enter_critical();
                {
                    /* Add this block to the list of free blocks. */
                    p_sandbox->xFreeBytesRemaining += pxLink->xBlockSize;
                    prvInsertBlockIntoFreeList(p_sandbox, (BlockLink_t *)pxLink);
                    p_sandbox->xNumberOfSuccessfulFrees++;
                }
                rt_exit_critical();
            }
        }
    }
}
/*-----------------------------------------------------------*/

rt_size_t mem_sandbox_get_free_size(mem_sandbox_t p_sandbox) /*xPortGetFreeHeapSize*/
{
    return p_sandbox->xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

rt_size_t mem_sandbox_get_min_free_size(mem_sandbox_t p_sandbox) /*xPortGetMinimumEverFreeHeapSize*/
{
    return p_sandbox->xMinimumEverFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

static void prvHeapInit(mem_sandbox_t p_sandbox) /* prvHeapInit */
{
    BlockLink_t * pxFirstFreeBlock;
    rt_uint8_t * pucAlignedHeap;
    rt_size_t uxAddress;
    rt_size_t xTotalHeapSize;

    /* Initialize global variables */
    p_sandbox->xStart.pxNextFreeBlock = RT_NULL;
    p_sandbox->xStart.xBlockSize = 0U;
    p_sandbox->pxEnd = RT_NULL;
    p_sandbox->xFreeBytesRemaining = 0U;
    p_sandbox->xMinimumEverFreeBytesRemaining = 0U;
    p_sandbox->xNumberOfSuccessfulAllocations = 0;
    p_sandbox->xNumberOfSuccessfulFrees = 0;
    p_sandbox->xBlockAllocatedBit = 0;

    xTotalHeapSize = p_sandbox->xAvailableTotalHeapSize;
    /* Ensure the heap starts on a correctly aligned boundary. */
    uxAddress = ( rt_size_t ) p_sandbox->ucHeap;

    if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
    {
        uxAddress += ( portBYTE_ALIGNMENT - 1 );
        uxAddress &= ~( ( rt_size_t ) portBYTE_ALIGNMENT_MASK );
        xTotalHeapSize -= uxAddress - ( rt_size_t ) p_sandbox->ucHeap;
    }

    pucAlignedHeap = ( rt_uint8_t * ) uxAddress;

    /* xStart is used to hold a pointer to the first item in the list of free
     * blocks.  The void cast is used to prevent compiler warnings. */
    p_sandbox->xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
    p_sandbox->xStart.xBlockSize = ( rt_size_t ) 0;

    /* pxEnd is used to mark the end of the list of free blocks and is inserted
     * at the end of the heap space. */
    uxAddress = ( ( rt_size_t ) pucAlignedHeap ) + xTotalHeapSize;
    uxAddress -= xHeapStructSize;
    uxAddress &= ~( ( rt_size_t ) portBYTE_ALIGNMENT_MASK );
    p_sandbox->pxEnd = ( void * ) uxAddress;
    p_sandbox->pxEnd->xBlockSize = 0;
    p_sandbox->pxEnd->pxNextFreeBlock = RT_NULL;

    /* To start with there is a single free block that is sized to take up the
     * entire heap space, minus the space taken by pxEnd. */
    pxFirstFreeBlock = ( void * ) pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize = uxAddress - ( rt_size_t ) pxFirstFreeBlock;
    pxFirstFreeBlock->pxNextFreeBlock = p_sandbox->pxEnd;

    /* Only one block exists - and it covers the entire usable heap space. */
    p_sandbox->xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
    p_sandbox->xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

    /* Work out the position of the top bit in a rt_size_t variable. */
    p_sandbox->xBlockAllocatedBit = ( ( rt_size_t ) 1 ) << ( ( sizeof( rt_size_t ) * heapBITS_PER_BYTE ) - 1 );
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(mem_sandbox_t p_sandbox, BlockLink_t * pxBlockToInsert)
{
    BlockLink_t * pxIterator;
    rt_uint8_t * puc;

    /* Iterate through the list until a block is found that has a higher address
     * than the block being inserted. */
    for( pxIterator = &p_sandbox->xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
    {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
     * make a contiguous block of memory? */
    puc = ( rt_uint8_t * ) pxIterator;

    if( ( puc + pxIterator->xBlockSize ) == ( rt_uint8_t * ) pxBlockToInsert )
    {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    }

    /* Do the block being inserted, and the block it is being inserted before
     * make a contiguous block of memory? */
    puc = ( rt_uint8_t * ) pxBlockToInsert;

    if( ( puc + pxBlockToInsert->xBlockSize ) == ( rt_uint8_t * ) pxIterator->pxNextFreeBlock )
    {
        if( pxIterator->pxNextFreeBlock != p_sandbox->pxEnd )
        {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = p_sandbox->pxEnd;
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
     * before and the block after, then it's pxNextFreeBlock pointer will have
     * already been set, and should not be set here as that would make it point
     * to itself. */
    if( pxIterator != pxBlockToInsert )
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
}
/*-----------------------------------------------------------*/

void mem_sandbox_get_stats(mem_sandbox_t p_sandbox, HeapStats_t * pxHeapStats) /*vPortGetHeapStats*/
{
    BlockLink_t * pxBlock;
    rt_size_t xBlocks = 0, xMaxSize = 0, xMinSize = RT_TICK_MAX; /* RT_TICK_MAX used as a portable way of getting the maximum value. */

    rt_enter_critical();
    {
        pxBlock = p_sandbox->xStart.pxNextFreeBlock;

        /* pxBlock will be RT_NULL if the heap has not been initialised.  The heap
         * is initialised automatically when the first allocation is made. */
        if( pxBlock != RT_NULL )
        {
            do
            {
                /* Increment the number of blocks and record the largest block seen
                 * so far. */
                xBlocks++;

                if( pxBlock->xBlockSize > xMaxSize )
                {
                    xMaxSize = pxBlock->xBlockSize;
                }

                if( pxBlock->xBlockSize < xMinSize )
                {
                    xMinSize = pxBlock->xBlockSize;
                }

                /* Move to the next block in the chain until the last block is
                 * reached. */
                pxBlock = pxBlock->pxNextFreeBlock;
            } while(pxBlock != p_sandbox->pxEnd);
        }
    }
    rt_exit_critical();

    pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;
    pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
    pxHeapStats->xNumberOfFreeBlocks = xBlocks;
    pxHeapStats->xAvailableHeapSpaceInBytes = p_sandbox->xFreeBytesRemaining;
    pxHeapStats->xNumberOfSuccessfulAllocations = p_sandbox->xNumberOfSuccessfulAllocations;
    pxHeapStats->xNumberOfSuccessfulFrees = p_sandbox->xNumberOfSuccessfulFrees;
    pxHeapStats->xMinimumEverFreeBytesRemaining = p_sandbox->xMinimumEverFreeBytesRemaining;
}

void *mem_sandbox_realloc(mem_sandbox_t p_sandbox, void * ptr, rt_size_t size)
{
    void * new_ptr;
    if(!size)
    {
        mem_sandbox_free(p_sandbox, ptr);
        return 0;
    }
    new_ptr=mem_sandbox_malloc(p_sandbox, size);
    if(!new_ptr)
        return 0;
    if(!ptr)
        return new_ptr;
    rt_memcpy(new_ptr,ptr,size);
    mem_sandbox_free(p_sandbox, ptr);
    return new_ptr;
}

void *mem_sandbox_calloc(mem_sandbox_t p_sandbox, rt_size_t count, rt_size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = mem_sandbox_malloc(p_sandbox, count * size);

    /* zero the memory */
    if (p)
        rt_memset(p, 0, count * size);

    return p;
}

char *mem_sandbox_strdup(mem_sandbox_t p_sandbox, const char *s)
{
    rt_size_t len = rt_strlen(s) + 1;
    char *tmp = (char *)mem_sandbox_malloc(p_sandbox, len);

    if (!tmp)
        return RT_NULL;

    rt_memcpy(tmp, s, len);

    return tmp;
}

char *mem_sandbox_strndup(mem_sandbox_t p_sandbox, const char *s, size_t n)
{
    char* new = mem_sandbox_malloc(p_sandbox, n+1);
    if (new)
    {
        rt_strncpy(new, s, n);
        new[n] = '\0';
    }
    return new;
}

mem_sandbox_t mem_sandbox_create(rt_size_t sandbox_size)
{
    mem_sandbox_t p_sandbox;

    if(sandbox_size == 0U)
    {
        return RT_NULL;
    }

    p_sandbox = rt_malloc(sizeof(struct mem_sandbox));
    if(p_sandbox == RT_NULL)
    {
        return RT_NULL;
    }
    rt_memset(p_sandbox, 0, sizeof(struct mem_sandbox));

    p_sandbox->ucHeap = rt_malloc(sandbox_size);
    if(p_sandbox->ucHeap == RT_NULL)
    {
        rt_free(p_sandbox);
        return RT_NULL;
    }

    p_sandbox->xAvailableTotalHeapSize = sandbox_size;
    p_sandbox->pxEnd = RT_NULL;

    return p_sandbox;
}

void mem_sandbox_delete(mem_sandbox_t p_sandbox)
{
    rt_free(p_sandbox->ucHeap);
    rt_free(p_sandbox);
}
