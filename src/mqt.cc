#include "../include/mqt.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


MQt::MQt(QObject* a_parent)
	: QObject(a_parent)
	, m_socket{ new QTcpSocket{ this } }
	, m_sockIO{ new MQtSockIO{ m_socket } }
	, m_reconnectTimer{ this }
{
	init();
}

MQt::MQt(const QString& a_ip, quint16 const a_port, QObject* a_parent)
	: QObject(a_parent)
	, m_socket{ new QTcpSocket{ this } }
	, m_sockIO{ new MQtSockIO{ m_socket } }
	, m_ipAddr{ a_ip }
	, m_port{ a_port }
	, m_reconnectTimer{ this }
{
	init();
}

MQt::~MQt()
{
	delete m_sockIO;
}

bool MQt::startConnection()
{
	if (m_isConnected || m_ipAddr.isEmpty())
	{
		return false;
	}

	m_socket->connectToHost(m_ipAddr, m_port);

	return true;
}

bool MQt::startConnection(const QString& a_ip, quint16 const a_port)
{
	if (m_isConnected || a_ip.isEmpty())
	{
		return false;
	}
	m_ipAddr = a_ip;
	m_port = a_port;

	m_socket->connectToHost(m_ipAddr, m_port);
	return true;
}

void MQt::endConnection()
{
	m_socket->close();
}

void MQt::setAutoReconnect(bool const a_true, int const a_msecRetry)
{
	m_isAutoReconnecting = a_true;

	if (!a_true)
	{
		m_reconnectTimer.stop();
	}

	m_reconnectTimer.setInterval(a_msecRetry);
}

void MQt::onSendMessage(QByteArray const a_msg)
{
	m_sockIO->sendMessage(a_msg);
}

void MQt::onConnection()
{
	m_isConnected = true;

	if (m_isAutoReconnecting)
	{
		m_reconnectTimer.stop();
	}

	emit(connected());
}

void MQt::onDisconnection()
{
	m_isConnected = false;

	if (m_isAutoReconnecting)
	{
		m_reconnectTimer.start();
	}

	emit(disconnected());
}

void MQt::onReconnectTimeout()
{
	startConnection();
}

void MQt::onSocketError(QAbstractSocket::SocketError a_socketError)
{
	if (m_isAutoReconnecting)
	{
		m_reconnectTimer.start();
	}

	emit(error(a_socketError));
}

void MQt::onSockMessage()
{
	Logger->trace("MQt::onSockMessage()");
	while (m_sockIO->hasMessages())
	{
		Logger->trace("MQt::onSockMessage() emit(newMessage(m_sockIO->nextMessage()");
		emit(newMessage(m_sockIO->nextMessage().rawData()));
	}
}

void MQt::init()
{
	connect(m_socket, &QTcpSocket::connected, this, &MQt::onConnection);
	connect(m_socket, &QTcpSocket::disconnected, this, &MQt::onDisconnection);
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &MQt::onSocketError);
	connect(m_sockIO, &MQtSockIO::newMessage, this, &MQt::onSockMessage);
	connect(&m_reconnectTimer, &QTimer::timeout, this, &MQt::onReconnectTimeout);
}