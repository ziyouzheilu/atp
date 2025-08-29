#include "data.h"
#include "memory.h"

//void TW_2_ON(char * srcdata, char * dstdata,int W,int H)
//{
//    int maxvalue = 0;
//    int minvulaue = 0x3fff;

//    static int maxvalue30 = 0;
//    static int minvalue30 = 0;

//    static int tempmaxvalue = 0;
//    static int tempminvalue = 0;

//    static int addnum = 0;
//    unsigned short *value0 = new unsigned short[H * W];
//    for(int i = 0;i < H ;i ++)
//    {
//        for(int j = 0;j < W ;j ++)
//        {
//            unsigned short value = (uchar)*(srcdata + (i * W + j) * 2 + 1) << 8|
//                        (char)*(srcdata + (i * W + j) * 2);
//            * (value0 + i * W + j) = value;
//            if( value > maxvalue)
//                maxvalue = value;
//            if(value < minvulaue)
//                minvulaue = value;
//        }
//    }
//    maxvalue30 = maxvalue30 + maxvalue;
//    minvalue30 = minvalue30 + minvulaue;
//    addnum ++;
//    if(tempmaxvalue == 0)
//    {
//        tempmaxvalue = maxvalue;
//        tempminvalue = minvulaue;
//    }

//    if(addnum >= 30)
//    {
//        maxvalue = maxvalue30 / 30;
//        minvulaue = minvalue30 / 30;
//        tempmaxvalue = maxvalue;
//        tempminvalue = minvulaue;
//    }
//    else
//    {
//        maxvalue = tempmaxvalue;
//        minvulaue = tempminvalue;
//    }
//    int Distance = maxvalue - minvulaue;
//    float tempvale = (1.0 / (Distance + 1)) * 255 ;
//    for(int i = 0;i < H ;i ++)
//    {
//        for(int j = 0;j < W ;j ++)
//        {
//            int num = i * W + j;

//            *(dstdata + num) = (*(value0 + num) - minvulaue) * tempvale;
//        }
//    }
//    delete[] value0;
//}
