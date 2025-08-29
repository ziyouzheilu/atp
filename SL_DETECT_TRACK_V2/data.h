#ifndef DATA_H
#define DATA_H
#include "stdbool.h"

#define M_IMG_H_UP 1024
#define M_IMG_W_UP 1024

//#define M_IMG_H_SRC 1024
//#define M_IMG_W_SRC 1280
#define M_IMG_H_SRC 1024
#define M_IMG_W_SRC 1280

#define M_IMG_H 1080//1024
#define M_IMG_W 1920//1280


#define INFOLEN 6

#define NETPACKLEN 1400
#if 0
struct target_info {

    int target_x;
    int target_y;
    int target_h;
    int target_w;

};

struct image_target_info {

    int target_num;
    struct target_info info[100];
};
#endif

typedef int image_target_info;

#define TARGET_LIST_NUM 10

struct target_info {
    float info[INFOLEN];
};

struct target_info_list
{
    int target_num;
    struct target_info T_info[TARGET_LIST_NUM];
};
struct target_state_list
{
    int effectflag;
    int newtarget; //0 new  1 old
    struct target_info T_info[TARGET_LIST_NUM];
};

//void TW_2_ON(char *srcdata,char *dstdata,int W, int H);

#endif // DATA_H

