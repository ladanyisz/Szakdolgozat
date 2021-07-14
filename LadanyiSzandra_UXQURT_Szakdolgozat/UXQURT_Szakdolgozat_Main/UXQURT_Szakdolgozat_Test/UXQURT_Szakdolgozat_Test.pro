QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../UXQURT_Szakdolgozat/
DEPENDPATH += $${INCLUDEPATH}

HEADERS +=  $$PWD/../UXQURT_Szakdolgozat/Model/adjnode.h \
            $$PWD/../UXQURT_Szakdolgozat/Model/algorithm.h \
            $$PWD/../UXQURT_Szakdolgozat/Model/graph.h \
            $$PWD/../UXQURT_Szakdolgozat/Model/node.h \
            $$PWD/../UXQURT_Szakdolgozat/Persistence/filemanagement.h \

SOURCES +=  tst_graphtest.cpp \
            $$PWD/../UXQURT_Szakdolgozat/Model/adjnode.cpp \
            $$PWD/../UXQURT_Szakdolgozat/Model/algorithm.cpp \
            $$PWD/../UXQURT_Szakdolgozat/Model/graph.cpp \
            $$PWD/../UXQURT_Szakdolgozat/Model/node.cpp \
            $$PWD/../UXQURT_Szakdolgozat/Persistence/filemanagement.cpp

