#ifndef VIEW_SETTINGS_H
#define VIEW_SETTINGS_H

#include <memory>

#include <controller/forwarddecl.h>

namespace controller
{

class SettingsPrivate;
class Settings
{
public:
    static Settings& instance();

    controller::NSLProviderVersion saveProviderVersion();
    void setSaveProviderVersion(const controller::NSLProviderVersion&);

    bool experimentShowColor();
    void setExperimentShowColor(bool);

    bool experimentShowMark();
    void setExperimentShowMark(bool);

private:
    Settings();
    void rewriteSettingsFile();

    static const std::string s_settingsFilename;
    static const std::string s_saveProviderVersionPath;
    static const std::string s_experimentshowColorPath;
    static const std::string s_experimentshowMarkPath;

    std::unique_ptr<SettingsPrivate> m_;
};

}

#endif // VIEW_SETTINGS_H
