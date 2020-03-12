#ifndef MODEL_H
#define MODEL_H
#include  <QObject>
#include <QMetaType>


class MainModel {
    Q_GADGET

    Q_PROPERTY(qint16 leftRPM MEMBER m_leftRPM)
    Q_PROPERTY(qint16 rightRPM MEMBER m_rightRPM)
    Q_PROPERTY(qint32 leftDyno MEMBER m_leftDyno)
    Q_PROPERTY(qint32 rightDyno MEMBER m_rightDyno)
    Q_PROPERTY(qint16 diferentRPM MEMBER m_diferentRPM)

public:
    qint16 m_leftRPM = 0;
    qint16 m_rightRPM = 0;

    qint32 m_leftDyno = 0;
    qint32 m_rightDyno = 0;

    qint16 m_diferentRPM = 0;
};






#endif // MODEL_H
