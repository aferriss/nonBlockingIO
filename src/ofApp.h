#pragma once

#include "ofMain.h"
#include "ThreadedCin.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        vector<ofPolyline> parseData(vector<string> text, bool showConnections, string phr);
    
    
        int w, h;
    
        ThreadedCin tcin;
        string latest;
        vector<string> allLines;
//        vector<ofVec3f> pointData;
//        ofVec3f point;
    
        ofPolyline polyLine;
        vector<ofPolyline> polyLines;
        bool needsUpdate;
        int allLinesSize;
        bool rdy, eos;
        vector<ofPolyline> lineList;
        vector<ofRectangle> allDims;
        ofxDatGui* gui;
        ofxDatGuiTextInput* guiPhrase;
        ofxDatGuiSlider* guiBias;
        ofxDatGuiSlider* guiId;
        ofxDatGuiToggle* guiConnections;
        ofxDatGuiToggle* guiConsole;
        ofxDatGuiToggle* guiAllLines;
        ofxDatGuiSlider* guiMaxW;
        ofxDatGuiSlider* guiMaxH;
    
        bool showGui;
        bool useGuiVals;
        bool showConsole();
    
        ofRectangle dims;
    
        ofFbo consoleFbo, vvFbo;
        vector< vector<ofPolyline> > vvLines;
};
