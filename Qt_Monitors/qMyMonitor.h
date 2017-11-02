#pragma once
#ifndef MY_MONITOR_H
#define MY_MONITOR_H
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QLineedit>
#include <QPushButton>
#include "VideoPlayer.h"
#include "FaceDetector.h"
#include <QImage>
#include <qmutex.h>
#include <QRect>
#include <QVector>

class qMyMonitor : public QWidget
{
	Q_OBJECT

public:
	qMyMonitor(QWidget *parent = 0);
	~qMyMonitor();
private:
	void CreateMonitorLabel();
	void CreateScrollArea();
	void CreateControlGroup();
	void CreateStatusGroup();

	// 监控显示窗区域
	QGroupBox* MonitorGroupBox;
	QLabel* monitor_show_label;
	QGridLayout * monitor_layout;

	// 滑动显示窗区域
	QGroupBox* ScrollGroupBox;
	QScrollArea* scroll_photos;
	QVBoxLayout* scroll_Vlayout;
	QGridLayout * scroll_layout;
	QWidget* scroll_VWidget;

	// 控制Box
	QGroupBox* ControlGroupBox;
	QGridLayout * control_layout;
	QPushButton* Start_btn;
	QPushButton* OpenDoor_btn;
	

	// 状态Box
	QGroupBox* StatusGroupBox;
	QGridLayout * Status_layout;
	QLabel* RecogSpeed_lbl;
	QLineEdit* RecogSpeed_line;
	QLabel* DetectSpeed_lbl;
	QLineEdit*  DetectSpeed_line;
	QLabel* Human_num_lbl;
	QLineEdit* Human_num_line;
	QLabel* Camera_lbl;
	QLineEdit* Camera_num_line;

	// 显示视频获取
	VideoPlayer* mPlayer;
	BOOL mPlayer_Open;

	// 检测人脸
	FaceDetector* mDetector;
	void LoadDetectorModel();
	QRect face_rect;
	QVector<QRect> roi_rects;
	//
	QMutex mutex;

protected:
	

private slots:
	void Start_btn_clicked();
	void OpenDoor_btn_clicked();
	void updatePlayerUI(const QImage& img);
	void get_face_rects(const QVector<QRect>& _rects);
	void recog_show_faces(const QVector<cv::Mat>& _faces_v);
};

#endif //MY_MONITOR_H