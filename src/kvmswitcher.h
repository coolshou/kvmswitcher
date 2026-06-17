#ifndef KVMSWITCHER_H
#define KVMSWITCHER_H

#include <QObject>
#include <QTimer>

#ifdef Q_OS_LINUX
#include <hidapi/hidapi.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class kvmswitcher : public QObject
{
    Q_OBJECT
public:
    explicit kvmswitcher(QObject *parent = nullptr);
    int write();
    #ifdef Q_OS_WIN
    void simulateKeyStroke(WORD vkCode);
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
