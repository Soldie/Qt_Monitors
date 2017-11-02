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
	connect(mPlayer, SIGNAL(processedImage(const QImage)), this, SLOT(updatePlayerUI(const QImage)), Qt::QueuedConnection);	//����ʵʱ��ʾ

	// ����Detector
	mDetector = new FaceDetector(this);
	QFuture<void> future = QtConcurrent::run(this->mDetector, &FaceDetector::LoadDetectModel);
	bool ret = connect(mPlayer, SIGNAL(sendImage(const cv::Mat)), mDetector, SLOT(GetFrameFromCVImage(const cv::Mat)), Qt::QueuedConnection);	//����ͼ�����������
	ret = connect(mDetector, SIGNAL(SendFaceRects(const QVector<QRect>)), this, SLOT(get_face_rects(const QVector<QRect>)), Qt::UniqueConnection);	//���������⣬������������
	ret = connect(mDetector, SIGNAL(SendFaceMats(const QVector<cv::Mat>)), this, SLOT(recog_show_faces(const QVector<cv::Mat>)), Qt::UniqueConnection);	//���������⣬������������
	
	mPlayer_Open = FALSE;
	future.waitForFinished();
}

void qMyMonitor::CreateMonitorLabel()
{
	MonitorGroupBox = new QGroupBox(tr("Preview"));

	// �Լ���ӵ���ʾ�ؼ� QLabel
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
	
	// �趨Layoutװ��widget�е���������
	this->scroll_Vlayout = new QVBoxLayout(scroll_VWidget);

	// ��Layout�Ĳ����������ø�widget
	scroll_VWidget->setLayout(this->scroll_Vlayout);

	
	this->scroll_photos->setWidget(scroll_VWidget);	// ��widget�е����ݸ���ScrollArea
	this->scroll_photos->setMinimumSize(120, 480);	//����scrollArea�������С
	scroll_layout = new QGridLayout;	//scrollArea�Ĳ��ַ�ʽ
	
	scroll_layout->addWidget(scroll_photos, 0, 0, Qt::AlignTop);	//scrollArea�Դ��ϵ��µķ�ʽ���ֵ��ڲ�

	ScrollGroupBox->setLayout(scroll_layout);	//���������֣�����Scroll_groupbox��
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

// ���ؼ��ģ�ͣ��������������
void qMyMonitor::LoadDetectorModel()
{
	mDetector->LoadDetectModel();
}

void qMyMonitor::Start_btn_clicked()
{
	QMutexLocker locker(&mutex);
	std::cout << "����˿�ʼ��ť" << std::endl;
	if (this->Camera_num_line->text().isEmpty())
	{
		QMessageBox::information(NULL, tr("OK"), tr("Please input the Camera Number for open camera"));
		return;
	}

	// ��������ͷ����
	if (!mPlayer_Open)
	{
		int camera_index = this->Camera_num_line->text().toInt();
		mPlayer->SetCameraIndex(camera_index);
		bool ret = mPlayer->CaptrueVideo();
		if (!ret)
		{
			std::cout << "��������ͷʧ��" << std::endl;
		}
		else
		{
			mPlayer_Open = TRUE;
		}
	}
	
	// ��ʼ��ȡͼ��
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
	std::cout << "����˿��Ű�ť" << std::endl;
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
	// �����Ŀͼ�񳬹�500������ˢ��
	if (this->scroll_Vlayout->count())
	{
		std::cout << "scroll_Vlayout count is" << this->scroll_Vlayout->count() << std::endl;

		if (this->scroll_Vlayout->count() > 200)
		{
			QList<QLabel*> lbls = this->scroll_VWidget->findChildren<QLabel*>();
			std::cout << "for each ɾ��" << std::endl;
			foreach(QLabel* lbl, lbls) { delete lbl; }
			std::cout << "ɾ���˿ؼ�" << std::endl;
		}
	}
	

	// ���Ա���Ŀ��ʾ5��ͼ��
	if (this->scroll_VWidget != NULL)
	{
		// ��������ͼ��
		int i = 0;
		QString str("roi_lbl %1");
		QLabel* roi_label;

		for (QVector<cv::Mat>::const_iterator iter = _faces_v.begin(); iter<_faces_v.end(); ++iter, ++i)
		{
			std::cout << "�������" << str.toStdString() << std::endl;
			cv::Mat img = (*iter).clone();
			cv::cvtColor(img, img, CV_BGR2RGB);
			QImage qimg = mat2qimage_ref(img, QImage::Format::Format_RGB888);
			roi_label = new QLabel(str.arg(i + 1), this->scroll_VWidget);
			roi_label->setAlignment(Qt::AlignCenter);
			roi_label->setPixmap(QPixmap::fromImage(qimg));
			roi_label->setMinimumSize(96,112);
			this->scroll_Vlayout->addWidget(roi_label,1, Qt::AlignTop);
		}
		// ��Layout�Ĳ����������ø�widget
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
