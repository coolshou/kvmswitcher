#ifndef KVMSWITCHER_H
#define KVMSWITCHER_H

#include <QTextStream>
#include <QObject>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
// #pragma comment(lib, "user32.lib")
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <hidapi/hidapi.h>
#endif


class kvmswitcher : public QObject
{
    Q_OBJECT
public:
    explicit kvmswitcher(QObject *parent = nullptr);
    int write();
    #ifdef Q_OS_WIN
    void simulateKeyStroke(WORD vkCode);
    #elif defined(Q_OS_LINUX)
    void simulateKeyStrokeLinux(KeySym keySym);
    #endif
    void triggerKvmSwitch();
public slots:
    void reader();

signals:
private:
    int res;
#ifdef Q_OS_LINUX
    hid_device *handle;
#endif
    QTimer *timer;

};

#endif // KVMSWITCHER_H
