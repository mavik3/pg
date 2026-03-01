#include "ImageViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QLocale::setDefault(QLocale::c());

	QCoreApplication::setOrganizationName("MPM");//register kde mozem zapisovat cesti true false a take vyjde cash 
	QCoreApplication::setApplicationName("ImageViewer");

	QApplication a(argc, argv);
	ImageViewer w;
	w.show();
	return a.exec();
}