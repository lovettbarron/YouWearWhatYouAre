// Projection manager for easy configuration times!
// Homographic transform is basically taken from
// Kyle McDonald's ofxCv HomographyExample, either
// copied or changed only slightly.
//  
// This should probably be extending ofBaseApp 
// instead of being a separate class, but will
// consider this after


#ifndef _projManager_h
#define _projManager_h

#include "ofMain.h"
#include "ofxCv.h"
#include "face.h"
#include "canvas.h"

class ofxProjectionManager {
    
public:
    
    ofxProjectionManager();
    ~ofxProjectionManager();
    
    void update();
    void draw();
    void reset();
    void configure();
    //Add uses contour finder to gen the poly line
    void add(ofVec3f* _pos, ofImage* _map);
    ofxCv::ContourFinder contourFinder;
    
    void saveHomography();
    bool loadHomography( string * path );
    void updateHomography();
    bool movePoint(vector<ofVec2f>& points, ofVec2f point);
    
    vector<ofVec2f> destination;
    vector<ofVec2f> source;
    ofVec2f* curPoint;
    
    bool loadMap(string * path);
    void parseMap(ofImage * map);
    
    void mousePressed( int x, int y, int button);
    void mouseDragged( int x, int y, int button);
    void mouseReleased( int x, int y, int button);
    void keyPressed(int key);
    
    // Homography state
    bool isConfigHomograph;
    bool saveMatrix;
    bool loadMatrix;
    bool movingPoint;
    bool matrixReady;
    
    // Canvas frame states
    bool saveImage;
    
    float rotation;

    ofVec3f skew;
    cv::Mat homography;
    
    ofFbo screen;
    
    vector<ofVec2f> drawnPoints;
    vector<ofPolyline> canvasShapes;
    vector<ofCanvas> canvas;
    
    
};
#endif
