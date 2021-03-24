QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Model/adjnode.cpp \
    Model/algorithm.cpp \
    Model/filemanagement.cpp \
    Model/graph.cpp \
    Model/node.cpp \
    View/edgegraphics.cpp \
    View/graphscene.cpp \
    View/graphtexteditor.cpp \
    View/graphtextline.cpp \
    View/graphviewer.cpp \
    View/nodegraphics.cpp \
    main.cpp \

HEADERS += \
    Model/adjnode.h \
    Model/algorithm.h \
    Model/filemanagement.h \
    Model/graph.h \
    Model/node.h \
    View/edgegraphics.h \
    View/graphscene.h \
    View/graphtexteditor.h \
    View/graphtextline.h \
    View/graphviewer.h \
    View/nodegraphics.h \

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    graph_images.qrc

RC_ICONS = circle.ico

DISTFILES +=
