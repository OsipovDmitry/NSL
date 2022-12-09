#include <QMovie>

#include "waitbox.h"
#include "ui_waitbox.h"

namespace view
{

WaitBox::WaitBox(QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::WaitBox>())
{
    m_ui->setupUi(this);

    auto movie = new QMovie(this);
    movie->setFileName(":/res/wait.gif");
    movie->setBackgroundColor(QColor(0,0,0,0));
    m_ui->label->setMovie(movie);
    m_ui->label->setStyleSheet("background-color: rgba(225,255,255,0);");
    m_ui->label->setAttribute(Qt::WA_TranslucentBackground, true);
    movie->start();
}

}
