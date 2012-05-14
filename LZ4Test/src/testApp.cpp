#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    count=0;
    last = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::update(){
    
    if(ofGetElapsedTimef()-last > 1)
    {
        string message = "Compress string test "+ofToString(count);
        count++;
        
        char compressed[10000];
        int size = LZ4_compress(message.c_str(), compressed, message.length());
        cout << "input: " << message.length() << " output: " << size << endl;
        
        char uncompressed1[10000];
        LZ4_uncompress(compressed, uncompressed1, message.length());
        cout << "LZ4_uncompress: " << uncompressed1 << endl;
        
        
        char uncompressed2[10000];
        LZ4_uncompress_unknownOutputSize(compressed, uncompressed2, size, 10000);
         cout << "LZ4_uncompress_unknownOutputSize: " << uncompressed2 << endl;
        
        last = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void testApp::draw(){

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