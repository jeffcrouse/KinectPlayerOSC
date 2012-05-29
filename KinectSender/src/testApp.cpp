#include "testApp.h"

const UINT playerColors[ofxKinectNui::KINECT_PLAYERS_INDEX_NUM] = {
	0x00FFFFFF,	/// no user
	0xFF0000FF,
	0xFF00FF00,
	0xFFFF0000,
	0xFF00FFFF,
	0xFFFF00FF,
	0xFFFFFF00,
	0xFF6600FF
};


//--------------------------------------------------------------
void testApp::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	ofSetFrameRate(60);
	cout << "SCREEN_WIDTH: " << SCREEN_WIDTH  << endl;
	cout << "SCREEN_HEIGHT: " << SCREEN_HEIGHT  << endl;
	cout << "CELL_SIZE: " << CELL_SIZE  << endl;
	cout << "GRID_WIDTH: " << GRID_WIDTH  << endl;
	cout << "GRID_HEIGHT: " << GRID_HEIGHT  << endl;
	cout << "NUM_CELLS: " << NUM_CELLS  << endl;



	//
	// Get the OSC port and destination from the settings file
	//
	if(!oscSettings.loadFile("osc_settings.xml")) {
		ofLog(OF_LOG_ERROR, "Couldn't load settings file!");
	}
	oscSettings.pushTag("osc");
	string destination=oscSettings.getValue("destination", "localhost");
	int port = oscSettings.getValue("port", 12345);


	//
	// Set up sender
	//
	sender.setup(destination, port);
	
	
	
	//
	// Decide what the address of the OSC message will be
	//
	// get the hostname of the computer
	char szPath[128] = "";
    gethostname(szPath, sizeof(szPath));
	string hostname = string(szPath);
	messageAddress="player";
	for(int i=0; i<oscSettings.getNumTags("sender"); i++)
	{
		oscSettings.pushTag("sender", i);
		if(oscSettings.getValue("hostname", "none") == hostname)
			messageAddress = oscSettings.getValue("msgAddress", "p1");
		oscSettings.popTag();
	}


	cout <<  hostname << " --> " << destination << ":" << port << " (" << messageAddress << ")" << endl;



	//
	// Set up Kinect camera
	//
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
	bool nearmode = true;

	kinect.init(grabVideo, grabDepth, grabAudio, grabLabel, grabSkeleton, grabCalibratedVideo, grabLabelCv, useTexture, videoResolution, depthResolution);
	kinect.open(nearmode);
	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	cameraAngle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();


	bRecording = false;

	//
	// Allocate the textures
	//
	colorFrame.allocate(320, 240);
	grayFrame.allocate(320, 240);
	grayFrame.setUseTexture(true);
	labelImageRGBA.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
	labelImageRGB.allocate(320, 240);
	labelImageGray.allocate(320, 240);
	render.allocate(GRID_WIDTH, GRID_HEIGHT);

	//
	// Initialize the cells
	//
	for(int i=0; i<NUM_CELLS; i++) 
	{
		cells[i] = 0;
		updates[i] = 0;
	}
	

	//
	// Set up GUI
	//
	gui.addTitle("All Controls");
	gui.addSlider("Angle", newCameraAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 800, 1000);
	gui.addSlider("Far Clipping", farClipping, 2000, 4000);
	gui.addSlider("Brightness", brightness, -1, 1);
	gui.addSlider("Contrast", contrast, -1, 1);
	gui.addSlider("Change Threshold", changeThreshold, 1, 20); // pixels under this threshold aren't considered "changed"
	gui.addSlider("Right Crop", rightCrop, 0, 100);
	gui.addSlider("Left Crop", leftCrop, 0, 100);
	
	gui.addSlider("Erode", erode, 0, 10);
	gui.addSlider("Dilate", dilate, 0, 10);
	gui.addSlider("Blur", blur, 0, 10);

	gui.addTitle("Previews").setNewColumn(true);
	gui.addContent("Video", kinect.getVideoTextureReference(), 320);
	gui.addContent("Mask", labelImageGray, 320);
	gui.addContent("Gray", grayFrame, 320).setNewColumn(true);
	gui.addContent("Render", render, SCREEN_WIDTH);

	gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update() {

	kinect.update();

	if(kinect.isFrameNew())
	{
		int now = ofGetElapsedTimeMillis();

		labelImageRGBA.setFromPixels( kinect.getLabelPixels() );
		labelImageRGBA.setImageType( OF_IMAGE_COLOR );
		labelImageRGB.setFromPixels( labelImageRGBA.getPixels(), 320, 240 );
		labelImageGray = labelImageRGB;

		if(blur>0)
			labelImageGray.blurGaussian(blur);

		labelImageGray.threshold(80);

		for(int i=0; i<erode; i++)
			labelImageGray.erode();

		for(int i=0; i<dilate; i++)
			labelImageGray.dilate();

		colorFrame.setFromPixels( kinect.getCalibratedVideoPixels() );
		grayFrame = colorFrame;
		grayFrame.brightnessContrast(brightness, contrast);
		grayPixels = grayFrame.getPixelsRef();

		// Populate the cells
		for(int y=0; y < GRID_HEIGHT; y++)
		{
			for(int x=0; x < GRID_WIDTH; x++)
			{
				int i = y * GRID_WIDTH + x;
				assert(i<NUM_CELLS);

				// Where should we look in the 320x240 images for the depth and brightness values?
				ofPoint samplePos;
				samplePos.x = ofMap(x, 0, GRID_WIDTH, leftCrop,  kinect.getDepthResolutionWidth()-rightCrop);
				samplePos.y = ofMap(y, 0, GRID_HEIGHT, 0,  kinect.getDepthResolutionHeight());

				// Get depth and alpha values (for testing whether we want this pixel)
				int depth =  kinect.getDistanceAt(samplePos);
				//int a =  kinect.getLabelPixels().getColor(samplePos.x, samplePos.y).a;
				int a = labelImageGray.getPixelsRef().getColor(samplePos.x, samplePos.y).r;

				// Determine the brightness for cells[i]
				int bri;
				if(a == 255) {
					bri = 0;
				} else {
					int gray_x = ofClamp(samplePos.x+videoOffset.x, 0, grayPixels.getWidth());
					int gray_y = ofClamp(samplePos.y+videoOffset.y, 0, grayPixels.getHeight());
					bri = grayPixels.getColor(gray_x, gray_y).getBrightness();
				}
			
				// If it passes the tests, draw/send it
				int diff = abs(bri - cells[i]);
				if(diff > changeThreshold) // || now-updates[i] > 2000)
				{
					int packed = (i << 8) | bri;			
					message  << packed << ",";
					cells[i] = bri;
					updates[i] = now;
				}
				
				if(message.str().length() > 800) {
					sendMessage();
				}
			}
		}
	}

	if(bRecording) {
		outfile << "\n";
	}

	// Update the kinect values
	if(cameraAngle != newCameraAngle) 
	{
		kinect.setAngle( newCameraAngle );
		cameraAngle = newCameraAngle;
	}
	
	if(nearClipping != kinect.getNearClippingDistance()) 
		kinect.setNearClippingDistance( nearClipping );
	
	if(farClipping != kinect.getFarClippingDistance()) 
		kinect.setFarClippingDistance( farClipping );

}

//--------------------------------------------------------------
void testApp::sendMessage()
{
	if(bRecording) {
		outfile << message.str() + "\n";
	}

	ofxOscMessage m;
	m.setAddress(messageAddress);
	m.addStringArg( message.str() );
	sender.sendMessage( m );
	message.str("");
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(0);

	// Draw the grid
	render.begin();
	ofClear(0);
	for(int y=0; y < GRID_HEIGHT; y++)
	for(int x=0; x < GRID_WIDTH; x++)
	{
		int i = y * GRID_WIDTH + x;
		ofSetColor( cells[i], cells[i], cells[i] );
		ofRect(x, y, 1, 1);
	}
	render.end();

	// Draw player colors for reference
	for(int i=0; i<ofxKinectNui::KINECT_PLAYERS_INDEX_NUM; i++)
	{
		ofSetHexColor(playerColors[i]);
		ofRect(i*20, 0, 20, 20);
	}

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
	case 'r':
	case 'R':
		if(bRecording)
		{
			outfile.close();
			bRecording = false;
		} 
		else 
		{
			string defaultName = ofGetTimestampString()+".dat";
			string message = "Choose a save location";
			ofFileDialogResult result = ofSystemSaveDialog(defaultName, message);
			outfile.open(result.getPath(), ofFile::WriteOnly);
			bRecording = true;
		}
		break;
	case OF_KEY_RIGHT:
		videoOffset.x++;
		break;
	case OF_KEY_LEFT:
		videoOffset.x--;
		break;
	case OF_KEY_UP:
		videoOffset.y--;
		break;
	case OF_KEY_DOWN:
		videoOffset.y++;
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