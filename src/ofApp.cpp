#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    w = 1024;
    h = 1024;
    
    tcin.startThread();
    
    consoleFbo.allocate(w, h, GL_RGB);
    consoleFbo.begin();
        ofClear(0);
    consoleFbo.end();
    vvFbo.allocate(w, h, GL_RGB, 8);
    
    ofBuffer buffer = ofBufferFromFile("cat3.txt");
    string text = buffer.getText();
    vector<string> myLines = ofSplitString(text, "\n");

    polyLines = parseData(myLines, false, "cat");
    vvLines.push_back(polyLines);
    needsUpdate = false;
    allLinesSize = 0;
    rdy = false;
    eos = false;
    
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    guiPhrase = gui->addTextInput("Phrase", "cat");
    guiBias = gui->addSlider("Bias", -1, 10);
    guiBias->setValue(0.1);
    guiId = gui->addSlider("ID", 0, 9999);
    guiId->setPrecision(0);
    guiId->setValue(0);
    guiConnections = gui->addToggle("Show Connections", "true");
    guiConsole = gui->addToggle("Show Console", "false");
    guiAllLines = gui->addToggle("Show All Lines", "false");
    
    showGui = true;
    useGuiVals = true;
    
    ofxDatGuiLog::quiet();
    
    
}
//--------------------------------------------------------------
vector<ofPolyline> ofApp::parseData(vector<string> myLines, bool showConnections, string phr){
    ofVec2f muXY = ofVec2f(8.16576672, 0.11146504);
    ofVec2f devXY = ofVec2f(41.484401, 36.86198807);
    ofVec2f tempPoint = ofVec2f(0,0);
    
    vector<ofVec3f> pointData;
    ofVec3f point;
    ofFile file("textFiles/"+phr+"_"+ofGetTimestampString()+".txt", ofFile::WriteOnly);
    for(int i = 3; i<myLines.size(); i++){
        vector<string> line  = ofSplitString(myLines[i], " ");
        if(line[0] == "Sample"){            
            point.x = (ofToFloat(line[1])* devXY[0] + muXY[0]) + tempPoint.x;
            point.y = (ofToFloat(line[2])* devXY[1] + muXY[1]) + tempPoint.y;
            point.z = ofToInt(line[3]);
            
            tempPoint.x = point.x;
            tempPoint.y = point.y;
            
            pointData.push_back(point);
            
            //write to file
            file << ofToString(point.x) + " " + ofToString(point.y) + " " + ofToString(point.z)<<endl;
        }
    }
    file.close();
    
    float minX = pointData[0].x;
    float minY = pointData[0].y;
    float maxX = pointData[0].x;
    float maxY = pointData[0].y;
    
    int tempIndex = 0;
    
    vector<ofPolyline> ll;
    
    for(int i = 0; i<pointData.size(); i++){
        minX = min(minX, pointData[i].x);
        minY = min(minY, pointData[i].y);
        maxX = max(maxX, pointData[i].x);
        maxY = max(maxY, pointData[i].y);
    }
    
    ofPolyline poly;
    int maxW = 100;
    int maxH = 50;
    
    for(int i =0; i<pointData.size(); i++){
            poly.lineTo(ofMap(pointData[i].x, minX, maxX, 0,maxW), ofMap(pointData[i].y, minY, maxY, 0,maxH));
    }
    
    if(!showConnections){
        for(int i=0; i<pointData.size(); i++){
            ofPolyline p;
            ll.push_back(p);
        }
        
        for (int i = 0; i<pointData.size(); i++) {
            float mapX = ofMap(pointData[i].x, minX, maxX, 0,maxW);
            float mapY = ofMap(pointData[i].y, minY, maxY, 0,maxH);
            
            ll[tempIndex].curveTo(mapX, mapY);
            
            if(pointData[i].z == 1 && tempIndex < ll.size()){
                tempIndex++;
            }
        }
    }else if(showConnections){
        ll.push_back(poly);
    }
    
    dims = poly.getBoundingBox();
    allDims.push_back(dims);
    
    return ll;
    
    
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
    
    //show data if avail
    consoleFbo.begin();
    if(allLines.size() > 0){
        for(int i = 0; i<allLines.size(); i++){
            ofDrawBitmapString(allLines[i], 10, i*10);
        }
    }
        //green ready button
    if(rdy){
        ofSetColor(0, 255, 0);
    } else{
        ofSetColor(255, 0, 0);
    }
    
    ofDrawRectangle(ofGetWidth()-100, 0, 100, 100);
    ofSetColor(255);
    consoleFbo.end();
    
    
    
    //draw lines
    ofPushMatrix();
        ofTranslate(ofGetWidth()/2 - dims.width/2, ofGetHeight()/2 - dims.height/2);
        for(int i = 0; i<polyLines.size(); i++){
            polyLines[i].draw();
        }
    ofPopMatrix();
    

    
    
    //get new text and send when ready
    if(rdy){
        if(eos){
            allLines = tcin.getAllLines();
            
            if(allLines.size()>0){
                vvLines.push_back(polyLines);
//                polyLines.clear();
                polyLines = parseData(allLines, guiConnections->getEnabled(), guiPhrase->getText());
            }
            
            tcin.reset();
            if(!useGuiVals){
                string phrase = "ITS WORKING!!";
                string bias = ofToString(ofRandom(-1,1));
                string hand = ofToString((int)ofRandom(9999));
                cout<<phrase+":"+bias+":"+hand<<endl;
            } else if(useGuiVals){
                if(guiPhrase->getText() != ""){
                    cout<<guiPhrase->getText()+":"+ofToString(guiBias->getValue())+":"+ofToString(guiId->getValue())<<endl;
                }
            }
        }
    }
    
    if(guiConsole->getEnabled()){
        consoleFbo.draw(0,0);
    }
    
    vvFbo.begin();
    
        ofClear(255);
        ofSetColor(0);
    
        int xSpace = 0;
        int ySpace = 0;
            for(int i = 0; i<vvLines.size(); i++){
                ofPushMatrix();
                
                ofTranslate(xSpace, ySpace);
                for(int j = 0; j<vvLines[i].size(); j++){
                    
                    vvLines[i][j].draw();
                }
                
                xSpace += allDims[i].width;
                if(xSpace > w){
                    ySpace+= allDims[i].height;
                    xSpace = 0;
                }
                ofPopMatrix();
            }
        ofSetColor(255);
    
    vvFbo.end();
    
    if(guiAllLines->getEnabled()){
        vvFbo.draw(0,0);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        allLines = tcin.getAllLines();        
    }
    
    if(key == 'a'){
        cout<<guiPhrase->getText()+":0.1:0"<<endl;
    }
    
    if(key == 'l'){
        if(allLines.size()>0){
            polyLines.clear();
            polyLines = parseData(allLines, guiConnections->getEnabled(), "test");
            tcin.reset();
        }
    }
    
    if(key == 'g'){
        useGuiVals = !useGuiVals;
    }
    
    if(key == 'h'){
        showGui = !showGui;
        gui->setVisible(showGui);
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
