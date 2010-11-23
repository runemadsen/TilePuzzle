#include "testApp.h"

void testApp::setup() 
{
	ofSetFrameRate(60);
	ofBackground( 0, 0, 0 );

	Sensing::getInstance()->setListener(this);
	
	cells = new CellsController();
}

void testApp::update() 
{	
	Sensing::getInstance()->update();
	
	cells->update(Sensing::getInstance()->blobTracker.blobs);
}

void testApp::draw() 
{	
	ofSetColor(255, 255, 255);
	
	cells->draw();
	
	// draw grid
	/*float gx = Sensing::getInstance()->getGridX();
	float gy = Sensing::getInstance()->getGridY();
	float gw = Sensing::getInstance()->getGridWidth();
	float gh = Sensing::getInstance()->getGridHeight();
	float mul = 0.04;
	
	
	//ofSetColor(255, 255, 255);
	ofFill();
	//ofRect(gx, gy, gw, gh);
	
	ofSetColor(150, 150, 150);
	ofRect(gx, gy, gw * mul, gh);
	ofRect(gx + (gw / 3), gy, gw * mul, gh);
	ofRect(gx + 2 * (gw / 3), gy, gw * mul, gh);
	
	ofRect(gx, gy, gw, gh * mul);
	ofRect(gx, gy + (gw / 3), gw, gh * mul);
	ofRect(gx, gy  + 2 * (gw / 3), gw, gh * mul);
	
	// end grid*/
	
	
	Sensing::getInstance()->draw();
	
	if(Sensing::getInstance()->debug())
	{
		string fpsStr = "frame rate: "+ofToString(ofGetFrameRate(), 2);
		
		ofSetColor(255, 255, 255);
		
		ofDrawBitmapString(fpsStr, 10, 10);
	}
}

void testApp::keyPressed( int key ) 
{
	if(key == 'x')		Sensing::getInstance()->calibrateVar("x", -1);
	else if(key == 'X')		Sensing::getInstance()->calibrateVar("x", 1);
	else if(key == 'y')		Sensing::getInstance()->calibrateVar("y", -1);
	else if(key == 'Y')		Sensing::getInstance()->calibrateVar("y", 1);
	else if(key == 'w')		Sensing::getInstance()->calibrateVar("ratiox", -0.01);
	else if(key == 'W')		Sensing::getInstance()->calibrateVar("ratiox", 0.01);
	else if(key == 'h')		Sensing::getInstance()->calibrateVar("ratioy", -0.01);
	else if(key == 'H')		Sensing::getInstance()->calibrateVar("ratioy", 0.01);
	else if(key == 'c')		Sensing::getInstance()->calibrateVar("cellwidth", -1);
	else if(key == 'C')		Sensing::getInstance()->calibrateVar("cellwidth", 1);
	else if(key == 'v')		Sensing::getInstance()->calibrateVar("cellheight", -1);
	else if(key == 'V')		Sensing::getInstance()->calibrateVar("cellheight", 1);
    else if (key =='f' || key=='F') 
	{
		ofToggleFullscreen();
	} 
	else if (key == 'a')
	{
		cells->assignBlobsToCells();
	}
	else if (key == 't')
	{
		Sensing::getInstance()->showGrabScreen = !Sensing::getInstance()->showGrabScreen;
	}
	else if (key ==' ')
	{
		Sensing::getInstance()->show =! Sensing::getInstance()->show;
	}
	else if(key == 'p')
	{
		Sensing::getInstance()->printConfig();
	}
}

void testApp::mouseMoved( int x, int y ) {}
void testApp::mouseDragged( int x, int y, int button ) {}
void testApp::mousePressed( int x, int y, int button ) 
{
	Sensing::getInstance()->grabColorFromVideo(x, y);
}

void testApp::mouseReleased() {}


void testApp::blobOn( int x, int y, int id, int order ) 
{		
	cells->blobOn(Sensing::getInstance()->blobTracker.getById(id));	
}

void testApp::blobMoved( int x, int y, int id, int order)
{
	
}

void testApp::blobOff( int x, int y, int id, int order ) 
{	
	cells->blobOff(id);
}



