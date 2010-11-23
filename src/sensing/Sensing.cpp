#include "Sensing.h"

/* Singleton stuff
 ___________________________________________________________ */

Sensing * Sensing::m_pInstance = NULL;

Sensing * Sensing::getInstance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new Sensing;
	
	return m_pInstance;
}

/* Constructor
 ___________________________________________________________ */

Sensing::Sensing()
{
	trackColor.hue = 35;
	trackColor.sat = 73;
	trackColor.bri = 59;
	threshold = 0;
	blurAmount = 0;
	area = 30;
	hueMarginLow = 19;
	hueMarginHigh = 60;
	satMarginLow = 67;
	satMarginHigh = 173;
	briMarginLow = 31;
	briMarginHigh = 99;
	cellWidth = 197;
	cellHeight = 198;
	cellMarginX = 0;
	cellMarginY = 0;
	ratioX = 2.5;
	ratioY = 2.51;
	displaceX = -3;
	displaceY = -15;
	gridWidth = 775.781;
	gridHeight = 771.68;
	gridX = 148.828;
	gridY = 12.1094;	
	
	show = false;
	showGrabScreen = false;
	maskToggle = true;
	debugToggle = false;
	
	colorImg.allocate( CAM_WIDTH, CAM_HEIGHT );
	colorImgHSV.allocate(CAM_WIDTH, CAM_HEIGHT);  
	
	hueImg.allocate(CAM_WIDTH, CAM_HEIGHT);
    satImg.allocate(CAM_WIDTH, CAM_HEIGHT);
    briImg.allocate(CAM_WIDTH, CAM_HEIGHT);
	
	grayPixels = new unsigned char [CAM_WIDTH * CAM_HEIGHT]; 
	
	grayImg.allocate( CAM_WIDTH, CAM_HEIGHT );
	outputTexture.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGB);
	
	mask.loadImage("mask.png");
	mask.setImageType(OF_IMAGE_GRAYSCALE);
	maskPixels = mask.getPixels();
	
    trackedTextureRed.allocate(CAM_WIDTH, CAM_HEIGHT, GL_LUMINANCE);
	
	gui.addTitle("Input");
	gui.addContent("Input", outputTexture);
	gui.addSlider("Threshold", threshold , 0.0, 255);
	gui.addSlider("Bluring", blurAmount , 0, 40);
	gui.addContent("Difference", grayImg);
	gui.addSlider("Area",area,1,300);	
	gui.addSlider("Hue Margin Low", hueMarginLow , 0, 200);
	gui.addSlider("Hue Margin High", hueMarginHigh , 0, 200);
	gui.addSlider("Sat Margin Low", satMarginLow , 0, 200);
	gui.addSlider("Sat Margin High", satMarginHigh , 0, 500);
	gui.addSlider("Bri Margin Low", briMarginLow , 0, 200);
	gui.addSlider("Bri Margin High", briMarginHigh , 0, 200);
	gui.addToggle("Mask", maskToggle);
	gui.addToggle("Debug", debugToggle);
	gui.addSlider("Cell Width", cellWidth , 50, 500);
	gui.addSlider("Cell Height", cellHeight , 50, 500);
	
	gui.addSlider("ratioX", ratioX , 0.5, 6.0);
	gui.addSlider("ratioY", ratioY , 0.5, 6.0);
	gui.addSlider("Displace Origin X", displaceX , -800, 800);
	gui.addSlider("Displace Origin Y", displaceY , -800, 800);
	gui.addSlider("Grid width", gridWidth , 300, 1000);
	gui.addSlider("Grid height", gridHeight, 300, 1000);
	gui.addSlider("Grid X", gridX , -400, 600);
	gui.addSlider("Grid Y", gridY, -400, 600);
	gui.show();
	
	for (int i=0; i<CAM_WIDTH*CAM_HEIGHT; i++) 
	{
		if (maskPixels[i]!=0){
			maskPixels[i]==1;			
		} 
	}
	
	vidGrabber.setDeviceID(6);
	vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT );
}

/* Set listener
 ___________________________________________________________ */

void Sensing::setListener(ofxCvBlobListener * listener)
{
	blobTracker.setListener(listener);
}

/* Update
 ___________________________________________________________ */

void Sensing::update()
{
	vidGrabber.grabFrame();
	
	if( vidGrabber.isFrameNew() ) 
	{
        colorImg = vidGrabber.getPixels();
        colorImg.mirror(false, false );
		
		colorImgHSV = colorImg;                                                 
		colorImgHSV.convertRgbToHsv();                                          
		colorImgHSV.convertToGrayscalePlanarImages(hueImg, satImg, briImg);
		
		// Because of bug
		hueImg.flagImageChanged(); 
		satImg.flagImageChanged();
		briImg.flagImageChanged();
        
		// grayImg should get the red image
		unsigned char * huePixels = hueImg.getPixels();                         
		unsigned char * satPixels = satImg.getPixels();  
		unsigned char * briPixels = briImg.getPixels();
		
		for (int i = 0; i < CAM_WIDTH * CAM_HEIGHT; i++)
		{                                          
			if ((huePixels[i] >= trackColor.hue - hueMarginLow && huePixels[i] <= trackColor.hue + hueMarginHigh) &&    
				(satPixels[i] >= trackColor.sat - satMarginLow && satPixels[i] <= trackColor.sat + satMarginHigh) &&
				(briPixels[i] >= trackColor.bri - briMarginLow && briPixels[i] <= trackColor.bri + briMarginHigh))
			{    
				grayPixels[i] = 255;                                      
			} 
			else 
			{
				grayPixels[i] = 0;                                        
			}
		}
		
		if (maskToggle) 
		{
			for (int i=0; i<CAM_WIDTH*CAM_HEIGHT; i++) 
			{
				grayPixels[i]=maskPixels[i]&&grayPixels[i]; 
			}
		}
		
		grayImg.setFromPixels(grayPixels, CAM_WIDTH,CAM_HEIGHT);
		
		grayImg.blur( blurAmount );
        grayImg.threshold( threshold );
        
		//findContures( img, minSize, maxSize, nMax, inner contours yes/no )
        contourFinder.findContours( grayImg, area, 300000, 20, false);
        blobTracker.trackBlobs( contourFinder.blobs );
    }
}

/* Draw
 ___________________________________________________________ */

void Sensing::draw()
{	
	if (show) 
	{
		if(showGrabScreen)
		{
			ofSetColor(255, 255, 255);
			colorImg.draw(0, 0);
		}
		else 
		{
			gui.draw();
			
			outputTexture.begin();
			ofSetColor( 0xffffff );
			colorImg.draw(0, 0);
			blobTracker.draw(0, 0);
			outputTexture.end();
		}
	}
}

/* Grab color fro CAM
 ___________________________________________________________ */

void Sensing::grabColorFromVideo(int x, int y)
{
	if(showGrabScreen)
	{
		unsigned char * huePixels = hueImg.getPixels();  
		unsigned char * satPixels = satImg.getPixels();
		unsigned char * briPixels = briImg.getPixels();
		
		x = MIN(x,hueImg.width-1);
		y = MIN(y,hueImg.height-1);
		
		trackColor.hue = huePixels[x+(y*hueImg.width)];  
		trackColor.sat = satPixels[x+(y*satImg.width)]; 
		trackColor.bri = briPixels[x+(y*satImg.width)];
	}
}

/* Getter / Setter
 ___________________________________________________________ */

vector <ofxCvTrackedBlob> Sensing::getBlobs()
{
	return blobTracker.blobs;
}

bool Sensing::debug()
{
	return debugToggle;
}

int Sensing::getCellWidth()
{
	return cellWidth;
}

int Sensing::getCellHeight()
{
	return cellHeight;
}

void Sensing::calibrateVar(string variable, float addNum)
{
	
	if(variable == "x")				displaceX += addNum;
	else if(variable == "y")		displaceY += addNum;
	else if(variable == "ratiox")	ratioX += addNum;
	else if(variable == "ratioy")	ratioY += addNum;
	else if(variable == "cellwidth")	cellWidth += addNum;
	else if(variable == "cellheight")	cellHeight += addNum;
}

void Sensing::printConfig()
{
	cout << "trackColor.hue = " << trackColor.hue << ";" << endl;
	cout << "trackColor.sat = " << trackColor.sat << ";" << endl;
	cout << "trackColor.bri = " << trackColor.bri << ";" << endl;
	
	cout << "threshold = " << threshold << ";" << endl;
	cout << "blurAmount = " << blurAmount << ";" << endl;
	cout << "area = " << area << ";" << endl;
	cout << "hueMarginLow = " << hueMarginLow << ";" << endl;
	cout << "hueMarginHigh = " << hueMarginHigh << ";" << endl;
	cout << "satMarginLow = " << satMarginLow << ";" << endl;
	cout << "satMarginHigh = " << satMarginHigh << ";" << endl;
	cout << "briMarginLow = " << briMarginLow << ";" << endl;
	cout << "briMarginHigh = " << briMarginHigh << ";" << endl;

	
	cout << "cellWidth = " << cellWidth << ";" << endl;
	cout << "cellHeight = " << cellHeight << ";" << endl;
	cout << "cellMarginX = " << cellMarginX << ";" << endl;
	cout << "cellMarginY = " << cellMarginY << ";" << endl;
	
	cout << "ratioX = " << ratioX << ";" << endl;
	cout << "ratioY = " << ratioY << ";" << endl;
	cout << "displaceX = " << displaceX << ";" << endl;
	cout << "displaceY = " << displaceY << ";" << endl;
	
	cout << "gridWidth = " << gridWidth << ";" << endl;
	cout << "gridHeight = " << gridHeight << ";" << endl;
	cout << "gridX = " << gridX << ";" << endl;
	cout << "gridY = " << gridY << ";" << endl;
}

