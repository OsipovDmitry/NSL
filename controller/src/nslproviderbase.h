#ifndef IMPORTEXPORTBASE_H
#define IMPORTEXPORTBASE_H

#include <controller/inslprovider.h>

class QDataStream;

namespace controller
{

struct NSLHeader
{
    std::array<char, 3> code;
    NSLProviderVersion version;
};

class NSLProviderBase : public INSLProvider
{
public:
    NSLProviderBase(uint32_t major, uint32_t minor);

    NSLProviderVersion version() const override;

    static bool loadHeader(const std::string&, NSLHeader&);

protected:
    bool checkHeader(const NSLHeader&) const;
    void writeHeader(QDataStream&);
    static bool readHeader(QDataStream&, NSLHeader&);

    NSLProviderVersion m_version;
    static std::array<char, 3> s_code;
};

}

#endif // IMPORTEXPORTBASE_H
