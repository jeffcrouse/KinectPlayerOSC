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
	kinectAngle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

	brightnessBoost = 0;
	bCirclePixel = true;

	labelImageCV.allocate( kinect.getDepthResolutionWidth(), kinect.getDepthResolutionHeight() );

	gui.addTitle("All Controls");
	gui.addSlider("Angle", kinectAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 0, 200);
	gui.addSlider("Far Clipping", farClipping, 200, 4000);
	gui.addSlider("Z range", zRange, 0, 3000);
	gui.addSlider("Pixel Size", pixelSize, 3, 40);
	gui.addSlider("Rotate X", rotateX, -90, 90);
	gui.addSlider("Brightness Boost", brightnessBoost, 0, 100);
	gui.addToggle("Circle Pixel", bCirclePixel);

	gui.addTitle("Previews").setNewColumn(true);
	gui.addContent("Video", kinect.getVideoTextureReference());
	gui.addContent("Labels", kinect.getLabelTextureReference());

	ofSetFrameRate(60);

	gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update() {
	kinect.update();

	// Update the kinect values
	if(kinectAngle != kinect.getCurrentAngle()) {
		kinect.setAngle( kinectAngle );
	}
	if(nearClipping != kinect.getNearClippingDistance()) {
		kinect.setNearClippingDistance( nearClipping );
	}
	if(farClipping != kinect.getFarClippingDistance()) {
		kinect.setFarClippingDistance( farClipping );
	}

	/*
	labelImage.setFromPixels( kinectSource->getLabelPixels() );
	labelImage.setImageType( OF_IMAGE_GRAYSCALE );
	labelImageCV.setFromPixels( labelImage.getPixelsRef() );
	for(int i=0; i<dilate; i++)
		labelImageCV.erode();
	*/
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(255);

	ofSetColor(255,255,255);
	
	ofPushMatrix();
	ofTranslate(0, -ofGetHeight()/2.0);
	ofRotateX(rotateX);
	ofTranslate(0, ofGetHeight()/2.0);
	glEnable(GL_DEPTH);
	for(int y=0; y < kinect.getDepthResolutionHeight(); y+=4)
	{
		for(int x=0; x < kinect.getDepthResolutionWidth(); x+=4)
		{
			ofColor label  = kinect.getLabelPixels().getColor(x, y);
			
			if(label.a > 0)
			{
				int depth =  kinect.getDistanceAt(x, y);
			
				int px = ofMap(x, 0, kinect.getDepthResolutionWidth(), 0, ofGetWidth());
				int py = ofMap(y, 0, kinect.getDepthResolutionHeight(), 0, ofGetHeight());
				int pz = ofMap(depth, nearClipping, farClipping, zRange, -zRange);

				ofPushMatrix();
				ofTranslate(px, py, pz);
				ofColor color = kinect.getCalibratedColorAt(x, y);
				color.setBrightness( color.getBrightness() + brightnessBoost );
				ofSetColor(color);
				if(bCirclePixel)
					ofCircle(0, 0, pixelSize);
				else
					ofRect(0, 0, pixelSize, pixelSize);
				ofPopMatrix();
			}
		}
	}
	glDisable(GL_DEPTH);

	ofPopMatrix();


	gui.draw();

	// draw instructions
	ofSetColor(0);
	stringstream reportStream;
	reportStream << "fps: " << ofGetFrameRate() << "  Kinect Nearmode: " << kinect.isNearmode() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl;
				 //<< "press 'r' to record and 'p' to playback, record is: " << bRecord << ", playback is: " << bPlayback << endl;
	ofDrawBitmapString(reportStream.str(), 20, ofGetHeight() - 100);
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