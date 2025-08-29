#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pcie_func.h"
#include "qdebug.h"
//#include <JetsonGPIO.h>
#include "QProcess"
#include <QThread>
#include "qmath.h"
#include "PacketFramePool.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>
#include <vector>
#include <opencv2/opencv.hpp>
#include <bitset>
#include"detector.h"
//using namespace GPIO;
//int uint1_gpio = 11;
//int uint2_gpio = 11;
//int uint3_gpio = 11;
//int uint4_gpio = 11;
//int uint5_gpio = 11;


QQueue<target_info_list> detect_target_queue;
QMutex detect_target_mutex;
int detect_queue_num=30;

QQueue<target_info> track_target_queue;
QMutex track_target_mutex;
int track_queue_num=30;

bool is_open_Mdetect = true;

//bool is_open_MTrack = false;
//bool initFlag=true;

int detect_track_flag=0;

int M_center_x = 320;
int M_center_y = 256;

int seletct_detect_target=0; //0 center target,1 max target
bool is_maul_auto = true;//ture  aotu  false maul

QString g_remote_ip;
unsigned short g_remote_port;
unsigned short g_local_port;

int detect_track_state=1;//0  wait,1 typical-track,2 manual track

extern int track_state=1; //0 start tracking ,1 stop tracking
extern int init_state=0; //0 init fail, 1 init success
extern int detect_state=0;  //0 detect fail ,1 detect success
int track_type=0;  //0 粗跟踪 1 精跟踪

char buf[1500] ={0xEE,0xC0};

void MainWindow::signals_recv_track(float x,float y, int data_count)
{
    tx = x;
    ty = y;
    ct = data_count;

    response();
    return;

}

void send_UDP_Image(cv::Mat image,int sock,struct sockaddr_in serverAddress){

//        double start=(double)cv::getTickCount();
        // 发送图像数据
        for (int i = 0; i < image.rows; ++i) {
//             double mem_start=(double)cv::getTickCount();
            // 构造数据包
            char buf[1500] ={0xBB,0xC0};// 包头

            // 数据长度
            uint16_t dataLength = 1034;

            buf[2] = dataLength >> 8 & 0xff;
            buf[3] = dataLength & 0xff;
            // 帧编号
            buf[4] = image.at<uchar>(1,0);
            buf[5] = image.at<uchar>(1,1);
            // 行计数
            buf[6] = i >> 8 & 0xff;
            buf[7] = i & 0xff;

            // 图像数据
//            for (int j = 0; j < image.cols; ++j) {
//                packetData.push_back(image.at<uint8_t>(i, j));
//            }

            uchar* src_add = image.ptr<uchar>(i);
            memcpy(buf + 8, src_add, image.cols);

            // 包尾
            buf[1032] = 0x00;
            buf[1033] = 0xBB;


            int sentBytes = sendto(sock,buf,1034,0,(struct sockaddr*)&serverAddress, sizeof(serverAddress));


            if (sentBytes == -1) {
//                std::cerr << "Error sending data" << std::endl;
            }
//            double mem_timeConsume = ((double)cv::getTickCount() - mem_start)*1000 / cv::getTickFrequency();
//            qDebug()<<"mem send time count:"<<mem_timeConsume;

        }
//        double timeConsume = ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
//        qDebug()<<"line send time count:"<<timeConsume;
//        timeConsume = ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
//        qDebug()<<"send time count:"<<timeConsume;

        // 关闭套接字
        //close(sock);
}

//atuo focus
char num_list[10]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
void MainWindow::auto_focus()
{
    char buf[30]={0x81,0x01,0x04,0x38,0x02,0xFF};

    //0x31,0x30,0x30,0x30,0x30,0x0d
    serialPort0->write(buf,6);
    serialPort0->waitForReadyRead(1000);

}

void MainWindow::auto_exp()
{
    char buf[30]={0x81,0x01,0x04,0x39,0x00,0xFF};

    //0x31,0x30,0x30,0x30,0x30,0x0d
    serialPort0->write(buf,6);
    serialPort0->waitForReadyRead(1000);

}

void MainWindow::set_zoom(int zoom)
{
    char buf[30]={0x81,0x01,0x04,0x47};

    switch(zoom)
    {
       case 1:
        buf[4]=0x00;
        buf[5]=0x00;
        buf[6]=0x00;
        buf[7]=0x00;
        break;
       case 2:

         buf[4]=0x01;
         buf[5]=0x03;
         buf[6]=0x01;
         buf[7]=0x01;
         break;
    case 3:

      buf[4]=0x01;
      buf[5]=0x0C;
      buf[6]=0x03;
      buf[7]=0x03;
      break;
    case 4:

      buf[4]=0x02;
      buf[5]=0x02;
      buf[6]=0x00;
      buf[7]=0x02;
      break;
    case 5:

      buf[4]=0x02;
      buf[5]=0x06;
      buf[6]=0x02;
      buf[7]=0x03;
      break;
    case 6:

      buf[4]=0x02;
      buf[5]=0x09;
      buf[6]=0x04;
      buf[7]=0x0E;
      break;
    case 7:

      buf[4]=0x02;
      buf[5]=0x0B;
      buf[6]=0x0D;
      buf[7]=0x0D;
      break;
    case 8:

      buf[4]=0x02;
      buf[5]=0x0D;
      buf[6]=0x0F;
      buf[7]=0x0F;
      break;
    case 9:

      buf[4]=0x02;
      buf[5]=0x0F;
      buf[6]=0x0D;
      buf[7]=0x02;
      break;
    case 10:

      buf[4]=0x03;
      buf[5]=0x01;
      buf[6]=0x06;
      buf[7]=0x08;
      break;
    case 11:

      buf[4]=0x03;
      buf[5]=0x02;
      buf[6]=0x0C;
      buf[7]=0x0E;
      break;
    case 12:

      buf[4]=0x03;
      buf[5]=0x04;
      buf[6]=0x00;
      buf[7]=0x0F;
      break;
    case 13:

      buf[4]=0x03;
      buf[5]=0x05;
      buf[6]=0x03;
      buf[7]=0x01;
      break;
    case 14:

      buf[4]=0x03;
      buf[5]=0x06;
      buf[6]=0x03;
      buf[7]=0x08;
      break;

    case 15:

      buf[4]=0x03;
      buf[5]=0x07;
      buf[6]=0x02;
      buf[7]=0x0A;
      break;
    case 16:

      buf[4]=0x03;
      buf[5]=0x08;
      buf[6]=0x00;
      buf[7]=0x08;
      break;
    case 17:

      buf[4]=0x03;
      buf[5]=0x08;
      buf[6]=0x0D;
      buf[7]=0x05;
      break;
    case 18:

      buf[4]=0x03;
      buf[5]=0x09;
      buf[6]=0x09;
      buf[7]=0x05;
      break;
    case 19:

      buf[4]=0x03;
      buf[5]=0x0A;
      buf[6]=0x04;
      buf[7]=0x07;
      break;
    case 20:

      buf[4]=0x03;
      buf[5]=0x0A;
      buf[6]=0x0E;
      buf[7]=0x0D;
      break;
    case 21:

      buf[4]=0x03;
      buf[5]=0x0B;
      buf[6]=0x08;
      buf[7]=0x0A;
      break;
    case 22:

      buf[4]=0x03;
      buf[5]=0x0C;
      buf[6]=0x01;
      buf[7]=0x0D;
      break;
    case 23:

      buf[4]=0x03;
      buf[5]=0x0C;
      buf[6]=0x0A;
      buf[7]=0x08;
      break;
    case 24:

      buf[4]=0x03;
      buf[5]=0x0D;
      buf[6]=0x02;
      buf[7]=0x0C;
      break;
    case 25:

      buf[4]=0x03;
      buf[5]=0x0D;
      buf[6]=0x0A;
      buf[7]=0x09;
      break;
    case 26:

      buf[4]=0x03;
      buf[5]=0x0E;
      buf[6]=0x02;
      buf[7]=0x02;
      break;
    case 27:

      buf[4]=0x03;
      buf[5]=0x0E;
      buf[6]=0x09;
      buf[7]=0x08;
      break;
    case 28:

      buf[4]=0x03;
      buf[5]=0x0F;
      buf[6]=0x00;
      buf[7]=0x0D;
      break;
    case 29:

      buf[4]=0x03;
      buf[5]=0x0F;
      buf[6]=0x08;
      buf[7]=0x05;
      break;
    case 30:

      buf[4]=0x04;
      buf[5]=0x00;
      buf[6]=0x00;
      buf[7]=0x00;
      break;
    }
    buf[8]=0XFF;
    //0x31,0x30,0x30,0x30,0x30,0x0d
    serialPort0->write(buf,9);
    serialPort0->waitForReadyRead(1000);

    auto_focus();

}

void MainWindow::set_focus(int focus)
{
    char buf[30]={0x81,0x01,0x04,0x48,0x00,0x00,0x00,0x00,0x00};

    switch(focus)
    {
       case 1:
        buf[4]=0x01;
        break;
       case 2:
         buf[4]=0x02;
         break;
    case 3:

      buf[4]=0x03;
      break;
    case 4:

      buf[4]=0x04;
      break;
    case 5:

      buf[4]=0x05;
      break;
    case 6:

      buf[4]=0x06;
      break;
    case 7:

      buf[4]=0x07;
      break;
    case 8:

      buf[4]=0x08;
      break;
    case 9:

      buf[4]=0x09;
      break;
    case 10:

      buf[4]=0x0A;
      break;
    case 11:

      buf[4]=0x0B;
      break;
    case 12:

      buf[4]=0x0C;
      break;
    case 13:

      buf[4]=0x0D;
      break;
    case 14:

     buf[4]=0x0E;
      break;

    case 15:

      buf[4]=0x0F;
      break;

    }
    buf[8]=0XFF;
    //0x31,0x30,0x30,0x30,0x30,0x0d
    serialPort0->write(buf,9);
    serialPort0->waitForReadyRead(1000);

//    auto_focus();

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_configFileName = QCoreApplication::applicationDirPath() + "/config.ini";
    m_settings = new QSettings(m_configFileName, QSettings::IniFormat, this);
    g_remote_ip = m_settings->value("/remote/ip").toString();
    g_remote_port = m_settings->value("/remote/port").toInt();
    g_local_port = m_settings->value("/local/port").toInt();

    if(pcie_init()<0)
    {
        printf("PCIe init error\n");
    }



#if 1
    serialPort1 = new QSerialPort();
    serialPort1->setPortName("/dev/ttyTHS0");
    if (!serialPort1->open(QIODevice::ReadWrite)) {
        qDebug() << "serial1 port open failed";
    }
    serialPort1->setBaudRate(230400, QSerialPort::AllDirections);
    serialPort1->setDataBits((QSerialPort::Data8));
    serialPort1->setFlowControl(QSerialPort::NoFlowControl);
    serialPort1->setParity(QSerialPort::NoParity);
    serialPort1->setStopBits(QSerialPort::OneStop);

    connect(serialPort1,SIGNAL(readyRead()),this,SLOT(receiveInfo2()));

     //模拟发送数据
    SendTimer = new QTimer();
    connect(SendTimer,SIGNAL(timeout()),this,SLOT(response()));
    SendTimer->start(1000);
//    char buf[100]={0xaa,0xc0,0x00,0x11,0x01,0x00,0x08,0x02,0x00,0xff};
//    serialPort1->write(buf,100);
//    serialPort1->waitForReadyRead(1000);
    serialPort0 = new QSerialPort();
    serialPort0->setPortName("/dev/ttyTHS1");
    if (!serialPort0->open(QIODevice::ReadWrite)) {
        qDebug() << "serial0 port open failed";
    }
    serialPort0->setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections);
    serialPort0->setDataBits((QSerialPort::Data8));
    serialPort0->setFlowControl(QSerialPort::NoFlowControl);
    serialPort0->setParity(QSerialPort::NoParity);
    serialPort0->setStopBits(QSerialPort::OneStop);
    auto_exp();
    auto_focus();
//    connect(serialPort0,SIGNAL(readyRead()),this,SLOT(receiveInfo2()));
#endif



   detectTrackMthread = new DetectTrackMthread() ;

   FfmpegMthread = new ffmpegMthread () ;

   Pciemthread = new pciemthread() ;

   Trackmthread = new trackmthread() ;
   connect(detectTrackMthread,SIGNAL(signals_send_detect(int,int,int,int,int)),Trackmthread,SLOT(slots_recv_detect(int,int,int,int,int)));
   //connect(detectTrackMthread,SIGNAL(signals_send_detect_2(int,int)),Trackmthread,SLOT(slots_recv_detect_2(int,int)));
   connect(this,SIGNAL(signals_send_handle(int,int,int,int,int)),Trackmthread,SLOT(slots_recv_handle(int,int,int,int,int)));

  // connect(Trackmthread,SIGNAL(signals_send_track(float,float,int)),this,SLOT(signals_recv_track(float,float,int)));
   connect(Trackmthread,SIGNAL(send_target_data(float,float,int)),this,SLOT(signals_recv_track(float,float,int)));

   detectTrackMthread ->start();

   FfmpegMthread->start();

   Pciemthread ->start();
   qDebug() << "读图进程已启动。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。";

   //Tracklthread ->start();
   Trackmthread ->start();
  // imguploadthread->start();




//   udpsocket_pc = new QUdpSocket();
//   udpsocket_pc->bind(9000,QUdpSocket::ShareAddress);
//   connect(udpsocket_pc, SIGNAL(readyRead()), this, SLOT(UDP_PC_command_prase()));


//   GPIO::setmode(GPIO::BOARD);
//   GPIO::setup(uint1_gpio,GPIO::OUT);
//   GPIO::setup(uint2_gpio,GPIO::OUT);
//   GPIO::setup(uint3_gpio,GPIO::OUT);
//   GPIO::setup(uint4_gpio,GPIO::OUT);
//   GPIO::setup(uint5_gpio,GPIO::OUT);

     //MainWindow::response();

}
//void MainWindow::GetSystemInfo(void)
//{
//    /*1. 获取当前系统磁盘使用情况*/
//       /*
//        * 格式:  /dev/sda1        49G   38G  9.3G   81% /
//        */
//    QProcess process;
//    process.start("df -h");
//    process.waitForFinished();
//    QByteArray output = process.readAllStandardOutput();
//    QString str_output = output;
//    str_output=str_output.mid(str_output.indexOf("/dev/sda1"));
//    //得到: /dev/sda1        49G   38G  9.3G   81%
//    str_output=str_output.section('/',0,2);
//    str_output=str_output.section(' ',1);
//    //将多个空格换成单个空格
//    str_output=str_output.replace(QRegExp("[\\s]+"), " ");


//    QTimer::singleShot(1000, this, SLOT(GetSystemInfo()));
//}

MainWindow::~MainWindow()
{
    serialPort0->close();
    serialPort1->close();
    delete ui;
}



//void MainWindow::receiveInfo0()
//{
//    QByteArray tempbuffer;
//    static char databuf[1024];
//    static int EffectiveLength = 0;
//   // tempbuffer.append(serialPort0->readAll());

//    if(!tempbuffer.isEmpty())//如果非空说明有数据接收
//    {
//            memcpy(databuf + EffectiveLength,tempbuffer,tempbuffer.size());

//            int length = EffectiveLength + tempbuffer.size();
//            tempbuffer.clear();
//    }
//}
void MainWindow::response()
{
    // 初始化buffer大小，确保它可以容纳所有的字节
    //假设前7位是固定的
    char buffer[25] = {0xCC, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, // 为image_counter预留空间
                                0x00, 0x00, // 为data_counter预留空间
                                0x40, 0x90, 0x60, 0x33, 0x1A,
                                0x40, 0x90, 0x60, 0x33, 0x1A,
                                0xFF, 0xFF, 0xFF, 0xBB};

    char buf[25] ={0xaa,0xc0,0x00,0x11,0x01,0x02,0x08,0x02,0x00,0xff};
//    while(true){
    if(track_type==0){  //粗跟踪
        buffer[2]=0x01;
    }else if(track_type==1){  //精跟踪
        buffer[2]=0x02;
    }
    std::string state1(8,'0');
    if(detect_track_state==0){  //待机
        std::cout<<"进入待机状态"<<endl;
        state1[6]='1';
    }else if(detect_track_state==1){  //典型跟踪
        state1[5]='1';
    }else if(detect_track_state==2){  //手动跟踪
        state1[4]='1';
    }
    buffer[3]=binaryToChar(state1);

    std::string state2(8,'0');
    if(track_state==0){   //正常跟踪
        std::cout<<"进入正常跟踪"<<endl;
        state2[5]='1';
    }else if(track_state==1){  //跟踪失败
        std::cout<<"进入跟踪失败"<<endl;
        state2[3]='1';
    }
    if(detect_state==1){ //检测成功
        std::cout<<"进入检测成功"<<endl;
        state2[7]='1';
    }
    if(init_state==1){
        state2[6]='1';
    }
    std::cout<<"state2:"<<state2<<endl;
    buffer[4]=binaryToChar(state2);
    buffer[5]=0x00;

    int image_counter = 239;
    int data_counter = 240;

    // 图像计数器
//    buffer[6] = image_counter & 0x7F; // 低字节
//    buffer[7] = (image_counter >> 7) & 0x7F; // 高字节



    // 数据计数器
    buffer[9] = ct & 0x7F; // 低字节
    buffer[10] = (ct >> 7) & 0x7F; // 高字节

    // 这里假设固定脱靶量和其他部分是固定的，不需要改变
    //脱靶量1
//    float data1=10.5;
    int temp=*(int*)&tx;

    buffer[11]=int(temp) & 0x7f;
    buffer[12]=int(temp)>>7 & 0x7f;
    buffer[13]=int(temp)>>14 & 0x7f;
    buffer[14]=int(temp)>>21 & 0x7f;
    buffer[15]=int(temp)>>28 & 0x7f;

    //脱靶量2
//    float data2=20.7;
    temp=*(int*)&ty;

    buffer[16]=int(temp) & 0x7f;
    buffer[17]=int(temp)>>7 & 0x7f;
    buffer[18]=int(temp)>>14 & 0x7f;
    buffer[19]=int(temp)>>21 & 0x7f;
    buffer[20]=int(temp)>>28 & 0x7f;

    // 发送缓冲区中的数据到串口
    serialPort1->write(buffer, 25);

}
unsigned char MainWindow::binaryToChar(const std::string& binary) {
    // 确保长度为8位
    int length = binary.length();
    int padding = (8 - length % 8) % 8;
    std::string paddedBinary = std::string(padding, '0') + binary;

    // 将二进制字符串转换为整数值
    unsigned char value = std::bitset<8>(paddedBinary).to_ulong();

    return value;
}
void MainWindow::receiveInfo2()
{

       QByteArray tempbuffer;
       static char databuf[1024];
       tempbuffer.append(serialPort1->readAll());
       if(!tempbuffer.isEmpty())//如果非空说明有数据接收
       {
               memcpy(databuf,tempbuffer,tempbuffer.size());
               printf("recive data:%x\n",databuf);
               //解析串口数据
               //判断帧头
               if((uchar)databuf[0] == 0xAA && (uchar)databuf[1] == 0xC0)
               {
                   if((uchar)databuf[5] == 0x01){
                       //粗跟踪
                       track_type=0;
                       if((uchar)databuf[6] == 0x06)
                       {
                           //设备参数设置
                           if((uchar)databuf[7] == 0x03){
                               int result=0;
                               int value=databuf[13]<<24|databuf[12]<<16 | databuf[11]<<8 | databuf[10];
                               result = *((int*)&value);
                               if(result>0&&result<=15)
                                   set_focus(result);
                               std::cout << "实现调焦 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x04){

                               int result=0;
                               int value=databuf[13]<<24|databuf[12]<<16 | databuf[11]<<8 | databuf[10];
                               result = *((int*)&value);
                               if(result>0&&result<=30)
                                   set_zoom(result);
                               std::cout << "实现变倍" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x05){
//                               uint32_t result=0;
//                               for(int t=0;t<4;t++)
//                               {
//                                   result |= static_cast<uint32_t>(databuf[11+t])<<(8*t);
//                               }
                               int result=0;
                               int value=databuf[13]<<24|databuf[12]<<16 | databuf[11]<<8 | databuf[10];
                               result = *((int*)&value);
//                               set_expTime(result);
                               std::cout<<"result:"<<result<<std::endl;

                               std::cout << "实现曝光 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x06){

                               std::cout << "实现光学中心设置 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x08){
                               std::cout << "实现服务器参数设置 " << std::endl;
                           }
                       }
                       //跟踪设置
                       if((uchar)databuf[6] == 0x08)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "待机 " << std::endl;
                               detect_track_state=0;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "粗跟踪自动跟踪" << std::endl;
                               detect_track_state = 1;

                               if((uchar)databuf[11]==0x01)
                               {
                                   seletct_detect_target = 0;
                               }
                               else
                               {
                                   seletct_detect_target = 1;
                               }
    //                           initFlag=true;
    //                           is_open_MTrack = false;
                           }
                           if((uchar)databuf[7] == 0x03){
    //                           is_open_MTrack = true;
    //                           initFlag = true;
                               int targetx,targety,targetw,targeth, id;

                               id=(static_cast<int>(databuf[8])<<8)|static_cast<int>(databuf[9]);

                               targetx=(static_cast<int>(databuf[11])<<8)|static_cast<int>(databuf[10]);
                               targety=(static_cast<int>(databuf[13])<<8)|static_cast<int>(databuf[12]);
                               targetw=(static_cast<int>(databuf[15])<<8)|static_cast<int>(databuf[14]);
                               targeth=(static_cast<int>(databuf[17])<<8)|static_cast<int>(databuf[16]);
                               detect_track_state = 2;
                               targetx=targetx*1.778;
                               targety = (targety-224)*1.778;
                               targetw = targetw*1.778;
                               targeth=targeth*1.778;
                               if(targetx<0)targetx=0;
                               if(targety<0)targety=0;
                               if(targetx+targetw>=M_IMG_W)targetw=M_IMG_W-targetx;
                               if(targety+targeth>=M_IMG_H)targeth=M_IMG_H-targety;
                               emit signals_send_handle(targetx,targety,targetw,targeth, id);
                               std::cout << "手动跟踪 " << std::endl;
                           }
                       }
                       //检测参数设置
                       if((uchar)databuf[6] == 0x12)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "检测方法1" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "检测方法2" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x03){
                               std::cout << "检测方法3 " << std::endl;
                           }
                       }
                       //跟踪参数设置
                       if((uchar)databuf[6] == 0x13)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "跟踪方法1" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "跟踪方法2" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x03){
                               std::cout << "跟踪方法3 " << std::endl;
                           }
                       }
                       //图像传输设置
                       if((uchar)databuf[6] == 0x14)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "开始传输图像" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "停止传输图像" << std::endl;
                           }
                       }
                   }
                   if((uchar)databuf[5] == 0x02){
                       //精跟踪
                       track_type=1;
                       if((uchar)databuf[6] == 0x06)
                       {
                           //设备参数设置
                           if((uchar)databuf[7] == 0x03){
                               std::cout << "实现调焦 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x04){


                               std::cout << "实现变倍" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x05){
                               std::cout << "实现曝光 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x06){
                               std::cout << "实现光学中心设置 " << std::endl;
                           }
                           if((uchar)databuf[7] == 0x08){
                               std::cout << "实现服务器参数设置 " << std::endl;
                           }
                       }
                       //跟踪设置
                       if((uchar)databuf[6] == 0x08)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "待机 " << std::endl;
                               detect_track_state=0;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "精跟踪自动跟踪" << std::endl;
                               detect_track_state = 1;

                               if((uchar)databuf[11]==0x01)
                               {
                                   seletct_detect_target = 0;
                               }
                               else
                               {
                                   seletct_detect_target = 1;
                               }
    //                           initFlag=true;
    //                           is_open_MTrack = false;
                           }
                           if((uchar)databuf[7] == 0x03){
    //                           is_open_MTrack = true;
    //                           initFlag = true;
                               int targetx,targety,targetw,targeth, id;

                               id=(static_cast<int>(databuf[8])<<8)|static_cast<int>(databuf[9]);

                               targetx=(static_cast<int>(databuf[11])<<8)|static_cast<int>(databuf[10]);
                               targety=(static_cast<int>(databuf[13])<<8)|static_cast<int>(databuf[12]);
                               targetw=(static_cast<int>(databuf[15])<<8)|static_cast<int>(databuf[14]);
                               targeth=(static_cast<int>(databuf[17])<<8)|static_cast<int>(databuf[16]);
                               detect_track_state = 2;
                               emit signals_send_handle(targetx,targety,targetw,targeth, id);
                               std::cout << "手动跟踪 " << std::endl;
                           }
                       }
                       //检测参数设置
                       if((uchar)databuf[6] == 0x12)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "检测方法1" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "检测方法2" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x03){
                               std::cout << "检测方法3 " << std::endl;
                           }
                       }
                       //跟踪参数设置
                       if((uchar)databuf[6] == 0x13)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "跟踪方法1" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "跟踪方法2" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x03){
                               std::cout << "跟踪方法3 " << std::endl;
                           }
                       }
                       //图像传输设置
                       if((uchar)databuf[6] == 0x14)
                       {
                           if((uchar)databuf[7] == 0x01){
                               std::cout << "开始传输图像" << std::endl;
                           }
                           if((uchar)databuf[7] == 0x02){
                               std::cout << "停止传输图像" << std::endl;
                           }
                       }
                   }


               }
       }

       tempbuffer.clear();
}



void MainWindow::receiveInfo0()
{

       QByteArray tempbuffer;
       static char databuf[1024];
       static int EffectiveLength = 0;
       tempbuffer.append(serialPort0->readAll());
       uint16_t command = 0xAA00;
       char tempdata[100];
       if(!tempbuffer.isEmpty())//如果非空说明有数据接收
       {
           memcpy(databuf,tempbuffer,tempbuffer.size());
       }
        tempbuffer.clear();
}


