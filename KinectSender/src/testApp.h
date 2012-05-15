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

#include "ofxNetwork.h"
#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCV.h"
#include "ofxSimpleGuiToo.h"
#include "ofxOsc.h"
#ifdef _WIN32
#include <io.h>			// _setmode
#include <fcntl.h>		// _O_BINARY
#endif
#include "lz4.h"
#include "lz4hc.h"
#include "bench.h"



#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define CELL_SIZE 6
#define GRID_WIDTH (SCREEN_WIDTH)/(CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT)/(CELL_SIZE)
#define NUM_CELLS GRID_WIDTH*GRID_HEIGHT

#define USE_TCP

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

		ofxKinectNui kinect;
		int cells[NUM_CELLS];
		ofFbo render;
		stringstream message;

#ifdef USE_UDP
		ofxUDPManager udpConnection;
#endif
#ifdef USE_TCP
		ofxTCPServer TCP;
#endif
#ifdef USE_OSC
		ofxOscSender sender;
#endif
		bool bPlugged;
		bool bUnplugged;

		int kinectAngle;
		float nearClipping;
		float farClipping;
		float brightness, contrast;
		ofPoint videoOffset;
		int changeThreshold; 
		int leftCrop, rightCrop;
		float timeSinceLastSend;

		ofxCvColorImage colorFrame;
		ofxCvGrayscaleImage grayFrame;
		ofPixels grayPixels;
};
