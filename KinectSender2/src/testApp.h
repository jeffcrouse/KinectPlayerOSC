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
//#include "Winsock2.h"
#include "ofxNetwork.h"


#define GRID_WIDTH 256
#define GRID_HEIGHT 256
#define GRID_CELL_SIZE 5
#define NUM_CELLS (GRID_WIDTH*GRID_HEIGHT)/(GRID_CELL_SIZE)


class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void sendUDPMessage();
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
		int cells[NUM_CELLS];
		ofFbo render;
		ofxUDPManager udpConnection;
		stringstream message;

		bool bPlugged;
		bool bUnplugged;

		int kinectAngle;
		float nearClipping;
		float farClipping;
		float brightness, contrast;
		ofPoint videoOffset;

		ofxCvColorImage colorFrame;
		ofxCvGrayscaleImage grayFrame;
		ofPixels grayPixels;
};
