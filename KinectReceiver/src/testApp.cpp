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
    bool  weConnected = tcpClient.setup("192.168.1.107", port);
	//tcpClient.setMessageDelimiter("\n");
    tcpClient.setVerbose(true);
#endif
#ifdef USE_OSC
    receiver.setup( port );
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

#ifdef USE_UDP
    char compressed[7000];
	size = udpConnection.Receive(compressed, 7000);
    message = compressed;
#endif
    
    
#ifdef USE_TCP
    
    tcpClient.receiveRawBytes(<#char *receiveBytes#>, <#int numBytes#>)
    string message = tcpClient.receive();
    if(message.length()>0)
    {
        char uncompressed[10000];
        int size = LZ4_uncompress_unknownOutputSize(message.c_str(), uncompressed, message.length(), 10000);
        if(size>0)
        {

            string out;
            out.assign(uncompressed, size);

        }

    }
    /*
    if(message != "")
    {
        vector<string> player = ofSplitString(message, ":", true, true);
        if(player[0]=="p1") {
            m1 = player[1];
        }
        if(player[0]=="p2") {
            m2 = player[1];
        }
    }
     */
#endif
 
#ifdef USE_OSC
    while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        if(m.getAddress()=="/p1")
        {
            vector<string> updates = ofSplitString(m.getArgAsString(0), "&", false, false);
            for(int i=0; i<updates.size(); i++)
            {
                vector<string> cell = ofSplitString(updates[i], "=", false, false);
                if(cell.size()>0) {
                    int index = ofToInt( cell[0] );
                    int bri = 0;
                    if(cell.size()>1) {
                        bri = ofToInt( cell[1] );
                    }
                    p1[index] = bri;
                }
            }
        }
        
        if(m.getAddress()=="/p2")
        {
            vector<string> updates = ofSplitString(m.getArgAsString(0), "&", false, false);
            for(int i=0; i<updates.size(); i++)
            {
                vector<string> cell = ofSplitString(updates[i], "=", false, false);
                if(cell.size()>0) {
                    int index = ofToInt( cell[0] );
                    int bri = 0;
                    if(cell.size()>1) {
                        bri = ofToInt( cell[1] );
                    }
                    p2[index] = bri;
                }
            }
        }
        
    }
#endif
    
    /*
	if(m1!="")
	{

	}

    
    
	if(m2!="")
	{
		vector<string> updates = ofSplitString(m2, "&", true, true);
		for(int i=0; i<updates.size(); i++)
		{
			vector<string> cell = ofSplitString(updates[i], "=", true, true);
			int index = ofToInt( cell[0] );
			int bri = ofToInt( cell[1] );
			p2[index] = bri;
		}
	}

     */
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
