//vinjn's wrapper for OpenCV

#pragma once

#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4305 )
#pragma warning( disable: 4018 )
#pragma warning( disable: 4099 )
#pragma warning( disable: 4819 )

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <vector>
#include <map>

#include "point2d.h"

#ifdef WIN32
class videoInput;
#endif
#ifdef KINECT
class ofxKinectCLNUI; 
#endif

using std::vector;
//using std::set;
using std::map;
//CV_FOURCC('P','I','M','1')    = MPEG-1 codec
//CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
//CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
//CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
//CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
//CV_FOURCC('U', '2', '6', '3') = H263 codec
//CV_FOURCC('I', '2', '6', '3') = H263I codec
//CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
 
void vFlip(CvArr* src, int flipX, int flipY);

template<class T> class Image
{
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	void operator=(IplImage* img) {imgp=img;}
	inline T* operator[](const int rowIndx) {
		return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
	unsigned char b,g,r;
} RgbPixel;

typedef struct{
	float b,g,r;
} RgbPixelFloat;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

/*
For a single-channel byte image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
BwImage imgA(img);
imgA[i][j] = 111;
For a multi-channel byte image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
RgbImage  imgA(img);
imgA[i][j].b = 111;
imgA[i][j].g = 111;
imgA[i][j].r = 111;
For a multi-channel float image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_32F,3);
RgbImageFloat imgA(img);
imgA[i][j].b = 111;
imgA[i][j].g = 111;
imgA[i][j].r = 111;
*/
void vCopyImageTo(CvArr* small_image, IplImage* big_image, const CvRect& region);

void vDrawText(cv::Mat& img, int x,int y,char* str, CvScalar clr=CV_RGB(255,255,255));
void vPolyLine(cv::Mat& dst, vector<cv::Point>& pts, CvScalar clr=CV_RGB(255,255,255), int thick = 1);
CvScalar vDefaultColor(int idx);

#define show_image(img_name) do{\
	cvNamedWindow(#img_name);\
	cvShowImage(#img_name, img_name);}\
	while(0);

#define show_image2(img_name) do{\
	cvNamedWindow(#img_name, 0);\
	cvShowImage(#img_name, img_name);}\
	while(0);

#define show_mat(img_name) do{\
	cv::namedWindow(#img_name);\
	cv::imshow(#img_name, img_name);}\
	while(0);

//mask is 8bit���ڰ�ͼƬ��mask�����ص�ֵ > thresh����img��Ӧλ��Ϊԭɫ������Ϊ0
//img is 24bit
void feature_out(IplImage* img, IplImage* mask, int thresh);

char* get_time(bool full_length = true);

typedef vector<IplImage*> image_array_t;

const CvScalar CV_RED = CV_RGB(255,0,0);
const CvScalar CV_GREEN = CV_RGB(0,255,0);
const CvScalar CV_BLUE = CV_RGB(0,0,255);
const CvScalar CV_BLACK = CV_RGB(0,0,0);
const CvScalar CV_WHITE = CV_RGB(255,255,255);
const CvScalar CV_GRAY = CV_RGB(122,122,122); 

inline CvScalar vRandomColor()
{
	static CvRNG   rng = cvRNG((unsigned)-1);
	int icolor = cvRandInt(&rng);
	return CV_RGB(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
}

#define vDrawRect(image, rc, clr) cvDrawRect(image, cvPoint(rc.x,rc.y), cvPoint(rc.x+rc.width,rc.y+rc.height), clr)

/*

#include "OpenCV.h"

VideoInput input;

int main(int argc, char** argv )
{	
if (input.init(argc,argv))
{
while (true)
{
IplImage* raw = input.get_frame(); 
if (!raw)
break;
cvFlip(raw, 0, 1);

show_image(raw);
int key = cvWaitKey(1);
if (key == 0x1B)
break;
}
}

return 0;
}
*/

#define WRITE_(key, var) fs<<key<<var
#define WRITE_FS(var) fs<<(#var)<<(var)

#define READ_(key, var) fs[key]>>var
#define READ_FS(var) fs[#var]>>(var)

#define vGrayScale(clr, gray) cvCvtColor(clr, gray, CV_BGR2GRAY) 
#define vColorFul(gray, clr) cvCvtColor(gray, clr , CV_GRAY2BGR) 
#define vThresh(gray, thresh) cvThreshold( gray, gray, thresh, 255, CV_THRESH_BINARY )//if > thresh -> white
#define vThreshInv(gray, thresh) cvThreshold( gray, gray, thresh, 255, CV_THRESH_BINARY_INV )//if < thresh -> white
#define vAutoThresh(gray, max_value) cvAdaptiveThreshold(gray, gray, max_value)
#define vOpen(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_OPEN, times );//ȥ����ɫС����
#define vClose(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_CLOSE, times );//ȥ����ɫС����
#define vDilate(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_DILATE, times );
#define vErode(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_ERODE, times );

#define vFullScreen(win_name) \
	cvSetWindowProperty(win_name, CV_WND_PROP_FULLSCREEN, 1);

#define vCreateGray(clr) cvCreateImage(cvGetSize(clr), 8, 1);
#define vCreateColor(clr) cvCreateImage(cvGetSize(clr), 8, 3);

struct VideoInput
{
	int _fps;

	enum e_InputType
	{
		From_Image = 0,
		From_Video,
		From_Camera,
#ifdef KINECT
		From_Kinect,
#endif
		From_Count,
	}_InputType;

	int _argc;
	char** _argv;

	CvCapture* _capture;

	void showSettingsDialog();

	IplImage* _frame;
	int _cam_idx;
	cv::Size _size;
	cv::Size _half_size;
	int _channel;
	int _codec;

	int _frame_num;

	VideoInput();

	void resize(int w, int h);

	bool init(int cam_idx);
	bool init(char* video_file);
	bool init(int argc, char** argv);

#ifdef KINECT
	cv::Ptr<ofxKinectCLNUI> _kinect;
	bool init_kinect();
#endif
#ifdef WIN32
	cv::Ptr<videoInput> VI;
#endif
	void wait(int t);

	IplImage* get_frame();

	void _post_init();

	~VideoInput();

private:
	char buffer[256];
};


void vRotateImage(IplImage* image, float angle, float centreX, float centreY);

void vHighPass(IplImage* src, IplImage* dst, int blurLevel = 10, int noiseLevel = 3);

void vPerspectiveTransform(const CvArr* src, CvArr* dst, cv::Point srcQuad[4], cv::Point dstQuad[4]);

CvFGDStatModelParams cvFGDStatModelParams();

void vGetPerspectiveMatrix(CvMat*& warp_matrix, cv::Point2f xsrcQuad[4], cv::Point2f xdstQuad[4]);



void on_default(int );

//���ȱ任��nPercentΪ��ʱ����������䰵 
int ContrastAdjust(const IplImage* srcImg,
				   IplImage* dstImg,
				   float nPercent);

//�Աȶȱ任��brightnessС��1���ͶԱȶȣ�����1��ǿ�Աȶ�
int BrightnessAdjust(const IplImage* srcImg,
					 IplImage* dstImg,
					 float brightness);

void convertRGBtoHSV(const IplImage *imageRGB, IplImage *imageHSV);
void convertHSVtoRGB(const IplImage *imageHSV, IplImage *imageRGB);

#define cv_try_begin() try{

#define cv_try_end() 	}\
	catch (cv::Exception& ex)\
{\
	printf("[cv::Exception] %s\n", ex.what());\
}


#define vAddWeighted(src, alpha, dst) cvAddWeighted(src, alpha, dst, 1-alpha, 0, dst);
 
void cvSkinSegment(IplImage* img, IplImage* mask);

void vFillPoly(IplImage* img, const vector<cv::Point>& pt_list, const cv::Scalar& clr = cv::Scalar(255,255,255));
void vLinePoly(IplImage* img, const vector<cv::Point>& pt_list, const cv::Scalar& clr = cv::Scalar(255,255,255), int thick = 1);
void vLinePoly(IplImage* img, const vector<cv::Point2f>& pt_list, const cv::Scalar& clr = cv::Scalar(255,255,255), int thick = 1);

inline bool isPointInsideRect(int x, int y, const cv::Rect& rect)
{
	return (x >= rect.x && x <= rect.x+rect.width &&
		y >= rect.y && y <= rect.height);
}

// inline bool vTestRectHitRect(const cv::Rect& A, const cv::Rect& B)
// {
// 	int AminX = A.x, AminY = A.y, BminX = B.x, BminY = B.y;
// 	int AmaxX = A.x+A.width, AmaxY = A.y+A.height, BmaxX = B.x+B.width, BmaxY = B.y+B.height;
// // 	return (AminX <= BmaxX && AminY <= BmaxY &&
// // 		AmaxX >= BminX && AmaxY >= BminY);
// 	return isPointInsideRect()
// }

// Object-to-object bounding-box collision detector:
bool vTestRectHitRect(const cv::Rect& object1, const cv::Rect& object2);
