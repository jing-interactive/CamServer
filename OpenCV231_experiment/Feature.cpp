#if defined _DEBUG
#pragma comment(lib,"opencv_core231d.lib")
#pragma comment(lib,"opencv_imgproc231d.lib")
#pragma comment(lib,"opencv_highgui231d.lib")
//#pragma comment(lib,"opencv_objdetect231d.lib")
#pragma comment(lib,"opencv_features2d231d.lib")
#pragma comment(lib,"opencv_flann231d.lib")
#else
#pragma comment(lib,"opencv_core231.lib")
#pragma comment(lib,"opencv_imgproc231.lib")
#pragma comment(lib,"opencv_highgui231.lib")
//#pragma comment(lib,"opencv_objdetect231.lib")
#pragma comment(lib,"opencv_features2d231.lib")
#pragma comment(lib,"opencv_flann231.lib")
#endif

#include <opencv2/opencv.hpp>

using namespace cv;

void help()
{
	printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
		"Using the SURF desriptor:\n"
		"\n"
		"Usage:\n matcher_simple <image1> <image2>\n");
}

int min_dist = 1;
int max_dist = 10;
bool new_frame = true;

void onTracker(int pos, void* userdata)
{
	new_frame = true;
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		help();
		return -1;
	}

	Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	if(img1.empty() || img2.empty())
	{
		printf("Can't read one of the images\n");
		return -1;
	}

	namedWindow("feature");
	createTrackbar("min_dist", "feature", &min_dist, 50, onTracker);
	createTrackbar("max_dist", "feature", &max_dist, 200, onTracker);

	equalizeHist(img1,img1);
	equalizeHist(img2,img2);

	// detecting keypoints
	Ptr<FeatureDetector> detector = new SurfFeatureDetector;
	vector<KeyPoint> keypoints1, keypoints2;
	detector->detect(img1, keypoints1);
	detector->detect(img2, keypoints2);

	// computing descriptors
	Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor;
	Mat descriptors1, descriptors2;
	extractor->compute(img1, keypoints1, descriptors1);
	extractor->compute(img2, keypoints2, descriptors2);

	// matching descriptors
	Ptr<DescriptorMatcher> matcher;
	vector<DMatch> matches;
#if 0
	matcher = new BruteForceMatcher<L2<float> >;	
	matcher->match(descriptors1, descriptors2, matches);
#else
	matcher = new FlannBasedMatcher;
	matcher->match( descriptors1, descriptors2, matches );
#endif
#if 0
	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors1.rows; i++ )
	{ 
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist );
	printf("-- Min dist : %f \n", min_dist );
#endif

	Mat img_matches;
	std::vector< DMatch > good_matches;

	while (true)
	{
		if (new_frame)
		{
			//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
			//-- PS.- radiusMatch can also be used here.
			good_matches.clear();

			for( int i = 0; i < descriptors1.rows; i++ )
			{ 
				float dist = matches[i].distance;
				if( dist > min_dist*0.01 && dist < max_dist*0.01)
				{
					good_matches.push_back( matches[i]);
				}
			}   
			drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), 
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
			imshow("feature", img_matches);
			new_frame = false;
		}
		int key = waitKey(1);
		if (key == 0x1B)
			break;
	}

	return 0;
}
