#include "FaceDetector.h"



// QImage transfer to Opencv cv::Mat
cv::Mat QImage2cvMat(QImage image)
{
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

void FaceDetector::run()
{
	while (!stop)
	{
		try
		{
			if (frame.empty())
			{
				continue;
			}
			mutex.lock();
			dlib::cv_image<dlib::bgr_pixel> cimg(frame);
			mutex.unlock();
			// 检测人脸
			std::vector<dlib::rectangle> faces = dlib_detector(cimg);
			
			// 多人脸检测
			QVector<QRect> face_rects;
			QVector<cv::Mat> face_mats = GetCropSmis(faces, cimg, face_rects);
			
			// 发送人脸框给当前的视频
			emit SendFaceRects(face_rects);
			emit SendFaceMats(face_mats);
			msleep(10);
		}
		catch (dlib::serialization_error& e)
		{
			//std::cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << std::endl;
			std::cout << std::endl << e.what() << std::endl;
			//SL_LOG1("## ERROR DLib ", e.what());
			continue;
		}
		catch (std::exception& e)
		{
			continue;
		}
	}
}

void FaceDetector::msleep(int ms)
{
	long ts = ms * 1000;
	usleep(ts);
}

FaceDetector::FaceDetector(QObject *parent)
	: QThread(parent), quit(false)
{
	stop = false;
}


FaceDetector::~FaceDetector()
{
	mutex.lock();
	stop = true;
	quit = true;
	condition.wakeOne();
	mutex.unlock();
	wait();
}

void FaceDetector::LoadDetectModel()
{
	// 初始化DLib检测模型
	dlib_detector = dlib::get_frontal_face_detector();
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
}

void FaceDetector::Play()
{
	if (!isRunning())
	{
		if (isStopped())
		{
			stop = false;
		}
		start(LowestPriority);
	}
}

void FaceDetector::Stop()
{
	QMutexLocker locker(&mutex);
	stop = true;
}

bool FaceDetector::isStopped() const
{
	return this->stop;
}


QVector<cv::Mat> FaceDetector::GetCropSmis(std::vector<dlib::rectangle>  _faces, dlib::cv_image<dlib::bgr_pixel> _cmg, QVector<QRect>& _face_rects)
{
	_face_rects.clear();
	QVector<cv::Mat> CropSimis;
	if (_faces.empty())
	{
		return CropSimis;
	}
	try
	{

		// 设定最大检测范围
		std::vector<dlib::rectangle>::const_iterator face_end;
		if (_faces.size() < 6)
		{
			// 设置迭代终点
			face_end = _faces.end();
		}
		else
		{
			face_end = (_faces.begin() + 5);
		}
		for (std::vector<dlib::rectangle>::const_iterator face_begin = _faces.begin(); face_begin < face_end; face_begin++)
		{
			// 寻找人脸中人脸特征点的位置
			std::vector<dlib::full_object_detection> shapes;
			shapes.push_back(pose_model(_cmg, *face_begin));

			// 设定ROI的区域位置
			cv::Rect face_roi;
			face_roi.x = (*face_begin).left();
			face_roi.y = (*face_begin).top();
			face_roi.width = (*face_begin).width();
			face_roi.height = (*face_begin).height();

			QRect face_rect(face_roi.x, face_roi.y, face_roi.width, face_roi.height);
			_face_rects.push_back(face_rect);

			// 计算两眼的位置
			cv::Point2f left_eye;
			left_eye.x = (shapes[0].part(36).x() + shapes[0].part(39).x()) / 2.0;
			left_eye.y = (shapes[0].part(36).y() + shapes[0].part(39).y()) / 2.0;

			cv::Point2f right_eye;
			right_eye.x = (shapes[0].part(42).x() + shapes[0].part(45).x()) / 2.0;
			right_eye.y = (shapes[0].part(42).y() + shapes[0].part(45).y()) / 2.0;

			cv::Point2d direction;
			direction.x = right_eye.x - left_eye.x;
			direction.y = right_eye.y - left_eye.y;

			cv::Point2d mid_eye;
			mid_eye.x = (left_eye.x + right_eye.x) / 2.0;
			mid_eye.y = (left_eye.y + right_eye.y) / 2.0;

			// 计算两眼之间的旋转角度,并完成旋转,
			double theta = atan2(direction.y, direction.x)*180.0 / CV_PI;
			float scale = 96.0 / face_roi.width;
			cv::Mat matRotation = getRotationMatrix2D(mid_eye, theta, scale);
			cv::Mat imgRotated;
			cv::warpAffine(frame, imgRotated, matRotation, frame.size());
			cv::Mat roi_img;
			cv::Rect crop_roi;

			//设置保护，防止出现某处为负的情况。
			crop_roi.width = 96.0;
			crop_roi.height = face_roi.height * scale;
			crop_roi.x = mid_eye.x - crop_roi.width / 2;			// 左上原点x位置
			if (crop_roi.x < 0)
			{
				crop_roi.x = 0;
			}
			crop_roi.y = mid_eye.y - (crop_roi.height * 0.3);		// 左上原点y位置
			if (crop_roi.y < 0)
			{
				crop_roi.y = 0;
			}
			if ((crop_roi.height + crop_roi.y) > imgRotated.rows)
			{
				crop_roi.height = imgRotated.rows - crop_roi.y;
			}
			if ((crop_roi.width + crop_roi.x) > imgRotated.cols)
			{
				crop_roi.width = imgRotated.cols - crop_roi.x;
			}
			roi_img = imgRotated(crop_roi);
			cv::resize(roi_img, roi_img, cv::Size(96, 112));
			// 压入std::vecotr<Mat>向量中
			CropSimis.push_back(roi_img);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "GetCropSmiFace存在异常" << e.what() << std::endl;
	}
	return CropSimis;
}


// SLOT
void FaceDetector::GetFrameFromQImage(const QImage& qimg)
{
	try
	{
		mutex.lock();
		frame = QImage2cvMat(qimg);
		mutex.unlock();
	}
	catch (const std::exception&)
	{

	}
}

void FaceDetector::GetFrameFromCVImage(const cv::Mat& _cvImg)
{
	try
	{
		mutex.lock();
		frame = _cvImg.clone();
		mutex.unlock();
	}
	catch (const std::exception&)
	{

	}
}