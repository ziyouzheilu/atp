#ifdef __cplusplus
 extern "C" {
 #endif
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>  
#include <semaphore.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "rwif.h" 
#include "data.h"

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#  define ltohl(x)       (x)
#  define ltohs(x)       (x)
#  define htoll(x)       (x)
#  define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#  define ltohl(x)     __bswap_32(x)
#  define ltohs(x)     __bswap_16(x)
#  define htoll(x)     __bswap_32(x)
#  define htols(x)     __bswap_16(x)
#endif
#define MAP_SIZE 1024//(1024*2)
#define MAP_MASK (MAP_SIZE - 1)

#define FRAM_BUFADDR 0
#define VDMA_DATA_BYTES 4
#define FPGA_DDR_START_ADDR (0)

#define VIDEO_FRAME_STORE_NUM 3

void *control_base;

int c2h_dma_fd;

int control_fd;
int control_fd1;

int c2h_event_fd;
int c2h_event_fd1;

static int video_width=M_IMG_W/2+2;
static int video_height=M_IMG_H;

//static int video_width1=L_IMG_W /2 ;
//static int video_height1=L_IMG_H ;


static unsigned int c2h_fpga_ddr_addr[VIDEO_FRAME_STORE_NUM];
static unsigned int c2h_fpga_ddr_addr1[VIDEO_FRAME_STORE_NUM];

void timespec_sub(struct timespec *t1, const struct timespec *t2)
{
  assert(t1->tv_nsec >= 0);
  assert(t1->tv_nsec < 1000000000);
  assert(t2->tv_nsec >= 0);
  assert(t2->tv_nsec < 1000000000);
  t1->tv_sec -= t2->tv_sec;
  t1->tv_nsec -= t2->tv_nsec;
  if (t1->tv_nsec >= 1000000000)
  {
    t1->tv_sec++;
    t1->tv_nsec -= 1000000000;
  }
  else if (t1->tv_nsec < 0)
  {
    t1->tv_sec--;
    t1->tv_nsec += 1000000000;
  }
}
static int open_event(char *filename)
{
	int fd;
	fd = open(filename, O_RDWR | O_SYNC);
	if(fd == -1)
	{
		printf("open event error\n");
		return -1;
	}
	return fd;
}
static int read_event(int fd)
{
	int val;
	read(fd,&val,4);
	return val;
}
static int open_control(char *filename)
{
	int fd;
	fd = open(filename, O_RDWR | O_SYNC);
	if(fd == -1)
	{
		printf("open control error\n");
		return -1;
	}
	return fd;
}
static void *mmap_control(int fd,long mapsize)
{
	void *vir_addr;
	vir_addr = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	return vir_addr;
}
static void write_control(void *base_addr,int offset,uint32_t val)
{
	uint32_t writeval = htoll(val);
	*((uint32_t *)(base_addr+offset)) = writeval;
}
uint32_t read_control(int offset)
{
    uint32_t read_result = *((uint32_t *)(control_base+offset));
    read_result = ltohl(read_result);
    return read_result;
}

unsigned char src_m_image[M_IMG_H_SRC * M_IMG_W_SRC];

//static void get_video_from_fpga_ddr(unsigned int fpga_ddr_addr,unsigned char *buffer)
//{
//    lseek(c2h_dma_fd,fpga_ddr_addr,SEEK_SET);
//    static int num = 0;
//#if 1
//    num = read(c2h_dma_fd,buffer,video_height*video_width * 2);
// #else
//     num = read(c2h_dma_fd,src_m_image,M_IMG_H_SRC*M_IMG_W_SRC);
////     struct timespec ts_1, ts_2;
////     clock_gettime(CLOCK_MONOTONIC, &ts_1);
//     for(int i = 0; i < M_IMG_H;i ++)
//     {
//        memcpy(buffer + i * M_IMG_W, src_m_image + (i +  M_IMG_H / 2) * M_IMG_W_SRC  + M_IMG_W / 2,M_IMG_W);
//     }
////     clock_gettime(CLOCK_MONOTONIC, &ts_2);
////     timespec_sub(&ts_2, &ts_1);
////     float time = ts_2.tv_nsec / 1000000.0;

////    printf("num = %f\n",time);

//#endif


////     num ++;
////    if(num % 2)
////    {
////        memset(buffer,0xff,video_height*video_width);
////    }
////    else
////    {
////         memset(buffer,0x00,video_height*video_width);
////    }
//   // printf("num = %d\n",num);
//	//memcpy(buffer,c2h_align_mem,video_height*video_width*3);

//}

//char tempbuffer[IMGH*(IMGW + 1)*8];
char tempbuffer[1080*(962 + 1)*8];

int moveaddress = 960;
 static void get_video_from_fpga_ddr(unsigned int fpga_ddr_addr,unsigned char *buffer)
{
    lseek(c2h_dma_fd,fpga_ddr_addr,SEEK_SET);
    int num = 0;
   // num = read(c2h_dma_fd,buffer,video_height*video_width);

    num = read(c2h_dma_fd,tempbuffer,video_height*video_width*8);

    int nummmm = 0;
    for(int i = 0 ; i < video_height;i ++)
    {

        for(int j = 0 ; j < (video_width - 2);j ++)
        {


            buffer[i * 1920  * 3 +  j * 2 * 3   ] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8];
            buffer[i * 1920  * 3 +  j * 2 * 3+ 1] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8 + 1];
            buffer[i * 1920  * 3 +  j * 2 * 3+ 2] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8 + 2];
            buffer[i * 1920  * 3 +  j * 2 * 3+ 3] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8 + 3];
            buffer[i * 1920  * 3 +  j * 2 * 3+ 4] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8 + 4];
            buffer[i * 1920  * 3 +  j * 2 * 3+ 5] = tempbuffer[(i * video_width + j + moveaddress + nummmm * 2)*8 + 5];

            if(i % 95 == 0  && i >0)
            {
                // buffer[i * (IMGW - 2) + j] = tempbuffer[(i * IMGW + j + moveaddress  + nummmm * 2 +4)*8];
            }
        }
        if(i % 95 == 0  && i >0)
        {
             nummmm ++;
        }

    }

//    num = read(c2h_dma_fd,buffer + 20 * IMGH ,(video_height - 20) *video_width);

//    lseek(c2h_dma_fd,fpga_ddr_addr,(video_height - 20) *video_width);
//    num = read(c2h_dma_fd,buffer,20*video_width);
   // printf("num = %d\n",num);
    //memcpy(buffer,c2h_align_mem,video_height*video_width*3);

}


//static void get_video_from_fpga_ddr1(unsigned int fpga_ddr_addr,unsigned char *buffer)
//{
//    lseek(c2h_dma_fd,fpga_ddr_addr,SEEK_SET);
//    static int num = 0;
//    num = read(c2h_dma_fd,buffer,video_height1*video_width1 * 2);
//  if(num != 327680)
//     printf("num = %d\n",num);
//    //memcpy(buffer,c2h_align_mem,video_height*video_width*3);

////    static int num = 0;
////    num = read(c2h_dma_fd,buffer,video_height*video_width);
////     num ++;
////    if(num % 2)
////    {
////        memset(buffer,0xff,video_height1*video_width1);
////    }
////    else
////    {
////         memset(buffer,0x00,video_height1*video_width1);
////    }
//}


 struct timespec ts_1, ts_2;
 int pcie_read_frame(unsigned char *buffer)
 {
     int fps;
     uint32_t fbuf;
     static uint32_t fbuf_pre = 100;
     //clock_gettime(CLOCK_MONOTONIC, &ts_2);
     //timespec_sub(&ts_2, &ts_1);
     fps = 1000000000/ts_2.tv_nsec;
 //    fps = ts_2.tv_nsec / 1000000;
 //	clock_gettime(CLOCK_MONOTONIC, &ts_1);
 #if 1


     fbuf=read_control(0)&0x0f;
//     if(fbuf_pre == fbuf)
//         return 0;
//     fbuf_pre = fbuf;
//     if(fbuf==1)
//     {
//         fbuf=2;
//     }
//     else if(fbuf==2)
//     {
//         fbuf=0;
//     }
//     else
//     {
//         fbuf=1;
//     }

     if(fbuf==4) fbuf=1;
     else if(fbuf==2) fbuf = 0;
     else fbuf = 2;
     fbuf = 0;
     get_video_from_fpga_ddr(c2h_fpga_ddr_addr[fbuf],buffer);
 #else
     memset(buffer,0,M_IMG_W * M_IMG_H);
     static int mx = 270;
     static int my = 300;
     static int flag = 0;
    for(int i = mx - 5 ; i < mx + 5 ;i ++)
    {
        for(int j = my - 5 ; j < my + 5 ;j ++)
        {
            if(j > 120)
                 buffer[i * M_IMG_W + j ] = 255 - (j - my  + 5) * 20;
        }
    }

    for(int i = M_IMG_H - mx - 5 ; i < M_IMG_H - mx + 5 ;i ++)
    {
        for(int j = M_IMG_W - my - 5 ; j < M_IMG_W - my + 5 ;j ++)
        {
            buffer[i * M_IMG_W + j ] = 255 - (j - M_IMG_W + my + 5) * 20;
        }
    }

    if(flag == 0)
    {
        mx ++;
        if(mx > M_IMG_H - 250)
        {
           flag = 1;
        }
    }
    if(flag == 1)
    {
        my  ++;
        if(my > M_IMG_W - 250)
        {
           flag = 2;
        }
    }
    if(flag == 2)
    {
        mx  --;
        if(mx < 250)
        {
           flag = 3;
        }
    }
    if(flag == 3)
    {
        my --;
        if(my < 250)
        {
           flag = 0;
        }
    }
 #endif
     //clock_gettime(CLOCK_MONOTONIC, &ts_2);
    // timespec_sub(&ts_2, &ts_1);
     //    fps = 1000000000/ts_2.tv_nsec;
     //   fps = ts_2.tv_nsec / 1000000;

     return fps;
 }



static void gen_fpga_vdma_addr()
{
  int startaddress = 0 * 1024 * 1024;
  c2h_fpga_ddr_addr[0] = startaddress + 0;
  c2h_fpga_ddr_addr[1] = startaddress + 8388608  ;
  c2h_fpga_ddr_addr[2] = startaddress + 8388608 * 2;
}

static void gen_fpga_vdma_addr1()
{
  int startaddress = 0 * 1024 * 1024;

  c2h_fpga_ddr_addr1[0] = startaddress + 0;
  c2h_fpga_ddr_addr1[1] = startaddress + 8388608  ;
  c2h_fpga_ddr_addr1[2] = startaddress + 8388608 * 2;
}


//xdma0_events_0
int wait_for_s2mm_intr()
{
   //int fbuf = 0;

   //fbuf = read_control(0);
 //  read_event(c2h_event_fd);
 //  usleep(20000);
   //write_control(c2h_event_fd,0,1);
   {
     usleep(50000);
	return 1;
   }
}
//xdma0_events_1
int wait_for_s2mm_intr1()
{
   //int fbuf = 0;

   //fbuf = read_control(0);
   read_event(c2h_event_fd1);
   // read_event(c2h_event_fd);
 //  usleep(20000);
   //write_control(c2h_event_fd,0,1);
   {
  //   usleep(20000);
    return 1;
   }
}
void disp_reset()
{
}
void disp_start()
{
	usleep(1000);
}
int pcie_init()
{


    c2h_dma_fd = open("/dev/xdma0_c2h_0",O_RDONLY | O_NONBLOCK);
    if(c2h_dma_fd < 0)
        return -1;

    control_fd = open_control("/dev/xdma0_user");
    if(control_fd < 0)
        return -5;

   c2h_event_fd = open_event("/dev/xdma0_events_0");    //打开用户中断

   if(c2h_event_fd < 0)
       return -7;

    c2h_event_fd1 = open_event("/dev/xdma0_events_1");    //打开用户中断

   if(c2h_event_fd1 < 0)
    return -7;

   control_base = mmap_control(control_fd,MAP_SIZE);

    gen_fpga_vdma_addr();

    gen_fpga_vdma_addr1();

    return 1;
}
void pcie_deinit()
{
	close(c2h_dma_fd);
	close(control_fd);
}


#ifdef __cplusplus
}
#endif
