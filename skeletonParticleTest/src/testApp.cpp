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

	kinect.init(true, false, false, false, true);
	kinect.open(false);
//	kinect.open(true); // when you want to use near mode

	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	
	//ofSetVerticalSync(true);

	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

    bDrawVideo = false;
    bDrawSkeleton = false;
    
    // index 1, index 2, # particles, orbitRadius
    int bones[17][4] = {
        // HEAD
        {NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, 20, 40},
        {NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, 50, 40},
        {NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD, 100, 30},
        
        // BODY_LEFT
        {NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, 10, 10},
        {NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, 50, 20},
        {NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, 50, 15},
        {NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, 5, 10},
       
        // BODY_RIGHT
        {NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, 10, 10},
        {NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, 50, 20},
        {NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, 50, 15},
        {NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT, 5, 10},
        
        // LEG_LEFT
        //{NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, 10, 4},
        {NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, 70, 30},
        {NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, 50, 20},
        {NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT, 10, 10},
        
        // LEG_RIGHT
        //{NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, 10, 4},
        {NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, 70, 30},
        {NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, 50, 20},
        {NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT, 10, 10}
    };
    
	for(int i=0; i<kinect::nui::SkeletonFrame::SKELETON_COUNT; i++)
	{
		for(int j=0; j<17; j++)
		{
			int numParticles = bones[j][2];

			for(int n=0; n<numParticles; n++)
			{
				Particle p;
				p.skeleton = i;
				p.jointIndex1 = bones[j][0];
				p.jointIndex2 = bones[j][1];
				p.orbitRadius = bones[j][3];
				p.pct = j / (float)numParticles;
            
				particles.push_back( p );
			}
		}
	}

	cout << "created " << particles.size() << " particles" << endl;


	gui.addToggle("Draw Video", bDrawVideo);
	gui.addToggle("Draw Skeleton", bDrawSkeleton);
	
	gui.loadFromXML();
	gui.show();

	ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::update() {
	kinectSource->update();
    	
    for(int i=0; i<particles.size(); i++)
    {
        particles[i].update( kinect.getSkeletonPoints() );
    }
}

//--------------------------------------------------------------
void testApp::draw() {

	ofBackground(100, 100, 100);
	// Draw video only
	if(bDrawVideo)
		kinect.draw(0, 0);	// draw video images from kinect camera
	
	if(bDrawSkeleton)
		kinect.drawSkeleton(0, 0);

	glEnable( GL_DEPTH );
	ofEnableAlphaBlending();
    ofSetColor(255);
    glBegin(GL_POINTS);
    for(int i=0; i<particles.size(); i++) 
    {
        glVertex3f(particles[i].getX(), particles[i].getY(), particles[i].getZ());
    }
    glEnd();
    ofDisableAlphaBlending();
	glDisable( GL_DEPTH );

	gui.draw();

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << "fps: " << ofGetFrameRate() << "  Kinect Nearmode: " << kinect.isNearmode() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl
				 << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 << "press LEFT and RIGHT to change the far clipping distance: " << farClipping << " mm" << endl
				 << "press '+' and '-' to change the near clipping distance: " << nearClipping << " mm" << endl;
				// << "press 'r' to record and 'p' to playback, record is: " << bRecord << ", playback is: " << bPlayback << endl
				// << "press 'v' to show video only: " << bDrawVideo << ",      press 'd' to show depth + users label only: " << bDrawDepthLabel << endl
				 //<< "press 's' to show skeleton only: " << bDrawSkeleton << ",   press 'q' to show point cloud sample: " << bDrawCalibratedTexture;
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
	switch(key) {	
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
