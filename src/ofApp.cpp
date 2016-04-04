#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    w = 800;
    h = 800;
    
    ofSetWindowShape(w, h);
    
    tcin.startThread();
    
    consoleFbo.allocate(w, h, GL_RGB);
    consoleFbo.begin();
        ofClear(0);
    consoleFbo.end();
    
    vvFbo.allocate(w, h, GL_RGB, 8);
    
    vvFbo.begin();
        ofClear(255);
    vvFbo.end();
    
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    guiPhrase = gui->addTextInput("Phrase", "iiiii ");
    guiBias = gui->addSlider("Bias", -1, 10, -1);
//    guiBias->setValue(0.1);
    guiId = gui->addSlider("ID", -1, 9999, -1);
    guiId->setPrecision(0);
//    guiId->setValue(0);
    guiConnections = gui->addToggle("Show Connections", "false");
    guiConsole = gui->addToggle("Show Console", "false");
    guiAllLines = gui->addToggle("Show All Lines", "false");
    guiMaxW = gui->addSlider("Max Width", 1,200,64);
    guiMaxH = gui->addSlider("Max Height", 1, 200,32);
    guiMaxW->setPrecision(0);
    guiMaxH->setPrecision(0);
    
    showGui = true;
    useGuiVals = true;
    
    ofxDatGuiLog::quiet();
    
    ofBuffer buffer = ofBufferFromFile("cat3.txt");
    string text = buffer.getText();
    vector<string> myLines = ofSplitString(text, "\n");

    polyLines = parseData(myLines, false, "cat");
    vvLines.push_back(polyLines);
    vvLines.push_back(polyLines);
    
    needsUpdate = false;
    allLinesSize = 0;
    rdy = false;
    eos = false;
    

    
    biasInc = -1;
    
    ofSetBackgroundAuto(true);
    ofSetBackgroundColor(120);
}
//--------------------------------------------------------------
vector<ofPolyline> ofApp::parseData(vector<string> myLines, bool showConnections, string phr){
    ofVec2f muXY = ofVec2f(8.16576672, 0.11146504);
    ofVec2f devXY = ofVec2f(41.484401, 36.86198807);
    ofVec2f tempPoint = ofVec2f(0,0);
    vector<ofPolyline> ll;
    vector<ofVec3f> pointData;
    ofVec3f point;
    int tempIndex = 0;
    ofFile file("textFiles/"+phr+"_"+ofGetTimestampString()+".txt", ofFile::WriteOnly);
    
    //start at three to get rid of first couple bad data points
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
    
    
    //get bounds
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
    int maxW = guiMaxW->getValue();
    int maxH = guiMaxH->getValue();
    int xDistFromZero = maxX - minX;
    int yDistFromZero = maxY - minY;
    
    for(int i =0; i<pointData.size(); i++){
        poly.curveTo(ofMap(pointData[i].x, minX, maxX, 0,xDistFromZero), ofMap(pointData[i].y, minY, maxY, 0,xDistFromZero));
//        poly.lineTo(pointData[i].x, pointData[i].y);
    }
    
    Lineset l;
    l.edges = ofVec4f(0, 0, xDistFromZero, yDistFromZero);
    l.contLine = poly;
    l.dims = ofVec2f(poly.getBoundingBox().width, poly.getBoundingBox().height);
    
//    if(!showConnections){
    for(int i=0; i<pointData.size(); i++){
        ofPolyline p;
        ll.push_back(p);
    }
    
    for (int i = 0; i<pointData.size(); i++) {
        float mapX = ofMap(pointData[i].x, minX, maxX, 0,xDistFromZero);
        float mapY = ofMap(pointData[i].y, minY, maxY, 0,xDistFromZero);
//        float mapX = pointData[i].x;
//        float mapY = pointData[i].y;
        
        
        ll[tempIndex].curveTo(mapX, mapY);
        
        if(pointData[i].z == 1 && tempIndex < ll.size()){
            ll[tempIndex].end();
            tempIndex++;
        }
    }
    l.brokenLine = ll;
        
//    }else if(showConnections){
//        ll.push_back(poly);
//        l.brokenLine = ll;
//    }
    
    dims = poly.getBoundingBox();
    allDims.push_back(dims);
    linesets.push_back(l);
    
    return ll;
    
    
}
//--------------------------------------------------------------
ofApp::Lineset ofApp::resizeLine(Lineset ls, int maxW, int maxH){
    vector<ofPoint> verts = ls.contLine.getVertices();
    
    for(int i = 0; i<verts.size(); i++){
        float newX = ofMap(verts[i].x, 0,ls.edges.z, 0, maxW);
        float newY = ofMap(verts[i].y, 0,ls.edges.w, 0, maxH);
//        verts[i] = ofPoint(newX, newY);
        ls.contLine[i] = ofPoint(newX, newY);
    }
    
    for(int i = 0; i<ls.brokenLine.size(); i++){
        vector<ofPoint> segVerts = ls.brokenLine[i].getVertices();
        
        for(int j = 0; j<segVerts.size(); j++){
            float newX = ofMap(segVerts[i].x, 0,ls.edges.z, 0, maxW);
            float newY = ofMap(segVerts[i].y, 0,ls.edges.w, 0, maxH);
//            segVerts[j] = ofPoint(newX, newY);
            ls.brokenLine[i][j] = ofPoint(newX, newY);
        }
    }
    
    ls.edges = ofVec4f(0,0,maxW, maxH);
    ls.dims = ofVec2f(ls.contLine.getBoundingBox().width, ls.contLine.getBoundingBox().height);
    
    return ls;
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
    
//        ofTranslate(ofGetWidth()/2 - dims.width/2, ofGetHeight()/2 - dims.height/2);
//        for(int i = 0; i<polyLines.size(); i++){
//            polyLines[i].draw();
//        }

//        for(int i = 0; i<linesets.size(); i++){
    
            ofPushMatrix();
            ofTranslate(w/2 - linesets[linesets.size()-1].dims.x/2, h/2 - linesets[linesets.size()-1].dims.y/2);
            if(guiConnections->getEnabled()){
                
                linesets[linesets.size()-1].contLine.draw();
                ofSetColor(0, 255, 0);
                ofNoFill();
                ofDrawRectangle(linesets[linesets.size()-1].contLine.getBoundingBox());
                ofSetColor(255);
                ofFill();
            } else if(!guiConnections->getEnabled()){
                for(int c = 0; c<linesets[linesets.size()-1].brokenLine.size(); c++){
                    linesets[linesets.size()-1].brokenLine[c].draw();
                }
            }
            ofPopMatrix();
//        }
    
    
    
    //get new text and send when ready
    if(rdy){
        if(eos){
            allLines = tcin.getAllLines();
            if(allLines.size()>0){
                vvLines.push_back(polyLines);
                polyLines = parseData(allLines, guiConnections->getEnabled(), guiPhrase->getText());
            }
            
            tcin.reset();
            if(!useGuiVals){
                string phrase = "ITS WORKING!!";
                string bias = ofToString(ofRandom(-1,1));
                string hand = ofToString((int)ofRandom(9999));
                cout<<phrase+":"+bias+":"+hand<<endl;
            } else if(useGuiVals){
                if(guiPhrase->getText() == ""){
                    guiPhrase->setText(" ");
                }
                cout<<guiPhrase->getText()+":"+ofToString(guiBias->getValue())+":"+ofToString(guiId->getValue())<<endl;
                biasInc += 0.005;
            }
        }
    }

    
    //draw grid
    if(allDims.size()>0){
        vvFbo.begin();
        
            ofClear(255);
            ofSetColor(0);
        
            int xSpace = 0;
            int ySpace = 0;
        
        for(int i = 0; i<linesets.size(); i++){
            ofPushMatrix();
            ofTranslate(xSpace, ySpace);
            
            for(int j = 0; j<linesets[i].brokenLine.size(); j++){
                linesets[i].brokenLine[j].draw();
            }
            
            xSpace += linesets[i].dims.x;
            if(xSpace > w){
                ySpace += linesets[i].dims.y;
                xSpace = 0;
            }
            ofPopMatrix();
        }
//                for(int i = 0; i<vvLines.size(); i++){
//                    ofPushMatrix();
//                    
//                    ofTranslate(xSpace, ySpace);
//                    
//                    for(int j = 0; j<vvLines[i].size(); j++){
//                        vvLines[i][j].draw();
//                    }
//                    
//                    xSpace += allDims[i].width;
//                    
//                    if(xSpace > w){
//                        ySpace+= allDims[i].height;
//                        
//                        xSpace = 0;
//                    }
//                    ofPopMatrix();
//                }
        
        ofSetColor(0);
        ofDrawBitmapString(ofToString(biasInc), w/2,h/2);
        
        ofSetColor(255);
        vvFbo.end();
    }
    
    if(guiConsole->getEnabled()){
        consoleFbo.draw(0,0);
    }
    
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
    
    if(key == 'q'){
        linesets[linesets.size()-1] = resizeLine(linesets[linesets.size()-1], 64, 32);
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
