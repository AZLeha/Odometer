#include "hardware.h"
#include <QSerialPortInfo>
#include "model.h"

Hardware::Hardware(double wheelRadius, double shaftRadius, QObject *parent) :   QObject(parent),
    m_port (new QSerialPort(this)),
    m_wheelRadius(wheelRadius),
    m_shaftRadius(shaftRadius)
{
    connect(m_port, &QSerialPort::readyRead, this , &Hardware::sl_receivingData);
}
Hardware::~Hardware()
{
    delete  m_port;
}

bool Hardware::HardwareConnect(const QString &portName, const qint32 &baudRate)
{
    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);

    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);


    return  m_port->open(QSerialPort::ReadWrite);
}

bool Hardware::HardwareDisconnect()
{
    if(m_port->isOpen()) m_port->close();

    return m_port->isOpen();
}

QList<QSerialPortInfo> Hardware::GetListPorts()
{
    return QSerialPortInfo::availablePorts();
}

void Hardware::commnadRyader(CommandObject data)
{
    switch (static_cast<Hardware_COMAND>(data.comdan) )
    {
        case Hardware_COMAND::STOP_CMD:
            emit si_ComandIsReady(static_cast<Hardware_COMAND>(data.comdan)); break;

        case Hardware_COMAND::RUN_CMD:
            emit si_ComandIsReady(static_cast<Hardware_COMAND>(data.comdan)); break;

        default:
        {

            MainModel odometerData{};

            /*QDataStream s(data.data);
                    s >> odometerData.m_leftRPM;
                    s >> odometerData.m_rightRPM;
                    s >> odometerData.m_leftDyno;
                    s >> odometerData.m_rightDyno;*/

            odometerData.m_leftRPM = *(reinterpret_cast<int16_t*>( data.data.data()));
            odometerData.m_rightRPM = *(reinterpret_cast<int16_t*>(data.data.data()+2));
            odometerData.m_leftDyno = *(reinterpret_cast<int32_t*>( data.data.data()+4));
            odometerData.m_rightDyno = *(reinterpret_cast<int32_t*>( data.data.data()+8));



           odometerData.m_diferentRPM = calculateGlide(m_wheelRadius,odometerData.m_leftRPM,m_shaftRadius,odometerData.m_rightRPM, m_ofset);


            emit si_DataIsReady(odometerData);
        }
    }
}


qint8 Hardware::calculateGlide(double mainRadius, int mainRPM, double secondaryRadius, int secondaryRPM, qint8 ofset)
{
    // property real curentVlue: ((root.rightRPM*rV - root.leftRPM * (rC+delta) ) / (root.rightRPM*rV))

    double delta = mainRadius * (0.01 * ofset);

    double result =  ((secondaryRadius * secondaryRPM) - ((mainRadius + delta) * mainRPM)) / (secondaryRadius * secondaryRPM);

    return static_cast<qint8>(result * 100);
}

void Hardware::writeComand( Hardware_COMAND comand)
{
    m_port->write(reinterpret_cast<char*>(&comand),1);
}

void Hardware::sl_setOfset(qint8 ofset)
{
    m_ofset = ofset;
}



void Hardware::sl_receivingData()
{
    if(m_commandProcessor.FindeComand(m_port->readAll()))
        commnadRyader(m_commandProcessor.GetLastComand());
}


