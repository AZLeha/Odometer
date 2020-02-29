
#include "controller.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>

#define STOP_CMD 0x80
#define RUN_CMD 0x81

#include <QTimer>


#ifdef DEBUG

void Controller::debugSequence()
{
    static OdometerData odometerData{10,11,12,13,14};
    odometerData.diferentRPM = calculateGlide(0.285,odometerData.leftRPM,0.161,odometerData.rightRPM, offSetDiferentRPM());

    dataHandler(odometerData);
    odometerData.rightRPM++;

    timer->start(1000);
}
#endif

Controller::Controller(QObject *parent) : QObject(parent),
    m_port (new QSerialPort(this))
{
    m_isRun = false;
    connect(m_port, &QSerialPort::readyRead, this , &Controller::receivingData);

#ifdef DEBUG

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Controller::debugSequence);
    timer->start(1000);
#endif

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

double Controller::diferentRPM() const
{
    return m_diferentRPM;
}

bool Controller::isRun() const
{
    return m_isRun;
}

float Controller::wheelRadius() const
{
    return m_wheelRadius;
}

float Controller::shaftRadius() const
{
    return m_shaftRadius;
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

double Controller::windowFilter(double data)
{
#define filterSize 1
    static double array[filterSize]= {0};
    static int curentIndex = 0;

    array[curentIndex++] = data;

    curentIndex %=filterSize;

    double value=0;

    for(int i = 0; i<filterSize;i++)
    {
        value += array[i];
    }

    return value/filterSize;
}

int Controller::leftDyno() const
{
    return m_leftDyno;
}

int Controller::rightDyno() const
{
    return m_rightDyno;
}

int Controller::offSetDiferentRPM() const
{
    return m_offSetDiferentRPM;
}


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



            odometerData.diferentRPM = calculateGlide(wheelRadius(),odometerData.leftRPM,shaftRadius(),odometerData.rightRPM, offSetDiferentRPM());

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

    setDiferentRPM(data.diferentRPM);

    m_stream << QString::number(QDateTime::currentDateTime().time().hour()) +":" + \
                QString::number(QDateTime::currentDateTime().time().minute()) + ":" + \
                QString::number(QDateTime::currentDateTime().time().second()) + ":" + \
                QString::number(QDateTime::currentDateTime().time().msec()) + "#" + \
                QString::number(data.leftRPM) +"#" + \
                QString::number(data.rightRPM) +"#" + \
                QString::number(data.leftDyno) +"#" + \
                QString::number(data.rightDyno) +"#" + \
                QString::number(data.diferentRPM) +"\n";
}

int Controller::calculateGlide(double mainRadius, int mainRPM, double secondaryRadius, int secondaryRPM, int ofset)
{
    // property real curentVlue: ((root.rightRPM*rV - root.leftRPM * (rC+delta) ) / (root.rightRPM*rV))
    double delta = mainRadius * (0.01 * ofset);
    double result =  ((secondaryRadius * secondaryRPM) - ((mainRadius + delta) * mainRPM)) / (secondaryRadius * secondaryRPM);

    return static_cast<int>(result * 100);
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

void Controller::setDiferentRPM(double diferentRPM)
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

        m_stream << "hour:minute:second:msec#leftRPM#rightRPM#leftDyno#rightDyno#glide \n";
    }
    else {
        data = STOP_CMD;
        m_file.close();
    }

    m_port->write(reinterpret_cast<char*>(&data),1);

    qDebug()<<"IS Run";
    emit isRunChanged(m_isRun);
}

void Controller::setLeftDyno(int leftDyno)
{
    if (m_leftDyno == leftDyno)
        return;

    m_leftDyno = leftDyno;
    emit leftDynoChanged(m_leftDyno);
}

void Controller::setRightDyno(int rightDyno)
{
    if (m_rightDyno == rightDyno)
        return;

    m_rightDyno = rightDyno;
    emit rightDynoChanged(m_rightDyno);
}

void Controller::setOffSetDiferentRPM(int offSetDiferentRPM)
{
    if (m_offSetDiferentRPM == offSetDiferentRPM)
        return;

    m_offSetDiferentRPM = offSetDiferentRPM;
    emit offSetDiferentRPMChanged(m_offSetDiferentRPM);
}

void Controller::setWheelRadius(double wheelRadius)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_wheelRadius, wheelRadius))
        return;

    m_wheelRadius = wheelRadius;
    emit wheelRadiusChanged(m_wheelRadius);
}

void Controller::setShaftRadius(double shaftRadius)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_shaftRadius, shaftRadius))
        return;

    m_shaftRadius = shaftRadius;
    emit shaftRadiusChanged(m_shaftRadius);
}

void Controller::receivingData()
{
    //qDebug()<<"Data is Read";
    if(m_commandProcessor.FindeComand(m_port->readAll()))
        commnadRyader(m_commandProcessor.GetLastComand());

}

