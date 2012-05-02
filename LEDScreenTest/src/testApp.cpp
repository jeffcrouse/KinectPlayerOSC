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

	bool grabVideo = true;
	bool grabDepth = true;
	bool grabAudio = true;
	bool grabLabel = true;
	bool grabSkeleton = false;
	bool grabCalibratedVideo = true;
	bool grabLabelCv = true;
	bool useTexture = true;
	NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_320x240;
	NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240;
	bool nearmode = false;

	kinect.init(grabVideo, grabDepth, grabAudio, grabLabel, grabSkeleton, grabCalibratedVideo, grabLabelCv, useTexture, videoResolution, depthResolution);
	kinect.open(nearmode);
	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	
	ofSetVerticalSync(true);

	kinectSource = &kinect;
	bPlayback = false;
	bPlugged = kinect.isConnected();

	if(bPlugged) {
		kinectAngle = kinect.getCurrentAngle();
		nearClipping = kinect.getNearClippingDistance();
		farClipping = kinect.getFarClippingDistance();
	}

	bCirclePixel = true;

	gui.addTitle("All Controls");
	gui.addSlider("Angle", kinectAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 0, 200);
	gui.addSlider("Far Clipping", farClipping, 200, 4000);
	gui.addSlider("Z range", zRange, 0, 6000);
	gui.addSlider("Pixel Size", pixelSize, 3, 40);
	gui.addSlider("Rotate X", rotateX, -90, 90);
	gui.addToggle("Circle Pixel", bCirclePixel);

	//gui.addTitle("Previews").setNewColumn(true);	
	//gui.addContent("Video", kinectSource->getVideoTextureReference());
	//gui.addContent("Labels", kinectSource->getLabelTextureReference());

	ofSetFrameRate(60);

	gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update() {

	kinectSource->update();
	if(bPlugged)
	{
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
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(0);
	ofSetColor(255,255,255);
	
	ofPushMatrix();
	ofTranslate(0, -ofGetHeight()/2.0);
	ofRotateX(rotateX);
	ofTranslate(0, ofGetHeight()/2.0);
	glEnable(GL_DEPTH_TEST);

	float width = (bPlayback) ? kinectPlayer.getDepthWidth() : kinect.getDepthResolutionWidth();
	float height = (bPlayback) ? kinectPlayer.getDepthHeight() : kinect.getDepthResolutionHeight();
	ofPixels depthPixels = kinectSource->getDepthPixels();

	if((bPlugged || bPlayback))
	{
		for(int y=0; y < height; y+=4)
		{
			for(int x=0; x < width; x+=4)
			{
				ofColor label  = kinectSource->getLabelPixels().getColor(x, y);
			
				if(label.a > 0)
				{
					int depth = depthPixels.getColor(x,y).getBrightness(); 

					int px = ofMap(x, 0, width, 0, ofGetWidth());
					int py = ofMap(y, 0, height, 0, ofGetHeight());
					int pz = ofMap(depth, nearClipping, farClipping, zRange, -zRange);

					ofPushMatrix();
					ofTranslate(px, py, pz);
				
					ofColor color = kinectSource->getVideoPixels().getColor(x, y);
					//ofColor color = kinectSource->getCalibratedVideoPixels().getColor(x, y); //kinect.getCalibratedColorAt(x, y);
					ofSetColor(color);

					if(bCirclePixel)
						ofCircle(0, 0, pixelSize);
					else
						ofRect(0, 0, pixelSize, pixelSize);
					ofPopMatrix();
				}
			}
		}
	}
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();


	if((bPlugged || bPlayback) && gui.isOn())
	{
			
		kinectSource->getDepthTextureReference().draw(250, 110);
		kinectSource->getVideoTextureReference().draw(250, 380);
		ofEnableAlphaBlending();
		kinectSource->getLabelTextureReference().draw(250, 640);
		ofDisableAlphaBlending();
	}

	
	if(!playbackFile.empty()) 
	{
		ofSetColor(255);
		ofDrawBitmapString(playbackFile, 10, ofGetHeight()-20);
	}

	gui.draw();
}



//--------------------------------------------------------------
void testApp::exit() 
{
	kinect.setAngle(0);
	kinect.close();
	kinect.removeKinectListener(this);
	kinectPlayer.close();
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
	case 'f':
	case 'F':
		openPlaybackFile();
		break;
	case 'p': // playback recorded stream
	case 'P':
		if(!bPlayback){
			startPlayback();
		}else{
			stopPlayback();
		}
		break;
	}
}


//--------------------------------------------------------------
void testApp::openPlaybackFile() {
	ofFileDialogResult result;
	result = ofSystemLoadDialog("recordings", false);
	if(result.bSuccess)
	{
		playbackFile = result.getPath();
	}
}

//--------------------------------------------------------------
void testApp::startPlayback(){
	if(!bPlayback){
		kinect.close();

		if(playbackFile.empty()) {
			openPlaybackFile();
		}

		// set record file and source
		kinectPlayer.setup( playbackFile );
		kinectPlayer.loop();
		kinectPlayer.play();
		kinectSource = &kinectPlayer;
		bPlayback = true;
	}
}

//--------------------------------------------------------------
void testApp::stopPlayback(){
	if(bPlayback){
		kinectPlayer.close();
		kinect.open();
		kinectSource = &kinect;
		bPlayback = false;
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