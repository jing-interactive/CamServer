/*
* vBlobTracker.h
* by stefanix
* Thanks to touchlib for the best fit algorithm!
*
* This class tracks blobs between frames.
* Most importantly it assignes persistent id to new blobs, correlates
* them between frames and removes them as blobs dissappear. It also
* compensates for ghost frames in which blobs momentarily dissappear.
*
* Based on the trackning it fires events when blobs come into existence,
* move around, and disappear. The object which receives the callbacks
* can be specified with setListener().
*
*/
#pragma once

#include <map>
#include <vector>

#include "Blob.h"

struct vBlobListener 
{
	virtual void blobOn( int x, int y, int id, int order ) = 0;
	virtual void blobMoved( int x, int y, int id, int order ) = 0;
	virtual void blobOff( int x, int y, int id, int order ) = 0;
};



///////////////////////////////////////////////////////////////////////////////////////////

// This cleans up the forground segmentation mask derived from calls to cvBackCodeBookDiff
//
// mask			Is a grayscale (8 bit depth) "raw" mask image which will be cleaned up
//
// OPTIONAL PARAMETERS:
// poly1_hull0	If set, approximate connected component by (DEFAULT) polygon, or else convex hull (false)
// areaScale 	Area = image (width*height)*areaScale.  If contour area < this, delete that contour (DEFAULT: 0.1)
//

void vFindBlobs(IplImage *src, vector<vBlob>& blobs, int minArea, int maxArea, bool convexHull=true, bool (*sort_func)(const vBlob& a, const vBlob& b)  = NULL);

void vFindBlobs(IplImage *mask, 
				int minArea = 1, int maxArea = 3072000, bool convexHull=true);//draw trackedBlobs only

// parameters:
//  silh - input video frame
//  dst - resultant motion picture
//  args - optional parameters
vector<vBlob>  vUpdateMhi( IplImage* silh, IplImage* dst);

struct vBlobTracker 
{
	vector<vTrackedBlob>  trackedBlobs;
	vector<vTrackedBlob>  leaveBlobs;

	vBlobTracker();
	void setListener( vBlobListener* _listener );
	void trackBlobs( const vector<vBlob>& newBlobs );
	int findOrder( int id );  // order by which the present
	// blobs came into existence
	vTrackedBlob& getById( int id );  // returns a reference to the
	// corresponding blob in blobs vector

protected:

	int currentID;
	int extraIDs;
	int numcheck;

	vBlobListener* listener;

	int reject_distance_threshold;
	int ghost_frames;
	float minimumDisplacementThreshold;

	vector<vector<int> > matrix;
	vector<int> ids;
	vector<vector<vTrackedBlob> > history;

	vector<vTrackedBlob>  prevBlobs;

	void doBlobOn( vTrackedBlob& b );
	void doBlobMoved( vTrackedBlob& b );
	void doBlobOff( vTrackedBlob& b );

	inline void permute( int k );
	inline bool checkValid( int start );
	inline bool checkValidNew( int start );
};



class vBlobTracker2 
{
public: 
	enum{
		KNN = 3,
	};

	vBlobTracker2();

	//setup a event listener
	void setListener( vBlobListener* _listener );

	//assigns IDs to each blob in the contourFinder
	void trackBlobs(const vector<vBlob>& newBlobs);

	std::vector<vTrackedBlob>	trackedBlobs; //tracked blobs
	std::vector<vTrackedBlob>  leaveBlobs;

private:
	int trackKnn(const vector<vTrackedBlob>& newBlobs, vTrackedBlob& track, int k, double thresh=0);
	int						IDCounter;	  //counter of last blob

protected:

	vBlobListener* listener;

	//blob Events
	void doBlobOn(vTrackedBlob& b );
	void doBlobMoved(vTrackedBlob& b );    
	void doBlobOff(vTrackedBlob& b );

};

struct vFingerDetector
{		
	vFingerDetector();
	
	bool findFingers(const vBlob& blob, int k = 10);
	bool findHands(const vBlob& smblob, int k = 200);
	
	float dlh,max;
	
	int handspos[2];
	
	vector<cv::Point2f>		ppico;
	vector<cv::Point2f>		smppico;
	
	vector<float>				kpointcurv;
	vector<float>				smkpointcurv;
	
	vector<bool>				bfingerRuns;
	
	vector<cv::Point2f>		lhand;
	vector<cv::Point2f>		rhand;
	
//	cv::Vec2f	v1,v2,aux1;
	 
	cv::Vec3f	v1D,vxv;
	cv::Vec3f	v2D;
	 	
	 float teta,lhd;
};



struct vHaarFinder
{
	vector<vBlob> blobs;
	float scale;
	//
	bool init(char* cascade_name);
	void find(IN IplImage* img, int minArea = 1, bool findAllFaces = true);

	vHaarFinder();
	~vHaarFinder();

protected:
	CvHaarClassifierCascade* cascade;
	CvMemStorage* storage;
};

struct vOpticalFlowLK
{
	//blocksize must be odd
        vOpticalFlowLK(IplImage* gray, int blocksize = 5); 
                
		void update(IplImage* gray);
        
		cv::point2df flowAtPoint(int x, int y);
		bool flowInRegion(int x, int y, int w, int h, cv::point2df& vec) ;
        
        //Used to filter noisey or erroneous vectors
        float minVector;
        float maxVector;

        int width;
        int height;

        Ptr<IplImage> vel_x;
        Ptr<IplImage> vel_y;
		Ptr<IplImage> prev;

		int block_size;
};
