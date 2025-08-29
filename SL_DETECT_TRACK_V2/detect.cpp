#include "detect.h"

#define DRAWLINE 0
struct target_info_list  target_detector1(Detector detector, Mat gray){
    struct target_info_list returninfo;
    auto det_image = detector.mat_to_image_resize(gray);
//       std::cout << det_image->w << std::endl;
//       std::cout << det_image->h << std::endl;
//       std::cout << mat_img.size()<< std::endl;
    double timeConsume,start;
    start = static_cast<double>(cv::getTickCount());
    std::vector<bbox_t> result_vec = detector.detect_resized(*det_image, gray.size().width, gray.size().height,0.4);
    timeConsume = ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
    qDebug()<<"time count:"<<timeConsume<<endl;
    int num=0;
    if (result_vec.empty()){
        returninfo.target_num=0;
        std::cout <<"未检测到目标！！！"<< std::endl;
        return returninfo;
    }else{
        for (auto &i : result_vec) {
//            cv::Scalar color = cv::Scalar(0, 0, 255);
//            cv::rectangle(gray, cv::Rect(i.x, i.y, i.w, i.h), color, 2);
            struct target_info info;
            info.info[0]=i.x;
            info.info[1]=i.y;
            info.info[2]=i.w;
            info.info[3]=i.h;
            returninfo.target_num=num+1;
            memcpy(&returninfo.T_info[num],&info,sizeof (struct target_info));
            num++;
            if(num==10){
                break;
            }
        }
        cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/test4.jpg",gray);
    }
    return returninfo;
}
struct target_info_list  target_detector(Mat gray,
                    float area_div,
                    float area_thr,
                    float a_r_thr,
                    float const_thr,
                    float res
                    )
{
    struct target_info_list returninfo;

   // area_div = seg_thres;
   // area_thr = 4;
   // a_r_thr = 0.5;
   // const_thr = 0.2;
   int sumtotal[2000];

//   imshow("gray",gray);
//   imwrite("test.jpg",gray);
   // Mat gray;
    //cv::cvtColor(Im_in,gray, COLOR_BGR2GRAY);
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

#if 0
   clc_Gradient(Scharrxy,sumtotal);
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
#else
   #if DRAWLINE
   clc_Gradient_X(Scharrxy,sumtotal);
   float templ[9];
   int r = 4;
   float sgm = 2;//0.8;
   gausskernel(templ,r,sgm);

   int  ave_num = gauss_Filtering_x(sumtotal,templ,Scharrxy.cols, r,res) + 3;

   Point p1(ave_num,0), p2(ave_num,Scharrxy.rows);

   int thickness = 2;

   // Line drawn using 8 connected
   // Bresenham algorithm
  // line(Im_in, p1, p2, Scalar(255, 255, 255),
   //     thickness, LINE_8);
#endif
#endif
   addWeighted(gray, 0.5, Scharrxy, 0.5, 0,gray_gxy);

  // int time_Diff = time.elapsed();
  // qDebug() << "time_Diff = " << time_Diff;

# if 0
   Mat zeroMat0= Mat::zeros(100,gray_gxy.cols,CV_8UC1);

   cv::Rect roi_rect0 = cv::Rect(0, 0, zeroMat0.cols, zeroMat0.rows);
   zeroMat0.copyTo(gray_gxy(roi_rect0));


   Mat zeroMat= Mat::zeros(gray_gxy.rows - ave_num,gray_gxy.cols,CV_8UC1);

   cv::Rect roi_rect = cv::Rect(0, ave_num, zeroMat.cols, zeroMat.rows);
   zeroMat.copyTo(gray_gxy(roi_rect));
#else

#if DRAWLINE
   Mat zeroMat= Mat::zeros(gray_gxy.rows,ave_num,CV_8UC1);

   cv::Rect roi_rect = cv::Rect(0, 0, ave_num, zeroMat.rows);
   zeroMat.copyTo(gray_gxy(roi_rect));
#endif
#endif

 //  imshow("img_pre",gray_gxy);

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
//        for (int j = 0;j < 256;j++)
//        {
//              qDebug() << hist.at<float>(255 - j);//histSize[255 - j];

//        }
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
//     imshow("dst0", dst);
    dilate(dst, dst, Mat());

   // imshow("dst0", dst);
    //cvWaitKey(20);


//    qDebug() <<"************438*************************";
    Mat labels, stats, centroids;
    size_t num_labels = connectedComponentsWithStats(dst, labels, stats, centroids, 8, CV_16U);

//      imshow("dst", dst);
    //  cvWaitKey(20);
//    qDebug() <<"************441*************************";
 //   for(int i = 0; i < num_labels;i++)
 //   {

//        qDebug()<<stats.at<int>(i,0)
//                  <<stats.at<int>(i,1)
//                    <<stats.at<int>(i,2)
//                      <<stats.at<int>(i,3)
//                        <<stats.at<int>(i,4);

 //   }
//    qDebug() <<"*************************************";

     bool sky_ground_flag = is_sky_ground(gray_gxy);

    QVector < struct target_info> stats_new;
     float img_mean=0.0, img_mean_xy=0.0;
    img_mean = sumElem(gray)*1.0/im_w/im_h;
    img_mean_xy = sumElem(gray_gxy)*1.0/im_w/im_h;
    for( int i = 1; i < num_labels; i++ )
    {
       float  w_h_r = min(stats.at<int>(i, cv::CC_STAT_WIDTH), stats.at<int>(i, cv::CC_STAT_HEIGHT)) * 1.0
               / max(stats.at<int>(i, cv::CC_STAT_WIDTH), stats.at<int>(i, cv::CC_STAT_HEIGHT));
       float a_r =stats.at<int>(i, cv::CC_STAT_AREA) * 1.0 / stats.at<int>(i, cv::CC_STAT_WIDTH)/ stats.at<int>(i, cv::CC_STAT_HEIGHT);
       float temp_M = stats.at<int>(i, cv::CC_STAT_WIDTH);
       float temp_H = stats.at<int>(i, cv::CC_STAT_HEIGHT);

       if(stats.at<int>(i,cv::CC_STAT_AREA) > area_thr * 100)
       {
            continue;
       }
       if((temp_M / temp_H) > 2 ||
          (temp_M / temp_H) < 0.5 ||
           stats.at<int>(i,0) < 10 ||
           stats.at<int>(i,1) < 10 ||
           stats.at<int>(i,0) > (gray_gxy.cols - 20) ||
           stats.at<int>(i,1) > (gray_gxy.rows - 20) ||
           w_h_r < 0.5)
       {
           continue;
       }

       int y =  int(stats.at<int>(i,1) + stats.at<int>(i,3)/2);
     int x =  int(stats.at<int>(i,0) + stats.at<int>(i,2)/2);
     int label = labels.at<int16_t>(y, x);

     if(label == 0)
     {
         //zrx   continue;
     }

       float const0=0.0, target_mean=0.0;//,img_mean=0.0;
       get_target_cons(gray, stats,labels,i,label, &const0, &target_mean,&img_mean);

       if(const0 < 0.2)
       {
           continue;
       }
       float const_xy=0.0, target_mean_xy=0.0;//, img_mean_xy=0.0;
       get_target_cons(gray_gxy, stats,labels,i, label,&const_xy, &target_mean_xy, &img_mean_xy);

       if(const_xy < 0.2)
       {
           continue;
       }
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
           if((a_r > a_r_thr) && const_xy > const_thr)
           {
                stats_new << info;
           }
       }
       else
       {
            if (stats.at<int>(i, cv::CC_STAT_AREA) > area_thr /*&&  a_r > a_r_thr*/)
            {
               stats_new << info;
            }
       }
    }
    //剔除重合的
//    qDebug()<< "stats_new.count() =" << stats_new.count();
    delete_iou_overlap(&stats_new,5);
    qDebug()<< "stats_new.count() =" << stats_new.count();

   int num_labels1 = stats_new.count();
   if(num_labels1 == 0)
   {
       returninfo.target_num = 0;
   }
   else if(num_labels1 == 1)
   {
        returninfo.target_num = 1;
        memcpy(returninfo.T_info[0].info,stats_new.at(0).info,sizeof (struct target_info));
   }
   else
   {
       if(num_labels1 > 10)
            {
                returninfo.target_num = 10;

                int target_seq[TARGET_LIST_NUM] ={-1,-1,-2,-3,-4,-5,-6,-7,-8,-9,};
                sqort_AREA(stats_new , target_seq,10);
                if(stats_new.at(target_seq[0]).info[10] > const_thr)
                {
                    for(int i = 0; i < returninfo.target_num;i++)
                    {
                        memcpy(returninfo.T_info[i].info,stats_new.at(target_seq[i]).info,sizeof (struct target_info));
                    }
                }
                else
                {
                    int target_seq[TARGET_LIST_NUM] ={-1,-1,-2,-3,-4,-5,-6,-7,-8,-9,};
                     sqort(stats_new , target_seq,10);

                     for(int i = 0; i < returninfo.target_num;i++)
                     {
                         memcpy(returninfo.T_info[i].info,stats_new.at(target_seq[i]).info,sizeof (struct target_info));
                     }
                }
            }
            else
            {
                returninfo.target_num = num_labels1;
                int target_seq[TARGET_LIST_NUM] ={-1,-1,-2,-3,-4,-5,-6,-7,-8,-9,};
                sqort_AREA(stats_new , target_seq,num_labels1);
                if(stats_new.at(0).info[10] > const_thr)
                {
                    for(int i = 0; i < returninfo.target_num;i++)
                    {
                        memcpy(returninfo.T_info[i].info,stats_new.at(target_seq[i]).info,sizeof (struct target_info));
                    }
                }
                else
                {
                    int target_seq[TARGET_LIST_NUM] ={-1,-1,-2,-3,-4,-5,-6,-7,-8,-9,};
                     sqort(stats_new , target_seq,num_labels1);

                     for(int i = 0; i < returninfo.target_num;i++)
                     {
                         memcpy(returninfo.T_info[i].info,stats_new.at(target_seq[i]).info,sizeof (struct target_info));
                     }
                }
            }


   }
   for(int i =0;i < num_labels1;i++)
   {
       Rect rect;
       rect.x = stats_new.at(i).info[0];
       rect.y = stats_new.at(i).info[1];
       rect.width = stats_new.at(i).info[2];
       rect.height = stats_new.at(i).info[3];
       //在输入图像上绘制矩形
      // rectangle(gray, rect, Scalar(0, 0,255), 3, 8,0);
   }
 //  qDebug() <<"************546*************************";
   //imshow("src",gray);
   //waitKey(2);
   return returninfo;

}

