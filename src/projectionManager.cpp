#include "ofMain.h"
#include "projectionManager.h"

using namespace ofxCv;
using namespace cv;


ofxProjectionManager::ofxProjectionManager()
{
    isConfigHomograph = false; // If currently configuring transform
    saveMatrix = false;
    loadMatrix = false;
    matrixReady = false;

    saveImage = false;
    
    
}

ofxProjectionManager::~ofxProjectionManager() 
{
    
}

/******************************************
    Homographic transforms + img change
*******************************************/
void ofxProjectionManager::updateHomography() {
    
}

void ofxProjectionManager::saveHomography() {
    
}

bool ofxProjectionManager::loadHomography( string * path) {
    return false;
}


/******************************************
 Manage canvases
 *******************************************/
void ofxProjectionManager::add(ofCanvas * canvas) {

}

bool ofxProjectionManager::loadMap(string * path) {
    ofFile previous(*path);
    if(previous.exists()) {
        FileStorage fs(ofToDataPath(*path), FileStorage::READ);
        fs[*path] >> homography;
        matrixReady = true;
    }
    return false;
}

void ofxProjectionManager::parseMap(ofImage * map) {
    
}

void ofxProjectionManager::update() {
    for(int i=0; i<canvas.size(); i++) {
        canvas.draw(0,0);
    }
}

/******************************************
            Core draw and update
*******************************************/

void ofxProjectionManager::draw() {
    if(isConfigHomograph) {
    }
}


void ofxProjectionManager::configure() {
    
}

void ofxProjectionManager::reset() {
    
}
                   
/******************************************
            Events and interface
*******************************************/             
                   
void ofxProjectionManager::mousePressed(int x, int y, int button) {
    if(isConfigHomograph) {
        ofVec2f cur(x, y);
        ofVec2f rightOffset(left.getWidth(), 0);
        if(!movePoint(leftPoints, cur) && !movePoint(rightPoints, cur)) {
           if(x > left.getWidth()) {
               cur -= rightOffset;
           }
           leftPoints.push_back(cur);
           rightPoints.push_back(cur + rightOffset);
        }
    }
}

void ofxProjectionManager::mouseDragged(int x, int y, int button) {
   if(movingPoint) {
       curPoint->set(x, y);
   }
}

void ofxProjectionManager::mouseReleased(int x, int y, int button) {
   movingPoint = false;
}

void testApp::keyPressed(int key) {
   if(key == ' ') {
       saveMatrix = true;
   }
}
