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
	int skeleton;
    int jointIndex1;
	int jointIndex2;
    float pct;
    float inc;
    float angle;
    float orbitRadius;

    Particle() {
        angle = ofRandom(0, 360);
        inc = ofRandom(-3, 3);
        setOrientation(ofVec3f(ofRandom(0, 360), ofRandom(0, 360), ofRandom(0, 360)));
    }
    
    void update(ofPoint** skeletons)
    {
        ofPoint p1 = skeletons[skeleton][ jointIndex1 ];
        ofPoint p2 = skeletons[skeleton][ jointIndex2 ];
        
        ofPoint diff = p2 - p1;
        ofPoint center = p1 + (diff * pct);
        center.z = 0;
		center *= 2;
		//if(center.x != -1)
			//cout << "center= " << center << endl;

        setPosition(center);
        //lookAt(p2);
        //tilt(90);
        //pan(angle);
        //dolly(orbitRadius);
        
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
