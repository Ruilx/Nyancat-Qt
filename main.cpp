#include "NyanDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NyanDialog w;
	if(QApplication::arguments().contains("window", Qt::CaseInsensitive)){
		w.show();
	}else{
		w.showFullScreen();
	}

	return a.exec();
}
