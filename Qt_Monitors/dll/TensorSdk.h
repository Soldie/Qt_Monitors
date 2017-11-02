// File : TensorSdk.h
// Author : LC
// Date : 2017/10/27
// Narrative : Declare class tensor sdk.


#ifndef TENSORSDK_H
#define TENSORSDK_H

#if defined( _WINDLL) 
#define TENSORSDK extern "C" __declspec(dllexport)
#else
#define TENSORSDK extern "C" __declspec(dllimport)
#endif


#include <vector>
using std::vector;
#include <string>
using std::string;

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core.hpp>


//TENSORSDK void __stdcall GetFeaMat(cv::Mat image, const int iWidth, const int iHeight, string &strFeature)
//TENSORSDK bool __stdcall ExtractFeature(const unsigned char* bufBgr24, const int iWidth, const int iHeight)							//此处的roi_frame是一个灰度图像


TENSORSDK bool __stdcall ExtractFeature(const unsigned char* bufBgr24, const int iWidth, const int iHeight);							//此处的roi_frame是一个灰度图像
TENSORSDK void __stdcall SetModelFile(const string modelPath);
TENSORSDK bool __stdcall LoadGraphModel();

TENSORSDK cv::Mat __stdcall GetFeaMat(cv::Mat image, string &strFeature);
TENSORSDK cv::Mat __stdcall NormFeature(string &Feature_str);

#endif  // TENSORSDK_H