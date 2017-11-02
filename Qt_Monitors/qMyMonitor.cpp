#include "qMyMonitor.h"
#include <QLabel>
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QBrush>
#include <QScrollBar>
#include <QTime>
#include <QFuture>
#include <QtConcurrentRun>
#include <QProcess>

extern QImage mat2qimage_ref(cv::Mat &m, QImage::Format format);

qMyMonitor::qMyMonitor(QWidget *parent)
	: QWidget(parent)
{
	CreateMonitorLabel();
	CreateScrollArea();
	CreateControlGroup();
	CreateStatusGroup();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(MonitorGroupBox, 0, 0);
	layout->addWidget(ScrollGroupBox, 0, 1);
	layout->addWidget(ControlGroupBox, 1, 1);
	layout->addWidget(StatusGroupBox, 1, 0);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(layout);
	setWindowTitle(tr("Monitor Face	"));

	mPlayer = new VideoPlayer(this);
	connect(mPlayer, SIGNAL(processedImage(const QImage)), this, SLOT(updatePlayerUI(const QImage)), Qt::QueuedConnection);	//进行实时显示

	// 创建Detector
	mDetector = new FaceDetector(this);
	QFuture<void> future = QtConcurrent::run(this->mDetector, &FaceDetector::LoadDetectModel);
	bool ret = connect(mPlayer, SIGNAL(sendImage(const cv::Mat)), mDetector, SLOT(GetFrameFromCVImage(const cv::Mat)), Qt::QueuedConnection);	//进行图像中人脸检测
	ret = connect(mDetector, SIGNAL(SendFaceRects(const QVector<QRect>)), this, SLOT(get_face_rects(const QVector<QRect>)), Qt::UniqueConnection);	//完成人脸检测，给出人脸矩形
	ret = connect(mDetector, SIGNAL(SendFaceMats(const QVector<cv::Mat>)), this, SLOT(recog_show_faces(const QVector<cv::Mat>)), Qt::UniqueConnection);	//完成人脸检测，给出人脸矩形
	
	mPlayer_Open = FALSE;
	future.waitForFinished();
}

void qMyMonitor::CreateMonitorLabel()
{
	MonitorGroupBox = new QGroupBox(tr("Preview"));

	// 自己添加的显示控件 QLabel
	monitor_show_label = new QLabel();

	monitor_show_label->setMinimumSize(640, 480);
	monitor_show_label->setVisible(true);
	
	monitor_layout = new QGridLayout;
	
	monitor_layout->addWidget(monitor_show_label, 0, 0, Qt::AlignCenter);

	MonitorGroupBox->setLayout(monitor_layout);
}

void qMyMonitor::CreateScrollArea()
{
	ScrollGroupBox = new QGroupBox(tr("Scroll"));
	scroll_photos = new QScrollArea(this);
	scroll_photos->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	scroll_photos->setWidgetResizable(true);
	
	scroll_VWidget = new QWidget(this);
	/*scroll_VWidget->setMinimumSize(120, 750);
	scroll_VWidget->setMaximumSize(120, 750);*/
	
	// 设定Layout装在widget中的排列内容
	this->scroll_Vlayout = new QVBoxLayout(scroll_VWidget);

	// 将Layout的布局内容设置给widget
	scroll_VWidget->setLayout(this->scroll_Vlayout);

	
	this->scroll_photos->setWidget(scroll_VWidget);	// 将widget中的内容付给ScrollArea
	this->scroll_photos->setMinimumSize(120, 480);	//设置scrollArea的区域大小
	scroll_layout = new QGridLayout;	//scrollArea的布局方式
	
	scroll_layout->addWidget(scroll_photos, 0, 0, Qt::AlignTop);	//scrollArea以从上到下的方式布局到内部

	ScrollGroupBox->setLayout(scroll_layout);	//将整个布局，放入Scroll_groupbox中
}


void qMyMonitor::CreateControlGroup()
{
	ControlGroupBox = new QGroupBox(tr("Control"));
	
	this->Start_btn = new QPushButton(QString("Start"),this);

	QObject::connect(Start_btn, SIGNAL(clicked()), this, SLOT(Start_btn_clicked()));
	this->OpenDoor_btn = new QPushButton(QString("Open"),this);
	QObject::connect(OpenDoor_btn, SIGNAL(clicked()), this, SLOT(OpenDoor_btn_clicked()));
	this->control_layout = new QGridLayout;
	this->control_layout->addWidget(Start_btn, 0, 0, Qt::AlignCenter);
	this->control_layout->addWidget(OpenDoor_btn, 0, 1, Qt::AlignCenter);
	
	ControlGroupBox->setLayout(control_layout);
}
void qMyMonitor::CreateStatusGroup()
{
	StatusGroupBox = new QGroupBox(tr("Status"));
	this->RecogSpeed_lbl = new QLabel();
	this->RecogSpeed_lbl->setText("Recog Speed: ");

	this->RecogSpeed_line = new QLineEdit();

	this->DetectSpeed_lbl = new QLabel();
	this->DetectSpeed_lbl->setText("Detect Speed: ");
	this->DetectSpeed_line = new QLineEdit();

	this->Human_num_lbl = new QLabel();
	this->Human_num_lbl->setText("Recog Number: ");

	this->Human_num_line = new QLineEdit();

	this->Camera_lbl = new QLabel();
	this->Camera_lbl->setText("Open the Camera: ");
	this->Camera_num_line = new QLineEdit();

	this->Status_layout = new QGridLayout;

	this->Status_layout->addWidget(RecogSpeed_lbl, 0, 0, Qt::AlignCenter);
	this->Status_layout->addWidget(RecogSpeed_line, 0, 1, Qt::AlignCenter);
	this->Status_layout->addWidget(DetectSpeed_lbl, 1, 0, Qt::AlignCenter);
	this->Status_layout->addWidget(DetectSpeed_line, 1, 1, Qt::AlignCenter);
	this->Status_layout->addWidget(Human_num_lbl, 2, 0, Qt::AlignCenter);
	this->Status_layout->addWidget(Human_num_line, 2, 1, Qt::AlignCenter);
	this->Status_layout->addWidget(Camera_lbl, 3, 0, Qt::AlignCenter);
	this->Status_layout->addWidget(Camera_num_line, 3, 1, Qt::AlignCenter);

	StatusGroupBox->setLayout(Status_layout);
}

// 加载检测模型，并开启检测流程
void qMyMonitor::LoadDetectorModel()
{
	mDetector->LoadDetectModel();
}

void qMyMonitor::Start_btn_clicked()
{
	QMutexLocker locker(&mutex);
	std::cout << "点击了开始按钮" << std::endl;
	if (this->Camera_num_line->text().isEmpty())
	{
		QMessageBox::information(NULL, tr("OK"), tr("Please input the Camera Number for open camera"));
		return;
	}

	// 开启摄像头捕获
	if (!mPlayer_Open)
	{
		int camera_index = this->Camera_num_line->text().toInt();
		mPlayer->SetCameraIndex(camera_index);
		bool ret = mPlayer->CaptrueVideo();
		if (!ret)
		{
			std::cout << "开启摄像头失败" << std::endl;
		}
		else
		{
			mPlayer_Open = TRUE;
		}
	}
	
	// 开始获取图像
	if (mPlayer->isStopped())
	{
		mPlayer->Play();
		mDetector->Play();
		this->Start_btn->setText(tr("Stop"));
	}
	else
	{
		mPlayer->Stop();
		mDetector->Stop();
		this->Start_btn->setText(tr("Start"));
	}

}


void qMyMonitor::OpenDoor_btn_clicked()
{
	std::cout << "点击了开门按钮" << std::endl;
}


void qMyMonitor::updatePlayerUI(const QImage& qimg)
{
	QMutexLocker locker(&mutex);
	if (!qimg.isNull())
	{
		monitor_show_label->setAlignment(Qt::AlignCenter);
		monitor_show_label->setPixmap(QPixmap::fromImage(qimg));
		if (roi_rects.empty())
		{
			monitor_show_label->setPixmap(QPixmap::fromImage(qimg));
		}
		else
		{
			QImage tmp(monitor_show_label->pixmap()->toImage());
			QPainter painter(&tmp);
			QPen paintpen(Qt::red);
			paintpen.setWidth(2);
			painter.setPen(paintpen);
			for (QVector<QRect>::const_iterator roi_face = roi_rects.begin(); roi_face < roi_rects.end(); ++roi_face)
			{
				painter.drawRect(*roi_face);
			}
			monitor_show_label->setPixmap(QPixmap::fromImage(tmp));
		}

	}
}

void qMyMonitor::get_face_rects(const QVector<QRect>& _rects)
{
	QMutexLocker locker(&mutex);
	roi_rects = _rects;
}

void qMyMonitor::recog_show_faces(const QVector<cv::Mat>& _faces_v)
{
	//QMutexLocker locker(&mutex);
	// 如果栏目图像超过500则重新刷新
	if (this->scroll_Vlayout->count())
	{
		std::cout << "scroll_Vlayout count is" << this->scroll_Vlayout->count() << std::endl;

		if (this->scroll_Vlayout->count() > 200)
		{
			QList<QLabel*> lbls = this->scroll_VWidget->findChildren<QLabel*>();
			std::cout << "for each 删除" << std::endl;
			foreach(QLabel* lbl, lbls) { delete lbl; }
			std::cout << "删除了控件" << std::endl;
		}
	}
	

	// 在旁边栏目显示5个图像
	if (this->scroll_VWidget != NULL)
	{
		// 纵向排列图像
		int i = 0;
		QString str("roi_lbl %1");
		QLabel* roi_label;

		for (QVector<cv::Mat>::const_iterator iter = _faces_v.begin(); iter<_faces_v.end(); ++iter, ++i)
		{
			std::cout << "添加人脸" << str.toStdString() << std::endl;
			cv::Mat img = (*iter).clone();
			cv::cvtColor(img, img, CV_BGR2RGB);
			QImage qimg = mat2qimage_ref(img, QImage::Format::Format_RGB888);
			roi_label = new QLabel(str.arg(i + 1), this->scroll_VWidget);
			roi_label->setAlignment(Qt::AlignCenter);
			roi_label->setPixmap(QPixmap::fromImage(qimg));
			roi_label->setMinimumSize(96,112);
			this->scroll_Vlayout->addWidget(roi_label,1, Qt::AlignTop);
		}
		// 将Layout的布局内容设置给widget
		this->scroll_VWidget->setLayout(this->scroll_Vlayout);
		QScrollBar* vertical_srcoll;
		this->scroll_photos->verticalScrollBar()->setValue(this->scroll_photos->verticalScrollBar()->maximum());
		
	}
}



qMyMonitor::~qMyMonitor()
{
	mPlayer->Stop();
	mDetector->Stop();
	
}
