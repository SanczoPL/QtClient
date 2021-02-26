#include "../include/mqtsockio.h"
#include "includespdlog.h"

MQtSockIO::MQtSockIO(QTcpSocket* a_socket, QObject* parent)
	: QObject(parent)
	, m_socket{ a_socket }
{
	connect(m_socket, &QTcpSocket::readyRead, this, &MQtSockIO::onReadyRead);
}

bool MQtSockIO::hasMessages()
{
	return !m_messageQueue.isEmpty();
}

MQtMessage MQtSockIO::nextMessage()
{
	if (!hasMessages()) qFatal("No mesages in queue!");

	MQtMessage const MESSAGE{ m_messageQueue[0] };
	m_messageQueue.pop_front();
	return MESSAGE;
}

bool MQtSockIO::sendMessage(MQtMessage const& a_message)
{
	if (m_socket->write(a_message.rawData()) < 0) {
		return false;
	}

	return true;
}

bool MQtSockIO::sendMessage(QByteArray const& a_message)
{
	if (m_socket->write(a_message) < 0) {
		return false;
	}
	return true;
}

void MQtSockIO::onReadyRead()
{
	Logger->trace("MQtSockIO::onReadyRead()");
	m_bufer += m_socket->readAll();
	qDebug() << "m_bufer:" << m_bufer;
	while (m_bufer.size() >= static_cast<int>(sizeof(MQtMessage::Header))) {
		Logger->trace("while");
		if (MQtMessage::checkPrefix(m_bufer)) {
			Logger->trace("checkPrefix ok");
			auto messageSize = MQtMessage::validate(m_bufer);
			Logger->trace("messageSize:{}", messageSize);
			if (messageSize > 0) {
				m_messageQueue.push_back(MQtMessage{ m_bufer });
				Logger->trace("emit(newMessage());");
				emit(newMessage());
				m_bufer.remove(0, messageSize);
			}
			else {
				Logger->warn("messageSize == 0");
				break;
			}
		}
		else {
			Logger->warn("bad prefix");
			m_bufer.remove(0, 1);
		}
	}
}
