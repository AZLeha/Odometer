
#include "controller.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>




Controller::Controller(QObject *parent) : QObject(parent)

{





    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Controller::TimerHandler);
    timer->start(300);


}


Controller::~Controller()
{

}


QJsonArray Controller::portInfo()
{
    QList<QSerialPortInfo> com_ports = Hardware::GetListPorts();

    QJsonArray listCom;
    for(int i(0); i < com_ports.count(); i++)
        listCom << com_ports.at(i).portName();

    return listCom;
}

bool Controller::run(const QString &portName, const qint32 &baudRate, const QString filePath)
{
    if(HardwareInstance) return false;


    HardwareInstance = new Hardware(m_wheelRadius, m_shaftRadius,this);

    m_folderPath = filePath;

    if(!HardwareInstance->HardwareConnect(portName,baudRate)) return false;

    hardThread =new QThread(this);

    HardwareInstance->moveToThread(hardThread);


    connect(HardwareInstance,&Hardware::si_DataIsReady, this, &Controller::sl_receivingData);
    connect(HardwareInstance,&Hardware::si_ComandIsReady, this, &Controller::sl_receivingComand);
    connect(this, &Controller::setWheelRadiusOffSet, HardwareInstance, &Hardware::sl_setOfset);

    hardThread->start();
    return true;
}

void Controller::driveTransmissionInfo(double shaftRadius, double wheelRadius)
{

    m_wheelRadius = wheelRadius;
    m_shaftRadius = shaftRadius;
}



bool Controller::isRun() const
{
    return m_isRun;
}

void Controller::setIsRun(bool isRun)
{


    if (m_isRun == isRun)
        return;
    m_isRun = isRun;
    if(isRun)
    {
        #if defined(Q_OS_LINUX)
           m_file.setFileName( m_folderPath.split("file://").at(1)+"/log"+ \
                               QString::number(QDateTime::currentDateTime().time().hour())+"_" + \
                               QString::number(QDateTime::currentDateTime().time().minute())+"_" + \
                               QString::number(QDateTime::currentDateTime().time().second()) + ".txt"
                               );
        #elif defined(Q_OS_WIN)
           m_file.setFileName( m_folderPath.split("file:///").at(1)+"/log"+ \
                               QString::number(QDateTime::currentDateTime().time().hour())+"_" + \
                               QString::number(QDateTime::currentDateTime().time().minute())+"_" + \
                               QString::number(QDateTime::currentDateTime().time().second()) + ".txt"
                               );
        #endif


        qDebug() << m_file.fileName() << m_file.open(QIODevice::WriteOnly);

        m_stream.setDevice(&m_file);

        m_stream << "hour:minute:second:msec#leftRPM#rightRPM#leftDyno#rightDyno#glide \n";


        HardwareInstance->writeComand(Hardware_COMAND::RUN_CMD);

    }

    else
    {
        m_file.close();
        HardwareInstance->writeComand(Hardware_COMAND::STOP_CMD);
    }



 //   m_isRun = isRun;
    emit isRunChanged(m_isRun);
}



void Controller::sl_receivingComand(Hardware_COMAND comand)
{
    if(comand == Hardware_COMAND::RUN_CMD)
    {
        setIsRun(true);

    }

    else if(comand == Hardware_COMAND::STOP_CMD)
    {
        setIsRun(false);

    }


}

void Controller::sl_receivingData(MainModel data)
{
    if(isRun())
    {
        m_stream <<   QString::number(QDateTime::currentDateTime().time().hour()) +":" + \
                      QString::number(QDateTime::currentDateTime().time().minute()) + ":" + \
                      QString::number(QDateTime::currentDateTime().time().second()) + ":" + \
                      QString::number(QDateTime::currentDateTime().time().msec()) + "#" + \
                      QString::number(data.m_leftRPM) +"#" + \
                      QString::number(data.m_rightRPM) +"#" + \
                      QString::number(data.m_leftDyno) +"#" + \
                      QString::number(data.m_rightDyno) +"#" + \
                      QString::number(data.m_diferentRPM) +"\n";

        model = data;
    }
}


void Controller::TimerHandler()
{
    emit readyRead(model);
}

