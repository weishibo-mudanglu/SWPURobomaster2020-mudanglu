#include <imgproduceprocessing.h>
#define DATA_IMAGE_SIZE 6
#define DEBUG_IMAGE_NUM 0
#define ThreadMuilt 0
using namespace cv;
struct dataImage
{
    Mat data[DATA_IMAGE_SIZE];
    unsigned long imageframe;//表示读取和生产图片数量之差
};
dataImage g_dataImageForProdProc;

imgProduceProcessing::imgProduceProcessing()
{
    this->processingFrames=0;
    this->producingFrames=0;
    this->BreakFlag=false;
}

imgProduceProcessing::imgProduceProcessing(const arrmor& a,const camera& v)
{
    this->processingFrames=0;
    this->producingFrames=0;
    this->arrmorDection=a;
    this->BreakFlag=false;
    this->video=v;
}

void imgProduceProcessing::ImageProducing()
{
    cv::Mat producedTemp;

    while(true)
    {
         if(video.isCamera)                 //用摄像头
         {
             if(video.CameraState)
             {
                 if(video.IGetFrame(producedTemp)==true)
                 {
                     g_dataImageForProdProc.data[producingFrames%DATA_IMAGE_SIZE]=producedTemp;
                     producingFrames++;
                     g_dataImageForProdProc.imageframe=producingFrames-processingFrames;
                     usleep(10000);
                 }
                 else
                 {
                     cout << "false get image!" <<endl;
                 }
             }
             else
             {
                 video.restartConnect();
                 waitKey(100);
             }
         }
         else                               //用摄像头
         {
             if(video.IGetFrame(producedTemp)==true)
             {
                 g_dataImageForProdProc.data[this->producingFrames%DATA_IMAGE_SIZE]=producedTemp;
                 this->producingFrames++;
                 g_dataImageForProdProc.imageframe=this->producingFrames-this->processingFrames;
                 usleep(10000);
             }
             else
             {
                 cout << "退出" <<endl;
                 break;
             }
         }


    }

}
void imgProduceProcessing::ImageProcessing()
{
    while(true)
    {
        if(BreakFlag)break;
        while(g_dataImageForProdProc.imageframe==0)if(BreakFlag)break;//等待生产图片完成
//        double time0=saturate_cast<double>(getTickCount());
        cv::Mat src=g_dataImageForProdProc.data[processingFrames%DATA_IMAGE_SIZE];
        processingFrames++;
        g_dataImageForProdProc.imageframe=producingFrames-processingFrames;
    #if DEBUG_IMAGE_NUM
        //   打印出采集和处理图像的数量/
        std::cout << "producingFrames="<<producingFrames;
        std::cout << "processingFrames="<<processingFrames;
        std::cout << "生产和处理图像数量之差="<<g_dataImageForProdProc.imageframe;
        std::cout <<std::endl;
    #endif

        //arrmorDection.judgeArrmorState();
        arrmorDection.setImage(src);
        ArmorFindFlag state=ARMOR_NO;
        state=arrmorDection.ArrmorDection();
        algorithms.get_Point(arrmorDection.Points,arrmorDection.high)
//        namedWindow("endsrc");
//        imshow("endsrc",src);
//        double getTime=(saturate_cast<double>(getTickCount())-time0)/getTickFrequency()*1000;
        cv::waitKey(1);
//        cout << "运行时间为：" << getTime << "ms" <<endl;
    }

}

void imgProduceProcessing::ImageProducing2ImageProcessing()
{
    cv::Mat producedTemp;
    //producedTemp.t()// zhuanzhi
    //producedTemp.reshape()//restart channels and image is row
    if(producedTemp.empty())
    {
        std::cout << "iamge load fauilt"<<std::endl;
        BreakFlag=true;
        exit(-1);
    }
    g_dataImageForProdProc.data[this->producingFrames%DATA_IMAGE_SIZE]=producedTemp;
    this->producingFrames++;
    g_dataImageForProdProc.imageframe=this->producingFrames-this->processingFrames;


    //if(BreakFlag)break;
    while(g_dataImageForProdProc.imageframe==0)if(BreakFlag)break;//等待生产图片完成
    double time0=saturate_cast<double>(getTickCount());
    cv::Mat src=g_dataImageForProdProc.data[processingFrames%DATA_IMAGE_SIZE];
    processingFrames++;
    g_dataImageForProdProc.imageframe=producingFrames-processingFrames;
#if DEBUG_IMAGE_NUM
    //   打印出采集和处理图像的数量/
    std::cout << "producingFrames="<<producingFrames;
    std::cout << "processingFrames="<<processingFrames;
    std::cout << "生产和处理图像数量之差="<<g_dataImageForProdProc.imageframe;
    std::cout <<std::endl;
#endif

    arrmorDection.judgeArrmorState();
    arrmorDection.setImage(src);
    arrmorDection.ArrmorDection();
    namedWindow("endsrc");
    imshow("endsrc",src);
//    double getTime=(saturate_cast<double>(getTickCount())-time0)/getTickFrequency()*1000;
    cv::waitKey(1);
//    cout << "运行时间为：" << getTime << "ms" <<endl;
}

void imgProduceProcessing::serial_read()
{
    
    algorithms.serial_read();
}
void imgProduceProcessing::dataprocessing()
{
    algorithms.dataprocessing();
}
