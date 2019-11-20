#include "controller.h"
#include <QDebug>
#include <QSerialPortInfo>

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

bool Controller::connectToPort(const QString &portName, const qint32 &baudRate)
{

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
    m_port->flush();
    m_port->waitForReadyRead(100);
    qDebug() << "Rx: " << m_port->readAll();

    if(m_port->isOpen()) m_port->close();

    return m_port->isOpen();
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

    if (m_isRun) data = RUN_CMD;
    else data = STOP_CMD;

    m_port->write( reinterpret_cast<char*>(&data),1);

    emit isRunChanged(m_isRun);
}

void Controller::receivingData()
{
    //qDebug()<<"Data is Read";

    if(m_commandProcessor.FindeComand(m_port->readAll()))
        qDebug() << "Comanda is:" <<hex<< m_commandProcessor.GetLastComand().comdan;



    setRightRPM(m_rightRPM+1);
}


