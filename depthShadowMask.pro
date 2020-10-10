# QVector3D and QMatrix classes require gui modules
QT += gui

# Create command line program
CONFIG += c++11 console

# For macOS create command line program without application bundle
CONFIG -= app_bundle

TARGET = depthShadowMask

HEADERS += Array2D.h \
           Box.h \
           Camera.h \
           Cube.h \
           KRtCamera.h \
           OptionParser.h \
           PLYData.h \
           Ray.h \
           rply.h \
           StreamUtilities.h \
           TextProgress.h \
           VoxelPixelArea.h

SOURCES += Box.cpp \
           Camera.cpp \
           Cube.cpp \
           depthShadowMask.cpp \
           KRtCamera.cpp \
           OptionParser.cpp \
           PLYData.cpp \
           rply.c \
           StreamUtilities.cpp \
           TextProgress.cpp \
           VoxelPixelArea.cpp
