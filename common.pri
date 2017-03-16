CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
	LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

LIBS_PATH = $${_PRO_FILE_PWD_}/libs/
BIN_PATH = $${_PRO_FILE_PWD_}/bin/$${BUILD_FLAG}/
BUILD_PATH = $${_PRO_FILE_PWD_}/build/$${BUILD_FLAG}/$${TARGET}/

RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/

INCLUDEPATH += $${_PRO_FILE_PWD_}/src/include/
LIBS += -L$${LIBS_PATH}/

QMAKE_CXXFLAGS += -O2

DEFINES *= \
    QT_USE_QSTRINGBUILDER \
    #FONTA_DETAILED_DEBUG \
    FONTA_MEASURES
