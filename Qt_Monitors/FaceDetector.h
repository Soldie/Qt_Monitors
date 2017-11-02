#pragma once

#include <QObject>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QMutex>
#include <QVector>
#include <QRect>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>

#include <iostream>



class FaceDetector : public QThread
{
	Q_OBJECT
private:
	bool quit;
	bool stop;
	QMutex mutex;
	QWaitCondition condition;
	cv::Mat RGBframe;
	cv::Mat frame;
	dlib::frontal_face_detector dlib_detector;
	dlib::shape_predictor pose_model;
	QVector<cv::Mat> GetCropSmis(std::vector<dlib::rectangle> _faces, dlib::cv_image<dlib::bgr_pixel> _cmg, QVector<QRect>& _face_rects);
public slots:
	void GetFrameFromQImage(const QImage& qimg);
	void GetFrameFromCVImage(const cv::Mat& _cvImg);

signals:
	void FindFaceRect(const QRect& face_rect);
	void SendFaceRects(const QVector<QRect>& _rects);
	void SendFaceMats(const QVector<cv::Mat>& img_v);

protected:
	void run();
	void msleep(int ms);
public:
	FaceDetector(QObject *parent = 0);
	~FaceDetector();
	void LoadDetectModel();

	// Ω¯––»À¡≥ºÏ≤‚
	void Play();

	void Stop();

	bool isStopped() const;
};
