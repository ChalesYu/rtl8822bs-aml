#ifndef __POWER_H__
#define __POWER_H__



int power_on(hif_node_st *node);
int power_off(hif_node_st *node);
int side_road_cfg(hif_node_st *node);
int power_suspend(hif_node_st *node);
int power_resume (hif_node_st *node);

#endif

