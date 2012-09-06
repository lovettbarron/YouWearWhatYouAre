#ifndef _canvas_h
#define _canvas_h

#include "ofMain.h"
#include "face.h"
#include "ofxCv.h"

class ofCanvas {
    
public:
    
    ofCanvas(ofBaseApp * base,ofVec3f _pos, ofImage _map, ofPolyline _border);
    ~ofCanvas();
    
    ofBaseApp * app;
    
    void update();
    void draw(int _x, int _y);
    void draw();
    void reset();
    void configure();
    void select();
    void deselect();
    bool selected;
    void add(ofFace * face);
    void testImages();
    bool debug;
    void checkSize();
    int limit;
    ofVec3f mouse;
    ofVec3f pos;
    ofFbo frame;
    ofImage map;
    ofPolyline border;
    void drawBoundingLines();
    ofFace & get(int index);
    int width;
    int height;
    int cx;
    int cy;
    ofVec3f center;
    float scale;
    int size();
    bool bActive;
    long age;
    float padding;
    int newFaceTimer;
    int newFaceTimerThresh;
    vector<ofFace> canvas;
    
    float sizeOnMap(ofFace * _face);
    bool compareWithStillActive( ofImage * _img, ofVec3f * _loc);
    
private:
    void pack();
    float distance(ofFace & face1, ofFace & face2);
    float distance(ofPoint pt1, ofPoint pt2);
        //bool compare(ofFace* face1, ofFace* face2);
};
#endif
