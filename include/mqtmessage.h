#ifndef MQT_MESSAGE_H
#define MQT_MESSAGE_H

#include <QByteArray>
#include <QtGlobal>


class MQtMessage {
public:
	MQtMessage();
	MQtMessage(QByteArray const& a_data);
	~MQtMessage();

	enum MessageType { PING = 0, JSON = 1, BINARY = 2 };
	static int validate(QByteArray const& a_data);
	static bool checkPrefix(QByteArray const& a_data);
	static MQtMessage& createFromRaw(QByteArray const& a_data);

	bool parse(QByteArray const& a_data);
	bool isValid() { return m_valid; }
	void fromData(QByteArray const& a_data, MessageType const a_type, qint32 const a_sender);

	typedef struct Header {
		char prefix[4]{};
		qint32 size{};
		qint32 type{};
		qint32 sender{};
	} Header;

	QByteArray const& content() const { return m_content; }
	Header const& header() const { return m_header; }
	QByteArray const& rawData() const { return m_rawData; }

	qint32 sender() { return m_header.sender; }
	qint32 size() { return m_header.size; }
	qint32 type() { return m_header.type; }

private:
	QByteArray m_rawData{};
	Header m_header{};
	QByteArray m_content{};

	bool m_valid{};
};
#endif // MQT_MESSAGE_H