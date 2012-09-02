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
    void add(ofCanvas * _canvas);
    void saveHomography();
    void loadHomography( string * path );
    
    void loadMap(string * path);
    void parseMap(ofImage * map);
    
    void mousePressed( int x, int y, int button);
    void mouseDragged( int x, int y, int button);
    void mouseReleased( int x, int y, int button);
    
    void updateHomography();
    
    bool isConfigHomograph;
    bool saveMatrix;
    bool loadMatrix;
    bool matrixReady;
    bool saveImage;
    
    float rotation;

    ofVec3f skew;
    cv::Mat homography;
    
    vector<ofVec2f> drawnPoints;
    vector<ofPolyline> canvasShapes;
    vector<ofCanvas> canvas;
    
    
};
#endif
