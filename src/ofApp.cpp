#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    tcin.startThread();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    if(allLines.size() > 0){
        for(int i = 0; i<allLines.size(); i++){
            ofDrawBitmapString(allLines[i], 10, 40+i*30);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        allLines = tcin.getAllLines();        
    }
    
    if(key == 'a'){
        cout<<"test test"<<endl;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
