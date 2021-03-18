QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adjnode.cpp \
    edgegraphics.cpp \
    filemanagement.cpp \
    graph.cpp \
    graphscene.cpp \
    graphtexteditor.cpp \
    graphtextline.cpp \
    main.cpp \
    graphviewer.cpp \
    node.cpp \
    nodegraphics.cpp

HEADERS += \
    adjnode.h \
    edgegraphics.h \
    filemanagement.h \
    graph.h \
    graphscene.h \
    graphtexteditor.h \
    graphtextline.h \
    graphviewer.h \
    node.h \
    nodegraphics.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    graph_images.qrc

RC_ICONS = circle.ico

DISTFILES +=
