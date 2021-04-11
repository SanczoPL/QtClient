#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QVector>
#include <stdio.h>

#include "mqt.h"
#include "includespdlog.h"


class Broadcaster : public QObject {
	Q_OBJECT

public:
	Broadcaster(QJsonObject const& a_config);
	~Broadcaster();

private:
	MQt m_IO{};

public slots:
	void configure(const QJsonObject& a_config);
	void onConnect();
	void onNewMessage(const QByteArray a_message);
	void onSendCommand(const qint32 topic, const QJsonObject json);

	void onConnected();
	void onDisconnected();

signals:
	void sendMessageRequest(QByteArray const a_message);
	void newMessage(const QJsonObject & a_json);
	void newMessage(const QJsonArray & a_json);
	void newBinaryMessage(const QByteArray& a_message);
	void connected();
	void disconnected();

private:
	QString m_ip{};
	quint16 m_port{};
	qint32 m_id{};
};
#endif // BROADCASTER_H
