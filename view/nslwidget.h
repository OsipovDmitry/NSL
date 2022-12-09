#ifndef BUSINESS_H
#define BUSINESS_H

#include <memory>

#include <QMainWindow>

namespace Ui {
class NSLWidget;
}

namespace view
{

class NSLWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit NSLWidget(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent*) override;

private:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileClose();

    void showWaitBox();
    void closeWaitBox();

    void updateTitle();
    void updateUi();

    bool showProductnameDialog(const QString&, const std::wstring&, std::wstring&) const;

    std::shared_ptr<Ui::NSLWidget> m_ui;
};

}

#endif // BUSINESS_H
