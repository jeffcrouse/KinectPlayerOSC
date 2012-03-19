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
#include "ofxSimpleGuiToo.h"


class Particle : public ofNode {
public:
    int bone[2];
    float pct;
    float inc;
    float angle;
    float orbitRadius;

    Particle() {
        angle = ofRandom(0, 360);
        inc = ofRandom(-3, 3);
        setOrientation(ofVec3f(ofRandom(0, 360), ofRandom(0, 360), ofRandom(0, 360)));
    }
    
    void update(ofPoint* joints)
    {
        ofPoint p1 = joints[ bone[0] ];
        ofPoint p2 = joints[ bone[1] ];
        
        ofPoint diff = p2 - p1;
        ofPoint center = p1 + (diff * pct);
        
        setPosition(center);
        lookAt(p2);
        tilt(90);
        pan(angle);
        dolly(orbitRadius);
        
        angle += inc;
    }
    
};

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

		bool bPlugged;
		bool bUnplugged;
		bool bDrawVideo;
		bool bDrawSkeleton;

		unsigned short nearClipping;
		unsigned short farClipping;
		int angle;

		vector<Particle> particles;
};
