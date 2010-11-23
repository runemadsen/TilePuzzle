#pragma once

#include "ofMain.h"
#include "Constants.h"
#include "ofxCvTrackedBlob.h"
#include "ofQuad.h"
#include "Sensing.h"

#define MOVE_MARGIN 0.5

class VideoCell
{
public:
	
	VideoCell(ofxCvTrackedBlob & model);
	
	void update(ofxCvTrackedBlob & model);
	void draw();
	void drawDebug();
	
	int getId();
	void setCellNum(int order);
	int getCellNum() { return _cellNum; }
	
	ofPoint getCentroid();
	
private:
	
	int _id;
	int _cellNum;
	ofColor _color;
	
	float _texCoords[8];
	
	// blob
	ofRectangle _boundingRect;
	ofPoint	_centroid;
	
	void setTextureRect(float x, float y, int w, int h);
};