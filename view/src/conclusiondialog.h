#ifndef CONCLUSIONDIALOG_H
#define CONCLUSIONDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui {
class ConclusionDialog;
}

namespace view
{

class ConclusionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConclusionDialog(const QString&, QWidget *parent = 0);

    QString text() const;
    void setText(const QString&);

private:
    std::shared_ptr<Ui::ConclusionDialog> m_ui;
};

}

#endif // CONCLUSIONDIALOG_H
