TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    arrmor.cpp \
    opencv_extended.cpp \
    imgproduceprocessing.cpp \
    arrmor_svm.cpp \
    serial.cpp \
    algorithm.cpp \
    camera.cpp

INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv \
               /usr/local/include/opencv2 \

INCLUDEPATH = ./include
LIBS += /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so \
        /usr/local/lib/libopencv_imgcodecs.so \
        /usr/local/lib/libopencv_video.so \
        /usr/local/lib/libopencv_videoio.so \
        /usr/local/lib/libopencv_calib3d.so \
        /usr/local/lib/libopencv_ml.so \
        /usr/lib/x86_64-linux-gnu/libpthread.so

HEADERS += \
    arrmor.h \
    opencv_extended.h \
    opencv_extended.h \
    imgproduceprocessing.h \
    arrmor_svm.h \
    serial.h \
    algorithm.h \
    camera.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/depends/release/ -lMVSDK
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/depends/debug/ -lMVSDK
else:unix: LIBS += -L$$PWD/depends/ -lMVSDK

INCLUDEPATH += $$PWD/depends
DEPENDPATH += $$PWD/depends

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/depends/release/ -lImageConvert
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/depends/debug/ -lImageConvert
else:unix: LIBS += -L$$PWD/depends/ -lImageConvert

INCLUDEPATH += $$PWD/depends
DEPENDPATH += $$PWD/depends
