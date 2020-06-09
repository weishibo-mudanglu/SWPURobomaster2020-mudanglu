#ifndef ALGORITHM_H
#define ALGORITHM_H
#include<opencv2/opencv.hpp>
#include<serial.h>
#include <mutex>

using namespace cv;
class pidcontral{
public:
    void PID_init();
//    pidcontral& operator=(const pidcontral& another);
    float PID_realize(float p1,float p2,float p3,float p4,int a);

private:
    float x_err;
    float x_err_next;
    float x_err_last;
    float x_Kp, x_Ki, x_Kd;
    float y_err;
    float y_err_next;
    float y_err_last;
    float y_Kp, y_Ki, y_Kd;
    int index;
};

class algorithm{
public:
    algorithm();
    algorithm(const serial& b,const pidcontral& c,const pidcontral& d);
    algorithm& operator=(const algorithm& another);

    void get_Point(Point2f p1,Point2f p2,Point2f p3,Point2f p4,float high);

    void load_Point(Point2f p1,Point2f p2,Point2f p3,Point2f p4,float high);


    void ranging(float high);//测距

    float gravity();//重力补偿

    bool colorjudge();//判断自瞄颜色

    void serial_read();//串口接收

    void dataprocessing();//数据处理和串口发送

    void serial_translate();//翻译串口数据

    void serial_send();//发送数据



private:
    cv::Point2f my_arrmorPoint[4];
    serial usbtty;
    unsigned char reversebff[9];
    float distance,light_high;
    pidcontral xpid;
    pidcontral ypid;
    float xangle,yangle;
    unsigned short int Big_speed,Lit_speed;
    unsigned char GONEID,COLOR,SYMBOL;
    mutex lock_1,lock_2;
};


#endif // ALGORITHM_H
