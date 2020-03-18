
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#include "hardware.h"
#include "model.h"


#include <QThread>






class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isRun READ isRun WRITE setIsRun NOTIFY isRunChanged)

public:

    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    Q_INVOKABLE QJsonArray portInfo();
    Q_INVOKABLE bool run(const QString &portName, const qint32 & baudRate = 115200, const QString filePath = "./");

    //! \property [in]  shaftRadius Радиус привода
    //! \property [in]  wheelRadius Радиус Колеса
    Q_INVOKABLE void driveTransmissionInfo(double shaftRadius, double wheelRadius);


    bool isRun() const;

public slots:
    void setIsRun(bool isRun);

protected:
    bool disconnectPort();
    bool connectPort(const QString &portName, const qint32 & baudRate = 115200);

private:

    double m_shaftRadius;
    double m_wheelRadius;
    qint8 m_WheelRadiusOffSet;

    QFile m_file;
    QTextStream m_stream;
    QString m_folderPath;

    bool m_isRun = false;

    Hardware *HardwareInstance = nullptr;

    QTimer *timer;
    MainModel model;

    QThread *hardThread;

signals:
    void readyRead(MainModel data);
    void setWheelRadiusOffSet(qint8 offset);

    void isRunChanged(bool isRun);

private slots:
    void sl_receivingData(MainModel data);
    void sl_receivingComand(Hardware_COMAND comand);


    void TimerHandler();


};



#endif // CONTROLLER_H
