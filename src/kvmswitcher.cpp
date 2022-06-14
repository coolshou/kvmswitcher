#include "kvmswitcher.h"

#include <QDebug>

kvmswitcher::kvmswitcher(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(reader()));
    res = hid_init();

    handle = hid_open(0x10d5, 0x55a4, NULL);//# VID, PID, serial
    if (handle){
        timer->start(2000);
    }else{
        qDebug() << "no usbhid device (0x10d5, 0x55a4)!!" << Qt::endl;
    }


}

void kvmswitcher::reader()
{
    int res=0;
    unsigned char buf[128];
    res = hid_read(handle, buf, 128);
    if (res>0){
        qDebug() << "read(" << res << "):"<<buf << Qt::endl;
    }else{
        qDebug() << "no data read!!" << Qt::endl;
    }
}
