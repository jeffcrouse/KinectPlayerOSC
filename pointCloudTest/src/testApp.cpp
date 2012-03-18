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

	kinect.init(true, true, false, true, false, true, true, true);
	kinect.open(false);

	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);

	ofSetVerticalSync(true);

	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();
	depthMultiplier = 0.1;

	bDrawVideo = false;

	labelImageCV.allocate( kinect.getDepthResolutionWidth(), kinect.getDepthResolutionHeight() );

	gui.addTitle("All Controls");
	gui.addToggle("Draw video", bDrawVideo);
	gui.addSlider("Angle", angle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 0, 1000);
	gui.addSlider("Far Clipping", farClipping, 200, 6000);
	gui.addSlider("Depth Multiplier", depthMultiplier, 0, 1);
	gui.addSlider("Dilate", dilate, 0, 5);
	ofSetFrameRate(60);
	
	gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update() {
	kinectSource->update();

	// Update the kinect values
	if(angle != kinect.getCurrentAngle()) {
		kinect.setAngle( angle );
	}
	if(nearClipping != kinect.getNearClippingDistance()) {
		kinect.setNearClippingDistance( nearClipping );
	}
	if(farClipping != kinect.getFarClippingDistance()) {
		kinect.setFarClippingDistance( farClipping );
	}

	labelImage.setFromPixels( kinect.getLabelPixels() );
	labelImage.setImageType( OF_IMAGE_GRAYSCALE );
	labelImageCV.setFromPixels( labelImage.getPixelsRef() );
	for(int i=0; i<dilate; i++)
		labelImageCV.erode();
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(255);


	if(bDrawVideo){
		kinect.draw(0, 0);	// draw video images from kinect camera
	}

	glEnable(GL_DEPTH);
	for(int y=0; y < kinect.getDepthResolutionHeight(); y+=4)
	{
		for(int x=0; x < kinect.getDepthResolutionWidth(); x+=4)
		{
			ofColor label  = kinect.getLabelPixels().getColor(x, y);
			if(label.r < 255)
			{
				int depth = kinect.getDistanceAt(x, y);
				int px = ofMap(x, 0, kinect.getDepthResolutionWidth(), 0, ofGetWidth());
				int py = ofMap(y, 0, kinect.getDepthResolutionHeight(), 0, ofGetHeight());
				int pz = ofMap(depth, nearClipping, farClipping, 300, -300);

				ofPushMatrix();
				ofTranslate(px, py, pz);
				ofColor color = kinect.getCalibratedColorAt(x, y);
				ofSetColor(color);
				ofRect(0, 0, 8, 8);
				ofPopMatrix();
			}
		}
	}
	glDisable(GL_DEPTH);

	gui.draw();

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << "fps: " << ofGetFrameRate() << "  Kinect Nearmode: " << kinect.isNearmode() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl;
	ofDrawBitmapString(reportStream.str(), 20, 648);
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
	case 'v': // draw video only
	case 'V':
		bDrawVideo = !bDrawVideo;
		if(bDrawVideo){
			glDisable(GL_DEPTH_TEST);
		}
		break;
	case 'g':
	case 'G':
		gui.toggleDraw();
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
