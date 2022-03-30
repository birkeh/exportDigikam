#include "cmainwindow.h"


#include <QApplication>
#include <QSettings>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationVersion(APP_VERSION);
	a.setApplicationDisplayName("exportDigikam");
	a.setOrganizationName("WIN-DESIGN");
	a.setOrganizationDomain("windesign.at");
	a.setApplicationName("exportDigikam");

	QSettings		settings;

	cMainWindow w;

	if(settings.value("main/maximized").toBool())
		w.showMaximized();
	else
		w.show();

	return a.exec();
}
