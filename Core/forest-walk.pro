TEMPLATE = app
CONFIG += c++11
TARGET = forest-walk
INCLUDEPATH += . /opt/local/include

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += maths.h string_functions.h mesh.h turtle.h arithmetic_tokeniser.h expression_evaluator.h char_queue.h l_system.h ForestWalkWindow.h ChartWidget.h multiset_l_system.h MLSystemWidget.h

SOURCES += main.cpp maths.cpp string_functions.cpp mesh.cpp turtle.cpp arithmetic_tokeniser.cpp expression_evaluator.cpp char_queue.cpp l_system.cpp ForestWalkWindow.cpp ChartWidget.cpp multiset_l_system.cpp MLSystemWidget.cpp
