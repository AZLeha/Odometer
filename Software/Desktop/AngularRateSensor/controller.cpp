#include "controller.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QDateTime>

#define STOP_CMD 0x80
#define RUN_CMD 0x81





Controller::Controller(QObject *parent) :
    QObject(parent),
    m_port (new QSerialPort(this))
{
    m_isRun = false;
    connect(m_port, &QSerialPort::readyRead, this , &Controller::receivingData);


}

Controller::~Controller()
{
    delete  m_port;
}

int Controller::leftRPM() const
{
    return m_leftRPM;
}

int Controller::rightRPM() const
{
    return m_rightRPM;
}

int Controller::diferentRPM() const
{
    return m_diferentRPM;
}

bool Controller::isRun() const
{
    return m_isRun;
}

QJsonArray Controller::getListPorts()
{
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QJsonArray listCom;
    for(int i(0); i < com_ports.count(); i++)
        listCom << com_ports.at(i).portName();


    return listCom;
}

bool Controller::connectToPort(const QString &portName, const qint32 & baudRate, const QString filePath )
{
    m_folderPath = filePath;

    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);

    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    bool testP=m_port->open(QSerialPort::ReadWrite);


    return  testP;
}

bool Controller::disconnectPort()
{
    if(m_port->isOpen()) m_port->close();
    return m_port->isOpen();
}

#include <QByteArray>
#include <QDataStream>
void Controller::commnadRyader(CommandObject data)
{



    switch (data.comdan )
    {
        case STOP_CMD: setIsRun(false); break;
        case RUN_CMD: setIsRun(true); break;

        default:
        {
            if(isRun()) //если всё запущенно
            {
                OdometerData odometerData{};

                odometerData.leftRPM = *(reinterpret_cast<int16_t*>( data.data.data()));
                odometerData.rightRPM = *(reinterpret_cast<int16_t*>(data.data.data()+2));

                odometerData.leftDyno = *(reinterpret_cast<int32_t*>( data.data.data()+4));
                odometerData.rightDyno = *(reinterpret_cast<int32_t*>( data.data.data()+8));
                /*QDataStream strems(data.data);

                strems >> odometerData.leftRPM;
                strems >> odometerData.rightRPM;
                strems >> odometerData.leftDyno;
                strems >> odometerData.rightDyno;*/

                dataHandler(odometerData);

            }
        }
    }
}

void Controller::dataHandler(const Controller::OdometerData &data)
{
    setLeftRPM(data.leftRPM);
    setRightRPM(data.rightRPM);
    setLeftDyno(data.leftDyno);
    setRightDyno(data.rightDyno);

    m_stream << QString::number(QDateTime::currentDateTime().time().hour()) +":" + \
                QString::number(QDateTime::currentDateTime().time().minute()) + ":" + \
                QString::number(QDateTime::currentDateTime().time().second()) + ":" + \
                QString::number(QDateTime::currentDateTime().time().msec()) + "#" + \
                QString::number(data.leftRPM) +"#" + \
                QString::number(data.rightRPM) +"#" + \
                QString::number(data.leftDyno) +"#" + \
                QString::number(data.rightDyno) +"\n";





}


void Controller::setLeftRPM(int leftRPM)
{
    if (m_leftRPM == leftRPM)
        return;

    m_leftRPM = leftRPM;
    emit leftRPMChanged(m_leftRPM);
}

void Controller::setRightRPM(int rightRPM)
{
    if (m_rightRPM == rightRPM)
        return;

    m_rightRPM = rightRPM;
    emit rightRPMChanged(m_rightRPM);
}

void Controller::setDiferentRPM(int diferentRPM)
{
    if (m_diferentRPM == diferentRPM)
        return;

    m_diferentRPM = diferentRPM;
    emit diferentRPMChanged(m_diferentRPM);
}

void Controller::setIsRun(bool isRun)
{
    if (m_isRun == isRun)
        return;


    m_isRun = isRun;
    uint8_t data;


    if (m_isRun) {
        data = RUN_CMD;
        m_file.setFileName( m_folderPath.split("file://").at(1)+"/log"+ \
                            QString::number(QDateTime::currentDateTime().time().hour())+"_" + \
                            QString::number(QDateTime::currentDateTime().time().minute())+"_" + \
                            QString::number(QDateTime::currentDateTime().time().second()) + ".txt"
                           );

        qDebug() << m_file.fileName()<< m_file.open(QIODevice::WriteOnly);

        m_stream.setDevice(&m_file);

        m_stream << "hour:minute:second:msec#leftRPM#rightRPM#leftDyno#rightDyno \n";
    }
    else {
        data = STOP_CMD;
        m_file.close();
    }

    m_port->write(reinterpret_cast<char*>(&data),1);

    qDebug()<<"IS Run";
    emit isRunChanged(m_isRun);
}

void Controller::receivingData()
{
    qDebug()<<"Data is Read";
    if(m_commandProcessor.FindeComand(m_port->readAll()))
        commnadRyader(m_commandProcessor.GetLastComand());

}




