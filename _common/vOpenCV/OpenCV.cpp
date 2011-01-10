#include "OpenCV.h"

#define _I_REALLY_NEED_DEBUG

#if defined _I_REALLY_NEED_DEBUG && _DEBUG
#pragma comment(lib,"cv210d.lib")
#pragma comment(lib,"cvaux210d.lib")
#pragma comment(lib,"cxcore210d.lib")
#pragma comment(lib,"highgui210d.lib")
#else
#pragma comment(lib,"cv210.lib")
#pragma comment(lib,"cvaux210.lib")
#pragma comment(lib,"cxcore210.lib")
#pragma comment(lib,"highgui210.lib")
#endif

#include <set>

#define NO_FLIP 1000
/*
flip_param -> flip_mode
0 -> NO_FLIP
1 -> 0	:	horizontal
2 -> 1	:	vertical
3 -> -1	:	both
*/
void vFlip(const CvArr* src, int flipX, int flipY)
{
	int flip_param = flipX*2 + flipY;
	int mode = NO_FLIP;//NO FLIP
	switch(flip_param)
	{
	case 1:
		mode = 0;break;
	case 2:
		mode = 1;break;
	case 3:
		mode = -1;break;
	default:
		break;
	}
	if (mode != NO_FLIP)
		cvFlip(src, 0, mode);
}
//============================================================================
static bool IsEdgeIn(int ind1, int ind2,
					 const std::vector<std::vector<int> > &edges)
{
	for (int i = 0; i < edges.size (); i++)
	{
		if ((edges[i][0] == ind1 && edges[i][1] == ind2) ||
			(edges[i][0] == ind2 && edges[i][1] == ind1) )
			return true;
	}
	return false;
}

//============================================================================
static bool IsTriangleNotIn(const std::vector<int>& one_tri,
							const std::vector<std::vector<int> > &tris)
{
	std::set<int> tTriangle;
	std::set<int> sTriangle;

	for (int i = 0; i < tris.size (); i ++)
	{
		tTriangle.clear();
		sTriangle.clear();
		for (int j = 0; j < 3; j++ )
		{
			tTriangle.insert(tris[i][j]);
			sTriangle.insert(one_tri[j]);
		}
		if (tTriangle == sTriangle)    return false;
	}

	return true;
}

void vCopyImageTo(CvArr* tiny_image, IplImage* big_image, const CvRect& region)
{
	CV_Assert(tiny_image && big_image);
	// Set the image ROI to display the current image
	cvSetImageROI(big_image, region);

	IplImage* t = (IplImage*)tiny_image;
	if (t->nChannels == 1 && big_image->nChannels == 3)
	{
		Ptr<IplImage> _tiny = cvCreateImage(cvGetSize(t), 8, 3);
		cvCvtColor(tiny_image, _tiny, CV_GRAY2BGR);
		cvResize(_tiny, big_image);
	}
	else
	{
		// Resize the input image and copy the it to the Single Big Image
		cvResize(tiny_image, big_image);
	}
	// Reset the ROI in order to display the next image
	cvResetImageROI(big_image);
}

void vDrawText(IplImage* img, int x,int y,char* str, CvScalar clr)
{
	static CvFont* font = NULL;

	if (!font)
	{
		font = new CvFont();
		cvInitFont(font,CV_FONT_VECTOR0,0.5,0.5, 0, 1);
	}
	cvPutText(img, str, cvPoint(x,y),font, clr);
}


CvScalar default_colors[] =
{
	{{255,128,0}},
	{{255,255,0}},
	{{0,0,255}},
	{{0,128,255}},
	{{0,255,255}},
	{{0,255,0}},
	{{255,0,0}},
	{{255,0,255}}
};

const int sizeOfColors = sizeof(default_colors)/sizeof(CvScalar);
CvScalar vDefaultColor(int idx){ return default_colors[idx%sizeOfColors];}


char* get_time(bool full_length)
{
	static char str[MAX_PATH];
	time_t timep;
	tm *p;
	time(&timep);
	p = gmtime(&timep);

	if (full_length)
		sprintf(str, "%d-%d-%d__%d_%d_%d",
		1900+p->tm_year, 1+p->tm_mon, p->tm_mday,
		p->tm_hour, p->tm_min, p->tm_sec);
	else
		sprintf(str, "%d_%d_%d",
		p->tm_hour, p->tm_min, p->tm_sec);

	return str;
}

void feature_out(IplImage* img, IplImage* mask, int thresh)
{
	int w = img->width;
	int h = img->height;
	int step = img->widthStep;
	int channels = img->nChannels;
	uchar* data   = (uchar *)img->imageData;
	uchar* mdata   = (uchar *)mask->imageData;

	for(int i=0;i<h;i++)
		for(int j=0;j<w;j++)
			for(int k=0;k<channels;k++)
			{
				if (mdata[i*mask->widthStep+j] < thresh)
					data[i*step+j*channels+k] = 0;
			}

}



VideoInput::VideoInput()
{
	_fps = 0;
	_capture = NULL;
	_frame = NULL;
	_InputType = From_Count;
}

#ifndef CUSTOM_OPENCV_LIB
#define CV_CAP_PROP_AUTO_EXPOSURE	100
#define CV_CAP_PROP_SHOW_DIALOG		101
#endif

void VideoInput::setAutoExplosure(bool is)
{
	if (_capture)
		cvSetCaptureProperty(_capture,CV_CAP_PROP_AUTO_EXPOSURE,(double)is);
}

bool VideoInput::getAutoExplosure()
{
	if (_capture)
		return cvGetCaptureProperty(_capture,CV_CAP_PROP_AUTO_EXPOSURE);
	else
		return false;
}

void VideoInput::showSettingsDialog()
{
	if (_capture)
		cvSetCaptureProperty(_capture,CV_CAP_PROP_SHOW_DIALOG, true);
}

void VideoInput::setParamExplosure(int value)
{
	if (_capture)
		cvSetCaptureProperty(_capture,CV_CAP_PROP_EXPOSURE,(double)value);
}

bool VideoInput::init(int cam_idx)
{
	_capture = cvCaptureFromCAM(CV_CAP_DSHOW+cam_idx);

	if( !_capture )
	{
		_capture = cvCaptureFromCAM(cam_idx);

		if (!_capture)
		{
			printf("Could not initialize camera # %d\n", cam_idx);
			return false;
		}
		else
		{
			printf("Reading from camera # %d.\n", cam_idx);
			return true;
		}
	}
	else
	{
		_InputType = From_Camera;
		printf("Reading from camera # %d via DirectShow.\n", cam_idx);
		_post_init();
		return true;
	}
}

bool VideoInput::init(char* file_name)
{
	_frame = cvLoadImage(file_name);

	if (_frame)
	{
		printf("Reading from image %s.\n", file_name);
		_InputType = From_Image;
	}
	else
	{
		_capture = cvCaptureFromAVI(file_name);
		if( _capture )
		{
			printf("Reading from video %s.\n", file_name);
			_InputType = From_Video;
		}
		else
		{
			printf("Could not open file %s.\n", file_name);
			return false;
		}
	}

	_post_init();
	return true;

}

bool VideoInput::init(int argc, char** argv)
{
	_argc = argc;
	_argv = argv;
	if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && ::isdigit(argv[1][0])))
		return init( argc == 2 ? argv[1][0] - '0' : 0 );
	else if( argc == 2 )
		return init( argv[1] );
	return false;
}

void VideoInput::wait(int t)
{
	if (_InputType == From_Image)
		return;
	for (int i=0;i<t;i++)
		get_frame();
}

IplImage* VideoInput::get_frame()
{
	if (_InputType != From_Image)
	{
		_frame = cvQueryFrame(_capture);
		_frame_num ++;
		if (_frame == NULL)
		{
			cvReleaseCapture(&_capture);
			init(_argc, _argv);
		}
	}
	return _frame;
}

void VideoInput::_post_init()
{
	if (_InputType != From_Image)
	{
		_frame = get_frame();
		_fps = cvGetCaptureProperty(_capture, CV_CAP_PROP_FPS);
		_codec = cvGetCaptureProperty(_capture, CV_CAP_PROP_FOURCC);
		if (_fps == 0)
			printf("Fps: unknown");
		else
			printf("Fps: %d", _fps);
	}

	_size.width = _frame->width;
	_size.height = _frame->height;
	_half_size.width  = _size.width/2;
	_half_size.height  = _size.height/2;
	_channel = _frame->nChannels;
	_frame_num = 0;

	printf("; Size: <%d,%d>\n",  _size.width, _size.height);
}

VideoInput::~VideoInput()
{
	if (_capture != NULL)
		cvReleaseCapture( &_capture );
	//	if (_frame != NULL)
	//		cvReleaseImage(&_frame);
}

vBackCodeBook::vBackCodeBook()
{
	isLearning = true;
	model = 0;
	yuvImage = 0;
	ImaskCodeBook = 0;
	ImaskCodeBookCC = 0;
}

vBackCodeBook::~vBackCodeBook()
{
	release();
}

void vBackCodeBook::init(CvSize size)
{
	model = cvCreateBGCodeBookModel();
	//Set color thresholds to default values
	model->modMin[0] = model->modMin[1] = model->modMin[2] = 3;
	model->modMax[0] = model->modMax[1] = model->modMax[2] = 10;
	model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;
	// CODEBOOK METHOD ALLOCATION
	yuvImage = cvCreateImage( size, IPL_DEPTH_8U, 3 );
	ImaskCodeBook = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	ImaskCodeBookCC = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	cvSet(ImaskCodeBook,cvScalar(255));
}

bool vBackCodeBook::learn(IplImage* image)
{
	if (isLearning)
	{
		cvCvtColor( image, yuvImage, CV_BGR2YCrCb );
		//This is where we build our background model
		cvBGCodeBookUpdate( model, yuvImage );
		return true;
	}
	else
		return false;
}

void vBackCodeBook::finish_learn()
{
	if (isLearning)
	{
		cvBGCodeBookClearStale( model, model->t/2 );
		isLearning = false;
	}
}

//!!!!never release the returned image
IplImage* vBackCodeBook::getForeground(IplImage* image)
{
	cvCvtColor( image, yuvImage, CV_BGR2YCrCb );
	// Find foreground by codebook method
	cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook );
	// This part just to visualize bounding boxes and centers if desired
	cvCopy(ImaskCodeBook,ImaskCodeBookCC);
	//cvSegmentFGMask( ImaskCodeBookCC );

	return ImaskCodeBookCC;
}


void vBackCodeBook::release()
{

}


void vHighPass(IplImage* src, IplImage* dst, int blurLevel/* = 10*/, int noiseLevel/* = 3*/)
{
	if (blurLevel > 0 && noiseLevel > 0)
	{
		// create the unsharp mask using a linear average filter
		cvSmooth(src, dst, CV_BLUR, blurLevel*2+1);

		cvSub(src, dst, dst);

		// filter out the noise using a median filter
		cvSmooth(dst, dst, CV_MEDIAN, noiseLevel*2+1);
	}
	else
		cvCopy(src, dst);
}

void vGetPerspectiveMatrix(CvMat*& warp_matrix, cv::Point2f xsrcQuad[4], cv::Point2f xdstQuad[4])
{
	static CvPoint2D32f srcQuad[4];
	static CvPoint2D32f dstQuad[4];
	for (int i=0;i<4;i++)
	{
		srcQuad[i] = xsrcQuad[i];
		dstQuad[i] = xdstQuad[i];
	}

	if (warp_matrix == NULL)
		warp_matrix = cvCreateMat(3, 3, CV_32FC1);
	cvGetPerspectiveTransform(srcQuad, dstQuad, warp_matrix);
}

void vPerspectiveTransform(const CvArr* src, CvArr* dst, cv::Point xsrcQuad[4], cv::Point xdstQuad[4])
{
	static CvPoint2D32f srcQuad[4];
	static CvPoint2D32f dstQuad[4];
	for (int i=0;i<4;i++)
	{
		srcQuad[i] = xsrcQuad[i];
		dstQuad[i] = xdstQuad[i];
	}

	static CvMat* warp_matrix = cvCreateMat(3, 3, CV_32FC1);
	cvGetPerspectiveTransform(srcQuad, dstQuad, warp_matrix);
	cvWarpPerspective(src, dst, warp_matrix);
}

CvFGDStatModelParams cvFGDStatModelParams()
{
	CvFGDStatModelParams p;
	p.Lc = CV_BGFG_FGD_LC;			/* Quantized levels per 'color' component. Power of two, typically 32, 64 or 128.				*/
	p.N1c = CV_BGFG_FGD_N1C;			/* Number of color vectors used to model normal background color variation at a given pixel.			*/
	p.N2c = CV_BGFG_FGD_N2C;			/* Number of color vectors retained at given pixel.  Must be > N1c, typically ~ 5/3 of N1c.			*/
	/* Used to allow the first N1c vectors to adapt over time to changing background.				*/

	p.Lcc = CV_BGFG_FGD_LCC;			/* Quantized levels per 'color co-occurrence' component.  Power of two, typically 16, 32 or 64.			*/
	p.N1cc = CV_BGFG_FGD_N1CC;		/* Number of color co-occurrence vectors used to model normal background color variation at a given pixel.	*/
	p.N2cc = CV_BGFG_FGD_N2CC;		/* Number of color co-occurrence vectors retained at given pixel.  Must be > N1cc, typically ~ 5/3 of N1cc.	*/
	/* Used to allow the first N1cc vectors to adapt over time to changing background.				*/

	p.is_obj_without_holes;/* If TRUE we ignore holes within foreground blobs. Defaults to TRUE.						*/
	p.perform_morphing;	/* Number of erode-dilate-erode foreground-blob cleanup iterations.						*/
	/* These erase one-pixel junk blobs and merge almost-touching blobs. Default value is 1.			*/

	p.alpha1 = CV_BGFG_FGD_ALPHA_1;		/* How quickly we forget old background pixel values seen.  Typically set to 0.1  				*/
	p.alpha2 = CV_BGFG_FGD_ALPHA_2;		/* "Controls speed of feature learning". Depends on T. Typical value circa 0.005. 				*/
	p.alpha3 = CV_BGFG_FGD_ALPHA_3;		/* Alternate to alpha2, used (e.g.) for quicker initial convergence. Typical value 0.1.				*/

	p.delta = CV_BGFG_FGD_DELTA;		/* Affects color and color co-occurrence quantization, typically set to 2.					*/
	p.T = CV_BGFG_FGD_T;			/* "A percentage value which determines when new features can be recognized as new background." (Typically 0.9).*/
	p.minArea = CV_BGFG_FGD_MINAREA;		/* Discard foreground blobs whose bounding box is tinyer than this threshold.					*/

	return  p;
}

void vBackFGDStat::init(IplImage* initial, void* param)
{
	CvFGDStatModelParams* p = (CvFGDStatModelParams*)param;
	bg_model = cvCreateFGDStatModel( initial, p );
}

void vBackGaussian::init(IplImage* initial, void* param)
{
	CvGaussBGStatModelParams* p = (CvGaussBGStatModelParams*)param;
	bg_model = cvCreateGaussianBGModel( initial, p );
}

void vBackGrayDiff::setIntParam(int idx, int value)
{
	IBackGround::setIntParam(idx, value);
	if (idx == 1)
		dark_thresh = 255-value;
}

void vBackGrayDiff::init(IplImage* initial, void* param/* = NULL*/){
	cv::Size size = cvGetSize(initial);

	Frame.release();
	Frame = cvCreateImage(size, 8, 1);
	Bg.release();
	Bg = cvCreateImage(size, 8, 1);
	Fore.release();
	Fore = cvCreateImage(size, 8, 1);

	thresh = 50;
	dark_thresh = 200;

	vGrayScale(initial, Bg);
}

void vBackGrayDiff::update(IplImage* image, int mode/* = 0*/){
	vGrayScale(image, Frame);
	if (mode == DETECT_BOTH)
	{
		BwImage frame(Frame);
		BwImage bg(Bg);
		BwImage fore(Fore);

		cvZero(Fore);
		for (int y=0;y<image->height;y++)
			for (int x=0;x<image->width;x++)
			{
				int delta = frame[y][x] - bg[y][x];
				if (delta >= thresh || delta <= -dark_thresh)
					fore[y][x] = 255;
			}
	}
	else if (mode == DETECT_DARK)
	{
		cvSub(Bg, Frame, Fore);
		vThresh(Fore, dark_thresh);
	}
	else if (mode == DETECT_BRIGHT)
	{
		cvSub(Frame, Bg, Fore);
		vThresh(Fore, thresh);
	}
}


void vBackColorDiff::init(IplImage* initial, void* param/* = NULL*/){
	cv::Size size = cvGetSize(initial);
	nChannels = initial->nChannels;

	Frame.release();
	Frame = cvCloneImage(initial);
	Bg.release();
	Bg = cvCloneImage(initial);
	Fore.release();
	Fore = cvCreateImage(size, 8, 1);

	thresh = 220;
	dark_thresh = 30;
}

void vBackColorDiff::update(IplImage* image, int mode/* = 0*/){
//	vGrayScale(image, Frame);
	cvCopyImage(image, Frame);
	if (mode == DETECT_BOTH)
	{
		if (nChannels == 1)
		{
			BwImage frame(Frame);
			BwImage bg(Bg);
			BwImage fore(Fore);

			cvZero(Fore);
			for (int y=0;y<image->height;y++)
				for (int x=0;x<image->width;x++)
				{
					int delta = frame[y][x] - bg[y][x];
					if (delta >= thresh || delta <= -dark_thresh)
						fore[y][x] = 255;
				}
		}
		else
		{
			RgbImage frame(Frame);
			RgbImage bg(Bg);
			BwImage fore(Fore);

			int min_t = 255-thresh;
			int max_t = 255-dark_thresh;
			cvZero(Fore);
			for (int y=0;y<image->height;y++)
				for (int x=0;x<image->width;x++)
				{
					int r = frame[y][x].r - bg[y][x].r;
					int g = frame[y][x].g - bg[y][x].g;
					int b = frame[y][x].b - bg[y][x].b;
#if 1
					if ((r >= thresh || r <= -dark_thresh)
						&& (g >= thresh || g <= -dark_thresh)
							&& (b >= thresh || b <= -dark_thresh))
#else
					int delta = r*r+g*g+b*b;
					if (delta >= min_t*min_t && delta <= max_t*max_t)
#endif
						fore[y][x] = 255;
				}
		}
	}
	else if (mode == DETECT_DARK)
	{
		cvSub(Bg, Frame, Fore);
		vThresh(Fore, dark_thresh);
	}
	else if (mode == DETECT_BRIGHT)
	{
		cvSub(Frame, Bg, Fore);
		vThresh(Fore, thresh);
	}
}

void vThreeFrameDiff::init(IplImage* initial, void* param/* = NULL*/)
{
	cv::Size size = cvGetSize(initial);

	grayFrameOne.release();
	grayFrameOne = cvCreateImage(size, 8, 1);
	vGrayScale(initial, grayFrameOne);
	grayFrameTwo.release();
	grayFrameTwo = cvCreateImage(size, 8, 1);
	vGrayScale(initial, grayFrameTwo);
	grayFrameThree.release();
	grayFrameThree = cvCreateImage(size, 8, 1);
	vGrayScale(initial, grayFrameThree);
	grayDiff.release();
	grayDiff = cvCreateImage(size, 8, 1);
}

void vThreeFrameDiff::update(IplImage* image, int mode/* = 0*/){
	vGrayScale(image, grayFrameThree);

	BwImage one(grayFrameOne);
	BwImage two(grayFrameTwo);
	BwImage three(grayFrameThree);
	BwImage diff(grayDiff);

	cvZero(grayDiff);
	for (int y=0;y<image->height;y++)
		for (int x=0;x<image->width;x++)
		{
			if (abs(one[y][x] - two[y][x]) > thresh ||
				abs(three[y][x] - two[y][x]) > thresh)
				diff[y][x] = 255;
		}

		show_image(grayFrameOne);
		show_image(grayFrameTwo);
		show_image(grayFrameThree);
		cvCopyImage(grayFrameTwo, grayFrameOne);
		cvCopyImage(grayFrameThree, grayFrameTwo);

	//if (mode == DETECT_BOTH)
	//	cvAbsDiff(grayFrame, grayBg, grayDiff);
	//else if (mode == DETECT_DARK)
	//	cvSub(grayBg, grayFrame, grayDiff);
	//else if (mode == DETECT_BRIGHT)
	//	cvSub(grayFrame, grayBg, grayDiff);
	//vThresh(grayDiff, thresh);
}

void vPolyLine(IplImage* dst, vector<Point>& pts, CvScalar clr, int thick)
{
	int n = pts.size();
	if (n > 1)
	{
		int k =0;
		for (;k<n-1;k++)
		{
			cvLine(dst, pts[k], pts[k+1], clr, thick);
		}
		cvLine(dst, pts[k], pts[0], clr, thick);
	}
}

bool operator < (const Point& a, const Point& b)
{
	return a.x < b.x && a.y < b.y;
}

static void draw_edge( IplImage* img, CvSubdiv2DEdge edge, CvScalar color )
{
	CvSubdiv2DPoint* org_pt = cvSubdiv2DEdgeOrg(edge);
	CvSubdiv2DPoint* dst_pt = cvSubdiv2DEdgeDst(edge);

	if( org_pt && dst_pt )
	{
		CvPoint2D32f org = org_pt->pt;
		CvPoint2D32f dst = dst_pt->pt;

		CvPoint iorg = cvPoint( cvRound( org.x ), cvRound( org.y ));
		CvPoint idst = cvPoint( cvRound( dst.x ), cvRound( dst.y ));

		cvLine( img, iorg, idst, color, 1, CV_AA, 0 );
	}
}


static void draw_facet( CvSubdiv2D * subdiv, IplImage * dst, IplImage * src, CvSubdiv2DEdge edge, bool drawLine )
{
	CvSubdiv2DEdge e = edge;
	int i, count = 0;
	vector<CvPoint> buf;

	// count number of edges in facet
	do
	{
		count++;
		e = cvSubdiv2DGetEdge( e, CV_NEXT_AROUND_LEFT );
	}
	while( e != edge && count < subdiv->quad_edges * 4 );

	// gather points
	e = edge;
	assert(count == 3);
	for( i = 0; i < count; i++ )
	{
		CvSubdiv2DPoint *pt = cvSubdiv2DEdgeOrg( e );

		if( !pt )
			break;
		assert( fabs( pt->pt.x ) < 10000 && fabs( pt->pt.y ) < 10000 );
		buf.push_back(cvPoint( cvRound( pt->pt.x ), cvRound( pt->pt.y )));

		e = cvSubdiv2DGetEdge( e, CV_NEXT_AROUND_LEFT );
	}

	if( i == count )
	{
		CvSubdiv2DPoint *pt = cvSubdiv2DEdgeDst( cvSubdiv2DRotateEdge( edge, 1 ));
		if (!pt)
			pt = cvSubdiv2DEdgeOrg( cvSubdiv2DRotateEdge( edge, 0 ));
		CvPoint ip = cvPoint( cvRound( pt->pt.x ), cvRound( pt->pt.y ));
		CvScalar color = {{0,0,0,0}};

		//printf("count = %d, (%d,%d)\n", ip.x, ip.y );

		if( 0 <= ip.x && ip.x < src->width && 0 <= ip.y && ip.y < src->height )
		{
			uchar *ptr = (uchar*)(src->imageData + ip.y * src->widthStep + ip.x * 3);
			color = CV_RGB( ptr[2], ptr[1], ptr[0] );
		}

		cvFillConvexPoly( dst, &buf[0], count, color );

		if (drawLine)
		{
			for (i = 1;i<count;i++)
			{
				cvDrawLine(dst, buf[i], buf[i-1], CV_RGB(30,30,30),1);
			}
		}
	}
}




void vDrawDelaunay( CvSubdiv2D* subdiv,IplImage* src,IplImage * dst , bool drawLine)
{
	int i, total = subdiv->edges->total;

	cvCalcSubdivVoronoi2D( subdiv );

	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D *edge = (CvQuadEdge2D *) cvGetSetElem( subdiv->edges, i );

		if( edge && CV_IS_SET_ELEM( edge ))
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge) edge;

			//	draw_edge( src, (CvSubdiv2DEdge)edge + 1, CV_RGB(0,0,0) );//voroni edge
			//	draw_edge( src, (CvSubdiv2DEdge)edge, CV_RGB(0,0,0) );//delaunay edge

			//e itslef
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 0 ), drawLine);
			//reversed e
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 2 ), drawLine);
		}
	}
}

void vDrawVoroni( CvSubdiv2D * subdiv, IplImage * src, IplImage * dst, bool drawLine )
{
	int i, total = subdiv->edges->total;

	cvCalcSubdivVoronoi2D( subdiv );

	//icvSet( dst, 255 );
	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D *edge = (CvQuadEdge2D *) cvGetSetElem( subdiv->edges, i );

		if( edge && CV_IS_SET_ELEM( edge ))
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge) edge;

			// left
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 1 ), drawLine);
			// right
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 3 ), drawLine);
		}
	}
}



DelaunaySubdiv::DelaunaySubdiv(int w, int h)
{
	storage = cvCreateMemStorage();
	rect = cvRect(0, 0, w, h);

	subdiv = cvCreateSubdivDelaunay2D(rect, storage);
}

void DelaunaySubdiv::insert(float x, float y)
{
	Point2f pt(x,y);
	cvSubdivDelaunay2DInsert(subdiv, pt);
	pt_map.insert(std::make_pair(point2di(x, y), points.size() ) );
	points.push_back(Point(x,y));
}

void DelaunaySubdiv::clear()
{
	cvClearMemStorage(storage);
	subdiv = cvCreateSubdivDelaunay2D(rect, storage);
	pt_map.clear();
	points.clear();
}

void DelaunaySubdiv::intoEdge(CvSubdiv2DEdge edge)
{
	CvSubdiv2DEdge e = edge;
	int i;
	const int count = 3;
	Point triple[count];

	//// count number of edges in facet
	//do
	//{
	//	count++;
	//	e = cvSubdiv2DGetEdge( e, CV_NEXT_AROUND_LEFT );
	//}
	//while( e != edge && count < subdiv->quad_edges * 4 );

	//// gather points
	//e = edge;
	//assert(count == 3);
	for( i = 0; i < count; i++ )
	{
		CvSubdiv2DPoint *pt = cvSubdiv2DEdgeOrg( e );
		if( !pt || fabs( pt->pt.x ) > 1500 || fabs( pt->pt.y ) > 1500)
			break;

		triple[i] = Point(pt->pt.x ,  pt->pt.y);
		e = cvSubdiv2DGetEdge( e, CV_NEXT_AROUND_LEFT );
	}

	if( i == count )
	{
		Triangle aTri;

		for (int k=0;k<3;k++)
		{
			aTri[k] = getIndex(triple[k].x, triple[k].y);
			if (aTri[k] == -1) return;
		}
		aTri.center.X = (triple[0].x + triple[1].x + triple[2].x)/3;
		aTri.center.Y = (triple[0].y + triple[1].y + triple[2].y)/3;

		triangles.push_back(aTri);
	}
}

int  DelaunaySubdiv::getIndex(float x, float y)
{
	int ret = -1;
	map<point2di, int>::const_iterator it =  pt_map.find(point2di(x,y));
	if (it != pt_map.end())
		ret = it->second;
	return ret;
}

void DelaunaySubdiv::buildTriangles()
{
	triangles.clear();
	int i, total = subdiv->edges->total;

	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D *edge = (CvQuadEdge2D *) cvGetSetElem( subdiv->edges, i );

		if( edge && CV_IS_SET_ELEM( edge ))
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge) edge;
			//e itslef
			intoEdge( cvSubdiv2DRotateEdge( e, 0 ));
			//reversed e
			intoEdge( cvSubdiv2DRotateEdge( e, 2 ));
		}
	}
	sort(triangles.begin(),triangles.end());
	std::vector<Triangle>::iterator it = unique(triangles.begin(),triangles.end());
	triangles.erase(it, triangles.end());
}


void DelaunaySubdiv::build()
{
	//	cvCalcSubdivVoronoi2D( subdiv );

	buildTriangles();

	return;

	cv_try_begin();

	int count = points.size();
	int __n = count;
	//hull.resize(count);
	//	hull = Mat(1, count, CV_32FC2);

	//CvPoint* _points =new CvPoint[count];
	//int* _hull = new int[count];
	cv::Mat pointMat( 1, count, CV_32SC2, &points[0] );
	//	 CvMat hullMat = cvMat( 1, count, CV_32SC1, &hull[0]);
	//  for(int  i = 0; i < count; i++ )
	//  {
	//      pt0.x = rand() % (width/2) + width/4;
	//      pt0.y = rand() % (height/2) + height/4;
	//     _points[i] = pt0;
	//points[i] = points[i];
	//  }

	//  cvConvexHull2( &pointMat, ConvexHull, CV_CLOCKWISE, 0 );
	cv::convexHull(pointMat, hull);

	CvMat ConvexHull = cvMat (1, __n, CV_32SC2, &hull[0]);

	//	cv::Mat pointMat(1, count, CV_32SC2, &points[0] );

	//		cv::convexHull(pointMat, hullMat);
	//	convexHull(pointMat, hull);
	//
	////	CvMat* pointMa = cvCreateMat(1, count, CV_32SC2);
	//	cvConvexHull2(&(CvMat)pointMat, ConvexHull, CV_CLOCKWISE, 0);
	//	cv::Mat hullMat(1, hull.size(), CV_32SC2, &hull[0] );
	//
	//	doDelaunay(subdiv,  &ConvexHull);
	//	cvReleaseMat(&pointMa);

	cv_try_end();
}


void DelaunaySubdiv::drawDelaunay( IplImage* src,IplImage * dst , bool drawLine)
{
	int i, total = subdiv->edges->total;

	cvCalcSubdivVoronoi2D( subdiv );

	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D *edge = (CvQuadEdge2D *) cvGetSetElem( subdiv->edges, i );

		if( edge && CV_IS_SET_ELEM( edge ))
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge) edge;

			//	draw_edge( src, (CvSubdiv2DEdge)edge + 1, CV_RGB(0,0,0) );//voroni edge
			//	draw_edge( src, (CvSubdiv2DEdge)edge, CV_RGB(0,0,0) );//delaunay edge

			//e itslef
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 0 ), drawLine);
			//reversed e
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 2 ), drawLine);
		}
	}
}


void DelaunaySubdiv::drawVoroni( IplImage * src, IplImage * dst, bool drawLine )
{
	int i, total = subdiv->edges->total;

	cvCalcSubdivVoronoi2D( subdiv );

	//icvSet( dst, 255 );
	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D *edge = (CvQuadEdge2D *) cvGetSetElem( subdiv->edges, i );

		if( edge && CV_IS_SET_ELEM( edge ))
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge) edge;

			// left
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 1 ), drawLine);
			// right
			draw_facet( subdiv, dst, src, cvSubdiv2DRotateEdge( e, 3 ), drawLine);
		}
	}
}


void on_default(int )
{

}


int BrightnessAdjust(const IplImage* srcImg,
					 IplImage* dstImg,
					 float brightness)
{
	assert(srcImg != NULL);
	assert(dstImg != NULL);

	int h = srcImg->height;
	int w = srcImg->width;
	int n = srcImg->nChannels;

	int x,y,i;
	float val;
	for (i = 0; i < n; i++)//²ÊÉ«Í¼ÏñÐèÒª´¦Àí3¸öÍ¨µÀ£¬»Ò¶ÈÍ¼ÏñÕâÀï¿ÉÒÔÉ¾µô
	{
		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{

				val = ((uchar*)(srcImg->imageData + srcImg->widthStep*y))[x*n+i];
				val += brightness;
				//¶Ô»Ò¶ÈÖµµÄ¿ÉÄÜÒç³ö½øÐÐ´¦Àí
				if(val>255)	val=255;
				if(val<0) val=0;
				((uchar*)(dstImg->imageData + dstImg->widthStep*y))[x*n+i] = (uchar)val;
			}
		}
	}

	return 0;
}

int ContrastAdjust(const IplImage* srcImg,
				   IplImage* dstImg,
				   float nPercent)
{
	assert(srcImg != NULL);
	assert(dstImg != NULL);

	int h = srcImg->height;
	int w = srcImg->width;
	int n = srcImg->nChannels;

	int x,y,i;
	float val;
	for (i = 0; i < n; i++)//²ÊÉ«Í¼ÏñÐèÒª´¦Àí3¸öÍ¨µÀ£¬»Ò¶ÈÍ¼ÏñÕâÀï¿ÉÒÔÉ¾µô
	{
		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{

				val = ((uchar*)(srcImg->imageData + srcImg->widthStep*y))[x*n+i];
				val = 128 + (val - 128) * nPercent;
				//¶Ô»Ò¶ÈÖµµÄ¿ÉÄÜÒç³ö½øÐÐ´¦Àí
				if(val>255) val=255;
				if(val<0) val=0;
				((uchar*)(dstImg->imageData + dstImg->widthStep*y))[x*n+i] = (uchar)val;
			}
		}
	}
	return 0;
}

// Create a HSV image from the RGB image using the full 8-bits, since OpenCV only allows Hues up to 180 instead of 255.
// ref: "http://cs.haifa.ac.il/hagit/courses/ist/Lectures/Demos/ColorApplet2/t_convert.html"
// Remember to free the generated HSV image.
void convertRGBtoHSV(const IplImage *imageRGB, IplImage *imageHSV)
{
	float fR, fG, fB;
	float fH, fS, fV;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	// Create a blank HSV image
	//IplImage *imageHSV = cvCreateImage(cvGetSize(imageRGB), 8, 3);
	//if (!imageHSV || imageRGB->depth != 8 || imageRGB->nChannels != 3) {
	//	printf("ERROR in convertImageRGBtoHSV()! Bad input image.\n");
	//	exit(1);
	//}

	int h = imageRGB->height;		// Pixel height.
	int w = imageRGB->width;		// Pixel width.
	int rowSizeRGB = imageRGB->widthStep;	// Size of row in bytes, including extra padding.
	char *imRGB = imageRGB->imageData;	// Pointer to the start of the image pixels.
	int rowSizeHSV = imageHSV->widthStep;	// Size of row in bytes, including extra padding.
	char *imHSV = imageHSV->imageData;	// Pointer to the start of the image pixels.
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			int bB = *(uchar*)(pRGB+0);	// Blue component
			int bG = *(uchar*)(pRGB+1);	// Green component
			int bR = *(uchar*)(pRGB+2);	// Red component

			// Convert from 8-bit integers to floats.
			fR = bR * BYTE_TO_FLOAT;
			fG = bG * BYTE_TO_FLOAT;
			fB = bB * BYTE_TO_FLOAT;

			// Convert from RGB to HSV, using float ranges 0.0 to 1.0.
			float fDelta;
			float fMin, fMax;
			int iMax;
			// Get the min and max, but use integer comparisons for slight speedup.
			if (bB < bG) {
				if (bB < bR) {
					fMin = fB;
					if (bR > bG) {
						iMax = bR;
						fMax = fR;
					}
					else {
						iMax = bG;
						fMax = fG;
					}
				}
				else {
					fMin = fR;
					fMax = fG;
					iMax = bG;
				}
			}
			else {
				if (bG < bR) {
					fMin = fG;
					if (bB > bR) {
						fMax = fB;
						iMax = bB;
					}
					else {
						fMax = fR;
						iMax = bR;
					}
				}
				else {
					fMin = fR;
					fMax = fB;
					iMax = bB;
				}
			}
			fDelta = fMax - fMin;
			fV = fMax;				// Value (Brightness).
			if (iMax != 0) {			// Make sure its not pure black.
				fS = fDelta / fMax;		// Saturation.
				float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
				if (iMax == bR) {		// between yellow and magenta.
					fH = (fG - fB) * ANGLE_TO_UNIT;
				}
				else if (iMax == bG) {		// between cyan and yellow.
					fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
				}
				else {				// between magenta and cyan.
					fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
				}
				// Wrap outlier Hues around the circle.
				if (fH < 0.0f)
					fH += 1.0f;
				if (fH >= 1.0f)
					fH -= 1.0f;
			}
			else {
				// color is pure Black.
				fS = 0;
				fH = 0;	// undefined hue
			}

			// Convert from floats to 8-bit integers.
			int bH = (int)(0.5f + fH * 255.0f);
			int bS = (int)(0.5f + fS * 255.0f);
			int bV = (int)(0.5f + fV * 255.0f);

			// Clip the values to make sure it fits within the 8bits.
			if (bH > 255)
				bH = 255;
			if (bH < 0)
				bH = 0;
			if (bS > 255)
				bS = 255;
			if (bS < 0)
				bS = 0;
			if (bV > 255)
				bV = 255;
			if (bV < 0)
				bV = 0;

			// Set the HSV pixel components.
			uchar *pHSV = (uchar*)(imHSV + y*rowSizeHSV + x*3);
			*(pHSV+0) = bH;		// H component
			*(pHSV+1) = bS;		// S component
			*(pHSV+2) = bV;		// V component
		}
	}
}


// Create an RGB image from the HSV image using the full 8-bits, since OpenCV only allows Hues up to 180 instead of 255.
// ref: "http://cs.haifa.ac.il/hagit/courses/ist/Lectures/Demos/ColorApplet2/t_convert.html"
// Remember to free the generated RGB image.
void convertHSVtoRGB(const IplImage *imageHSV, IplImage *imageRGB)
{
	float fH, fS, fV;
	float fR, fG, fB;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	// Create a blank RGB image
	//IplImage *imageRGB = cvCreateImage(cvGetSize(imageHSV), 8, 3);
	//if (!imageRGB || imageHSV->depth != 8 || imageHSV->nChannels != 3) {
	//	printf("ERROR in convertImageHSVtoRGB()! Bad input image.\n");
	//	exit(1);
	//}

	int h = imageHSV->height;			// Pixel height.
	int w = imageHSV->width;			// Pixel width.
	int rowSizeHSV = imageHSV->widthStep;		// Size of row in bytes, including extra padding.
	char *imHSV = imageHSV->imageData;		// Pointer to the start of the image pixels.
	int rowSizeRGB = imageRGB->widthStep;		// Size of row in bytes, including extra padding.
	char *imRGB = imageRGB->imageData;		// Pointer to the start of the image pixels.
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the HSV pixel components
			uchar *pHSV = (uchar*)(imHSV + y*rowSizeHSV + x*3);
			int bH = *(uchar*)(pHSV+0);	// H component
			int bS = *(uchar*)(pHSV+1);	// S component
			int bV = *(uchar*)(pHSV+2);	// V component

			// Convert from 8-bit integers to floats
			fH = (float)bH * BYTE_TO_FLOAT;
			fS = (float)bS * BYTE_TO_FLOAT;
			fV = (float)bV * BYTE_TO_FLOAT;

			// Convert from HSV to RGB, using float ranges 0.0 to 1.0
			int iI;
			float fI, fF, p, q, t;

			if( bS == 0 ) {
				// achromatic (grey)
				fR = fG = fB = fV;
			}
			else {
				// If Hue == 1.0, then wrap it around the circle to 0.0
				if (fH >= 1.0f)
					fH = 0.0f;

				fH *= 6.0;			// sector 0 to 5
				fI = floor( fH );		// integer part of h (0,1,2,3,4,5 or 6)
				iI = (int) fH;			//		"		"		"		"
				fF = fH - fI;			// factorial part of h (0 to 1)

				p = fV * ( 1.0f - fS );
				q = fV * ( 1.0f - fS * fF );
				t = fV * ( 1.0f - fS * ( 1.0f - fF ) );

				switch( iI ) {
					case 0:
						fR = fV;
						fG = t;
						fB = p;
						break;
					case 1:
						fR = q;
						fG = fV;
						fB = p;
						break;
					case 2:
						fR = p;
						fG = fV;
						fB = t;
						break;
					case 3:
						fR = p;
						fG = q;
						fB = fV;
						break;
					case 4:
						fR = t;
						fG = p;
						fB = fV;
						break;
					default:		// case 5 (or 6):
						fR = fV;
						fG = p;
						fB = q;
						break;
				}
			}

			// Convert from floats to 8-bit integers
			int bR = (int)(fR * FLOAT_TO_BYTE);
			int bG = (int)(fG * FLOAT_TO_BYTE);
			int bB = (int)(fB * FLOAT_TO_BYTE);

			// Clip the values to make sure it fits within the 8bits.
			if (bR > 255)
				bR = 255;
			if (bR < 0)
				bR = 0;
			if (bG > 255)
				bG = 255;
			if (bG < 0)
				bG = 0;
			if (bB > 255)
				bB = 255;
			if (bB < 0)
				bB = 0;

			// Set the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			*(pRGB+0) = bB;		// B component
			*(pRGB+1) = bG;		// G component
			*(pRGB+2) = bR;		// R component
		}
	}
}

void cvSkinSegment(IplImage* img, IplImage* mask)
{
	CvSize imageSize = cvSize(img->width, img->height);
	IplImage *imgY = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
	IplImage *imgCr = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
	IplImage *imgCb = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);


	IplImage *imgYCrCb = cvCreateImage(imageSize, img->depth, img->nChannels);
	cvCvtColor(img,imgYCrCb,CV_BGR2YCrCb);
	cvSplit(imgYCrCb, imgY, imgCr, imgCb, 0);
	int y, cr, cb, l, x1, y1, value;
	unsigned char *pY, *pCr, *pCb, *pMask;

	pY = (unsigned char *)imgY->imageData;
	pCr = (unsigned char *)imgCr->imageData;
	pCb = (unsigned char *)imgCb->imageData;
	pMask = (unsigned char *)mask->imageData;
	cvSetZero(mask);
	l = img->height * img->width;
	for (int i = 0; i < l; i++){
		y  = *pY;
		cr = *pCr;
		cb = *pCb;
		cb -= 109;
		cr -= 152
			;
		x1 = (819*cr-614*cb)/32 + 51;
		y1 = (819*cr+614*cb)/32 + 77;
		x1 = x1*41/1024;
		y1 = y1*73/1024;
		value = x1*x1+y1*y1;
		if(y<100)    (*pMask)=(value<700) ? 255:0;
		else        (*pMask)=(value<850)? 255:0;
		pY++;
		pCr++;
		pCb++;
		pMask++;
	}
	cvReleaseImage(&imgY);
	cvReleaseImage(&imgCr);
	cvReleaseImage(&imgCb);
	cvReleaseImage(&imgYCrCb);
}


void vFillPoly(IplImage* img, const vector<Point>& pt_list, const Scalar& clr/* = Scalar(255,255,255)*/)
{
	const Point* pts = &pt_list[0];
	const int npts = pt_list.size();
	Mat mat(img);
	cv::fillPoly(mat, &pts, &npts, 1, clr);
}

void vLinePoly(IplImage* img, const vector<Point>& pt_list, const Scalar& clr/* = Scalar(255,255,255)*/, int thick/* = 1*/)
{
	const Point* pts = &pt_list[0];
	const int npts = pt_list.size();
	Mat mat(img);
	cv::polylines(mat, &pts, &npts, 1, true, clr, thick);
}

void vLinePoly(IplImage* img, const vector<Point2f>& pt_list, const Scalar& clr/* = Scalar(255,255,255)*/, int thick/* = 1*/)
{
	const int npts = pt_list.size();
	Point* pts = new Point[npts];
	for (int i=0;i<npts;i++)
		pts[i] = pt_list[i];

    Mat mat(img);
	cv::polylines(mat, (const Point**)&pts, &npts, 1, true, clr, thick);

	delete[] pts;
}

bool vTestRectHitRect(const Rect& object1, const Rect& object2)
{
	int left1, left2;
	int right1, right2;
	int top1, top2;
	int bottom1, bottom2;

	left1 = object1.x;
	left2 = object2.x;
	right1 = object1.x + object1.width;
	right2 = object2.x + object2.width;
	top1 = object1.y;
	top2 = object2.y;
	bottom1 = object1.y + object1.height;
	bottom2 = object2.y + object2.height;

	if (bottom1 < top2) return false;
	if (top1 > bottom2) return false;

	if (right1 < left2) return false;
	if (left1 > right2) return false;

	return true;
};
