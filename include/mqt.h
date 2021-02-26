#ifndef MQT_H
#define MQT_H

#include <QObject>
#include <QTimer>
#include <QtNetwork>
#include "../include/mqtmessage.h"
#include "../include/mqtsockio.h"
#include "includespdlog.h"

class MQtSockIO;

class MQt : public QObject
{
	Q_OBJECT

public:
	explicit MQt(QObject* a_parent = nullptr);
	explicit MQt(QString const& a_ip, quint16 const a_port, QObject* a_parent = nullptr);
	~MQt();

	bool startConnection();
	bool startConnection(QString const& a_ip, quint16 const a_port);
	void endConnection();
	void setAutoReconnect(bool const a_true, int const a_msecRetry);

signals:
	void connected();
	void disconnected();
	void error(QAbstractSocket::SocketError a_socketError);
	void newMessage(QByteArray const a_msg);

public slots:
	void onSendMessage(QByteArray const a_msg);

private slots:
	void onConnection();
	void onDisconnection();
	void onReconnectTimeout();
	void onSocketError(QAbstractSocket::SocketError a_socketError);
	void onSockMessage();

private:
	QTcpSocket* m_socket;
	MQtSockIO* m_sockIO;
	QString m_ipAddr{};
	quint16 m_port{};
	bool m_isConnected{};

	bool m_isAutoReconnecting{};
	QTimer m_reconnectTimer;

	QVector<qint32> m_subscribedTopics{};

private:
	void init();
};

#endif // MQt_H