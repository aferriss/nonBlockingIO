#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    tcin.startThread();
    
    ofBuffer buffer = ofBufferFromFile("/tmp/cat3.txt");
    string text = buffer.getText();
    vector<string> myLines = ofSplitString(text, "\n");

    polyLine = parseData(myLines);
    polyLines.push_back(polyLine);
    
    needsUpdate = false;
    allLinesSize = 0;
    rdy = false;
    eos = false;
}
//--------------------------------------------------------------
ofPolyline ofApp::parseData(vector<string> myLines){
    ofVec2f muXY = ofVec2f(8.16576672, 0.11146504);
    ofVec2f devXY = ofVec2f(41.484401, 36.86198807);
    ofVec2f tempPoint = ofVec2f(0,0);
    
    vector<ofVec3f> pointData;
    ofVec3f point;
    
    for(int i = 0; i<myLines.size(); i++){
        vector<string> line  = ofSplitString(myLines[i], " ");
        if(line[0] == "Sample"){
            
            point.x = (ofToFloat(line[1])* devXY[0] + muXY[0]) + tempPoint.x;
            point.y = (ofToFloat(line[2])* devXY[1] + muXY[1]) + tempPoint.y;
            point.z = ofToInt(line[3]);
            
            tempPoint.x = point.x;
            tempPoint.y = point.y;
            
            pointData.push_back(point);
        }
    }
    
    
    float minX = pointData[0].x;
    float minY = pointData[0].y;
    float maxX = pointData[0].x;
    float maxY = pointData[0].y;
    
    for(int i = 0; i<pointData.size(); i++){
        minX = min(minX, pointData[i].x);
        minY = min(minY, pointData[i].y);
        maxX = max(maxX, pointData[i].x);
        maxY = max(maxY, pointData[i].y);
    }
    
    

    
    
    ofPolyline poly;
    for(int i =0; i<pointData.size(); i++){
        
        if(point.z == 0){
            poly.lineTo(ofMap(pointData[i].x, minX, maxX, 0,400), ofMap(pointData[i].y, minY, maxY, 0,200));
        }
        if(point.z == 1){
//        lineList.push_back(poly);
        poly.clear();
        }
    }
    
    return poly;
    
    
}
//--------------------------------------------------------------
void ofApp::update(){
    if(rdy == false){
        rdy = tcin.isReady();
    }
    
    eos = tcin.isEnd();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
//    if(allLines.size() != allLinesSize){
//        needsUpdate = true;
//    } else{
//        needsUpdate = false;
//    }
    
//    if(needsUpdate){
////        parseData(allLines);
//    }
    
    if(allLines.size() > 0){
        for(int i = 0; i<allLines.size(); i++){
            ofDrawBitmapString(allLines[i], 10, i*10);
        }
    }
    
    ofPushMatrix();
    //  ofTranslate(ofGetWidth()/2 , ofGetHeight()/2);
    
        ofTranslate(ofGetWidth()/2 - polyLine.getBoundingBox().width/2, ofGetHeight()/2-polyLine.getBoundingBox().height/2);
//        for(int i = 0; i<lineList.size(); i++){
            polyLines[polyLines.size()-1].draw();
//            lineList[i].draw();
//        }
    ofPopMatrix();
    
    if(rdy){
        ofSetColor(0, 255, 0);
    } else{
        ofSetColor(255, 0, 0);
    }
    
    ofDrawRectangle(ofGetWidth()-100, 0, 100, 100);
    ofSetColor(255);
    
    if(rdy){
        if(eos){
            allLines = tcin.getAllLines();
            
            if(allLines.size()>0){
                ofPolyline p = parseData(allLines);
                polyLines.push_back(p);
            }
            
            tcin.reset();
            string phrase = "ITS WORKING!!";
            string bias = ofToString(ofRandom(-1,1));
            string hand = ofToString((int)ofRandom(9999));
            cout<<phrase+":"+bias+":"+hand<<endl;
        }
    }
    
//    allLinesSize = allLines.size();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        allLines = tcin.getAllLines();        
    }
    
    if(key == 'a'){
        cout<<"cat:10.0:9053"<<endl;
    }
    
    if(key == 'l'){
        if(allLines.size()>0){
            ofPolyline p = parseData(allLines);
            polyLines.push_back(p);
            tcin.reset();
        }
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
