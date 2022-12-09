#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QTextEdit>
#include <QToolBar>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextList>
#include <QMessageBox>
#include <QMimeData>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "richtextedit.h"

namespace view
{

const QString rsrcPath = ":/res/richtextedit";

class CustomTextEdit : public QTextEdit
{
public:
    explicit CustomTextEdit(QWidget *parent = Q_NULLPTR) : QTextEdit(parent) {}
    ~CustomTextEdit() override = default;

protected:
    bool canInsertFromMimeData(const QMimeData *source) const override
    {
        if (source->hasImage())
            return true;
        else
            return QTextEdit::canInsertFromMimeData(source);
    }

    void insertFromMimeData(const QMimeData *source)
    {
        if (source->hasImage())
        {
            document()->addResource(QTextDocument::ImageResource, QUrl("newPixmap123"), qvariant_cast<QPixmap>(source->imageData()));
            textCursor().insertImage("newPixmap123");
        }
        else
            QTextEdit::insertFromMimeData(source);
    }
};

RichTextEdit::RichTextEdit(QWidget *parent)
    : QWidget(parent)
{
    textEdit = new CustomTextEdit(this);
    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &RichTextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &RichTextEdit::cursorPositionChanged);

    auto hLayout = new QHBoxLayout;
    hLayout->setMargin(1);
    hLayout->addWidget(setupEditActions());
    hLayout->addWidget(setupTextActions());
    hLayout->addStretch();

    auto layout = new QVBoxLayout;
    layout->setMargin(1);
    layout->addLayout(hLayout);
    layout->addWidget(setupFormatActions());
    layout->addWidget(textEdit);

    setLayout(layout);

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable, actionRedo, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &RichTextEdit::clipboardDataChanged);
#endif

    textEdit->setFocus();
}

QString RichTextEdit::text() const
{
    return textEdit->toHtml();
}

void RichTextEdit::setText(const QString& data)
{
    textEdit->setHtml(data);
}

QToolBar *RichTextEdit::setupEditActions()
{
    QToolBar *tb = new QToolBar(tr("Edit Actions"));

    const QIcon undoIcon(rsrcPath + "/editundo.png");
    actionUndo = new QAction(undoIcon, tr("&Undo"));
    connect(actionUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon(rsrcPath + "/editredo.png");
    actionRedo = new QAction(redoIcon, tr("&Redo"));
    connect(actionRedo, &QAction::triggered, textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon(rsrcPath + "/editcut.png");
    actionCut = new QAction(cutIcon, tr("Cu&t"));
    connect(actionCut, &QAction::triggered, textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon(rsrcPath + "/editcopy.png");
    actionCopy = new QAction(copyIcon, tr("&Copy"));
    connect(actionCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon(rsrcPath + "/editpaste.png");
    actionPaste = new QAction(pasteIcon, tr("&Paste"));
    connect(actionPaste, &QAction::triggered, textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
#endif

    return tb;
}

QToolBar *RichTextEdit::setupTextActions()
{
    QToolBar *tb = new QToolBar(tr("Format Actions"));

    const QIcon boldIcon(rsrcPath + "/textbold.png");
    actionTextBold = new QAction(boldIcon, tr("&Bold"));
    connect(actionTextBold, &QAction::triggered, this, &RichTextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon(rsrcPath + "/textitalic.png");
    actionTextItalic = new QAction(italicIcon, tr("&Italic"));
    connect(actionTextItalic, &QAction::triggered, this, &RichTextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon(rsrcPath + "/textunder.png");
    actionTextUnderline = new QAction(underlineIcon, tr("&Underline"));
    connect(actionTextUnderline, &QAction::triggered, this, &RichTextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    const QIcon leftIcon(rsrcPath + "/textleft.png");
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon(rsrcPath + "/textcenter.png");
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon(rsrcPath + "/textright.png");
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon(rsrcPath + "/textjustify.png");
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &RichTextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = new QAction(pix, tr("&Color..."));
    connect(actionTextColor, &QAction::triggered, this, &RichTextEdit::textColor);
    tb->addAction(actionTextColor);

    return tb;
}

QToolBar *RichTextEdit::setupFormatActions()
{
    QToolBar *tb = new QToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &RichTextEdit::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &RichTextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &RichTextEdit::textSize);

    return tb;
}

void RichTextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void RichTextEdit::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void RichTextEdit::textColor()
{
    auto col = QColorDialog::getColor(textEdit->textColor(), this, "", QColorDialog::DontUseNativeDialog);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void RichTextEdit::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void RichTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void RichTextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
}

void RichTextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    bool canBePasted = false;

    if (textEdit->canPaste())
        canBePasted = true;

    actionPaste->setEnabled(canBePasted);
#endif
}

void RichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void RichTextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void RichTextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void RichTextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

}

