#include "ThreadedCin.h"

#pragma mark - Lifecycle

//Modified from https://gist.github.com/rc1/97c3e88298a9a9341e6e

ThreadedCin::ThreadedCin () {
    latest = "";
}

ThreadedCin::~ThreadedCin () {
    stopThread();
}

#pragma mark - Threads

void ThreadedCin::threadedFunction () {
    
    int cnt;
    
    while( isThreadRunning() && !feof( stdin ) ) {
        
        cnt = read( STDIN_FILENO, buffer, sizeof buffer );
        
        // Remove new line
        buffer[ std::strcspn( buffer, "\n" ) ] = '\0';
        
        lock();
        latest = string( buffer );
        
        if(latest == "READY"){
            ready = true;
        } else{
            ready = false;
        }
        
        if(latest == "END"){
            end = true;
        } else{
            end = false;
        }
        
        lines.push_back(latest);
        
        unlock();
    }
}

bool ThreadedCin::isReady(){
    if (ready) {
        return true;
    } else{
        return false;
    }
}

bool ThreadedCin::isEnd(){
    if (end) {
        return true;
    } else{
        return false;
    }
}

bool ThreadedCin::pingMain(){
    if(ping){
        return true;
    } else{
        return false;
    }
}

string ThreadedCin::getLatest () {
    string result;
    lock();
    result = latest;
    unlock();
    return result;
}

vector<string> ThreadedCin::getAllLines(){
    vector<string> result;
    lock();
    result = lines;
    unlock();
    return result;
}

void ThreadedCin::reset () {
    lock();
    latest = "";
    lines.clear();
    unlock();
}