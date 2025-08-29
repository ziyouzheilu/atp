#include "track.h"
#include "fdssttracker.hpp"


//#include <windows.h>
//#include <dirent.h>



using namespace std;
using namespace cv;

//std::vector <cv::Mat> imgVec;
//void initfirst(FDSSTTracker tracker,Mat initImg,Rect firstMessage){
//    int initX = firstMessage.x;
//    int initY = firstMessage.y;
//    int initWidth = firstMessage.width;
//    int initHegiht = firstMessage.height;
//    cv::Rect initRect = cv::Rect(initX, initY, initWidth, initHegiht);
//    tracker.init(initRect, initImg);
//    assert(shmdt(shm_ptr) == 0);


//}
//void track(Rect firstMessage,Mat initImg){
//    while(true){
//        bool HOG = true;
//        bool FIXEDWINDOW = false;
//        bool MULTISCALE = true;
//        bool SILENT = true;
//        bool LAB = false;
//        // Create KCFTracker object
//        FDSSTTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
//        int run_flag=0;
//        initfirst(tracker,track_init_Queue,init_shm_id);
//        run_flag=1;
//        double duration = 0;
//        int count = 1;
//        int last_id = -1;
//        while(true)
//        {
//            auto t_start = clock();
//            cv::Rect showRect;
//            if(track_init_Queue.empty()&&run_flag){
//                // 连接到共享内存
//                void* shm_ptr = shmat(shm_id, nullptr, 0);
//                assert(shm_ptr != (void*) -1);

//                // 使用OpenCV从JPEG字符串加载图像
//                unsigned char* shm_data = static_cast<unsigned char*>(shm_ptr);
//                int id = static_cast<int>(*shm_data); // 读取第一个字节作为ID
//                // 创建Mat对象来保存图像数据
//                cv::Mat processImg(1024, 1024, CV_8UC3, shm_data + 1); // 跳过第一个字节
//                if (processImg.empty()) {
//                    std::cerr << "Failed to decode the image from shared memory." << std::endl;
//                    continue;
//                }
//                if(id==last_id){
//                    continue;
//                }
//                last_id=id;
//                showRect = tracker.update(processImg);
//                auto t_end = clock();
//                duration += (double)(t_end - t_start) / CLOCKS_PER_SEC;
//                std::cout << "FPS: " <<count / duration << "\n";
//                std::vector<int> trackResult;
//                trackResult.push_back(id); // 存储ID
//                trackResult.push_back(showRect.x); // 存储X坐标
//                trackResult.push_back(showRect.y); // 存储Y坐标
//                trackResult.push_back(showRect.width); // 存储宽度
//                trackResult.push_back(showRect.height); // 存储高度
//                track_result_Queue.push(trackResult);
//                count++;
//                assert(shmdt(shm_ptr) == 0);

//            }else{
//                while(true){
//                    initfirst(tracker,track_init_Queue,init_shm_id);
//                    run_flag=1;
//                    break;
//                }

//            }

//        }
//        system("pause");
//    }


//}
struct target_info  target_track(Mat gray,
                    float area_div,
                    float area_thr,
                    float a_r_thr,
                    float const_thr,
                    float res
                    )
{
    struct target_info returninfo;

    memset(returninfo.info,0,sizeof(struct target_info));

    area_div = 0.005;
    area_thr = 4;
    a_r_thr = 0.5;
   int sumtotal[2000];

   // Mat gray;
   // cv::cvtColor(Im_in,gray, COLOR_BGR2GRAY);

//    Mat gray=Mat::zeros(gray0.rows,gray0.cols,CV_8UC1);



//    cv::Rect m_select = Rect(targetx - 100 ,targety -100,200 ,200);
//    Mat ROI = gray0(m_select);

//     ROI.copyTo(gray(m_select));

//     imshow("img_pre", gray);
//     cvWaitKey(20);

    // 对图像进行预处理
    Mat   Scharrx0 ,Scharry0;
 //   Scharr(gray, Scharrx, CV_64F, 1, 0, 1, 0, BORDER_DEFAULT);
 //   Scharr(gray, Scharry, CV_64F, 0, 1, 0, 1, BORDER_DEFAULT);
    Scharr(gray, Scharrx0, CV_64F, 1, 0);
    Scharr(gray, Scharry0, CV_64F, 0, 1);
    Mat   Scharrx ,Scharry;
    convertScaleAbs(Scharrx0,Scharrx);
    convertScaleAbs(Scharry0,Scharry);

//    clc_Gradient(Scharry,sumtotal);
//    float templ[9];
//    int r = 4;
//    float σ = 2;//0.8;
//    gausskernel(templ,r,σ);
//    int  ave_num = gauss_Filtering(sumtotal,templ,Scharry.rows, r) - 10;

//    Point p1(0,ave_num), p2(Scharry.cols,ave_num);

//    int thickness = 2;

//    // Line drawn using 8 connected
//    // Bresenham algorithm
//    line(Im_in, p1, p2, Scalar(255, 255, 255),
//         thickness, LINE_8);


    Mat   Scharrxy ,gray_gxy;
    addWeighted(Scharrx, 0.5, Scharry, 0.5, 0,Scharrxy);

    clc_Gradient(Scharrxy,sumtotal);
    #if 0
    float templ[9];
    int r = 4;
    float σ = 2;//0.8;
    gausskernel(templ,r,σ);
    int  ave_num = gauss_Filtering(sumtotal,templ,Scharrxy.rows, r,res) - 10;

    Point p1(0,ave_num), p2(Scharrxy.cols,ave_num);

    int thickness = 2;

    // Line drawn using 8 connected
    // Bresenham algorithm
    line(Im_in, p1, p2, Scalar(255, 255, 255),
         thickness, LINE_8);
#endif

    addWeighted(gray, 0.5, Scharrxy, 0.5, 0,gray_gxy);
#if 0
    Mat zeroMat0= Mat::zeros(100,gray_gxy.cols,CV_8UC1);

    cv::Rect roi_rect0 = cv::Rect(0, 0, zeroMat0.cols, zeroMat0.rows);
    zeroMat0.copyTo(gray_gxy(roi_rect0));


    Mat zeroMat= Mat::zeros(gray_gxy.rows - ave_num,gray_gxy.cols,CV_8UC1);

    cv::Rect roi_rect = cv::Rect(0, ave_num, zeroMat.cols, zeroMat.rows);
    zeroMat.copyTo(gray_gxy(roi_rect));
#endif
//    imshow("img_pre", gray_gxy);
//    cvWaitKey(20);

    Mat hist;//定义输出Mat类型

    int dims = 1;//设置直方图维度
    const int histSize[] = { 256 }; //直方图每一个维度划分的柱条的数目
    //每一个维度取值范围
    float pranges[] = { 0, 255 };//取值区间
    const float* ranges[] = { pranges };

    calcHist(&gray_gxy, 1, 0, Mat(), hist, dims, histSize, ranges);//计算直方图

    int t_count = 0;
    int obj_flag = 0;
    int thr_div = 0;
//    for (int j = 0;j < 256;j++)
//    {
//           t_count = t_count + hist.at<float>(255 - j);//histSize[255 - j];
//           if (t_count > area_div)
//           {
//               obj_flag = 1;
//               thr_div = 255 - j;
//               break;
//           }
//    }
    int im_w = gray.cols;
    int im_h = gray.rows;
    for (int j = 0;j < 256;j++)
    {
          t_count = t_count + hist.at<float>(255 - j);//histSize[255 - j];
          if (t_count > (im_w*im_h*area_div))
          {
              obj_flag = 1;
              thr_div = 255 - j;
              break;
          }
    }
    if( obj_flag == 0)
    {
        return returninfo;
    }
    Mat dst;
    double retval = threshold(gray_gxy,dst, thr_div, 255, 0);
    dilate(dst, dst, Mat());

//    imshow("dst", dst);
//    cvWaitKey(20);

//    Mat gray0=Mat::zeros(dst.rows,dst.cols,CV_8UC1);
//    cv::Rect m_select = Rect(targetx - 100 ,targety -100,200 ,200);
//    Mat ROI = dst(m_select);

//     ROI.copyTo(gray0(m_select));

//     dst = gray0.clone();

//     imshow("img_pre", dst);
//     cvWaitKey(20);


    Mat labels, stats, centroids;
    size_t num_labels = connectedComponentsWithStats(dst, labels, stats, centroids, 4, CV_16U);
 //   qDebug() <<"*************************************";
 //   for(int i = 0; i < num_labels;i++)
 //   {

//        qDebug()<<stats.at<int>(i,0)
//                  <<stats.at<int>(i,1)
//                    <<stats.at<int>(i,2)
//                      <<stats.at<int>(i,3)
//                        <<stats.at<int>(i,4);

  //  }
 //   qDebug() <<"*************************************";
    QVector < struct target_info> stats_new;
    for( int i = 1; i < num_labels; i++ )
    {
       float  w_h_r = min(stats.at<int>(i, cv::CC_STAT_WIDTH), stats.at<int>(i, cv::CC_STAT_HEIGHT)) * 1.0
               / max(stats.at<int>(i, cv::CC_STAT_WIDTH), stats.at<int>(i, cv::CC_STAT_HEIGHT));
       float a_r =stats.at<int>(i, cv::CC_STAT_AREA) * 1.0 / stats.at<int>(i, cv::CC_STAT_WIDTH)/ stats.at<int>(i, cv::CC_STAT_HEIGHT);
       float temp_M = stats.at<int>(i, cv::CC_STAT_WIDTH);
       float temp_H = stats.at<int>(i, cv::CC_STAT_HEIGHT);

       if((temp_M / temp_H) > 2 ||
          (temp_M / temp_H) < 0.5 ||
           stats.at<int>(i,0) < 10 ||
           stats.at<int>(i,1) < 10 ||
           stats.at<int>(i,0) > (gray_gxy.cols - 20))
       {
           continue;
       }

       int y =  int(stats.at<int>(i,1) + stats.at<int>(i,3)/2);
     int x =  int(stats.at<int>(i,0) + stats.at<int>(i,2)/2);
     int label = labels.at<int16_t>(y, x);

     if(label == 0)
     {
         //zrx continue;
     }

       float const0=0.0, target_mean=0.0,img_mean=0.0;
       get_target_cons(gray, stats,labels,i,label, &const0, &target_mean,&img_mean);
       float const_xy=0.0, target_mean_xy=0.0, img_mean_xy=0.0;
       get_target_cons(gray_gxy, stats,labels,i,label, &const_xy, &target_mean_xy, &img_mean_xy);

       struct target_info info;

       info.info[0] = stats.at<int>(i,0);
       info.info[1] = stats.at<int>(i,1);
       info.info[2] = stats.at<int>(i,2);
       info.info[3] = stats.at<int>(i,3);
       info.info[4] = stats.at<int>(i,4);
       info.info[5] = a_r;
       info.info[6] = w_h_r;
       info.info[7] = const0;
       info.info[8] = target_mean;
       info.info[9] = img_mean;
       info.info[10] = const_xy;
       info.info[11] = target_mean_xy;
       info.info[12] = img_mean_xy;

       if(stats.at<int>(i, cv::CC_STAT_AREA) > area_thr*10)
       {
          stats_new << info;
       }
       else
       {
            if (stats.at<int>(i, cv::CC_STAT_AREA) > area_thr &&  a_r > a_r_thr)
            {
               stats_new << info;
            }
       }
    }
   int num_labels1 = stats_new.count();
   if(num_labels1 == 0)
   {

   }
   else if(num_labels1 == 1)
   {
        memcpy(returninfo.info,stats_new.at(0).info,sizeof (struct target_info));
   }
   else
   {
        returninfo = sqort_max(stats_new);
   }
//   for(int i =0;i < num_labels1;i++)
//   {
//       Rect rect;
//       rect.x = stats_new.at(i).info[0];
//       rect.y = stats_new.at(i).info[1];
//       rect.width = stats_new.at(i).info[2];
//       rect.height = stats_new.at(i).info[3];
//       //在输入图像上绘制矩形
//       rectangle(gray, rect, Scalar(0, 0,255), 3, 8,0);
//   }
//   imshow("src111",gray);
//   waitKey(2);
   return returninfo;

}
