#ifndef EXPERIMENTDIALOG_H
#define EXPERIMENTDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui {
class ExperimentDialog;
}

namespace view
{

class ExperimentDialog : public QDialog
{
    Q_OBJECT

public:
    ExperimentDialog(const QColor&, const QString&, QWidget *parent = 0);

    QColor color() const;
    void setColor(const QColor&);

    QString name() const;
    void setName(const QString&);

private:
    void onColorButtonClicked();

    std::shared_ptr<Ui::ExperimentDialog> m_ui;
};

}

#endif // EXPERIMENTDIALOG_H
