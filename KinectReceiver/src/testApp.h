#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxNetwork.h"
#include "lz4.h"
#include "lz4hc.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define CELL_SIZE 6
#define GRID_WIDTH (SCREEN_WIDTH)/(CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT)/(CELL_SIZE)
#define NUM_CELLS GRID_WIDTH*GRID_HEIGHT

#define USE_TCP

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

#ifdef USE_UDP
		ofxUDPManager udpConnection;
#endif
#ifdef USE_TCP
        ofxTCPClient tcpClient;
#endif
#ifdef USE_OSC
        ofxOscReceiver	receiver;
#endif
    
		int p1[NUM_CELLS];
		int p2[NUM_CELLS];
};

