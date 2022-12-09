#include <QFile>
#include <QDataStream>

#include "nslproviderbase.h"

namespace controller
{

std::array<char, 3> NSLProviderBase::s_code {'N', 'S', 'L'};

NSLProviderBase::NSLProviderBase(uint32_t major, uint32_t minor)
    : m_version(major, minor)
{
}

NSLProviderVersion NSLProviderBase::version() const
{
    return m_version;
}

bool NSLProviderBase::loadHeader(const std::string& filename, NSLHeader& header)
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream stream(&file);
    auto result = readHeader(stream, header);
    file.close();

    return result;
}

bool NSLProviderBase::checkHeader(const NSLHeader& header) const
{
    return (header.code[0] == s_code[0]) &&
           (header.code[1] == s_code[1]) &&
           (header.code[2] == s_code[2]) &&
           (header.version == m_version);
}

void NSLProviderBase::writeHeader(QDataStream& stream)
{
    stream.writeRawData(s_code.data(), s_code.size() * sizeof(s_code.front()));
    stream << m_version.first;
    stream << m_version.second;
}

bool NSLProviderBase::readHeader(QDataStream& stream, NSLHeader& header)
{
    stream.readRawData(header.code.data(), header.code.size() * sizeof(header.code.front()));
    stream >> header.version.first;
    stream >> header.version.second;
    return true;
}

}
