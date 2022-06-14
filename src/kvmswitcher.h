#ifndef KVMSWITCHER_H
#define KVMSWITCHER_H

#include <QObject>
#include <QTimer>

#include <hidapi/hidapi.h>

class kvmswitcher : public QObject
{
    Q_OBJECT
public:
    explicit kvmswitcher(QObject *parent = nullptr);

public slots:
    void reader();

signals:
private:
    int res;
    hid_device *handle;
    QTimer *timer;

};

#endif // KVMSWITCHER_H
