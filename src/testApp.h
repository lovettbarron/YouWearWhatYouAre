#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxCv.h"

#include "face.h"
#include "canvas.h"
#include "projectionManager.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void mouseMoved(int x, int y);
    void keyPressed( int key );
    void debugDraw();
    vector<ofVec3f> points;
    ofTrueTypeFont raleway;
    
    ofxAutoControlPanel panel;
    ofImage thresh;
    ofImage bgThresh;

        //TEST
//   ofImage testMap;
//    ofPolyline testPoly;
//		ofxCv::ContourFinder contourFinder;
        //TEST
    
    ofxProjectionManager* manager;
    
    int panelWidth;
    bool debug;
    
    float threshold;
    ofVideoGrabber cam;
    cv::Mat currentFrame;
    bool showLabels;
    
    //Face track
    ofImage gray, graySmall;
    cv::CascadeClassifier classifier;
    vector<cv::Rect> objects;
    float scaleFactor;
    
    // Face Object
    void saveFace();
        //    vector<ofImage> faces;
    vector<ofFace> faces;
    
    // Canvas object
private:
    ofCanvas * canvas1;
     
};