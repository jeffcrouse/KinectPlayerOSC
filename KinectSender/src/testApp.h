/******************************************************************/
/**
 * @file	testApp.h
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCV.h"
#include "ofxSimpleGuiToo.h"
#include "ofxOsc.h"
//#include "Winsock2.h"


class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void kinectPlugged();
		void kinectUnplugged();

		ofxKinectNui kinect;
		ofxBase3DVideo* kinectSource;
		ofxOscSender sender;

		bool bPlugged;
		bool bUnplugged;

		ofPoint renderPos;
		float pixelSize;
		int pixelSpacing;
		int kinectAngle;
		float nearClipping;
		float farClipping;
		float brightness, contrast;
		bool bModulatePixelSize;
		float pctFilled;
		ofPoint videoOffset;
		ofFbo render;
		bool bDragging;

		//ofImage rgbaLabelImage;
		//ofxCvColorImage labelImage;

		ofxCvColorImage colorFrame;
		ofxCvGrayscaleImage grayFrame;
		ofPixels grayPixels;

};
