/******************************************************************/
/**
 * @file	testApp.cpp
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);

	kinect.init(true, true, false, false, true, true);
	kinect.open(true);

	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	
	ofSetVerticalSync(true);

	kinectSource = &kinect;
	kinect.setAngle( 0 );
	angle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

	ofSetFrameRate(60);
	t = LONG_MAX;
}

//--------------------------------------------------------------
void testApp::update() {
	kinectSource->update();

	if(ofGetElapsedTimeMillis() > t)
	{
		ofPoint** skeletons = kinect.getSkeletonPoints();

		for(int i=0; i<kinect::nui::SkeletonFrame::SKELETON_COUNT; i++)
		{
			ofPoint top = skeletons[i][NUI_SKELETON_POSITION_HEAD] * 2.0;
			ofPoint bottom = skeletons[i][NUI_SKELETON_POSITION_SHOULDER_CENTER] * 2.0;

			if(top.x != -2 && bottom.x != -2)
			{	
				float size = (bottom.y - top.y);
				r.x = top.x - size;
				r.width = size * 2;

				r.y = top.y;
				r.height = size * 2;

				headshot.setFromPixels( kinect.getVideoPixels() );
				headshot.crop( r.x, r.y, r.width, r.height);
				headshot.resize( 200, 200);
			} 
			else 
			{
				cout << i << ": no skeleton found" << endl;
			}
		}
		t = LONG_MAX;
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(100, 100, 100);

	ofSetColor(255);
	kinect.draw(0, 0);
	kinect.drawSkeleton(0, 0);

	ofSetColor(255, 0, 0);
	ofNoFill();
	ofRect(r.x, r.y, r.width, r.height);

	ofSetColor(255);
	headshot.draw(660, 20);

	if(t != LONG_MAX)
	{
		ofSetColor(255);
		ofDrawBitmapString(ofToString(t - ofGetElapsedTimeMillis()), ofGetWidth()-100, 20);
	}
}


//--------------------------------------------------------------
void testApp::exit() {

	kinect.setAngle(0);
	kinect.close();
	kinect.removeKinectListener(this);
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch(key){
	case ' ':
		t = ofGetElapsedTimeMillis() + 5000;
		break;

	case 'o': // open stream
	case 'O':
		kinect.open();
		break;
	case 'c': // close stream
	case 'C':
		kinect.close();
		break;

	case OF_KEY_UP: // up the kinect angle
		angle++;
		if(angle > 27){
			angle = 27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_DOWN: // down the kinect angle
		angle--;
		if(angle < -27){
			angle = -27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_LEFT: // increase the far clipping distance
		if(farClipping > nearClipping + 10){
			farClipping -= 10;
			kinectSource->setFarClippingDistance(farClipping);
		}
		break;
	case OF_KEY_RIGHT: // decrease the far clipping distance
		if(farClipping < 4000){
			farClipping += 10;
			kinectSource->setFarClippingDistance(farClipping);
		}
		break;
	case '+': // increase the near clipping distance
		if(nearClipping < farClipping - 10){
			nearClipping += 10;
			kinectSource->setNearClippingDistance(nearClipping);
		}
		break;
	case '-': // decrease the near clipping distance
		if(nearClipping >= 10){
			nearClipping -= 10;
			kinectSource->setNearClippingDistance(nearClipping);
		}
		break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::kinectPlugged(){
	bPlugged = true;
}

//--------------------------------------------------------------
void testApp::kinectUnplugged(){
	bPlugged = false;
}
