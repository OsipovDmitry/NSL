#include "textbrowser.h"

namespace view
{

TextBrowser::TextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{

}

void TextBrowser::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubleClicked();
    QTextBrowser::mouseDoubleClickEvent(event);
}

}
