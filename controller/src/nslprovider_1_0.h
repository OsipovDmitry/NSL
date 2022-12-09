#ifndef NSLPROVIDER_1_0_H
#define NSLPROVIDER_1_0_H

#include "nslproviderbase.h"

namespace controller
{

class NSLProvider_1_0 : public NSLProviderBase
{
public:
    NSLProvider_1_0();

    std::shared_ptr<model::Product> load(const std::string&, std::wstring&) override;
    bool save(const std::string&, std::shared_ptr<model::Product>, std::wstring&) override;
};

}

#endif // NSLPROVIDER_1_0_H
