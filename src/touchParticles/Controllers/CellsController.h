#pragma once

#include "ofMain.h"
#include "ofxVec2f.h"
#include "Constants.h"
#include "VideoCell.h"
#include "Tools.h"
#include "ofxCvTrackedBlob.h"
#include "Sensing.h"

class CellsController
{
	struct xsorter
	{
		bool operator()(SortObject a, SortObject b) const
		{
			if(a.centroid.x < b.centroid.x)
			{
				return true;
			}
			
			return false;
		}
	};
	
	struct ysorter
	{
		bool operator()(SortObject a, SortObject b) const
		{
			if(a.centroid.y < b.centroid.y)
			{
				return true;
			}
			
			return false;
		}
	};
	
public:
	
	CellsController();
	
	void update(vector <ofxCvTrackedBlob> blobs);
	void draw();

	void blobOn(ofxCvTrackedBlob& blob);
	void blobOff(int blobid);
	
	void assignBlobsToCells();
	
private:
	
	void findOrder();
	void setRandom();
	
	void updateRatio();
	
	vector <VideoCell *> _cells;
	
	float _ratioX;
	float _ratioY;
	
	ofVideoPlayer _mov;
	
	vector <int> delCellNums;
};