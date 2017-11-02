#include "Qt_Monitors.h"
#include <QtWidgets/QApplication>
#include <QMetaType>	//×¢²áµÄÀàÐÍ
#include <QVector>
#include <QRect>

Q_DECLARE_METATYPE(cv::Mat);
Q_DECLARE_METATYPE(QVector<cv::Mat>);
Q_DECLARE_METATYPE(QVector<QRect>);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<QVector<cv::Mat>>("QVector<cv::Mat>");
	qRegisterMetaType<QVector<QRect>>("QVector<QRect>");
	Qt_Monitors w;
	w.show();
	return a.exec();
}
