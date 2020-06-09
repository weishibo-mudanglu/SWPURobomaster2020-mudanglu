#ifndef SERIAL_H
#define SERIAL_H
#include <iostream>
#include <sys/stat.h>
#include <sys/termios.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/termios.h>

using namespace std;

//int speed_a[] = {B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
//           B38400, B19200, B9600, B4800, B2400, B1200, B300};

//unsigned int name_arr[] = {230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300,
//          38400, 19200, 9600, 4800, 2400, 1200, 300};


class serial
{
public:
    serial();
    serial(char *name,unsigned int Speeds,unsigned int Data,unsigned int Stop,char parity);
    bool serial_open();
    void serial_speed(int speed,termios &opt);
    void set_data(unsigned int dataBit,struct termios &opt);
    void set_stop(unsigned int stop,struct termios &opt);
    void set_parity(char parity,struct termios &opt);
    bool serialParameter();
    bool serialInit();

public:
    int                    fd;      //串口的文件句柄
    char       *serialDevName;      //DEV路径名
    unsigned int serialSpeeds;      //波特率
    unsigned int   serialData;      //数据位
    unsigned int   serialStop;      //停止位
    char         serialParity;      //奇偶校验位
};

#endif // SERIAL_H
