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

    bool showKcalInIngredients();
    void setShowKcalInIngredients(bool);

    bool showKcalInExperiments();
    void setShowKcalInExperiments(bool);

private:
    Settings();

    bool readBool(const std::string&, bool);
    void writeBool(const std::string&, bool);

    void rewriteSettingsFile();

    static const std::string s_settingsFilename;
    static const std::string s_saveProviderVersionPath;
    static const std::string s_experimentShowColorPath;
    static const std::string s_experimentShowMarkPath;
    static const std::string s_showKcalInIngredients;
    static const std::string s_showKcalInExperiments;

    std::unique_ptr<SettingsPrivate> m_;
};

}

#endif // VIEW_SETTINGS_H
