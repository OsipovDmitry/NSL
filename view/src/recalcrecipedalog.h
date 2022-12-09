#ifndef RECALCRECIPEDALOG_H
#define RECALCRECIPEDALOG_H

#include <memory>

#include <QDialog>

namespace Ui {
class RecalcRecipeDalog;
}

namespace view
{

class RecalcRecipeDalog : public QDialog
{
    Q_OBJECT

public:
    explicit RecalcRecipeDalog(double, QWidget *parent = 0);

    double weight() const;
    void setWeight(double);

private:
    std::shared_ptr<Ui::RecalcRecipeDalog> m_ui;
};

}

#endif // RECALCRECIPEDALOG_H
