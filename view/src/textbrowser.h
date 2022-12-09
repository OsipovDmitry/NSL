#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QTextBrowser>

namespace view
{

class TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit TextBrowser(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void doubleClicked();
};

}

#endif // TEXTBROWSER_H
