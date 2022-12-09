#ifndef RICHTEXTEDIT_H
#define RICHTEXTEDIT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QToolBar;
QT_END_NAMESPACE

namespace view
{

class CustomTextEdit;

class RichTextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit RichTextEdit(QWidget *parent = 0);

    QString text() const;
    void setText(const QString&);

protected:
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();

private:
    QToolBar *setupEditActions();
    QToolBar *setupTextActions();
    QToolBar *setupFormatActions();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    CustomTextEdit *textEdit;
};

}

#endif // RICHTEXTEDIT_H
