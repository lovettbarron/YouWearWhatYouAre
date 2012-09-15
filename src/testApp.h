#pragma once
// Projection manager for easy configuration times!
// Homographic transform is basically taken from
// Kyle McDonald's ofxCv HomographyExample, either
// copied or changed only slightly.
//  
// This should probably be extending ofBaseApp 
// instead of being a separate class, but will
// consider this after

//#define _TWOCAM 1

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAutoControlPanel.h"
#include "face.h"
#include "canvas.h"

class testApp: public ofBaseApp {
    
public:
        //   virtual void setup();
        //    testApp();
        //    ~testApp();
    void setup();
    
    void update();
    void draw();
    void exit();
    
    vector<ofCanvas*> canvases; // Holds canvases
    vector<ofFace*> faces; // Holds ALL ofFace obj
    vector<ofFace*> trackedFaces; // Holds temp faces to be included in main
    ofImage * map;
    ofImage * mapCopy;
    ofPolyline * poly;
    ofxCv::RunningBackground background;
    cv::Mat prevCVFrame;
    cv::Mat curCVFrame;
    ofVec3f velocity;
    void setupType();
    void reset();
    void updateCamera();
    void configure();
    int numberOfCanvases();
    void updateConditional();
    void setDebug(bool _debug);
    void add(ofVec3f _pos, ofImage _map);
    
    void saveHomography();
    bool loadHomography( string * path );
    void updateHomography();
    bool movePoint(vector<ofVec2f>& points, ofVec2f point);
    
    vector<ofVec2f> destination;
    vector<ofVec2f> source;
    ofVec2f* curPoint;
        //ofCanvas tempCanvas;
    bool loadMap(string * path);
    void parseMap(ofImage * map);
    
        // Canvas frame states
    bool saveImage;
    bool isConfigCanvases;
    float rotation;
    
    ofVec3f skew;
    cv::Mat homography;
    
    ofFbo screen;
    
    vector<ofVec2f> drawnPoints;
    
        // Setup declarations
    vector<ofVec3f> points;
    ofTrueTypeFont raleway;
    void setupPanel();
    ofxAutoControlPanel panel;
    ofImage thresh;
    
    int panelWidth;
    bool debug;
    float fliph;
    float flipv;
    
    float threshold;
#ifdef _TWOCAM
    ofImage cam;
    ofVideoGrabber cam1;
    ofVideoGrabber cam2;
#else
    ofVideoGrabber cam;
    ofImage newFrame;
#endif
    
    cv::Mat currentFrame;
    
        //Face tracking stuff
    ofImage gray, graySmall;
    cv::CascadeClassifier classifier;
    vector<cv::Rect> objects;
    vector<cv::Rect> unfilteredObjects;
    vector<cv::Rect> smoothedObjects;
    int smoothingCounter;
    float objectSmoothingThresh;
    float scaleFactor;
    
    
    
    
    
    
    
        // Methods and variables to track
        // and smooth face rec
    void filterFace(cv::Rect* objects);
    ofPolyline getContour(ofImage * map);
    void delegateToCanvas(ofImage _face, int x, int y, int w, int h);
    int faceSmoothing;
    int panX, panY;
    
    
    
    
        //Events
    void mousePressed( int x, int y, int button);
    void mouseDragged( int x, int y, int button);
    void mouseReleased( int x, int y, int button);
    void keyPressed(int key);
    
private:
    
        // Homography state
    bool isConfigHomograph;
    bool saveMatrix;
    bool loadMatrix;
    bool movingPoint;
    bool matrixReady;
};