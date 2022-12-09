#include <QFile>

#include <controller/nslcontroller.h>

#include "settings.h"

namespace view
{

Settings &Settings::instance()
{
    static Settings settings;
    return settings;
}

Settings::Settings()
{
    QFile file("settings.json");
    if (!file.open(QFile::ReadOnly))
    {
        emit controller::NSLController::instance().critical(L"Can't open \"settings.json\"");
        return;
    }

    auto byteArray = file.readAll();
    file.close();
    m_document.Parse(byteArray);
}

rapidjson::Document::ValueType* Settings::read(const std::string& path)
{
    static const auto oneStep = [](rapidjson::Document::ValueType *curDoc, const std::string& path, std::size_t& current, std::size_t& previous)
            -> rapidjson::Document::ValueType*
    {
        const auto member = path.substr(previous, current - previous);

        if (!curDoc->HasMember(member.c_str()))
            return nullptr;

        previous = current + 1;
        current = path.find('.', previous);
        return &(curDoc->operator [](member.c_str()));
    };

    rapidjson::Document::ValueType *curDocument = &m_document;
    std::size_t current = path.find('.'), previous = 0;

    while (current != std::string::npos && curDocument)
        curDocument = oneStep(curDocument, path, current, previous);

    if (curDocument)
        curDocument = oneStep(curDocument, path, current, previous);

    return curDocument;
}

}
