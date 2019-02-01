#include <stdio.h>
#include <QApplication>
#include "LSystemEditorWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	AppWindow* window = new AppWindow(NULL);
	window->resize(512, 512);
	window->show();
	window->init_GLwidget();
	app.exec();
	delete window;
	return 0;
}
