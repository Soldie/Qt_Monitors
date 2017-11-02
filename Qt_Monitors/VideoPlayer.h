#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#pragma once

#include <QObject>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QMutex>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class VideoPlayer : public QThread
{
	Q_OBJECT
private:
	bool quit;
	bool stop;
	QMutex mutex;
	QWaitCondition condition;
	cv::Mat frame;
	int frameRate;
	cv::VideoCapture capture;
	cv::Mat RGBframe;
	QImage img;
	int camera_index = 0;


signals:
	void processedImage(const QImage& image);
	void sendImage(const cv::Mat& image);

protected:
	void run();
	void msleep(int ms);

public:
	VideoPlayer(QObject *parent = 0);
	~VideoPlayer();

	// capture video from camera
	bool CaptrueVideo();

	// play video
	void Play();

	void Stop();

	bool isStopped() const;

	void SetCameraIndex(int _index);

};

#endif // !VIDEOPLAYER_H

