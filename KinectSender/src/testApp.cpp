#include "testApp.h"

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
	// Set up sender
	//
	char szPath[128] = "";
    gethostname(szPath, sizeof(szPath));
	string hostname = string(szPath);
	string destination="192.168.1.106";
	int port = 12345;
	cout <<  hostname << " --> " << destination << ":" << port << endl;

	sender.setup(destination, port);

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
	bool nearmode = false;

	kinect.init(grabVideo, grabDepth, grabAudio, grabLabel, grabSkeleton, grabCalibratedVideo, grabLabelCv, useTexture, videoResolution, depthResolution);
	kinect.open(nearmode);
	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	kinectAngle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

	//
	// Allocate the textures
	//
	colorFrame.allocate(320, 240);
	grayFrame.allocate(320, 240);
	grayFrame.setUseTexture(true);
	render.allocate(GRID_WIDTH, GRID_HEIGHT);

	//
	// Initialize the cells
	//
	for(int i=0; i<NUM_CELLS; i++)
		cells[i] = 0;

	


	//
	// Set up GUI
	//
	gui.addTitle("All Controls");
	gui.addSlider("Angle", kinectAngle, -27, 27); 
	gui.addSlider("Near Clipping", nearClipping, 800, 1000);
	gui.addSlider("Far Clipping", farClipping, 2000, 4000);
	gui.addSlider("Brightness", brightness, -1, 1);
	gui.addSlider("Contrast", contrast, -1, 1);
	gui.addSlider("Change Threshold", changeThreshold, 1, 20); // pixels under this threshold aren't considered "changed"
	//gui.addSlider("Min Cells Change", minCellsChangedToSend, 1, 40); // don't send messages with less than x changed cells
	gui.addSlider("Right Crop", rightCrop, 0, 100);
	gui.addSlider("Left Crop", leftCrop, 0, 100);
	
	gui.addTitle("Previews").setNewColumn(true);
	gui.addContent("Video", kinect.getVideoTextureReference(), 320);
	gui.addContent("Gray", grayFrame, 320);
	gui.addContent("Render", render, SCREEN_WIDTH).setNewColumn(true);

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
				int a =  kinect.getLabelPixels().getColor(samplePos.x, samplePos.y).a;


				// Determine the brightness for cells[i]
				int bri;
				if(a == 0) {
					bri = 0;
				} else {
					int gray_x = ofClamp(samplePos.x+videoOffset.x, 0, grayPixels.getWidth());
					int gray_y = ofClamp(samplePos.y+videoOffset.y, 0, grayPixels.getHeight());
					bri = grayPixels.getColor(gray_x, gray_y).getBrightness();
				}
			
				// If it passes the tests, draw/send it
				int diff = abs(bri - cells[i]);
				if(diff > changeThreshold)
				{
					int packed = (i << 8) | bri;			
					message  << packed << ",";
					cells[i] = bri;
				}
				
				if(message.str().length() > 800) {
					sendMessage();
				}
			}
		}
	}

	// Update the kinect values
	if(kinectAngle != kinect.getCurrentAngle()) 
		kinect.setAngle( kinectAngle );
	
	if(nearClipping != kinect.getNearClippingDistance()) 
		kinect.setNearClippingDistance( nearClipping );
	
	if(farClipping != kinect.getFarClippingDistance()) 
		kinect.setFarClippingDistance( farClipping );

}

//--------------------------------------------------------------
void testApp::sendMessage()
{
	ofxOscMessage m;
	m.setAddress("/p1");
	m.addStringArg( message.str() );
	sender.sendMessage( m );

	m.setAddress("/p2");
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