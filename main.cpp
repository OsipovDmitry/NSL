#include <QApplication>

#include <view/nslwidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    view::NSLWidget w;
    w.show();

    return a.exec();
}
