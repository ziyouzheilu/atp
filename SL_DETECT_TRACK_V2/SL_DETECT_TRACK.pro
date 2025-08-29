#-------------------------------------------------
#
# Project created by QtCreator 2022-12-29T17:44:28
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
#CONFIG += console
CONFIG -= app_bundle

TARGET = SL_DETECT_TRACK
TEMPLATE = app
QT += serialport

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH  += /usr/local/include
LIBS += /usr/local/lib/libavcodec.so      \
        /usr/local/lib/libavdevice.so     \
        /usr/local/lib/libavfilter.so     \
        /usr/local/lib/libavformat.so     \
        /usr/local/lib/libavutil.so       \
        /usr/local/lib/libswresample.so   \
        /usr/local/lib/libpostproc.so   \
        /usr/local/lib/libswscale.so


SOURCES += \
    detecttrackmthread.cpp \
    fdssttracker.cpp \
    ffmpegmthread.cpp \
    fhog.cpp \
    main.cpp \
    mainwindow.cpp \
    PacketFramePool.cpp \
    pciemthread.cpp \
    trackmthread.cpp \
    crc16.c \
    data.c \
    pcie_func.c

HEADERS += \
    crc16.h \
    data.h \
    detector.h \
    detecttrackmthread.h \
    fdssttracker.hpp \
    ffmpegmthread.h \
    ffttools.hpp \
    fhog.h \
    labdata.hpp \
    mainwindow.h \
    PacketFramePool.h \
    pcie_func.h \
    pciemthread.h \
    recttools.hpp \
    sse.hpp \
    SSE2NEON.h \
    sse2neon.h \
    tracker.h \
    trackmthread.h

FORMS += \
        mainwindow.ui


#unix:!macx: LIBS += -L$$PWD/../../../usr/lib/aarch64-linux-gnu/ -lJetsonGPIO

INCLUDEPATH += /usr/lib/aarch64-linux-gnu
DEPENDPATH += /usr/lib/aarch64-linux-gnu



INCLUDEPATH +=/usr/local/include \
              /usr/local/include/opencv4 \
             /usr/local/include/opencv4/opencv2
#LIBS +=/usr/local/lib/libopencv_highgui.so \
#      /usr/local/lib/libopencv_core.so \
#      /usr/local/lib/libopencv_imgproc.so \
#      /usr/local/lib/libopencv_imgcodecs.so \
#      /usr/local/lib/libopencv_*.so
LIBS +=/usr/local/lib/libopencv_*.so
INCLUDEPATH +=/home/nvidia/darknet_hdmi_to_usb/include
LIBS += /home/nvidia/darknet_hdmi_to_usb/libdarknet.so

#DISTFILES += \
#    runtracker.cpp.bak

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix: CONFIG += link_pkgconfig
#unix: PKGCONFIG += /usr/local/lib/pkgconfig/opencv.pc
#CUDA
DEFINES += GPU CUDNN
LIBS += \
    -L"/usr/local/lib" \
    -L"/usr/local/cuda/lib64" \
    -lcudart -lcufft
DEPENDPATH += .
INCLUDEPATH += /usr/local/cuda/include
QMAKE_LIBDIR += /usr/local/cuda/lib64

CUDA_DIR = /usr/local/cuda
CUDA_SDK = /usr/local/cuda
SYSTEM_NAME = linux
SYSTEM_TYPE = 64
CUDA_ARCH = compute_50
CUDA_CODE = [sm_50,compute_50]

CUDA_OBJECTS_DIR = ./Release/obj
CUDA_LIBS = cudart cufft
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

NVCC_OPTIONS = --use_fast_math
NVCC_LIBS = $$join(CUDA_LIBS,' -l','-l','')

CONFIG(Release){
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
    cuda.commands = $$CUDA_DIR/bin/nvcc $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -O3 -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -ldarknet
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -ldarknet
#else:unix: LIBS += -L$$PWD/./ -ldarknet

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
