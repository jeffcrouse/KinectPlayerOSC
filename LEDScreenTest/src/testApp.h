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
#include "ofxKinectNuiPlayer.h"
#include "ofxKinectNuiRecorder.h"

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

		void startPlayback();
		void stopPlayback();
		void openPlaybackFile();

		ofxKinectNui kinect;
		ofxKinectNuiPlayer kinectPlayer;
		ofxBase3DVideo* kinectSource;

		bool bPlayback;
		bool bPlugged;
		bool bUnplugged;
		
		int nearClipping;
		int farClipping;

		string playbackFile;
		float zRange;
		float pixelSize;
		float rotateX;
		int kinectAngle;
		bool bCirclePixel;
};
