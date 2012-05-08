#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);


	char szPath[128] = "";
    gethostname(szPath, sizeof(szPath));
	string hostname = string(szPath);
	string destination="192.168.2.255";
	int port = 12345;
	cout <<  hostname << " --> " << destination << ":" << port << endl;
	sender.setup( destination, port );

	bool grabVideo = true;
	bool grabDepth = true;
	bool grabAudio = false;
	bool grabLabel = true;
	bool grabSkeleton = true;
	bool grabCalibratedVideo = true;
	bool grabLabelCv = false;
	bool useTexture = true;
	NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480;
	NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240;
	bool nearmode = false;

	kinect.init(grabVideo, grabDepth, grabAudio, grabLabel, grabSkeleton, grabCalibratedVideo, grabLabelCv, useTexture, videoResolution, depthResolution);
	kinect.open(nearmode);
	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	kinectSource = &kinect;
	kinectAngle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	//nearClipping = kinect.getNearClippingDistance();
	//farClipping = kinect.getFarClippingDistance();
	renderPos.set(400, 110);

	render.allocate(DEST_WIDTH, DEST_HEIGHT);
	colorFrame.allocate(320, 240);
	grayFrame.allocate(320, 240);

	gui.addTitle("All Controls");
	gui.addSlider("Angle", kinectAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 800, 1000);
	gui.addSlider("Far Clipping", farClipping, 2000, 4000);
	gui.addSlider("Pixel Size", pixelSize, 2, 10);
	gui.addSlider("Pixel Spacing", pixelSpacing, 0, 10);
	gui.addSlider("Brightness", brightness, -1, 1);
	gui.addSlider("Contrast", contrast, -1, 1);
	gui.addToggle("Modulate Size", bModulatePixelSize);
	

	gui.addTitle("Previews").setNewColumn(true);
	gui.addContent("Video", kinect.getVideoTextureReference(), 320);
	gui.addContent("Gray", grayFrame.getTextureReference(), 320);
	gui.addContent("Render", render.getTextureReference(), DEST_WIDTH).setNewColumn(true);


	ofSetFrameRate(60);

	gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update() {

	kinect.update();

	if(kinect.isFrameNew())
	{
		colorFrame.setFromPixels( kinect.getCalibratedVideoPixels() );
		grayFrame = colorFrame;
		grayFrame.brightnessContrast(brightness, contrast);
		grayPixels = grayFrame.getPixelsRef();
	}

	// Update the kinect values
	if(kinectAngle != kinect.getCurrentAngle()) {
		kinect.setAngle( kinectAngle );
	}

	/*
	if(nearClipping != kinect.getNearClippingDistance()) {
		kinect.setNearClippingDistance( nearClipping );
	}
	if(farClipping != kinect.getFarClippingDistance()) {
		kinect.setFarClippingDistance( farClipping );
	}
	*/
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(0);

	int numPixels=0;
	render.begin();
	ofClear(0);
	ofFill(); 
	
	ofxOscMessage m;
	m.setAddress( "/p1" );

	for(int y=0; y<render.getHeight(); y+=pixelSize+pixelSpacing)
	{
		for(int x=0; x<render.getWidth(); x+=pixelSize+pixelSpacing)
		{
			if(x >= DEST_WIDTH || y >= DEST_HEIGHT || m.getNumArgs() > 810) continue;

			// calculate where in the kinect stuff we should sample
			ofPoint samplePos;
			samplePos.x = ofMap(x, 0, render.getWidth(), 0,  kinect.getDepthResolutionWidth());
			samplePos.y = ofMap(y, 0, render.getHeight(), 0,  kinect.getDepthResolutionHeight());

			// Get depth and alpha values (for testing whether we want this pixel)
			int depth =  kinect.getDistanceAt(samplePos);
			int a =  kinect.getLabelPixels().getColor(samplePos.x, samplePos.y).a;

			// If it passes the tests, draw/send it
			if(a > 0 && ofInRange(depth, nearClipping, farClipping) )
			{
				// Get the color from the sample position
				samplePos+=videoOffset;
				samplePos.x = ofClamp(samplePos.x, 0, grayPixels.getWidth());
				samplePos.y = ofClamp(samplePos.y, 0,  grayPixels.getHeight());
				ofColor color = grayPixels.getColor(samplePos.x, samplePos.y);

				// Calculate size of pixel
				float size = bModulatePixelSize 
					?  ofMap(depth, nearClipping, farClipping, pixelSize, 2, true)
					: pixelSize;
				
				// Add this pixel to the message
				m.addIntArg( x );
				m.addIntArg( y );
				m.addIntArg( color.getBrightness() );

				// Draw the pixel into the FBO
				ofPushMatrix();
				ofTranslate(x, y, 0);
				ofSetColor(color);
				ofRect(0, 0, size, size);
				numPixels++;
				ofPopMatrix();
			}
		}
	}
	/*
	cout << "sending message: " << m.getAddress() << ": ";
	for(int i=0; i<m.getNumArgs(); i++) {
		cout << m.getArgAsFloat(i) << ", ";
	}
	cout << endl;
	*/
	sender.sendMessage( m );
	m.setAddress("/p2");
	sender.sendMessage( m );
	render.end();
	

	ofSetColor(255);
	stringstream report;
	report << "total pixels: " << numPixels << endl;
	ofDrawBitmapString(report.str(), 10, ofGetHeight()-20);

	gui.draw();
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
	case OF_KEY_RIGHT:
		renderPos.x++;
		break;
	case OF_KEY_LEFT:
		renderPos.x--;
		break;
	case OF_KEY_UP:
		renderPos.y--;
		break;
	case OF_KEY_DOWN:
		renderPos.y++;
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