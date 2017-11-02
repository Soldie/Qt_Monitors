#include "Qt_Monitors.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core.hpp>
#include "qMyMonitor.h"



Qt_Monitors::Qt_Monitors(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	CreateMonitorWidget1();
	CreateMonitorWidget2();

	QWidget* new_widget = new QWidget();
	this->setCentralWidget(new_widget);
	QGridLayout* my_layout = new QGridLayout();
	my_layout->addWidget(MonitorGroupBox1, 0, 0);
	my_layout->addWidget(MonitorGroupBox2, 0, 1);
	new_widget->setLayout(my_layout);
	setWindowTitle(tr("Monitor SoftWare"));
}

void Qt_Monitors::on_push_btn_clicked()
{
	
}

void Qt_Monitors::CreateMonitorWidget1()
{
	MonitorGroupBox1 = new QGroupBox(tr("Monitor1"));

	MyMonitorWidget1 = new qMyMonitor;
	MyMonitorWidget1->setVisible(true);

	monitor_layout1 = new QGridLayout;

	monitor_layout1->addWidget(MyMonitorWidget1, 0, 0, Qt::AlignCenter);

	MonitorGroupBox1->setLayout(monitor_layout1);
}

void Qt_Monitors::CreateMonitorWidget2()
{
	MonitorGroupBox2 = new QGroupBox(tr("Monitor1"));

	MyMonitorWidget2 = new qMyMonitor;
	MonitorGroupBox2->setVisible(true);

	monitor_layout2 = new QGridLayout;

	monitor_layout2->addWidget(MyMonitorWidget2, 0, 0, Qt::AlignCenter);

	MonitorGroupBox2->setLayout(monitor_layout2);
}
