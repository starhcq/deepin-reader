QT += core gui sql printsupport dbus xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-reader

TEMPLATE = app

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

CONFIG += c++11 link_pkgconfig

PKGCONFIG += ddjvuapi dtkwidget

SRCPWD=$$PWD    #用于被单元测试方便的复用
3RDPARTTPATH = $$SRCPWD/../3rdparty
INCLUDEPATH += $$SRCPWD/uiframe
INCLUDEPATH += $${3RDPARTTPATH}/poppler-0.89.0/qt5/src

LIBS += -L"$${3RDPARTTPATH}/lib" -ldeepin-poppler-qt -ldeepin-poppler
!system(mkdir -p $${3RDPARTTPATH}/output && cd $${3RDPARTTPATH}/output && cmake $${3RDPARTTPATH}/poppler-0.89.0 && make){
    error("Build deepin-poppler library failed.")
}
QMAKE_RPATHDIR += /usr/lib/deepin-reader

QMAKE_CXXFLAGS += "-Wl,--as-needed -fPIE"
QMAKE_LFLAGS += -pie
contains(QMAKE_HOST.arch, mips64):{
    QMAKE_CXXFLAGS += "-O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi"
}

include ($$SRCPWD/app/app.pri)
include ($$SRCPWD/browser/browser.pri)
include ($$SRCPWD/sidebar/sidebar.pri)
include ($$SRCPWD/widgets/widgets.pri)
include ($$SRCPWD/document/document.pri)
include ($$SRCPWD/uiframe/uiframe.pri)

SOURCES += \
    $$SRCPWD/Application.cpp \
    $$SRCPWD/main.cpp \
    $$SRCPWD/MainWindow.cpp

HEADERS +=\
    $$SRCPWD/Application.h\
    $$SRCPWD/MainWindow.h

RESOURCES +=         \
    $$SRCPWD/../resources/resources.qrc

TRANSLATIONS += \
    $$SRCPWD/../translations/deepin-reader.ts

target.path   = /usr/bin

desktop.path  = /usr/share/applications
desktop.files = $$SRCPWD/deepin-reader.desktop

poppler.path = /usr/lib/deepin-reader
poppler.files = $${3RDPARTTPATH}/lib/*.so*

icon.path = /usr/share/icons/hicolor/scalable/apps
icon.files = $$SRCPWD/deepin-reader.svg

manual.path = /usr/share/deepin-manual/manual-assets/application
manual.files = $$PWD/../assets/*

INSTALLS += target desktop icon poppler manual

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$SRCPWD/../translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/$$TARGET/translations
    dtk_translations.files = $$SRCPWD/../translations/*.qm
    INSTALLS += dtk_translations
}
