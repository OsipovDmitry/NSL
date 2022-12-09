#-------------------------------------------------
#
# Project created by QtCreator 2022-11-24T10:46:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = business
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    # controller
    controller/src/nslproviderbase.cpp \
    controller/src/nslprovider_1_0.cpp \
    controller/src/nslcontroller.cpp \
    # view
    view/src/richtextedit.cpp \
    view/src/recipeitemdialog.cpp \
    view/src/productwidget.cpp \
    view/src/experimentitemwidget.cpp \
    view/src/experimentdialog.cpp \
    view/src/conclusiondialog.cpp \
    view/src/ingredientdialog.cpp \
    view/src/waitbox.cpp \
    view/src/newproductdialog.cpp \
    view/src/settings.cpp \
    view/src/recalcrecipedalog.cpp \
    view/src/textbrowser.cpp \
    view/src/nslwidget.cpp

RESOURCES += \
    # view
    view/view.qrc

FORMS += \
    # view
    view/ui/recipeitemdialog.ui \
    view/ui/productwidget.ui \
    view/ui/Ingredientdialog.ui \
    view/ui/experimentdialog.ui \
    view/ui/conclusiondialog.ui \
    view/ui/waitbox.ui \
    view/ui/newproductdialog.ui \
    view/ui/recalcrecipedalog.ui \
    view/ui/nslwidget.ui

HEADERS += \
    # model
    model/types.h \
    model/color.h \
    model/forwarddecl.h \
    # controller
    controller/nslcontroller.h \
    controller/inslprovider.h \
    controller/forwarddecl.h \
    controller/src/nslproviderbase.h \
    controller/src/nslprovider_1_0.h \
    # view
    view/src/richtextedit.h \
    view/src/recipeitemdialog.h \
    view/src/productwidget.h \
    view/src/experimentitemwidget.h \
    view/src/experimentdialog.h \
    view/src/conclusiondialog.h \
    view/src/ingredientdialog.h \
    view/src/waitbox.h \
    view/src/newproductdialog.h \
    view/src/settings.h \
    view/src/recalcrecipedalog.h \
    view/src/textbrowser.h \
    view/nslwidget.h

settings.commands = $(COPY_FILE) $$PWD/settings.json $$OUT_PWD
samplecases.commands = $(COPY_DIR) $$PWD/samplecases $$OUT_PWD
first.depends = $(first) settings samplecases
export(first.depends)
export(settings.commands)
export(samplecases.commands)
QMAKE_EXTRA_TARGETS += first settings samplecases
