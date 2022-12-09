#ifndef NSLCONTROLLER_H
#define NSLCONTROLLER_H

#include <memory>
#include <map>

#include <QObject>

#include <model/forwarddecl.h>
#include <controller/forwarddecl.h>

namespace controller
{

class NSLController : public QObject
{
    Q_OBJECT
public:
    static NSLController& instance();

    void registerNSLProvider(std::shared_ptr<INSLProvider>);

    std::shared_ptr<const model::Product> product() const;
    const std::string& productFilename() const;
    bool isThereUnsavedChanges() const;

    void onNewFile(const std::wstring&);
    void onOpenFile(const std::string&);
    void onSaveFile(const std::string&);
    void onCloseFile();

    void removeIngredient(uint32_t);
    void addIngredient(const model::Ingredient&);
    void changeIngredient(uint32_t, const model::Ingredient&);

    void removeExperiment(uint32_t);
    void addExperiment(const model::Experiment&);
    void changeExperiment(uint32_t, const model::Experiment&);

    void removeFromRecipe(uint32_t, uint32_t);
    void addToRecipe(uint32_t, const model::RecipeItem&);
    void changeInRecipe(uint32_t, uint32_t, const model::RecipeItem&);
    void changeRecipe(uint32_t, const model::Recipe&);

    void changeConclusion(uint32_t, const std::wstring&);

signals:
    void critical(const std::wstring&);
    void error(const std::wstring&);
    void warning(const std::wstring&);
    void info(const std::wstring&);

    void fileChanged();
    void productChanged();

private:
    NSLController();

    std::map<NSLProviderVersion, std::shared_ptr<INSLProvider>> m_providers;

    std::shared_ptr<model::Product> m_product;
    std::string m_productFilename;
    bool m_unsavedChanges;
};

}

#endif // NSLCONTROLLER_H
