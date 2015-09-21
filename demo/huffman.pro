TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += console

Debug:DESTDIR=build
Release:DESTDIR=build

# Define this to see the algorithm in action
#QMAKE_CXXFLAGS += -DALGORITHM_OUTPUT

QMAKE_CXXFLAGS_DEBUG += -DKX_DEBUG
unix: {
    QMAKE_CXXFLAGS += --std=c++0x
    QMAKE_CXXFLAGS_DEBUG += -g
}
win32: {
    QMAKE_CXXFLAGS += -DNOMINMAX
    # generate debug symbols for profiling
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /DEBUG
}

INCLUDEPATH += ../include
DEPENDPATH  += ../include

SOURCES += \
    main.cc

HEADERS += \
    ../include/kxhuffman/huffman.h \
    ../include/kxhuffman/encode.h \
    ../include/kxhuffman/decode.h \
    ../include/kxhuffman/HuffmanTree.h \
    ../include/kxhuffman/HuffmanNode.h \
    ../include/kxhuffman/common.h
