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
	void onPing(QJsonObject ping);

public slots:
	void configure(const QJsonObject& a_config);
	void onConnect();
	void onNewMessage(const QByteArray a_message);
	void onSendCommand(const qint32 topic, const QJsonObject json);
	void onSendImage(const qint32 topic, QByteArray image);
	void onSendPing(const qint32 topic);
	void onSendPingWithId(const qint32 topic, const qint32 id);
	void onSendPingPong(QJsonObject json);
	void onSendError(const qint32 topic, const qint32 error);
	void onConnected();
	void onDisconnected();

signals:
	void subscribeRequest(QVector<qint32> const a_topics);
	void unsubscribeRequest(QVector<qint32> const a_topics);
	void sendMessageRequest(QByteArray const a_message);
	void newMessage(QJsonObject const& a_json);
	void connected();
	void disconnected();
	void updateImage(QByteArray image);
	void updatePing(QJsonObject ping);
	void updateError(QJsonObject error);

private:
	QString m_ip{};
	quint16 m_port{};
	qint32 m_id{};
};
#endif // BROADCASTER_H
