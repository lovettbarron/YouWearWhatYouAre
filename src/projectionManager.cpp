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
    movingPoint = false;
    saveImage = false;
    scaleFactor = 1.0;
    canvases = new vector<ofCanvas>;
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

bool ofxProjectionManager::movePoint(vector<ofVec2f>& points, ofVec2f point) {
    for(int i = 0; i < points.size(); i++) {
        if(points[i].distance(point) < 20) {
            movingPoint = true;
            curPoint = &points[i];
            return true;
        }
    }
    return false;
}

/******************************************
 Manage canvases
 *******************************************/
    // Add a canvas to the vector from image and position.
void ofxProjectionManager::add(ofVec3f* _pos, ofImage* _map) {
    ofImage map = *_map;
    ofImage mapCopy = *_map;
    
    ofPolyline poly;
    mapCopy.allocate(map.width, map.height, OF_IMAGE_GRAYSCALE);
    contourFinder.setTargetColor(ofColor(0,255,0), TRACK_COLOR_RGB);
    contourFinder.setInvert(true);
    contourFinder.setThreshold(127);
    contourFinder.findContours(map);
    
    ofLog() << "Number of polylines: " << ofToString(contourFinder.size());

    if(contourFinder.size() != 0 ) {
        vector<ofPolyline> polylines;
        polylines = contourFinder.getPolylines();
        for(int i=0; i<polylines.size(); i++) {
            ofLog() << "Polyline" << ofToString(i) << " has " << ofToString(polylines[i].size());
            if(i==0) poly = polylines[i];
            if(polylines[i].size() >= poly.size()) poly = polylines[i];
        }
        ofLog() << "Found contours: " << ofToString(poly.size());
    } else {
        ofLog() << "Defaulting to image box";
        poly.addVertex(ofVec2f(0,0));
        poly.addVertex(ofVec2f(ofGetWidth(),0));
        poly.addVertex(ofVec2f(ofGetWidth(), ofGetHeight()));
        poly.addVertex(ofVec2f(0, ofGetHeight()));
    }
    ofCanvas tempCanvas = *new ofCanvas(ofVec3f(0,0,0),map,poly);
    canvases->push_back(tempCanvas);
    ofLog() << "Canvases added, size " << ofToString(canvases->size());
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

    // Update takes in new frame data and passes it to the canvas
void ofxProjectionManager::update(ofImage* _cam, vector<cv::Rect>* _obj) {
    
    
    for(int i=0; i < _obj->size(); i++) {   
        cv::Rect obj = _obj->at(i);
        filterFace(_cam,&obj);
    }
    for(int i=0;i<canvases->size();i++) {
        canvases->at(i).update();
    }
}

void ofxProjectionManager::filterFace(ofImage* cam, cv::Rect * objects) {
    ofImage newFace;
    newFace.allocate((int)objects->width * (1 / scaleFactor), (int)objects->height * (1 / scaleFactor), OF_IMAGE_COLOR);
    int x = objects->x * (1 / scaleFactor);
    int y = objects->y * (1 / scaleFactor);
    int w = objects->width * (1 / scaleFactor);
    int h = objects->height * (1 / scaleFactor);
        //ofLog() << "x" << ofToStrirng(x) << " y" << ofToString(y) << " w" << ofToString(w) << " h" << ofToString(h);
    
    ofImage pixels;
    pixels.setFromPixels(cam->getPixels(), cam->width, cam->height, OF_IMAGE_COLOR);
    newFace.cropFrom(pixels, x, y, w, h );
    newFace.reloadTexture();
    
    delegateToCanvas(newFace,x,y,w,h);
}

void ofxProjectionManager::delegateToCanvas(ofImage _face, int x, int y, int w, int h) {
    int index = 0;
    for(int i=0;i<canvases->size();i++) {
        if(canvases[i].size() < canvases[index].size()) {
            index = i;
        }
    }
    if(canvases->size()>0) {
            // The new face
        ofFace theFace = ofFace(_face, ofVec3f(x + (w/2), y + (h/2),0),ofVec3f(canvases->at(index).width+ofRandom(-5,5),canvases->at(index).height+ofRandom(-5,5),0));
        vector<ofFace>* faces = &canvases->at(index).canvas; // Faces stored in canvases
        faces->push_back( theFace );
            //        canvases[index].compareWithStillActive( &faces )
    }
}

/******************************************
            Core draw and update
*******************************************/

void ofxProjectionManager::draw() {
    ofLog() << "Size of array" << ofToString(canvases->size());
    for(int i=0; i<canvases->size(); i++) {
        canvases->at(i).draw(0,0);
        ofLog() << "Drawing canvases" << ofToString(i);
    }
    
    
    
    if(isConfigHomograph) {
        
    }
    GLboolean isDepthTesting;
    glGetBooleanv(GL_DEPTH_TEST, &isDepthTesting);
    if(isDepthTesting == GL_TRUE)
        glDisable(GL_DEPTH_TEST);
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
        ofVec2f rightOffset(screen.getWidth(), 0);
        if(!movePoint(source, cur) && !movePoint(destination, cur)) {
           if(x > screen.getWidth()) {
               cur -= rightOffset;
           }
           source.push_back(cur);
           destination.push_back(cur + rightOffset);
        }
    }
    
    if(isConfigCanvases) {
        for(int i=0;i<canvases->size();i++) {
            ofVec3f loc = canvases->at(i).pos;
            if(loc.x < x && loc.x+canvases->at(i).width > x) {
                if(loc.y < y && loc.y+canvases->at(i).height > y) {
                    canvases->at(i).select();
                }
            }
        }
    }
}

void ofxProjectionManager::mouseDragged(int x, int y, int button) {
   if(movingPoint) {
       curPoint->set(x, y);
   }
    if(isConfigCanvases) {
        for(int i=0;i<canvases->size();i++) {
            ofVec3f loc = canvases->at(i).pos;
            if(loc.x < x && loc.x+canvases->at(i).width > x) {
                if(loc.y < y && loc.y+canvases->at(i).height > y) {
                    canvases->at(i).select();
                }
            }
        }
    }
}

int ofxProjectionManager::numberOfCanvases() {
    return canvases->size();
}

void ofxProjectionManager::mouseReleased(int x, int y, int button) {
   movingPoint = false;
}

void ofxProjectionManager::keyPressed(int key) {
   if(key == ' ') {
       saveMatrix = true;
   }
}
