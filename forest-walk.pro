TEMPLATE = app
TARGET = forest-walk
INCLUDEPATH += . /opt/local/include

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += maths.h string_functions.h mesh.h turtle.h arithmetic_tokeniser.h expression_evaluator.h char_queue.h l_system.h LSystemWidget.h RenderWidget.h LSystemEditorWindow.h

SOURCES += main.cpp maths.cpp string_functions.cpp mesh.cpp turtle.cpp arithmetic_tokeniser.cpp expression_evaluator.cpp char_queue.cpp l_system.cpp LSystemWidget.cpp RenderWidget.cpp LSystemEditorWindow.cpp
