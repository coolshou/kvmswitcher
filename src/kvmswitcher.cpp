#include "kvmswitcher.h"

#include <QDebug>
#include <QtGlobal>
#include <QThread>

/*
 *
 * sudo usbhid-dump -d  10d5:55a4
001:023:000:DESCRIPTOR         1655173507.268943
 05 01 09 06 A1 01 05 07 19 E0 29 E7 15 00 25 01
 75 01 95 08 81 02 95 01 75 08 81 01 95 05 75 01
 05 08 19 01 29 05 91 02 95 01 75 03 91 01 95 06
 75 08 15 00 26 FF 00 05 07 19 00 2A FF 00 81 00
 C0
==(https://eleccelerator.com/usbdescreqparser/)==>
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x06,        // Usage (Keyboard)
0xA1, 0x01,        // Collection (Application)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0xE0,        //   Usage Minimum (0xE0)
0x29, 0xE7,        //   Usage Maximum (0xE7)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x75, 0x01,        //   Report Size (1)
0x95, 0x08,        //   Report Count (8)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x01,        //   Report Count (1)
0x75, 0x08,        //   Report Size (8)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x05,        //   Report Count (5)
0x75, 0x01,        //   Report Size (1)
0x05, 0x08,        //   Usage Page (LEDs)
0x19, 0x01,        //   Usage Minimum (Num Lock)
0x29, 0x05,        //   Usage Maximum (Kana)
0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x01,        //   Report Count (1)
0x75, 0x03,        //   Report Size (3)
0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x06,        //   Report Count (6)
0x75, 0x08,        //   Report Size (8)
0x15, 0x00,        //   Logical Minimum (0)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0x00,        //   Usage Minimum (0x00)
0x2A, 0xFF, 0x00,  //   Usage Maximum (0xFF)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

// 65 bytes

// best guess: USB HID Report Descriptor

*/
kvmswitcher::kvmswitcher(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(reader()));
#ifdef Q_OS_LINUX
    res = hid_init();
    // require root to open device!!
    handle = hid_open(0x10d5, 0x55a4, NULL);//# VID, PID, serial
    if (handle){
        timer->start(2000);
    }else{
        qDebug() << "no usbhid device (0x10d5, 0x55a4)!!" <<
        #if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            endl;
        #else
            Qt::endl;
        #endif
    }
#endif

}

int kvmswitcher::write()
{
#ifdef Q_OS_LINUX
    int res=0;	// 如果返回-1表示發送失敗
    unsigned char buf[4];
    buf[0] = 0x00;	// 這就是Report ID
    buf[1] = 0x01;
    buf[2] = 0x02;
    buf[3] = 0x03;
    res = hid_write(handle, buf, 4);
    return res;
#endif
    return 0;
}


void kvmswitcher::triggerKvmSwitch()
{
    qDebug() << "正在發送 KVM 切換訊號...";

    // 第一次 Scroll Lock (按下+放開)
#ifdef Q_OS_WIN
    simulateKeyStroke(VK_NUMLOCK);
    // KVM 需要一點點時間反應，延遲 50 ~ 100 毫秒效果最好
    QThread::msleep(50);
    // 第二次 Scroll Lock (按下+放開)
    simulateKeyStroke(VK_NUMLOCK);
#elif defined(Q_OS_LINUX)
    // Linux 實作
    simulateKeyStrokeLinux(XK_Num_Lock); // 第一次 Num Lock
    QThread::msleep(50);
    simulateKeyStrokeLinux(XK_Num_Lock); // 第二次 Num Lock
#endif
    qDebug() << "訊號發送完成！";
}

#ifdef Q_OS_WIN
void kvmswitcher::simulateKeyStroke(WORD vkCode)
{
    INPUT inputs[2];
    ZeroMemory(inputs, sizeof(inputs));

    // 1. 設定「按下」事件
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vkCode;

    // 2. 設定「放開」事件
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vkCode;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    // 送出訊號
    SendInput(2, inputs, sizeof(INPUT));
}
#elif defined(Q_OS_LINUX)
void kvmswitcher::simulateKeyStrokeLinux(KeySym keySym)
{
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        qWarning() << "無法開啟 X 顯示器！";
        return;
    }

    KeyCode keyCode = XKeysymToKeycode(display, keySym);
    if (keyCode != 0) {
        XTestFakeKeyEvent(display, keyCode, True, 0);  // Press
        XFlush(display);
        XTestFakeKeyEvent(display, keyCode, False, 0); // Release
        XFlush(display);
    }
    XCloseDisplay(display);
}
#endif

void kvmswitcher::reader()
{
#ifdef Q_OS_LINUX
    int res=0;
    unsigned char buf[128];
    res = hid_read(handle, buf, 128);
    if (res>0){
        qDebug() << "read(" << res << "):"<<buf <<
        #if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            endl;
        #else
            Qt::endl;
        #endif
    }else{
        qDebug() << "no data read!!" <<
        #if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            endl;
        #else
            Qt::endl;
        #endif

    }
#endif
}
