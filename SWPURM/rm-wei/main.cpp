#include <iostream>
#include <thread>
#include <unistd.h>
#include <arrmor.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <imgproduceprocessing.h>
#include<algorithm.h>
#define muiltThread 1
using namespace std;
using namespace cv;

bool waitAndBreak(void);
int main()
{
    class camera video("redTure1.avi");
//    char d[]={};
//    class serial usbtty(d,115200,8,1,'n');
//    usbtty.serialInit();
//    class pidcontral x_x;
//    class pidcontral y_y;
    Ptr<ml::SVM> SVM_Params;
    Ptr<ml::SVM> SVM_ArmorTypeParam;
    SVM_Params=ml::SVM::load("svmNum.xml");
//    class algorithm algorithms(usbtty,x_x,y_y);
    class arrmor Arrmor(algorithms,SVM_Params,SVM_ArmorTypeParam);
    class imgProduceProcessing imgPromachine(Arrmor,video);

#if muiltThread
   thread myMultiThread1(&imgProduceProcessing::ImageProducing,ref(imgPromachine));
   thread myMultiThread2(&imgProduceProcessing::ImageProcessing,ref(imgPromachine));
    thread myMultiThread3(&imgProduceProcessing::serial_read,ref(imgPromachine));//串口接收线程
    thread myMultiThread4(&imgProduceProcessing::dataprocessing,ref(imgPromachine));//串口发送线程

    myMultiThread1.join();
    myMultiThread2.join();
    myMultiThread3.join();
    myMultiThread4.join();

#else
    while(true)
    {
        imgPromachine.ImageProducing2ImageProcessing();
        if(imgPromachine.BreakFlag)break;
        /*
        char temp=char(waitKey(10));
        if(temp=='s')
        {
            waitKey(0);
        }
        else if(temp!=(-1))break;
        */

    }
#endif

    return 0;
}

