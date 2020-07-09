#ifndef ARRMOR_H
#define ARRMOR_H
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv_extended.h>
#include<algorithm.h>
#include<stdlib.h>
using namespace cv;
using namespace std;



//该结构体存储关于装甲检测的所有参数
struct ArmorParam
{
    int Light_Point_min;//灯条最小点集个数
    int Light_Point_max;
    float disRatio_max=5;
    float disRatio_min=1.1;
    float Light_Max_Min2Points;//轮廓点集很小的最大，最小比限幅
    float Light_Normal_Max_Min2Points;//轮廓点集正常的最大，最小比限幅
    float angleij_center_diffMax;//灯条自身角度与两灯条夹角的最大差值
    float angleij_diffMax;//灯条旋转角度的最大差值
    float len_diff_kp;//灯条最长边相差的最大比例

    //为各项参数赋默认值
    ArmorParam()
    {
        Light_Point_min=8;//灯条最小点集个数
        Light_Point_max=2100;
        Light_Max_Min2Points=2.3;//轮廓点集很小的最大，最小比限幅
        Light_Normal_Max_Min2Points=1.8;//轮廓点集正常的最大，最小比限幅
        angleij_center_diffMax=13;//灯条自身角度与两灯条夹角的最大差值
        angleij_diffMax=15;//灯条旋转角度的最大差值
        len_diff_kp=4;//灯条最长边相差的最大比例
    }
};



typedef enum _
{
    Lightleft=0,
    Lightright=1
}sense_of_roRect;

typedef enum __
{
    ARMOR_NO = 0,		// not found
    ARMOR_LOST = 1,		// lose tracking
    ARMOR_GLOBAL = 2,	// armor found globally
    ARMOR_LOCAL = 3		// armor found locally(in tracking mode)
}ArmorFindFlag;

typedef enum ___
{
    UNKNOWN_ARMOR = 0,
    SMALL_ARMOR   = 1,
    BIG_ARMOR     = 2,
    SMALL_BUFF    = 3,
    BIG_BUFF      = 4
}ObjectType;

typedef enum _____
{
    Hero = 1,      //英雄
    Engineer = 2,  //工程
    Standard = 3,  //步兵
    Sentry =4,     //哨兵
    Aerial = 5,     //无人机
    NoRotbor=6
}RobotType;

//灯条描述的类
class LightDescriptor
{
public:
    LightDescriptor();//无参构造
    LightDescriptor(const cv::RotatedRect& another);
    LightDescriptor& operator=(const LightDescriptor& another);
    cv::RotatedRect rotatedrect();//返回描述灯条最小的旋转包围矩形

public:

    sense_of_roRect sense;//表示灯条方向
    cv::Point2f center;
    float max;
    float min;
    float angle;
    float area;
};

//装甲板描述类
/*
 * 因为装甲板外形也是一个旋转矩形，只是多了一些它固有的特性，因此我选择直接从用装甲板描述类去继承旋转矩形。
 * */
class ArmorDescriptor : public RotatedRect
{
public:
    ArmorDescriptor();
    ArmorDescriptor(const RotatedRect& another,ObjectType type=UNKNOWN_ARMOR,RobotType ro=NoRotbor);
    ArmorDescriptor& operator =(const ArmorDescriptor& another);
    RotatedRect rotatedrect();
    void setRobotType(RobotType set);
    void setArmorrType(ObjectType set);
    void judgeArmorrType();

public:

    float              Longest;
    float             Shortest;
    float             lightLen;
    RobotType            robot;
    ObjectType       armorType;
    sense_of_roRect armorsense;
    
    

};

//关于装甲板操作的类
class arrmor
{
public:
    arrmor();
    arrmor(const algorithm& b,Ptr<ml::SVM>& c,Ptr<ml::SVM>& d);
    ArmorFindFlag ArrmorDection();
    void judgeArrmorState();
    void setImage(cv::Mat& set_src);
    void recrodArmorStatus(bool isFoundArmor);
    void histMaker(Mat& src_hist);//绘制直方图的函数
    void  getLightLen(vector<Point2f> &lightPoint2fs, float &len);
    void judgeArmorrType(ArmorDescriptor &a);
public:
    bool armorColor ;
    ArmorParam Param;//装甲板描述的结构体
    Point2f offset_roi_point;//
    Size ImageSize;
    Size roiImageSize;//找到的装甲板大小
    cv::Mat src_roi;//  寻找装甲板的ROI大小
    ArmorDescriptor targetArrmor;// 目标装甲板
    cv::RotatedRect targetArrmor2FindRoi;//
    vector<LightDescriptor> lightCountersRoRect;  // 筛选出来的单个灯条vector
    
    cv::Point2f    Points_coordinates;
    float          h_light;//目标装甲板灯条长度
private:
    ArmorFindFlag _armorFindFlag;
    int _trackCounter; //记录在追踪模式下处理图片的张数，达到max_track_num后变为全局搜索模式
    bool _isTracking;  //判断是否在追踪模式下
    float widthRatio;
    float heightRatio;
    int losed_counter;// 目标装甲板丢失计数器
    int find_counter;//目标装甲板找到计数器
    bool enableDigitsRecognize;
    Ptr<ml::SVM> SVM_Params;
    Ptr<ml::SVM> SVM_ArmorTypeParam;
    void drawMaxConnect(Mat& out,Mat& labels,int maxLabel);
    int mySvmArmorTypePredict(float ratio, float angle)   ;
    void mySvmPredict(Mat& src,int& armorNum);
};


#endif // ARRMOR_H
