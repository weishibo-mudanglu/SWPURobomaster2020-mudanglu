#include"algorithm.h"
#define row 640.0//宽度
#define col 512.0//高度
#define GRAVITY 9.78//重力加速度
using namespace std;
union CI{
    unsigned char st[4];
    float num;
};//联合体
union CS{
    unsigned char st[2];
    unsigned short int num;
};
//画面像素差计算
float maghrib_pixel(float p1,float p2,float p3,float p4)
{
    float coordinates =(p1+p2+p3+p4)/4;

    float erro = fabs(coordinates-row/2);

    return erro;

}


//像素差值转化为角度值
float maghrib_angle(float erro,int a)
{
    
    float angle_erro;
    //x方向角度计算
    if(a == 1)
    {
         angle_erro = erro/row*60;
    }
    //y方向角度计算
    if(a==2)
    {
         angle_erro = erro/col*60;
    }
    return angle_erro;
}

float BulletModel(float x, float v, float angle)
{
    //x:m,v:m/s,angle:rad
  float t, y;
  t = (float)(x/(v * cos(angle)));//计算水平方向弹丸击中目标的时间
  y = (float)(v * sin(angle) * t - GRAVITY * t * t / 2);//加速度速度公式计算了弹丸经过时间t后实际的高度
  return y;//返回计算出的实际高度
}

float GetPitch(float x,  float v) //函数只需要判断击打目标在某一个距离上，然后计算在该距离下的重力补偿值角度，其余角度差由pid计算产生
{
  float y_temp = 0.0;
  float y_actual, dy;
  float a;
  // by iteration
  for (int i = 0; i < 20; i++)
  {
    a = (float) atan2(y_temp, x);//返回夹角
    y_actual = BulletModel(x, v, a);//得到能打击的实际高度
    y_temp = y_temp - y_actual;
    if (fabs(y_actual) < 0.001)
    {
      break;
    }
    //printf("iteration num %d: angle %f,temp target y:%f,err of y:%f\n",i+1,a*180/3.1415926535,yTemp,dy);
  }
  return a;

}

float pidcontral::PID_realize(float p1,float p2,float p3,float p4,int a)//a用来标记计算x方向还是y方向的误差
{
    float Camera_Big_gone   = 0.0;//安装位置摄像头和大枪管水平角度差
    float Camera_Small_gone = 0.0;//安装位置摄像头和小枪管水平角度差
    if(a==1)
    {   //对于YAW轴来说，只需要对准在中心线上，并不需要计算额外误差，所以只需要画幅角度差，还有安装位置角度差
        float pixel_erro = maghrib_pixel(p1,p2,p3,p4);
        float angle_erro = maghrib_angle(pixel_erro,a);
//        if(GONEID==0x00)
//        {
//            x_err = angle_erro+Camera_Big_gone;
//        }
//        else
//        {
//            x_err = angle_erro+Camera_Small_gone;
//        }
        x_err = angle_erro;
        float incrementangle = x_Kp*(x_err - x_err_next) + x_Ki*x_err + x_Kd*(x_err - 2 * x_err_next + x_err_last);
        x_err_last = x_err_next;
        x_err_next = x_err;
        x_err = x_err_next - incrementangle;//改变当前误差，如果数据没有更新，改变结果直接用于下一次控制
        //cout<<"进入x_pid算法,计算得到目标到摄像头中心的Picth角度差:"<<incrementSpeed<<endl;
        return incrementangle;
    }
    else
    {   //相对于YAW轴，PICTH轴的误差应该由两个部分构成，除了画幅角度差，还应该包括重力补偿角
        float pixel_erro = maghrib_pixel(p1,p2,p3,p4);
        float angle_erro = maghrib_angle(pixel_erro,a);
        // y_err = angle_erro+gravity();
        y_err = angle_erro;
        float incrementangle = y_Kp*(y_err - y_err_next) + y_Ki*x_err + y_Kd*(y_err - 2 * y_err_next + y_err_last);
        y_err_last = y_err_next;
        y_err_next = y_err;
        y_err = y_err_next - incrementangle;//改变当前误差，如果数据没有更新，改变结果直接用于下一次控制
        //cout<<"进入y_pid算法且计算得到目标到摄像头中心的Yaw轴角度差:"<<incrementSpeed<<endl;
        
        return incrementangle;
    }
}
float pidcontral::PID_imitate(int a )
{
    
    if(a==1)
    {   
        float incrementangle = x_Kp*(x_err - x_err_next) + x_Ki*x_err + x_Kd*(x_err - 2 * x_err_next + x_err_last);
        x_err_last = x_err_next;
        x_err_next = x_err;
        x_err = x_err_next-incrementangle;
        //cout<<"进入x_pid算法,计算得到目标到摄像头中心的Picth角度差:"<<incrementSpeed<<endl;
        return incrementangle;
    }
    else
    {   

        y_err = angle_erro;
        float incrementangle = y_Kp*(y_err - y_err_next) + y_Ki*x_err + y_Kd*(y_err - 2 * y_err_next + y_err_last);
        y_err_last = y_err_next;
        y_err_next = y_err;
        y_err = y_err_next-incrementangle;
        //cout<<"进入y_pid算法且计算得到目标到摄像头中心的Yaw轴角度差:"<<incrementSpeed<<endl;
        
        return incrementangle;
    }
}
//pidcontral& pidcontral::operator =(const pidcontral& another)
//{
//    this->
//}
void pidcontral::PID_init()
{
    x_err = 0.0;
    x_err_last = 0.0;
    x_err_next = 0.0;
    x_Kp = 0.2;
    x_Ki = 0.015;
    x_Kd = 0.2;
    y_err = 0.0;
    y_err_last = 0.0;
    y_err_next = 0.0;
    y_Kp = 0.2;
    y_Ki = 0.015;
    y_Kd = 0.2;
}
algorithm::algorithm()
{
    std::cout<<"运行了algorithm的无参构造函数"<<std::endl;
}

algorithm::algorithm(const serial& b,const pidcontral &c,const pidcontral &d)
{
    this->usbtty=b;
    this->xpid = c;
    this->ypid = d;
}
algorithm& algorithm::operator =(const algorithm& another)
{
    this->usbtty = another.usbtty;
    this->xpid = another.xpid;
    this->ypid = another.ypid;
    return *this;
}


void algorithm::get_Point(Point2f p1, Point2f p2, Point2f p3, Point2f p4,float high)
{
    if(lock_2.try_lock())
    {
        my_arrmorPoint[0]=p1;
        my_arrmorPoint[1]=p2;
        my_arrmorPoint[2]=p3;
        my_arrmorPoint[3]=p4;
        light_high = high;
        SYMBOL = true;
        lock_2.unlock();
    }
    //std::cout<<"get point succsee"<<std::endl;
    
}
//让运算可能用到的值一次性全部加载，防止线程中断占据大量时间
void algorithm::load_Point(Point2f p1, Point2f p2, Point2f p3, Point2f p4,float high)
{
    lock_2.lock();
    p1 = my_arrmorPoint[0];
    p2 = my_arrmorPoint[1];
    p3 = my_arrmorPoint[2];
    p4 = my_arrmorPoint[3];
    high = light_high;
    SYMBOL = false;
    lock_2.unlock();
}

bool algorithm::colorjudge()
{
    if(COLOR==0x01)
        return true;
    else
        return false;
}


void algorithm::serial_read()
{

    while(1)
    {
        lock_1.lock();
        int readLength = read(usbtty.fd,reversebff,9);        
        if(readLength>1)
        {
            cout<<"接收数据长度"<<readLength<<endl;
            cout<<"接收数据"<<reversebff<<endl;
            cout<<"****************************************"<<endl;
        }
        else
        {
            cout<<"运行一次串口接收线程但没有收到数据"<<endl;
        }
        lock_1.unlock();
        usleep(20000);//将线程挂起20毫秒（单位是微秒）,这里用于控制接收频率
    }

}

void algorithm::serial_translate()
{
    CS speed_big,speed_lit;
    lock_1.lock();
    if(reversebff[0]==0xAA)
    {
        if(reversebff[1]==0xAA)
        {
            switch (reversebff[2])
            {
                case 0x01:
                    GONEID=reversebff[3];
                    speed_big.st[0] = reversebff[4];
                    speed_big.st[1] = reversebff[5];
                    speed_lit.st[0] = reversebff[6];
                    speed_lit.st[1] = reversebff[7];
                    Big_speed = speed_big.num;
                    Lit_speed = speed_lit.num;
                    break;
                case 0x02:
                    GONEID=0x02;
                    break;
                case 0x03:
                    GONEID=reversebff[3];
                    speed_big.st[0] = reversebff[4];
                    speed_big.st[1] = reversebff[5];
                    speed_lit.st[0] = reversebff[6];
                    speed_lit.st[1] = reversebff[7];
                    break;
                case 0x05:
                    COLOR = reversebff[3];
                    break;
            }
        }
    }
    lock_1.unlock();
}

void algorithm::serial_send()
{
    CI YAW_angle,PICTH_angle;
    unsigned char date[13];
    YAW_angle.num  = xangle;
    PICTH_angle.num= yangle;
    std::cout<<"YAW控制量:"<<YAW_angle.num<<std::endl;
    std::cout<<"PICTH控制量："<<PICTH_angle.num<<std::endl;
    date[0] = 0xAA;
    date[1] = 0xAA;
    date[2] = 0x04;
    date[3] = YAW_angle.st[0];
    date[4] = YAW_angle.st[1];
    date[5] = YAW_angle.st[2];
    date[6] = YAW_angle.st[3];
    date[7] = PICTH_angle.st[0];
    date[8] = PICTH_angle.st[1];
    date[9] = PICTH_angle.st[2];
    date[10] = PICTH_angle.st[3];
    if(xangle<=1&&yangle<=1)
    {
        date[11] = 0x00;
    }
    else
    {
        date[11] = 0x01;
    }
    date[12] = 0xBB;
    int writeLength=write(usbtty.fd,date,13);
    cout<<writeLength<<endl;

}


float algorithm::gravity()
{   //由于目标和摄像头的距离也可能在实时的发生改变，所以为了提高射击的准确度，有必要对距离也做预测
    switch(GONEID)
    {
        
        case 0x00:
            return GetPitch(distance,Big_speed);
            break;
        case 0x01:
            return GetPitch(distance,Lit_speed);
            break;
    }
}
void algorithm::ranging(float high)
{
//    std::cout<<"进入了测距算法"<<std::endl;
    distance = high*100;
}

void algorithm::dataprocessing()
{
    while(1)
    {

        float high;//灯条高度用于距离解算
        serial_translate();//翻译串口数据
        if(SYMBOL==true&&lock_2.try_lock())//这两项有任一不满足则表示本次数据不发生更新
        {
            
            Point2f p1 = my_arrmorPoint[0];
            Point2f p2 = my_arrmorPoint[1];
            Point2f p3 = my_arrmorPoint[2];
            Point2f p4 = my_arrmorPoint[3];//从图像处理线程加载装甲板位置信息和灯条长度信息
            high = light_high;
            lock_2.unlock();
            SYMBOL = false;//表示调用过一次位置数据，如果不发生更新则对输出进行滤波
            ranging(high);
            xangle = xpid.PID_realize(p1.x,p2.x,p3.x,p4.x,1);//计算YAW控制角度
            yangle = ypid.PID_realize(p1.y,p2.y,p3.y,p4.y,2);//计算PICTH控制角度
            serial_send();
            usleep(20000);//将线程挂起20毫秒（单位是微秒）,这里用于控制发送频率
        }
        else
        {
            //这里补充对输出的滤波，在图像处理线程帧率不够快的基础上补足控制频率
            xangle = xpid.PID_imitate(1);
            yangle = ypid.PID_imitate(2);
            serial_send();
            usleep(20000);//将线程挂起20毫秒（单位是微秒）,这里用于控制发送频率
        }

    }
}
