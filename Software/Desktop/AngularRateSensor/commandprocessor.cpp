#include "commandprocessor.h"
#include <QDebug>
#define SPLITTER 0x00AA3C55
CommandProcessor::CommandProcessor(QObject *parent) : QObject(parent)
{

}

bool CommandProcessor::FindeComand(const QByteArray &data)
{
    if((!m_aCommandReaderBuffer.count()) && (!data.count()))return false;

    for (QByteArray::const_iterator i = data.begin(); i != data.end(); ++i)
        m_aCommandReaderBuffer.enqueue(uint8_t(*i));



    while(!m_aCommandReaderBuffer.isEmpty())
    {
        switch (m_stusFindeComand)
        {  
            case isSplitterFinde:
            {
                m_Splitter = ((m_Splitter << 8)|  m_aCommandReaderBuffer.dequeue())&0x00ffffff;
                //qDebug()<< "Data read:"<<hex<<m_Splitter;
                if(m_Splitter == SPLITTER)
                {
                    m_stusFindeComand = isCommandFinde;
                }

            } break;

            case isCommandFinde:
            {
                m_lastCommand.comdan =  m_aCommandReaderBuffer.dequeue();

                if(0) //! \todo добавить проверку на размер блока данных
                    m_stusFindeComand = isDataFinde;
                else
                {
                    m_stusFindeComand = isSplitterFinde;
                    return  true;
                }


            } break;

            case isDataFinde:
            {
                m_stusFindeComand = isDataFinde;
            } break;

        }
    }
    return false;
}

CommandObject CommandProcessor::GetLastComand()
{
    return m_lastCommand;
}
