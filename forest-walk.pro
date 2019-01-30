TEMPLATE = app
TARGET = forest-walk
INCLUDEPATH += . /opt/local/include

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += maths.h
SOURCES += main.cpp maths.cpp
