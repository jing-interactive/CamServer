//vinjn's wrapper for OpenCV

#pragma once

#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4305 )
#pragma warning( disable: 4018 )
#pragma warning( disable: 4099 )
#pragma warning( disable: 4819 )

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <vector>
#include <map>

#include "vector2d.h"

using namespace cv;

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
 
void vFlip(const CvArr* src, int flipX, int flipY);

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

void vDrawText(IplImage* img, int x,int y,char* str, CvScalar clr=CV_RGB(255,255,255));
void vPolyLine(IplImage* dst, vector<Point>& pts, CvScalar clr=CV_RGB(255,255,255), int thick = 1);
CvScalar vDefaultColor(int idx);
//��title�����У���ʾw * h��СͼƬ(input)
void vShowManyImages(char* title, CvImage& input, int w, int h);

#define show_image(img_name) \
	cvNamedWindow(#img_name);\
	cvShowImage(#img_name, img_name); 

#define show_image2(img_name) \
	cvNamedWindow(#img_name, 0);\
	cvShowImage(#img_name, img_name); 

#define show_mat(img_name) \
	cv::namedWindow(#img_name, 0);\
	cv::imshow(#img_name, img_name); 

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

#include "OpenCV/OpenCV.h"

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
if (key == VK_ESCAPE)
break;
}
}

return 0;
}
*/

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
		From_Count,
	}_InputType;

	int _argc;
	char** _argv;

	CvCapture* _capture;

	void setAutoExplosure(bool is);
	bool getAutoExplosure();
	void setParamExplosure(int value);
	void showSettingsDialog();

	IplImage* _frame;
	int _cam_idx;
	Size _size;
	Size _half_size;
	int _channel;
	int _codec;

	int _frame_num;

	VideoInput();

	bool init(int cam_idx);
	bool init(char* video_file);
	bool init(int argc, char** argv);

	void wait(int t);

	IplImage* get_frame();

	void _post_init();

	~VideoInput();
};

struct IBackGround
{
	CvBGStatModel* bg_model;

	int thresh;
	
	IBackGround(){
		bg_model = NULL;
		thresh = 200;
	}

	virtual void init(IplImage* initial, void* param = NULL) = 0;

	virtual void update(IplImage* image, int mode = 0){
		cvUpdateBGStatModel( image, bg_model );
	}
	virtual void setIntParam(int idx, int value)
	{
		if (idx ==0) thresh = 255-value;
	}

	virtual IplImage* getForeground(){
		return bg_model->foreground;
	}

	virtual IplImage* getBackground(){
		return bg_model->background;
	}

	virtual ~IBackGround(){
		if (bg_model)
			cvReleaseBGStatModel(&bg_model);
	}
};


struct vBackFGDStat: public IBackGround
{
	void init(IplImage* initial, void* param = NULL);
};

struct vBackGaussian: public IBackGround
{
	void init(IplImage* initial, void* param = NULL);
};

#define DETECT_BOTH 0
#define DETECT_DARK 1
#define DETECT_BRIGHT 2

struct vBackGrayDiff: public IBackGround
{
	Ptr<IplImage> Frame;
	Ptr<IplImage> Bg;
	Ptr<IplImage> Fore ;
 
	int dark_thresh;

	void init(IplImage* initial, void* param = NULL);

	void setIntParam(int idx, int value);
	///mode: 0-> ������밵 1->���ڰ� 2->�������
	void update(IplImage* image, int mode = DETECT_BOTH);

	IplImage* getForeground(){
		return Fore;
	}
	IplImage* getBackground(){
		return Bg;
	}
};

struct vBackColorDiff: public vBackGrayDiff
{
	int nChannels;
	void init(IplImage* initial, void* param = NULL);

	///mode: 0-> ������밵 1->���ڰ� 2->�������
	void update(IplImage* image, int mode = DETECT_BOTH);
};

//��֡��ֵ��
struct vThreeFrameDiff: public IBackGround
{
	Ptr<IplImage> grayFrameOne;
	Ptr<IplImage> grayFrameTwo;
	Ptr<IplImage> grayFrameThree;
	Ptr<IplImage> grayDiff ;

	void init(IplImage* initial, void* param = NULL);

	void update(IplImage* image, int mode = 0);

	IplImage* getForeground(){
		return grayDiff;
	}
	IplImage* getBackground(){
		return grayDiff;
	}
};
//
//struct vBackColorDiff: public IBackGround
//{
//	Ptr<IplImage> colorBg;
//	Ptr<IplImage> grayDiff; 
//
//	int w;
//	int h;
//	int step; 
//	int step2;
//
//	void init(IplImage* initial, void* param = NULL){
//		cv::Size size = cvGetSize(initial);
//		colorBg.release();
//		colorBg = cvCloneImage(initial);
//
//		grayDiff.release();
//		grayDiff = cvCreateImage(size, 8, 1);
//
//		w = colorBg->width;
//		h = colorBg->height;
//		step = colorBg->widthStep; 
//		step2 = grayDiff->widthStep;
//	} 
//
//	void update(IplImage* image, int mode = 0){
//
//		cvZero(grayDiff);
//		for(int y=0;y<h;y++) 
//			for(int x=0;x<w;x++) 
//			{
//				uchar* pixel = &((uchar*)(image->imageData + step*y))[x*3];
//				uchar* pixel2 = &((uchar*)(colorBg->imageData + step*y))[x*3];
//				uchar* p = &((uchar*)(grayDiff->imageData + step2*y))[x];
//
//				uchar r = pixel[0] - pixel2[0];
//				uchar g = pixel[1] - pixel2[1];
//				uchar b = pixel[2] - pixel2[2];
//
//				if ((r*r+g*g+b*b) > thresh*thresh)						
//					*p = 255;
//				else
//					*p = 0;
//			}
//	}
//	IplImage* getForeground(){
//		return grayDiff;
//	}
//	IplImage* getBackground(){
//		return colorBg;
//	}
//};

struct vBackCodeBook
{
	CvBGCodeBookModel* model;
	Ptr<IplImage> yuvImage;
	Ptr<IplImage> ImaskCodeBook;
	Ptr<IplImage> ImaskCodeBookCC;
	bool isLearning;

	vBackCodeBook();

	~vBackCodeBook();

	void init(CvSize size);

	bool learn(IplImage* image);
	void finish_learn();

	//!!!!never release the returned image
	IplImage* getForeground(IplImage* image);

	void release();
};

void vRotateImage(IplImage* image, float angle, float centreX, float centreY);

void vHighPass(IplImage* src, IplImage* dst, int blurLevel = 10, int noiseLevel = 3);

void vPerspectiveTransform(const CvArr* src, CvArr* dst, cv::Point srcQuad[4], cv::Point dstQuad[4]);

CvFGDStatModelParams cvFGDStatModelParams();

void vGetPerspectiveMatrix(CvMat*& warp_matrix, cv::Point2f xsrcQuad[4], cv::Point2f xdstQuad[4]);



void vDrawDelaunay( CvSubdiv2D* subdiv,IplImage* src,IplImage * dst, bool drawLine = true );
void vDrawVoroni( CvSubdiv2D * subdiv, IplImage * src, IplImage * dst, bool drawLine = true );

struct Triangle
{
	int& operator[](int i){return idx[i];}
	const int operator[](int i) const{return idx[i];}
	int idx[3];
	point2di center;

	bool operator == (const Triangle& other) const 
	{
		return center == other.center;
	}

	bool operator < (const Triangle& other) const 
	{
		return center < other.center;
	}
};

struct DelaunaySubdiv
{
	DelaunaySubdiv(int w, int h);

	void insert(float x, float y);
	void clear();
	void build();

	void drawDelaunay( IplImage* src,IplImage * dst, bool drawLine = true );
	void drawVoroni( IplImage* src,IplImage * dst, bool drawLine = true );

	int getIndex(float x, float y);

	CvRect rect;
	MemStorage storage;
	CvSubdiv2D* subdiv;

	std::vector<Point> points;
	std::vector<Point> hull;

	std::vector<Triangle> triangles;//x,y,z -> vert_0, vert_1, vert_2
//	Mat hull;
	std::map<point2di, int> pt_map;

//	std::vector<std::vector<int> > triangles;	
private:
	void buildTriangles();
	void intoEdge(CvSubdiv2DEdge edge);
};

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

void vFillPoly(IplImage* img, const vector<Point>& pt_list, const Scalar& clr = Scalar(255,255,255));
void vLinePoly(IplImage* img, const vector<Point>& pt_list, const Scalar& clr = Scalar(255,255,255), int thick = 1);
void vLinePoly(IplImage* img, const vector<Point2f>& pt_list, const Scalar& clr = Scalar(255,255,255), int thick = 1);

inline bool isPointInsideRect(int x, int y, const Rect& rect)
{
	return (x >= rect.x && x <= rect.x+rect.width &&
		y >= rect.y && y <= rect.height);
}

// inline bool vTestRectHitRect(const Rect& A, const Rect& B)
// {
// 	int AminX = A.x, AminY = A.y, BminX = B.x, BminY = B.y;
// 	int AmaxX = A.x+A.width, AmaxY = A.y+A.height, BmaxX = B.x+B.width, BmaxY = B.y+B.height;
// // 	return (AminX <= BmaxX && AminY <= BmaxY &&
// // 		AmaxX >= BminX && AmaxY >= BminY);
// 	return isPointInsideRect()
// }

// Object-to-object bounding-box collision detector:
bool vTestRectHitRect(const Rect& object1, const Rect& object2);
