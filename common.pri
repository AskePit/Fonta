CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

LANGUAGE = C++
CONFIG *= c++11 precompile_header

LIBS_PATH = $$PWD/libs/
BIN_PATH = $$PWD/bin/$${BUILD_FLAG}/
BUILD_PATH = $$PWD/build/$${BUILD_FLAG}/$${TARGET}/
INCLUDE_PATH *= $$PWD/src/include/

RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/

INCLUDEPATH *= $${INCLUDE_PATH}/
LIBS *= -L$${LIBS_PATH}/

PRECOMPILED_HEADER = $${INCLUDE_PATH}/precompiled_headers.h

QMAKE_CXXFLAGS *= -O2

DEFINES *= \
    QT_USE_QSTRINGBUILDER \
    #FONTA_DETAILED_DEBUG \
    FONTA_MEASURES
