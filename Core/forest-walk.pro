TEMPLATE = app
CONFIG += c++11
TARGET = forest-walk
INCLUDEPATH += . /opt/local/include
QMAKE_CXXFLAGS += -Wno-write-strings

QT += widgets opengl gui core
LIBS += -lGLU 

HEADERS += maths.h string_functions.h mesh.h turtle.h arithmetic_tokeniser.h expression_evaluator.h char_queue.h l_system.h ForestWalkWindow.h ForestWidget.h multiset_l_system.h MLSystemWidget.h str_set.h tree_grid.h Ball.h BallMath.h BallAux.h timed.h tree_model_gen.h render_data.h

SOURCES += main.cpp maths.cpp string_functions.cpp mesh.cpp turtle.cpp arithmetic_tokeniser.cpp expression_evaluator.cpp char_queue.cpp l_system.cpp ForestWalkWindow.cpp ForestWidget.cpp multiset_l_system.cpp MLSystemWidget.cpp str_set.cpp tree_grid.cpp Ball.cpp BallMath.cpp BallAux.cpp timed.cpp tree_model_gen.cpp render_data.cpp
