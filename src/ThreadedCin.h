#pragma once

#include "ofMain.h"

class ThreadedCin : public ofThread {
public:
    // # Lifecycle
    ThreadedCin ();
    virtual ~ThreadedCin ();
    
    // # Threads
    void threadedFunction();
    
    string getLatest();
    vector<string> getAllLines();
    void reset();

private:
    char buffer[ 8192 ];
    string latest;
    vector<string> lines;
};