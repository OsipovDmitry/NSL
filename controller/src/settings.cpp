#include <QFile>

#include <controller/nslcontroller.h>
#include <controller/settings.h>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

namespace controller
{

const std::string Settings::s_settingsFilename = "settings.json";
const std::string Settings::s_saveProviderVersionPath = "SaveProviderVersion";
const std::string Settings::s_experimentshowColorPath = "ExperimentSettings.ShowColor";
const std::string Settings::s_experimentshowMarkPath = "ExperimentSettings.ShowMark";

struct SettingsPrivate
{
    rapidjson::Document document;

    rapidjson::Document::ValueType* getDocument(const std::string& path)
    {
        static const auto oneStep = [](rapidjson::Document::ValueType* &curDoc, const std::string& path, std::size_t& current, std::size_t& previous)
                -> bool
        {
            const auto member = path.substr(previous, current - previous);
            previous = current;

            if (!curDoc->IsObject() || !curDoc->HasMember(member.c_str()))
                return false;

            if (current != std::string::npos)
            {
                ++previous;
                current = path.find('.', previous);
            }

            curDoc = &(curDoc->operator [](member.c_str()));
            return true;
        };

        rapidjson::Document::ValueType *curDocument = &document;
        std::size_t current = path.find('.'), previous = 0;

        while (previous != std::string::npos)
            if (!oneStep(curDocument, path, current, previous))
                return nullptr;

        return curDocument;
    }

    rapidjson::Document::ValueType* getOrCreateDocument(const std::string& path)
    {
        static const auto oneStep = [](rapidjson::Document::ValueType* &curDoc, const std::string& path, std::size_t& current, std::size_t& previous)
                -> bool
        {
            const auto member = path.substr(previous, current - previous);

            if (!curDoc->IsObject() || !curDoc->HasMember(member.c_str()))
                return false;

            previous = current;
            if (current != std::string::npos)
            {
                ++previous;
                current = path.find('.', previous);
            }

            curDoc = &(curDoc->operator [](member.c_str()));
            return true;
        };

        rapidjson::Document::ValueType *curDocument = &document;
        std::size_t current = path.find('.'), previous = 0;

        bool stepResult = true;
        while ((previous != std::string::npos) && stepResult)
            stepResult = oneStep(curDocument, path, current, previous);

        if (!stepResult)
        {
            if (!curDocument->IsObject())
                return nullptr;

            while (previous != std::string::npos)
            {
                const auto member = path.substr(previous, current - previous);
                previous = current;

                curDocument->AddMember(rapidjson::Document::ValueType().SetString(member.c_str(), document.GetAllocator()),
                                       rapidjson::Document::ValueType(rapidjson::kObjectType),
                                       document.GetAllocator());
                curDocument = &(curDocument->operator [](member.c_str()));

                if (current != std::string::npos)
                {
                    ++previous;
                    current = path.find('.', previous);
                }
            }
        }

        return curDocument;
    }

    void removeDocument(const std::string& path)
    {
        static const auto oneStep = [](rapidjson::Document::ValueType* &curDoc, const std::string& path, std::size_t& current, std::size_t& previous)
                -> bool
        {
            const auto member = path.substr(previous, current - previous);
            previous = current;

            if (!curDoc->IsObject() || !curDoc->HasMember(member.c_str()))
                return false;

            if (current != std::string::npos)
            {
                ++previous;
                current = path.find('.', previous);
            }

            curDoc = &(curDoc->operator [](member.c_str()));
            return true;
        };

        rapidjson::Document::ValueType *curDocument = &document;
        std::size_t current = path.find('.'), previous = 0;

        while (current != std::string::npos)
            if (!oneStep(curDocument, path, current, previous))
                return;

        if (curDocument->IsObject())
        {
            const auto member = path.substr(previous, current - previous);
            curDocument->RemoveMember(member.c_str());
        }

    }
};

Settings &Settings::instance()
{
    static Settings settings;
    return settings;
}

controller::NSLProviderVersion Settings::saveProviderVersion()
{
    controller::NSLProviderVersion result(0, 0);

    if (auto versionArray = m_->getDocument(s_saveProviderVersionPath))
        if (versionArray->GetArray().Size() == 2)
        {
            result.first = versionArray->GetArray()[0].GetUint();
            result.second = versionArray->GetArray()[1].GetUint();
        }

    return result;
}

void Settings::setSaveProviderVersion(const controller::NSLProviderVersion& version)
{
    if (version == controller::NSLProviderVersion(0, 0))
        m_->removeDocument(s_saveProviderVersionPath);
    else if (auto versionArray = m_->getOrCreateDocument(s_saveProviderVersionPath))
    {
        versionArray->GetArray()[0].SetUint(version.first);
        versionArray->GetArray()[1].SetUint(version.second);
    }

    rewriteSettingsFile();
}

bool Settings::experimentShowColor()
{
    bool result = true;

    if (auto doc = m_->getDocument(s_experimentshowColorPath))
        result = doc->GetBool();

    return result;
}

void Settings::setExperimentShowColor(bool value)
{
    if (auto doc = m_->getOrCreateDocument(s_experimentshowColorPath))
        doc->SetBool(value);

    rewriteSettingsFile();
}

bool Settings::experimentShowMark()
{
    bool result = true;

    if (auto doc = m_->getDocument(s_experimentshowMarkPath))
        result = doc->GetBool();

    return result;
}

void Settings::setExperimentShowMark(bool value)
{
    if (auto doc = m_->getOrCreateDocument(s_experimentshowMarkPath))
        doc->SetBool(value);

    rewriteSettingsFile();
}

Settings::Settings()
    : m_(std::unique_ptr<SettingsPrivate>(new SettingsPrivate))
{
    QFile file(QString::fromStdString(s_settingsFilename));
    if (!file.open(QFile::ReadOnly))
    {
        emit controller::NSLController::instance().critical(L"Can't open \"settings.json\"");
        return;
    }

    auto byteArray = file.readAll();
    file.close();
    m_->document.Parse(byteArray);
}

void Settings::rewriteSettingsFile()
{
    QFile file(QString::fromStdString("123.json"/*s_settingsFilename*/));
    if (!file.open(QFile::WriteOnly))
    {
        emit controller::NSLController::instance().critical(L"Can't open \"settings.json\"");
        return;
    }

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    m_->document.Accept(writer);

    QByteArray byteArray(sb.GetString());
    file.write(byteArray);
    file.close();
}

}
