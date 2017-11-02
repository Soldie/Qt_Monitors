#pragma once
#include "VideoPlayer.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp> 


QImage mat2qimage_cpy(cv::Mat &m, QImage::Format format) {
	return QImage(m.data, m.cols, m.rows, m.step, format).copy();
}

QImage mat2qimage_ref(cv::Mat &m, QImage::Format format) {
	return QImage(m.data, m.cols, m.rows, m.step, format);
}


VideoPlayer::VideoPlayer(QObject *parent)
	: QThread(parent), quit(false)
{
	stop = true;

}

VideoPlayer::~VideoPlayer()
{
	mutex.lock();
	stop = true;
	capture.release();
	quit = true;
	condition.wakeOne();
	mutex.unlock();
	//std::cout << "VideoPlayerÊÍ·Å" << std::endl;
	wait();
}

bool VideoPlayer::CaptrueVideo()
{
	capture.open(camera_index);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	if (capture.isOpened())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void VideoPlayer::Play()
{
	if (!isRunning())
	{
		if (isStopped())
		{
			stop = false;
		}
		start(LowPriority);
	}
}

void VideoPlayer::run()
{
	int delay = 15;
	while (!stop) {
		if (!capture.read(frame))
		{
			continue;
		}
		if (frame.channels() == 3) {
			cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
			/*img = QImage((const unsigned char*)(RGBframe.data),
				RGBframe.cols, RGBframe.rows, QImage::Format_RGB888);*/
			img = mat2qimage_ref(RGBframe, QImage::Format_RGB888);
		}
		else
		{
			img = QImage((const unsigned char*)(frame.data),
				frame.cols, frame.rows, QImage::Format_Indexed8);
		}
		emit processedImage(img);
		emit sendImage(frame);
		this->msleep(delay);
	}
}

void VideoPlayer::Stop()
{
	stop = true;
}

void VideoPlayer::msleep(int ms)
{
	long ts = ms * 1000;
	usleep(ts);
}

bool VideoPlayer::isStopped() const
{
	return this->stop;
}

void VideoPlayer::SetCameraIndex(int _index)
{
	camera_index = _index;
}

