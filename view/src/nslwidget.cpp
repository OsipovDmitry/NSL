#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QSplashScreen>
#include <QCloseEvent>

#include <model/types.h>
#include <controller/nslcontroller.h>
#include <view/nslwidget.h>

#include "newproductdialog.h"
#include "waitbox.h"
#include "productwidget.h"
#include "ui_nslwidget.h"

namespace view
{

NSLWidget::NSLWidget(QWidget *parent)
   : QMainWindow(parent)
   , m_ui(std::make_shared<Ui::NSLWidget>())
{
    m_ui->setupUi(this);

    auto& ctrl = controller::NSLController::instance();
    connect(&ctrl, &controller::NSLController::critical, [this](const std::wstring& message){
        QMessageBox::critical(this, "", QString::fromStdWString(message));
        QApplication::quit();
    });
    connect(&ctrl, &controller::NSLController::error, [this](const std::wstring& message){
        QMessageBox::critical(this, "", QString::fromStdWString(message));
    });
    connect(&ctrl, &controller::NSLController::warning, [this](const std::wstring& message){
        QMessageBox::warning(this, "", QString::fromStdWString(message));
    });
    connect(&ctrl, &controller::NSLController::info, [this](const std::wstring& message){
        QMessageBox::information(this, "", QString::fromStdWString(message));
    });
    connect(&ctrl, &controller::NSLController::fileChanged, this, &NSLWidget::updateUi);
    connect(&ctrl, &controller::NSLController::productChanged, this, &NSLWidget::updateTitle);

    connect(m_ui->actionNew, &QAction::triggered, this, &NSLWidget::onFileNew);
    connect(m_ui->actionOpen, &QAction::triggered, this, &NSLWidget::onFileOpen);
    connect(m_ui->actionSave, &QAction::triggered, this, &NSLWidget::onFileSave);
    connect(m_ui->actionSaveAs, &QAction::triggered, this, &NSLWidget::onFileSaveAs);
    connect(m_ui->actionClose, &QAction::triggered, this, &NSLWidget::onFileClose);

    onFileClose();
}

void NSLWidget::closeEvent(QCloseEvent *event)
{
    auto& ctrl = controller::NSLController::instance();
    if (!ctrl.isThereUnsavedChanges())
    {
        QMainWindow::closeEvent(event);
    }
    else
    {
        auto result = QMessageBox::question(this,
                                            "",
                                            "The document has been modified.\nDo you want to save your changes?",
                                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                            QMessageBox::Save);
        if (result == QMessageBox::Save)
        {
            onFileSave();
            ctrl.isThereUnsavedChanges() ? event->ignore() : event->accept();
        }
        else if (result == QMessageBox::Discard)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

void NSLWidget::onFileNew()
{
    onFileClose();

    std::wstring newProductName;
    if (!showProductnameDialog("New product", L"", newProductName))
        return;

    QTimer::singleShot(0, [this, newProductName]() {
        controller::NSLController::instance().onNewFile(newProductName);
        closeWaitBox();
    });
    showWaitBox();
}

void NSLWidget::onFileOpen()
{
    onFileClose();

    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open NoSugarLife Product"),
                                                 QDir::currentPath(),
                                                 tr("NoSugarLife Products (*.nsl)"));
    if (fileName.isEmpty())
        return;

    QTimer::singleShot(0, [this, fileName]() {
        controller::NSLController::instance().onOpenFile(fileName.toStdString());
        closeWaitBox();
    });
    showWaitBox();
}

void NSLWidget::onFileSave()
{
    auto productFileName = QString::fromStdString(controller::NSLController::instance().productFilename());
    if (!QFile::exists(productFileName))
    {
        onFileSaveAs();
        return;
    }

    QTimer::singleShot(0, [this, productFileName]() {
        controller::NSLController::instance().onSaveFile(productFileName.toStdString());
        closeWaitBox();
    });
    showWaitBox();
}

void NSLWidget::onFileSaveAs()
{
    auto productFileName = QFileDialog::getSaveFileName(this,
                                                 tr("Save NoSugarLife Product"),
                                                 QDir::currentPath(),
                                                 tr("NoSugarLife Products (*.nsl)"));
    if (productFileName.isEmpty())
        return;

    if (!productFileName.endsWith(".nsl"))
        productFileName.append(".nsl");

    QTimer::singleShot(0, [this, productFileName]() {
        controller::NSLController::instance().onSaveFile(productFileName.toStdString());
        closeWaitBox();
    });
    showWaitBox();
}

void NSLWidget::onFileClose()
{
    if (controller::NSLController::instance().isThereUnsavedChanges())
    {
        auto productFileName = QString::fromStdString(controller::NSLController::instance().productFilename());
        if (productFileName.isEmpty()) productFileName = "\"Untitled\"";
        auto result =
                QMessageBox::question(this, "", "The product " + productFileName + " has been modified.\nDo you want to save it?",
                                      QMessageBox::StandardButtons(QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes),
                                      QMessageBox::StandardButton::Yes);
        if (result == QMessageBox::Cancel)
            return;
        else if (result == QMessageBox::Yes)
            onFileSave();
    }

    QTimer::singleShot(0, [this]() {
        controller::NSLController::instance().onCloseFile();
        closeWaitBox();
    });
    showWaitBox();
}

void NSLWidget::showWaitBox()
{
    closeWaitBox();

    auto waitBox = findChild<WaitBox*>(QString(), Qt::FindDirectChildrenOnly);
    if (!waitBox)
        waitBox = new WaitBox(this);

    waitBox->exec();
}

void NSLWidget::closeWaitBox()
{
    auto waitBox = findChild<WaitBox*>(QString(), Qt::FindDirectChildrenOnly);

    if (waitBox)
    {
        waitBox->accept();
        delete waitBox;
    }
}

void NSLWidget::updateTitle()
{
    auto& ctrl = controller::NSLController::instance();
    auto product = ctrl.product();
    const auto& productFilename = ctrl.productFilename();

// title
    QString title = "No Sugar Life";
    if (product)
    {
        title.append(" | ").append(QString::fromStdWString(product->name));
        title.append(" | ").append(productFilename.empty() ? "\"Untitled\"" : QString::fromStdString(productFilename));
        if (ctrl.isThereUnsavedChanges())
            title += "*";
    }
    setWindowTitle(title);
}

void NSLWidget::updateUi()
{
    updateTitle();

    auto& ctrl = controller::NSLController::instance();
    auto product = ctrl.product();

// menu
    m_ui->actionSave->setEnabled(product != nullptr);
    m_ui->actionSaveAs->setEnabled(product != nullptr);
    m_ui->actionClose->setEnabled(product != nullptr);

// central widget
    ProductWidget* productWidget = nullptr;
    if (product)
    {
        productWidget = findChild<ProductWidget*>(QString(), Qt::FindDirectChildrenOnly);
        if (!productWidget)
            productWidget = new ProductWidget(this);
    }
    setCentralWidget(productWidget);
}

bool NSLWidget::showProductnameDialog(const QString& title, const std::wstring& productName, std::wstring& resultName) const
{
    NewProductDialog newProductDialog(QString::fromStdWString(productName), const_cast<NSLWidget*>(this));
    newProductDialog.setWindowTitle(title);
    newProductDialog.adjustSize();

    bool result = false;
    if (newProductDialog.exec() == QDialog::Accepted)
    {
        QString newName = newProductDialog.productName();
        while (!newName.isEmpty() && newName[0].isSpace()) newName.remove(0, 1);
        while (!newName.isEmpty() && newName[newName.size()-1].isSpace()) newName.remove(newName.size()-1, 1);

        if (newName.isEmpty())
        {
            QString messageBoxText = "Prduct name can't be empty.";

            QMessageBox::warning(&newProductDialog, "", messageBoxText);
            result = showProductnameDialog(title, productName, resultName);
        }
        else
        {
            resultName = newName.toStdWString();
            result = true;
        }
    }

    return result;
}

}
