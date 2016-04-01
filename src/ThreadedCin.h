#pragma once

#include "ofMain.h"
//Modified from https://gist.github.com/rc1/97c3e88298a9a9341e6e

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