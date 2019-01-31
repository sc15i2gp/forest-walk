TEMPLATE = app
TARGET = forest-walk
INCLUDEPATH += . /opt/local/include

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += maths.h string_functions.h
SOURCES += main.cpp maths.cpp string_functions.cpp
