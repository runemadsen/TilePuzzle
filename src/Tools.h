#pragma once

#include "ofMain.h"

class Color
{
public:
	
	float hue, sat, bri;
	ofxVec2f pos;
};

class SortObject
{
public:
	
	int id;
	ofPoint centroid;
};