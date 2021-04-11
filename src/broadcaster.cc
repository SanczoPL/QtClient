#include "../include/broadcaster.h"

constexpr auto IP{ "Ip" };
constexpr auto PORT{ "Port" };
constexpr auto PID{ "Pid" };
constexpr auto ID{ "Id" };
constexpr auto COMMAND{ "Command" };
constexpr auto PING{ "Ping" };
constexpr auto PING_PONG{ "PingPong" };
constexpr auto MESSAGE_TYPE{ "MessageType" };
constexpr auto TIME{ "Time" };
constexpr auto ERROR_DATA{ "Error" };
constexpr auto SENDER{ "Sender" };
constexpr auto FROM{ "From" };
constexpr auto TO{ "To" };


Broadcaster::Broadcaster(QJsonObject const& a_config) 
	: m_id{ a_config[ID].toInt() } 
{
	configure(a_config);
}

Broadcaster::~Broadcaster() {}

void Broadcaster::configure(QJsonObject const& a_config) 	
{
	Logger->trace("Broadcaster::configure()");
	QObject::connect(&m_IO, &MQt::newMessage, this, &Broadcaster::onNewMessage);
	QObject::connect(this, &Broadcaster::sendMessageRequest, &m_IO, &MQt::onSendMessage);
	QObject::connect(&m_IO, &MQt::connected, this, &Broadcaster::onConnected);
	QObject::connect(&m_IO, &MQt::disconnected, this, &Broadcaster::onDisconnected);

	m_ip = a_config[IP].toString();
	m_id = a_config[ID].toInt();
	m_port = static_cast<quint16>(a_config[PORT].toInt());
	onConnect();
	Logger->info("Broadcaster::configure() ip:{} port:{}", m_ip.toStdString(), QString::number(m_port).toStdString());
}

void Broadcaster::onConnect() 
{ 
	m_IO.startConnection(m_ip, m_port); 
}

void Broadcaster::onNewMessage(QByteArray const a_message) {
	Logger->trace("Broadcaster::onNewMessage(QByteArray const a_message)");
	MQtMessage message{};
	message.parse(a_message);
	
	bool ret = message.parse(a_message);
	if (ret <= 0) {
		Logger->error("Broadcaster::onNewMessage() msg not correct");
	}
	else {
		MQtMessage::Header m_header = message.header();
		Logger->trace("Broadcaster::onNewMessage() a_message:{} m_header:{}", a_message.size() - 20, m_header.size);
		Logger->trace("Broadcaster::onNewMessage() sender:{}", message.sender());
		if (message.type() == MQtMessage::BINARY)
		{
			Logger->debug("Recived message that is Message::BINARY");
			QByteArray data = message.content();
			emit(newBinaryMessage(data));
		}
		if (message.type() == MQtMessage::JSON || message.type() == MQtMessage::PING)
		{
			Logger->debug("Recived msg that is MQtMessage::JSON or MQtMessage::PING");
			if (message.isValid()) {
				const QJsonDocument jDoc{ QJsonDocument::fromJson(message.content()) };
				if (jDoc.isObject())
				{
					QJsonObject jOut{ {"none", "none"} };
					jOut = jDoc.object()[COMMAND].toObject();
					Logger->trace("Message is a JSON object:");
					emit(newMessage(jOut));
					QByteArray stateData{ QJsonDocument{jOut}.toJson(QJsonDocument::Compact) };
					Logger->trace("Broadcaster::onNewMessage from:{} \n message:{}", message.sender(), stateData.toStdString().c_str());
				}
				else if (jDoc.isArray())
				{
					QJsonArray jOut{ {"none", "none"} };
					jOut = jDoc.object()[COMMAND].toArray();
					Logger->trace("Message is a JSON array:");
					emit(newMessage(jOut));
					QByteArray stateData{ QJsonDocument{jOut}.toJson(QJsonDocument::Compact) };
					Logger->trace("Broadcaster::onNewMessage from:{} \n message:{}", message.sender(), stateData.toStdString().c_str());
				}
			}
		}
	}
}

void Broadcaster::onSendCommand(const qint32 topic, const QJsonObject json) {
	QJsonObject cmd = { {COMMAND, json} };
	MQtMessage msg{};
	QByteArray stateData{ QJsonDocument{cmd}.toJson(QJsonDocument::Compact) };
	msg.fromData(stateData, MQtMessage::JSON, m_id);
	Logger->trace("Broadcaster::onSendCommand from {} \nmessage:{}", m_id, stateData.toStdString().c_str());
	emit(sendMessageRequest(msg.rawData()));
}

void Broadcaster::onConnected() { emit(connected()); }

void Broadcaster::onDisconnected() { emit(disconnected()); }
