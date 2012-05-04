#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);

	/*
	char szPath[128] = "";
    gethostname(szPath, sizeof(szPath));
	string hostname = string(szPath);
	string destination="localhost";
	int port = 10001;

	cout <<  hostname << " --> " << destination << ":" << port << endl;
	sender.setup( destination, port );
	*/

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

	render.allocate(256, 256);

	ofSetVerticalSync(false);

	//rgbaLabelImage.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
	//labelImage.allocate(320, 240);
	colorFrame.allocate(320, 240);
	grayFrame.allocate(320, 240);

	kinectSource = &kinect;
	kinectAngle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	bDragging = false;
	//nearClipping = kinect.getNearClippingDistance();
	//farClipping = kinect.getFarClippingDistance();
	renderPos.set(400, 110);
	pctFilled = 0;

	gui.addTitle("All Controls");
	gui.addSlider("Angle", kinectAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 800, 1000);
	gui.addSlider("Far Clipping", farClipping, 2000, 4000);
	gui.addSlider("Pixel Size", pixelSize, 2, 10);
	gui.addSlider("Pixel Spacing", pixelSpacing, 0, 10);
	gui.addSlider("Player Fill", pctFilled, 0, 1);
	gui.addSlider("Brightness", brightness, -1, 1);
	gui.addSlider("Contrast", contrast, -1, 1);
	gui.addToggle("Modulate Size", bModulatePixelSize);
	
	gui.addTitle("Positions").setNewColumn(true);
	gui.addSlider2d("Video Offset", videoOffset, -20, 20, -20, 20);
	gui.addSlider2d("Render Pos", renderPos, 0, ofGetWidth(), 0, ofGetHeight());

	//gui.addTitle("Previews").setNewColumn(true);
	//gui.addContent("Video", kinect.getVideoTextureReference());
	//gui.addContent("Gray", grayFrame.getTextureReference());

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

	//rgbaLabelImage.setFromPixels( kinect.getLabelPixels() );
	//rgbaLabelImage.setImageType( OF_IMAGE_COLOR );
	//labelImage.setFromPixels( rgbaLabelImage.getPixels(), 320, 240 );


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
	ofColor color;

	for(int y=0; y<render.getHeight(); y+=pixelSize+pixelSpacing)
	{
		for(int x=0; x<render.getWidth(); x+=pixelSize+pixelSpacing)
		{
			// calculate where in the kinect stuff we should sample
			float px = ofMap(x, 0, render.getWidth(), 0,  kinect.getDepthResolutionWidth());
			float py = ofMap(y, 0, render.getHeight(), 0,  kinect.getDepthResolutionHeight());

			int depth =  kinect.getDistanceAt(px, py);
			if( kinect.getLabelPixels().getColor(px, py).a > 0 && ofInRange(depth, nearClipping, farClipping) )
			{
				ofPoint sampleAt = ofPoint(px, py) + videoOffset;
				sampleAt.x = ofClamp(sampleAt.x, 0, grayPixels.getWidth());
				sampleAt.y = ofClamp(sampleAt.y, 0,  grayPixels.getHeight());

				color = grayPixels.getColor(sampleAt.x, sampleAt.y);

				// Calculate size of pixel
				float size = bModulatePixelSize 
					?  ofMap(depth, nearClipping, farClipping, pixelSize, 2, true)
					: pixelSize;
				
				float pct = 1 - ( y / (float)kinect.getDepthResolutionHeight() );
				if( pct < pctFilled )
					color.g = min(color.g+100, 255);
		
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
	for(float y=0; y < kinect.getDepthResolutionHeight(); y+=pixelSpacing)
	{
		for(float x=0; x < kinect.getDepthResolutionWidth(); x+=pixelSpacing)
		{
			ofColor label =  kinect.getLabelPixels().getColor(x, y);
			int depth =  kinect.getDistanceAt(x, y);
			if(label.a > 0 && ofInRange(depth, nearClipping, farClipping) )
			{
				ofPoint sampleAt = ofPoint(x, y) + videoOffset;
				sampleAt.x = ofClamp(sampleAt.x, 0, kinect.getVideoResolutionWidth());
				sampleAt.y = ofClamp(sampleAt.y, 0, kinect.getVideoResolutionHeight());
	
				//ofColor color = kinect.getCalibratedColorAt(sampleAt);
				color = grayPixels.getColor(sampleAt.x, sampleAt.y);

				// Calculate size of pixel
				float size = bModulatePixelSize 
					?  ofMap(depth, nearClipping, farClipping, pixelSize, 2, true)
					: pixelSize;

				float pct = 1 - ( y / (float)kinect.getDepthResolutionHeight() );
				if( pct < pctFilled )
					color.g = min(color.g+100, 255);
		

				float px = ofMap(x, 0, kinect.getDepthResolutionWidth(), 0, render.getWidth());
				float py = ofMap(y, 0, kinect.getDepthResolutionHeight(), 0, render.getHeight());

				ofPushMatrix();
				ofTranslate(px, py, 0);
				ofSetColor(color);
				ofRect(0, 0, size, size);
				numPixels++;
				ofPopMatrix();
			}
		}
	}
	*/
	render.end();
	


	ofPushMatrix();
	ofTranslate(renderPos);
	ofNoFill();
	ofSetColor(255);
	ofRect(-1, -1, render.getWidth()+2, render.getHeight()+2);
	ofSetColor(255,255,255);
	render.draw(0, 0);
	ofPopMatrix();
		


	stringstream report;
	report << "total pixels: " << numPixels << endl;
	ofDrawBitmapString(report.str(), 10, ofGetHeight()-20);

	grayFrame.draw(0, 0);
	//labelImage.draw(320, 0);

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
	if(bDragging) {
		
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	if(x > renderPos.x && x < renderPos.x+render.getWidth() 
		&& y > renderPos.y && y < renderPos.y+render.getHeight()) {
		bDragging = true;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bDragging = false;
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