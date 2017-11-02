#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Qt_Monitors.h"
#include "qMyMonitor.h"
//#include <TensorSdk.h>

class Qt_Monitors : public QMainWindow
{
	Q_OBJECT

public:
	Qt_Monitors(QWidget *parent = Q_NULLPTR);


private:
	Ui::Qt_MonitorsClass ui;

	// ������һ��WQ
	void CreateMonitorWidget1();
	// �����ڶ���WQ
	void CreateMonitorWidget2();


	// �ֲ��������1
	QGroupBox* MonitorGroupBox1;
	QGridLayout * monitor_layout1;
	qMyMonitor* MyMonitorWidget1;
	// �ֲ��������2
	QGroupBox* MonitorGroupBox2;
	QGridLayout * monitor_layout2;
	qMyMonitor* MyMonitorWidget2;
	// ������ʾ������
	QGroupBox* ScrollGroupBox;
	QScrollArea* scroll_photos;
	QVBoxLayout* scroll_Vlayout;
	QGridLayout * scroll_layout;
	QWidget* scroll_VWidget;
private slots:
	void on_push_btn_clicked();
};
