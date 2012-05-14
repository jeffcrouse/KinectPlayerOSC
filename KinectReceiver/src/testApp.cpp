#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	//we run at 60 fps!
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    
    int port = 12345;
    
#ifdef USE_UDP
    //create the socket and bind to port 11999
	udpConnection.Create();
	udpConnection.Bind(port);
	udpConnection.SetNonBlocking(true);
#endif
#ifdef USE_TCP
    bool  weConnected = tcpClient.setup("192.168.1.105", port);
	tcpClient.setMessageDelimiter("\n");
    tcpClient.setVerbose(true);
#endif
    
	//
	// Initialize the cells
	//
	for(int i=0; i<NUM_CELLS; i++) {
		p1[i] = 0;
		p2[i] = 0;
	}

	

}

//--------------------------------------------------------------
void testApp::update(){

    string message;
    int size;
   
    
    
#ifdef USE_UDP
     char messageCompressed[7000];
	size = udpConnection.Receive(messageCompressed, 7000);
    message = messageCompressed;
#endif
    
    
#ifdef USE_TCP
    message = tcpClient.receive();
#endif
    
	if(message!="")
	{
        char messageUncompressed[7000];
        size = LZ4_uncompress(message.c_str(), messageUncompressed, 7000);
        message = messageUncompressed;
        cout << message << endl;

        /*
		vector<string> updates = ofSplitString(message, "&", true, true);
		for(int i=0; i<updates.size(); i++)
		{
			vector<string> cell = ofSplitString(updates[i], "=", true, true);
			int index = ofToInt( cell[0] );
			int bri = ofToInt( cell[1] );
			p1[index] = bri;
			p2[index] = bri;
		}
       */
	}


}

//--------------------------------------------------------------
void testApp::draw(){

	ofPushMatrix();
	ofScale(4, 4);
	for(int y=0; y < GRID_HEIGHT; y++)
	for(int x=0; x < GRID_WIDTH; x++)
	{
		int i = y * GRID_WIDTH + x;
		ofSetColor( p1[i], p1[i], p1[i] );
		ofRect(x, y, 1, 1);

		ofSetColor( p2[i], p2[i], p2[i] );
		ofRect(x+GRID_WIDTH, y, 1, 1);
	}
	ofPopMatrix();
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
