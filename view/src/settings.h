#ifndef VIEW_SETTINGS_H
#define VIEW_SETTINGS_H

#include "rapidjson/document.h"

namespace view
{

class Settings
{
public:
    static Settings& instance();



private:
    Settings();
    rapidjson::Document::ValueType* read(const std::string &);

    rapidjson::Document m_document;
};

}

#endif // VIEW_SETTINGS_H
