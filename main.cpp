#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	QString locale = QLocale::system().name();
	translator.load(QString("Gobang_") + locale);
	a.installTranslator(&translator);

	MainWindow w;
	w.show();
	return a.exec();
}
