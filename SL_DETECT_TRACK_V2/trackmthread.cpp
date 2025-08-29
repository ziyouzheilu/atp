#include "trackmthread.h"
#include "memory.h"
#include "PacketFramePool.h"
#include "detector.h"
#include "pcie_func.h"
#include"mainwindow.h"
#include <opencv2/opencv.hpp>
#include<opencv2/tracking.hpp>
extern int M_target_x ;
extern int M_target_y ;
extern int M_target_w ;
extern int M_target_h ;

extern int detect_track_state;
//extern bool is_open_MTrack;
//extern bool initFlag;

extern int seletct_detect_target;

QMutex mutexMDetectsend;
struct PackettargetinfoLinkedList datalistMDetectsend;

QMutex mutexMTrack;
struct FrameLinkedList datalistMTrack;

extern bool is_open_Mdetect;

extern int detect_track_target_num;
extern int init_state;
extern int track_state;

extern QQueue<target_info_list> detect_target_queue;
extern QMutex detect_target_mutex;

extern QMutex mutexMCache;
extern struct FrameLinkedList datalistMCache;

extern QQueue<target_info> track_target_queue;
extern QMutex track_target_mutex;
extern int track_queue_num;

int draw_image_show(cv::Mat trackImage, cv::Rect showRect)
{
    // 将单通道图像转换为三通道图像
    cv::Mat bgrImage;
    cv::cvtColor(trackImage, bgrImage, cv::COLOR_GRAY2BGR);
    // 设置框的颜色（例如）
    cv::Scalar boxColor = cv::Scalar(0, 255, 0);
    if(showRect.width>0||showRect.height>0)
    {
        cv::rectangle(bgrImage, showRect, boxColor, 2);
        //cv::imwrite("/home/nvidia/zht/track_res/"+std::to_string(count)+".jpg",bgrImage);
            std::printf("保存跟踪图像成功");
        // info =target_track(trackImage,count, initRect,tracker);
        cv::imshow("track",bgrImage);
        cv::waitKey(3);
    }
    return 0;
}



trackmthread::trackmthread()
{


}
void trackmthread::slots_recv_detect(int x,int y, int w, int h, int id)
{
    if(detect_track_state==1 && is_open_MTrack == false)
    {
        is_open_MTrack = true;
        targetx = x;
        targety = y;
        targetw = w;
        targeth = h;
        frame_id = id;
        return;
    }

}

void trackmthread::slots_recv_handle(int x,int y, int w, int h, int id)
{
    if(detect_track_state==2)
    {
        is_open_MTrack = true;
        targetx = x;
        targety = y;
        targetw = w;
        targeth = h;
        frame_id = id;
        initFlag = true;
    }

}


void trackmthread::run()
{

    struct vb_frame_node *mL = NULL;
    Mat trackImage(M_IMG_H,M_IMG_W,CV_8UC1,imagedata);

    int  track_target_num = 0;
   // static int targetx = 0;
   // static int targety = 0;

    int count=1;
    mL = get_frame_from_Node(&mutexMTrack,&datalistMTrack);
    bool HOG = true;
    bool FIXEDWINDOW = false;
    bool MULTISCALE = true;
    bool SILENT = true;
    bool LAB = false;

    double duration=0;
    int fId=0;
    int sId=-1;

    char* imgPath="/home/nvidia/zht/video/test/";
    char* imgName=NULL;
    // Create KCFTracker object
    FDSSTTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
    //cv::Ptr<cv::Tracker>tracker = cv::TrackerMIL::create();
//    cv::Ptr<TrackerKCF> tracker=cv::TrackerKCF::create();
    //cv::Ptr<cv::Tracker> tracker = cv::TrackerFactory::create("TrackerNano");


    while(1)
    {
        mL = get_frame_from_Node(&mutexMTrack,&datalistMTrack);

        if (mL != NULL)
        {
            track_target_num++;
            memcpy(imagedata, mL->data ,M_IMG_H *M_IMG_W);
            fId = mL->frame_id;
            free(mL->data);  //释放内存
            free(mL); //释放内存

            //add detect algorithm
            //cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/track.jpg",trackImage);
           // clock_gettime(CLOCK_MONOTONIC, &ts_1);
           // M_target_1
           if(detect_track_state==0)
           {
               is_open_MTrack=false;
               track_state=1;
               target_info info;
               memset(info.info,0,INFOLEN);
               putTrackTargettoQueue(&track_target_mutex,&track_target_queue, info);
               continue;
           }

           if (detect_track_state == 2)
           {
              // initFlag = true;

               target_info info;
               memset(info.info,0,INFOLEN);
               cv::Rect showRect;
               cv::Rect initRect;//(10,10,50,50);

               if (initFlag)
               {
//                   mL = get_frame_from_Node(&mutexMTrack, &datalistMTrack);

                   initRect.x = targetx-targetw/2;
                   initRect.y = targety-targeth/2;
                   initRect.width = targetw;
                   initRect.height = targeth;

                   if(!trackImage.empty())
                       tracker.init(initRect, trackImage);
                   else
                       continue;
                   //                       tracker->init(trackImage,initRect);
                   info.info[0] = initRect.x;
                   info.info[1] = initRect.y;
                   info.info[2] = initRect.width;
                   info.info[3] = initRect.height;
                   info.info[4] = fId;
//                   cv::rectangle(trackImage,initRect,cv::Scalar(0,0,255),2);
//                   cv::imwrite("testrect.jpg",trackImage);
                   //                       if(!sucess){
                   //                           is_open_MTrack=true;
                   //                           initFlag = true;
                   //                        }

                   putTrackTargettoQueue(&track_target_mutex, &track_target_queue, info);
                   initFlag = false;
               }
               else
               {
                   showRect = tracker.update(trackImage);
                   //                      bool sucess = tracker->update(trackImage,showRect);
                   std::cout<<"showRect:"<<showRect.x<<" "<<showRect.y<<" "<<showRect.width<<" "<<showRect.height<<std::endl;
                   info.info[0] = showRect.x;
                   info.info[1] = showRect.y;
                   info.info[2] = showRect.width;
                   info.info[3] = showRect.height;
                   info.info[4] = fId;
//                   cv::rectangle(trackImage,showRect,cv::Scalar(0,0,255),2);
//                   cv::imwrite("testshow.jpg",trackImage);
                   putTrackTargettoQueue(&track_target_mutex, &track_target_queue, info);
               }
               double track_cx=M_IMG_W/2,track_cy=M_IMG_H/2,sendx=0,sendy=0;
               track_cx = info.info[0]+(info.info[2])/2;
               track_cy = info.info[1]+(info.info[3])/2;
               convertToAngle(track_cx, track_cy, &sendx,&sendy);
               track_state = 0;
               emit send_target_data(sendx,sendy,fId);

               continue;

           }

           if (detect_track_state == 1)
           {
               if(is_open_MTrack == true)
               {
                   track_state=0;
                   if(targetx < 0) targetx = 0;
                   if(targety < 0) targety = 0;
                   if(targetw < 0) targetw = 0;
                   if(targeth < 0) targeth = 0;
                   if(targetx > M_IMG_W - 1- targetw) targetx = M_IMG_W - 1-targetw;
                   if(targety > M_IMG_H - 1- targeth) targety = M_IMG_H - 1-targeth;

                   target_info info;
                   memset(info.info,0,INFOLEN);
                   cv::Rect showRect;
                   cv::Rect initRect;//(10,10,50,50);
                   if(targetw==0||targeth==0)
                   {
                       initFlag=true;
    //                   continue;
                   }
                   else
                   {
                       double start = cv::getTickCount();
    //                   std::printf("第%d帧的initflag：%d\n",count,initFlag);
                       if(initFlag)
                       {
                           std::cout<<"进行初始frame_id:"<<fId<<std::endl;
                           std::cout<<"frame_id:"<<frame_id<<std::endl;
                           frame_id = fId;
                           if(fId!=frame_id)
                           {
                               target_info_list mt_info = match_frame_id_info(&detect_target_mutex,&detect_target_queue, frame_id);

                               if(mt_info.target_num>0)
                               {
                                   sId = select_target(mt_info, seletct_detect_target);
                               }
                               mL = get_frame_from_Node2(&mutexMCache,&datalistMCache, frame_id);

                               if(mL!=NULL&&sId>-1)
                               {
                                   std::cout<<"mix id is:"<<mL->frame_id<<std::endl;
                                   memcpy(imagedata, mL->data ,M_IMG_H *M_IMG_W);
                                   free(mL->data);  //释放内存
                                   free(mL); //释放内存
                                   initRect.x=mt_info.T_info[sId].info[0];
                                   initRect.y=mt_info.T_info[sId].info[1];
                                   initRect.width = mt_info.T_info[sId].info[2];
                                   initRect.height =mt_info.T_info[sId].info[3];

                               }
                               else
                               {
                                   initRect.x=targetx;
                                   initRect.y=targety;
                                   initRect.width = targetw;
                                   initRect.height = targeth;


                               }
                           }
                           else
                           {
                               initRect.x=targetx;
                               initRect.y=targety;
                               initRect.width = targetw;
                               initRect.height = targeth;
                           }

                           initFlag = false;
                           init_state=1;

    //                       snprintf(imgName, sizeof(imgName), "%s/%d.jpg", imgPath, fId);
    //                       std::cout<<imgName<<std::endl;
    //                       cv::imwrite("/home/nvidia/zht/video/test/track.jpg",trackImage);
                           tracker.init(initRect, trackImage);
                           //tracker->init(trackImage,initRect);
                           info.info[0]=initRect.x;
                           info.info[1]=initRect.y;
                           info.info[2]=initRect.width;
                           info.info[3]=initRect.height;
                           info.info[4]=fId;
    //                       if(!sucess){
    //                           is_open_MTrack=true;
    //                           initFlag = true;
    //                        }

                           putTrackTargettoQueue(&track_target_mutex,&track_target_queue, info);
                           //info =target_track(trackImage,count, initRect,tracker);
                       }
                       else
                       {
                          std::printf("第%d帧进行跟踪\n",fId);
                          showRect = tracker.update(trackImage);

                          //bool sucess = tracker->update(trackImage,showRect);
                          info.info[0]=showRect.x;
                          info.info[1]=showRect.y;
                          info.info[2]=showRect.width;
                          info.info[3]=showRect.height;
                          info.info[4]=fId;
                          putTrackTargettoQueue(&track_target_mutex,&track_target_queue, info);
    //                      draw_image_show(trackImage, showRect);
                          double track_cx=512,track_cy=512,sendx=0,sendy=0;
                          track_cx = (info.info[0]+info.info[2])/2;
                          track_cy = (info.info[1]+info.info[3])/2;
                          convertToAngle(track_cx, track_cy, &sendx,&sendy);
                          emit send_target_data(targetx,targety,fId);
                       }


                       duration += ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
                       std::cout << "track ave time: " << duration/count << "\n";
                       count++;
                       //struct target_info info;
                   }

                   targetx = info.info[0];
                   targety = info.info[1];
                   targetw = info.info[2];
                   targeth = info.info[3];


                   if(count%fix_num==0&&count>0)
                   {
                       std::cout <<"开始进行匹配。。。。。。。。。。。。"<<"\n";
                       int fix_result= match_track_detect_id_info(&detect_target_mutex,&detect_target_queue, &track_target_mutex, &track_target_queue);
                       if(fix_result==0)
                       {
                           std::cout <<"检测和跟踪匹配失败"<<"\n";
                           is_open_MTrack=false;
                           initFlag = true;
                           init_state=0;
                       }

                   }
                    if(targetx < 1 ||
                       targety < 1 ||
                       targetx > trackImage.cols - 1 ||
                       targety >  trackImage.rows - 1 ||
                            targetw == 0 ||
                            targetw == 0||
                       is_open_Mdetect == false)
                    {
                        is_open_MTrack = false;
                        initFlag = true;
                        init_state=0;
                    }
                    //add 2023-11-17


                    M_target_x = targetx;
                    M_target_y = targety;
                    M_target_w = targetw;
                    M_target_h = targeth;


                }
                else
                {
                    track_state=1;
                    M_target_x = 0;
                    M_target_y = 0;
                    M_target_w = 0;
                    M_target_h = 0;
                }
           }

//            if(count%fix_num==0&&count>0)
//            {
                
//            }
//            count++;
        }
        else
        {
//            free(mL); //释放内存
            msleep(1);
        }

    }
}
//        double delta_x, double delta_y, double fov_x, double fov_y, int res_x, int res_y) {
void trackmthread::convertToAngle(double x, double y, double *angle_x,double *angle_y)
{
 *angle_x = ( (x-opt_cx) / M_IMG_W_UP)*fov_x;
 *angle_y = ( (y-opt_cy) / M_IMG_H_UP)*fov_y;

////输出结果
//std::cout << "偏移量（"<< delta_x << ", " << delta_y << " ) 转换为角度： \n";
//std::cout << "X方向角度：" << angle_x << " 度\n;
//std::cout << "Y方向角度：" << angle_y << " 度\n;
}

