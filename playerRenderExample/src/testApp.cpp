#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	receiver.setup( PORT );
}

//--------------------------------------------------------------
void testApp::update(){
		// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		// check for mouse moved message
		if ( m.getAddress() == "/player" )
		{
			int n = 0;
			pixels.clear();
			while(n < m.getNumArgs())
			{
				ofVec3f p;
				p.x = m.getArgAsFloat(n++);
				p.y = m.getArgAsFloat(n++);
				p.z = m.getArgAsFloat(n++); // gray value
				pixels.push_back( p );
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);

	for(int i=0; i<pixels.size(); i++)
	{
		ofSetColor(pixels[i].z);
		ofRect( pixels[i].x, pixels[i].y, 3, 3);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}