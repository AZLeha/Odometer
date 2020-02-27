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

                if(m_lastCommand.comdan & 0x80) //еденичка в старшем разяде признак команды
                {
                    m_stusFindeComand = isSplitterFinde;
                    return  true;
                }

                else
                {
                     m_stusFindeComand = isDataFinde;
                     m_DataCounter = m_lastCommand.comdan;
                     m_lastCommand.data.clear();
                }


            } break;

            case isDataFinde:
            {
                if(m_DataCounter--)
                    m_lastCommand.data.append(static_cast<char>(m_aCommandReaderBuffer.dequeue()));
                else
                {
                    m_stusFindeComand = isSplitterFinde;
                    return true;
                }
            } break;

        }
    }
    return false;
}

CommandObject CommandProcessor::GetLastComand()
{
    return m_lastCommand;
}
