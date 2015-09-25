TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../include \
    $$(SRC)/contrib/include

LIBS += -L$$(SRC)/contrib/boost/lib64-msvc-12.0

BUILD_DIR = build

SOURCES += main.cc

HEADERS += \
    ../include/kxhuffman/huffman.h \
    ../include/kxhuffman/encode.h \
    ../include/kxhuffman/decode.h \
    ../include/kxhuffman/HuffmanTree.h \
    ../include/kxhuffman/HuffmanNode.h \
    ../include/kxhuffman/Bitseq.h \
    ../include/kxhuffman/common.h
