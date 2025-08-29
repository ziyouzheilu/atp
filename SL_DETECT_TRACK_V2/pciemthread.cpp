#include "pciemthread.h"
#include "pcie_func.h"
#include "PacketFramePool.h"
#include "data.h"
#include "qdebug.h"
#include "math.h"
#include "memory.h"
#include "detector.h"
#include <sys/time.h>

QMutex mutexMDetect;
struct FrameLinkedList datalistMDetect;

QMutex mutexMFFPMEG;
struct FrameLinkedList datalistMFFMPEG;


extern QMutex mutexMDetectsend;
extern struct PackettargetinfoLinkedList datalistMDetectsend;


extern QMutex mutexMTrack;
extern struct FrameLinkedList datalistMTrack;

QMutex mutexMCache;
struct FrameLinkedList datalistMCache;
std::vector<struct image_info> dataCache;
#define cacheNum 30


extern bool is_maul_auto;

extern int M_center_x ;
extern int M_center_y ;

extern int M_target_x;
extern int M_target_y ;
extern int M_target_w;
extern int M_target_h;


int get_image(unsigned char* buffer)
{
    cv::Mat image=cv::imread("/home/nvidia/Downloads/017/newdxm/img/dxm2frame_10.jpg");
    if(image.empty()){
        //printf("image is None");
        return 0;
    }else{
        cv::Mat rsimage;
        cv::resize(image,rsimage,cv::Size(M_IMG_W,M_IMG_H));
        cv::Mat grayimage;
        cv::cvtColor(rsimage,grayimage,cv::COLOR_BGR2GRAY);
        cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/test1.jpg",grayimage);
        memcpy(buffer,grayimage.data,M_IMG_W*M_IMG_H);
    }

//    cv::imshow("test", grayimage);
//    cv::waitKey(3000);

    // buffer=grayimage.data;
    return 0;
}
int get_video_image(cv::Mat image,unsigned char* buffer)
{
    if(image.empty()){
        //printf("image is None");
        return 0;
    }else{
        cv::Mat rsimage;
        cv::resize(image,rsimage,cv::Size(M_IMG_W,M_IMG_H));
        cv::Mat grayimage;
        cv::cvtColor(rsimage,grayimage,cv::COLOR_BGR2GRAY);
        cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/test1.jpg",grayimage);
        memcpy(buffer,grayimage.data,M_IMG_W*M_IMG_H);
    }

//    cv::imshow("test", grayimage);
//    cv::waitKey(3000);

    // buffer=grayimage.data;
    return 0;
}

pciemthread::pciemthread()
{


}
void pciemthread::run()
{

    bool is_video=true;
    struct vb_packet_targetinfo_node *m = NULL;
    int num = 0;
    float data[100];
    cv::VideoCapture video;
    video.open("/home/nvidia/zht/video/01.mp4");
    if( !video.isOpened() )
    {
        printf("Read Video failed!\n");
    }
    cv::Mat frame;
    struct vb_frame_node *mT = NULL;
    while(1)
    {
#if 1
       wait_for_s2mm_intr();
       char tempdata[M_IMG_H * (M_IMG_W)*3];
       int fps = pcie_read_frame((unsigned char *)tempdata);
       Mat tempImage(M_IMG_H,M_IMG_W,CV_8UC3,tempdata);


       cv::Mat rgb;
       cvtColor(tempImage,rgb,COLOR_BGR2RGB,0);
       cv::imwrite("rgb.jpg",rgb);

       for(int t=0;t<M_IMG_H * (M_IMG_W);t++)
       {
           imagedata[t]=tempdata[3*t];
       }

#else
        if(is_video){
            video >> frame;
            if( frame.empty() )
            {

                std::cout << "frame empty"<<std::endl;
                break;
            }else{
                int fps=get_video_image(frame,(unsigned char *)imagedata);
            }
        } else{
            int fps = get_image((unsigned char *)imagedata);
        }
#endif


//       if(datalistMCache.Frame_num<cacheNum)
//       {
////           std::cout<<"input num is :"<<num<<std::endl;
//           put_frame_to_Node(&mutexMCache,&datalistMCache,imagedata,M_IMG_H * M_IMG_W,num);

//       }
//       else
//       {
////            std::cout<<"input num is :"<<num<<std::endl;
////           remove_frame_from_Node(&mutexMCache,&datalistMCache, 0);

//           mT = get_frame_from_Node(&mutexMCache,&datalistMCache);
////           datalistMCache.Frame_num--;
////           if(mT!=NULL)
////           {
////               free(mT->data);
////               free(mT);
////           }

//           put_frame_to_Node(&mutexMCache,&datalistMCache,imagedata,M_IMG_H * M_IMG_W,num);
//       }
        if(dataCache.size()<cacheNum)
        {
            struct image_info imageM;
            imageM.image=tempImage.clone();
            imageM.frame_id=num;
            put_frame_to_cache(&mutexMCache,dataCache,imageM);
            tempImage.release();
        }
        else
        {
            struct image_info imageM;
            imageM.image=tempImage.clone();
            imageM.frame_id=num;
            remove_frame_to_cache(&mutexMCache,dataCache,imageM);
        }
        tempImage.release();
       if(num % 1 == 0)
       {
           //TW_2_ON(imagedata0,imagedata,M_IMG_W,M_IMG_H);

           if(datalistMDetect.Frame_num < 2)
           {
                put_frame_to_Node(&mutexMDetect,&datalistMDetect,imagedata,M_IMG_H * M_IMG_W,num);
           }
           if(datalistMTrack.Frame_num < 2)
           {
                put_frame_to_Node(&mutexMTrack,&datalistMTrack,imagedata,M_IMG_H * M_IMG_W,num);
           }
       }
       if(num % 2 == 0 )
       {
           if(datalistMFFMPEG.Frame_num < 2)
           {
                put_frame_to_Node(&mutexMFFPMEG,&datalistMFFMPEG,imagedata,M_IMG_H * M_IMG_W,num);
           }
       }

       num++;
    }
}


