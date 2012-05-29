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
#include "ofxXmlSettings.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define CELL_SIZE 4
#define GRID_WIDTH (SCREEN_WIDTH)/(CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT)/(CELL_SIZE)
#define NUM_CELLS GRID_WIDTH*GRID_HEIGHT

class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void sendMessage();
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


		ofxXmlSettings oscSettings;
		ofxKinectNui kinect;
		int cells[NUM_CELLS];
		int updates[NUM_CELLS];
		ofFbo render;
		string messageAddress;
		stringstream message;

		ofxOscSender sender;

		bool bPlugged;
		bool bUnplugged;
		bool bRecording;
		ofFile outfile;

		int cameraAngle, newCameraAngle;
		float nearClipping;
		float farClipping;
		float brightness, contrast;
		ofPoint videoOffset;
		int changeThreshold; 
		int leftCrop, rightCrop;
		float lastSend;
		int erode, dilate, blur;

		ofImage labelImageRGBA;
		ofxCvColorImage labelImageRGB;
		ofxCvGrayscaleImage labelImageGray;
		ofxCvColorImage colorFrame;
		ofxCvGrayscaleImage grayFrame;
		ofPixels grayPixels;
};
