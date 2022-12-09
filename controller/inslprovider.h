#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include <memory>

#include <model/types.h>
#include <controller/forwarddecl.h>

namespace controller
{

class INSLProvider
{
public:
    virtual ~INSLProvider() = default;

    virtual NSLProviderVersion version() const = 0;

    virtual std::shared_ptr<model::Product> load(const std::string&, std::wstring&) = 0;
    virtual bool save(const std::string&, std::shared_ptr<model::Product>, std::wstring&) = 0;
};

}

#endif // IMPORTEXPORTINTERFACE_H
