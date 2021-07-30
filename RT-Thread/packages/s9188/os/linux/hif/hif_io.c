/*
 * hif_io.c
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "wf_os_api.h"
#include "hif.h"
#include "wf_debug.h"

int hif_io_write(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen)
{
    struct hif_node_ *hif_node = node;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    return hif_node->ops->hif_write(node, flag, addr,data,datalen);
}


int hif_io_read(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen)
{
    struct hif_node_ *hif_node = node;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_read != NULL);

    return hif_node->ops->hif_read(node, flag, addr,data,datalen);
}

unsigned char hif_io_read8(void *node, unsigned int addr, int *err)
{
    struct hif_node_ *hif_node = node;
    int ret = 0;
    wf_u8 value;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 1);
    if(err)
    {
        *err = ret;
    }
    return value;
}

unsigned long hif_io_read16(void *node, unsigned int addr,int *err)
{
    struct hif_node_ *hif_node = node;
    wf_u16 value;
    int ret = 0;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 2);
    if(err)
    {
        *err = ret;
    }
    return value;
}

unsigned int hif_io_read32(void *node, unsigned int addr, int *err)
{
    struct hif_node_ *hif_node = node;
    wf_u32 value;
    int ret = 0;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 4);
    if(err)
    {
        *err = ret;
    }

    return value;
}

int hif_io_write8(void *node, unsigned int addr, unsigned char value)
{
	struct hif_node_ *hif_node = node;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 1);
}

int hif_io_write16(void *node, unsigned int addr, unsigned long value)
{
    struct hif_node_ *hif_node = node;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 2);
}

int hif_io_write32(void *node, unsigned int addr, unsigned int value)
{
    struct hif_node_ *hif_node = node;

    WF_ASSERT(hif_node != NULL);
    WF_ASSERT(hif_node->ops != NULL);
    WF_ASSERT(hif_node->ops->hif_write != NULL);

    return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 4);
}

