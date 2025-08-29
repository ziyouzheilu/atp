#include "detecttrackmthread.h"
#include "memory.h"
#include "PacketFramePool.h"
#include "detector.h"
#include <sys/time.h>
#include "pcie_func.h"
#include "mainwindow.h"

extern QQueue<target_info_list> detect_target_queue;
extern QMutex detect_target_mutex;

extern QMutex mutexMDetect;
extern struct FrameLinkedList datalistMDetect;
extern int detect_queue_num;
extern int detect_state;
int M_target_x = 0;
int M_target_y = 0;
int M_target_w = 0;
int M_target_h = 0;


int frame_sum=0;
extern bool is_open_Mdetect ;

extern int detect_track_target_num;

extern int seletct_detect_target;
DetectTrackMthread::DetectTrackMthread()
{

}
void DetectTrackMthread::run()
{

    struct vb_frame_node *mM = NULL;
    Mat detectImage(M_IMG_H,M_IMG_W,CV_8UC1,imagedata);
    struct timespec ts_1, ts_2;
    static int detect_track = 0;//0检测 1 跟踪
   static int targetx = 0;
   static int targety = 0;
   static int targetw = 0;
   static int targeth = 0;

   //    std::string  names_file = "/home/nvidia/zht/newdxm/drone/drone.names";
   std::string  cfg_file = "/home/nvidia/zht/darknet_yolov7/newdxm/drone/yolov7-tiny.cfg";
   std::string  weights_file = "/home/nvidia/zht/darknet_yolov7/newdxm/drone/yolov7-tiny_drone.weights";
//   std::string  cfg_file = "/home/nvidia/Downloads/017/yolov7-tiny.cfg";
//   std::string  weights_file = "/home/nvidia/Downloads/017/yolov7-tiny_last.weights";

   //std::string  names_file = "/home/nvidia/zht/newdxm/ele/newdxm.names";
//   std::string  cfg_file = "/home/nvidia/zht/newdxm/ele/yolov7-tiny.cfg";
//   std::string  weights_file = "/home/nvidia/zht/newdxm/ele/yolov7-tiny_elm2.weights";

   Detector detector(cfg_file, weights_file);
   int count=1;
   int fId=0;
   double duration=0;
    while(1)
    {
        //std::printf("进入检测线程。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。");
        mM = get_frame_from_Node(&mutexMDetect,&datalistMDetect);
        if (mM != NULL)
        {
            //std::printf("图像有数据。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。");
            //cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/test2.jpg",detectImage);
            memcpy(imagedata, mM->data ,M_IMG_H * M_IMG_W);
            fId = mM->frame_id;
            free(mM->data);  //释放内存

            free(mM); //释放内存
//            cv::imshow("test",detectImage);
//            cv::waitKey(3);
            if(is_open_Mdetect == true)
            {
                //std::printf("detect_track == 0");
//                cv::imwrite("/home/nvidia/save_data/test_"+std::to_string(fId)+".jpg",detectImage);
                //struct target_info_list info = target_detector1(detector,detectImage);
                struct target_info_list returninfo;
                struct target_info_list showinfo;
                double start = cv::getTickCount();
                auto det_image = detector.mat_to_image_resize(detectImage);
                std::vector<bbox_t> result_vec = detector.detect_resized(*det_image, detectImage.size().width, detectImage.size().height,0.4);
                duration += ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
                std::cout << "detect ave time: " << duration/count << "\n";
                std::cout << "第" <<fId<<"帧在检测"<<"\n";
                int num=0;
                if (result_vec.empty()){
                    detect_state=0;
                    returninfo.target_num=0;
                    targetx = 0;
                    targety = 0;
                    targetw = 0;
                    targeth = 0;
                    std::cout <<"未检测到目标！！！"<< std::endl;
                }else{
                    detect_state=1;
                    for (auto &i : result_vec) {
                        struct target_info info;
                        info.info[0]=i.x+2;
                        info.info[1]=i.y+10;
                        info.info[2]=i.w;
                        info.info[3]=i.h;
                        info.info[4]=fId;
                        returninfo.target_num=num+1;
                        memcpy(&returninfo.T_info[num],&info,sizeof (struct target_info));
                        num++;
                        if(num==5){
                            break;
                        }
                    }
                }
                count++;
                struct target_info_list info_list;

                memcpy(&info_list,&returninfo,sizeof (struct target_info_list));
                putDetectTargettoQueue(&detect_target_mutex,&detect_target_queue, info_list);
                if(info_list.target_num==0)
                {
                    qDebug()<<"detect no target";
                }
                else
                {
                    int id=0;
                    if(info_list.target_num>1)
                    {
                        id = select_target(info_list,seletct_detect_target);
                     //   if(id<select_target0)id=0;
                    }

                    if(id<info_list.target_num)
                    {
                        targetx = info_list.T_info[id].info[0];
                        targety = info_list.T_info[id].info[1];
                        targetw = info_list.T_info[id].info[2];
                        targeth = info_list.T_info[id].info[3];

                        if(frame_sum<4)
                        {
                            frame_sum++;
                            continue;
                        }
                        else
                        {
                            emit signals_send_detect(targetx,targety,targetw,targeth, fId);
//                            std::cout<<"detect send result:"<<fId<<","<<targetx<<","<<targety<<","<<targetw<<","<<targeth<<std::endl;

                        }

                    }

                }
            }
        }
        else
        {
            targetx = 0;
            targety = 0;
            targetw = 0;
            targeth = 0;
            //std::printf("图像没有数据。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。");
            msleep(1);
        }
    }
}
