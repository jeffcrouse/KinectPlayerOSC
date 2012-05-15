#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define CELL_SIZE 2
#define GRID_WIDTH (SCREEN_WIDTH)/(CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT)/(CELL_SIZE)
#define NUM_CELLS GRID_WIDTH*GRID_HEIGHT


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

        ofxOscReceiver	receiver;

        int p1[NUM_CELLS];
		int p2[NUM_CELLS];
};

