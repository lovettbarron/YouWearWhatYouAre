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

void ofxProjectionManager::loadHomography( string * path) {
    
}


/******************************************
 Manage canvases
 *******************************************/
void ofxProjectionManager::add(ofCanvas * canvas) {

}

void ofxProjectionManager::loadMap(string * path) {
    
}

void ofxProjectionManager::parseMap(ofImage * map) {
    
}

void ofxProjectionManager::update() {
    for(int i=0; i<canvas.size; i++) {
        canvas.draw();
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
