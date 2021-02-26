#ifndef MQT_SOCKIO_H
#define MQT_SOCKIO_H

#include <QObject>
#include <QtNetwork>
#include "mqtmessage.h"

class MQtSockIO : public QObject {
	Q_OBJECT
public:
	explicit MQtSockIO(QTcpSocket* a_socket, QObject* a_parent = nullptr);

	bool hasMessages();
	MQtMessage nextMessage();
	bool sendMessage(MQtMessage const& a_message);
	bool sendMessage(QByteArray const& a_message);

	void applyFilter(QVector<qint32> a_topics);

signals:
	void newMessage();

private slots:
	void onReadyRead();

private:
	QTcpSocket* m_socket;
	QByteArray m_bufer{};
	QVector<MQtMessage> m_messageQueue{};
	QVector<qint32> m_topicFilter{};
};

#endif // MQT_SOCKIO_H
