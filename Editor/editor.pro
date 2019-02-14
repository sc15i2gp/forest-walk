TEMPLATE = app
CONFIG += c++11
TARGET = l_system_editor
INCLUDEPATH += . /opt/local/include ../Core/

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += ../Core/maths.h ../Core/string_functions.h ../Core/mesh.h ../Core/turtle.h ../Core/arithmetic_tokeniser.h ../Core/expression_evaluator.h ../Core/char_queue.h ../Core/l_system.h LSystemWidget.h ../Core/RenderWidget.h LSystemEditorWindow.h

SOURCES += main.cpp ../Core/maths.cpp ../Core/string_functions.cpp ../Core/mesh.cpp ../Core/turtle.cpp ../Core/arithmetic_tokeniser.cpp ../Core/expression_evaluator.cpp ../Core/char_queue.cpp ../Core/l_system.cpp LSystemWidget.cpp ../Core/RenderWidget.cpp LSystemEditorWindow.cpp
