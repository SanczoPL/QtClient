#include "../include/mqtmessage.h"

auto constexpr DATA_PREFIX = "%^^%";
int constexpr PREFIX_SIZE = 4;
int constexpr BIN_HEADER_SIZE = sizeof(MQtMessage::Header);

MQtMessage::MQtMessage() {}

MQtMessage::MQtMessage(QByteArray const& a_data)
{
	parse(a_data);
}

MQtMessage::~MQtMessage() {}

bool MQtMessage::checkPrefix(QByteArray const& a_data)
{
	if (memcmp(a_data.constData(), DATA_PREFIX, PREFIX_SIZE) != 0) return false;

	return true;
}

int MQtMessage::validate(QByteArray const& a_data)
{
	if (memcmp(a_data.constData(), DATA_PREFIX, PREFIX_SIZE) != 0) return false;

	qint32 size{};
	memcpy(&size, a_data.constData() + PREFIX_SIZE, sizeof(qint32));

	if (a_data.size() < size + BIN_HEADER_SIZE) return false;

	return size + BIN_HEADER_SIZE;
}

bool MQtMessage::parse(QByteArray const& a_data)
{
	m_valid = false;
	if (!validate(a_data)) return false;

	m_rawData = a_data;
	memcpy(&m_header, m_rawData.constData(), BIN_HEADER_SIZE);
	m_content = QByteArray{ m_rawData.constData() + BIN_HEADER_SIZE, m_header.size };

	m_valid = true;
	return true;
}

void MQtMessage::fromData(QByteArray const& a_data, MessageType const a_type, qint32 const a_sender)
{
	memcpy(m_header.prefix, DATA_PREFIX, PREFIX_SIZE);
	m_header.size = a_data.size();
	m_header.type = a_type;
	m_header.sender = a_sender;

	char tempHeader[BIN_HEADER_SIZE];
	memcpy(tempHeader, &m_header, BIN_HEADER_SIZE);

	m_rawData = QByteArray::fromRawData(tempHeader, BIN_HEADER_SIZE);
	m_rawData += a_data;

	m_content = QByteArray(m_rawData.constData() + BIN_HEADER_SIZE, a_data.size());

	m_valid = true;
}
