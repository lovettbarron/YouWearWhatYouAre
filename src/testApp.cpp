#include "testApp.h"

using namespace ofxCv;
using namespace cv;

//
//testApp::testApp()
//{    
//    isConfigHomograph = false; // If currently configuring transform
//    saveMatrix = false;
//    loadMatrix = false;
//    matrixReady = false;
//    movingPoint = false;
//    saveImage = false;
//    scaleFactor = 1.0;
//        //ofCanvas tempCanvas;
//    vector<ofFace*> faces;
//    vector<ofCanvas*> canvases;
//    canvases = *new vector<ofCanvas*>;
//}
//
//testApp::~testApp() 
//{
//    
//}

void testApp::setup() {
    setupType();
    setupPanel();
    ofBackground(0);
    
        // Allocate and setup openCV methods
    cam.initGrabber(640, 480);
    thresh.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    bgThresh.allocate(640,480,OF_IMAGE_GRAYSCALE);
    scaleFactor = panel.getValueF("faceScale");
    classifier.load(ofToDataPath("haarcascade_frontalface_alt2.xml"));
    graySmall.allocate(cam.getWidth() * scaleFactor, cam.getHeight() * scaleFactor, OF_IMAGE_GRAYSCALE);
    debug = false;
        //    velocity = ofVec3F(1.1,1.1,0);
    
    // Make first canvas
    ofImage jacket;
    jacket.loadImage("test_thresh.jpg");
    jacket.reloadTexture();
    ofVec3f loc = ofVec3f(0,0,0);
    add(loc,jacket);
    loc = ofVec3f(jacket.width,0,0);
    add(loc,jacket);
    loc = ofVec3f(jacket.width,0,0);
    add(loc,jacket);
}


/******************************************
 Manage canvases
 *******************************************/
    // Add a canvas to the vector from image and position.
void testApp::add(ofVec3f _pos, ofImage _map) {
    canvases.push_back( new ofCanvas( ofVec3f(0,0,0), _map, getContour(&_map)) );
    ofLog() << "Canvases added, size " << ofToString(canvases.size());
}

ofPolyline testApp::getContour(ofImage * map) {
    ofPolyline poly;
        //    ofImage * mapCopy = new ofImage;
        //    mapCopy->clone(*map);
        //    mapCopy->allocate(map->width, map->height, OF_IMAGE_GRAYSCALE);
    contourFinder.setTargetColor(ofColor(0,255,0), TRACK_COLOR_RGB);
    contourFinder.setInvert(true);
    contourFinder.setThreshold(127);
    contourFinder.findContours(toCv(*map));
    
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
    } 
    
    return poly;
}


    // Update takes in new frame data and passes it to the canvas
void testApp::update() {
    updateCamera();
    
        // convertColor(graySmall, curCVFrame,CV_8UC1);
    // Use Lucas Kanade method to track faces across time
    int width = cam.getWidth() * scaleFactor;
    int height = cam.getHeight() * scaleFactor;
//    cv::Mat velX = cvCreateImage( cvSize( width ,height ), IPL_DEPTH_32F, 1  );
//    cv::Mat velY = cvCreateImage( cvSize( width ,height ), IPL_DEPTH_32F, 1  );
    
/*    buildOpticalFlowPyramid(graySmall)
     calcOpticalFlowPyrLK(prevCVFrame,graySmall,cam.getWidth() * scaleFactor, (CvArr)velX, (CvArr)velY);
    prevCVFrame = curCVFrame.clone(); */
    
    for(int i=0; i < objects.size(); i++) {   
        cv::Rect obj = objects[i];
        filterFace(&obj);
    }
    for(int i=0;i<canvases.size();i++) {
        canvases[i]->update();
    }
}

void testApp::filterFace(cv::Rect * objects) {
    ofImage newFace;
    newFace.allocate((int)objects->width * (1 / scaleFactor), (int)objects->height * (1 / scaleFactor), OF_IMAGE_COLOR);
    int x = objects->x * (1 / scaleFactor);
    int y = objects->y * (1 / scaleFactor);
    int w = objects->width * (1 / scaleFactor);
    int h = objects->height * (1 / scaleFactor);
    
    newFace.cropFrom(newFrame, x, y, w, h );
    newFace.reloadTexture();
    
    delegateToCanvas(newFace,x,y,w,h);
}

void testApp::delegateToCanvas(ofImage _face, int x, int y, int w, int h) {
    int index = 0;
    for(int i=0;i<canvases.size();i++) {
        if(canvases[i]->size() < canvases[index]->size()) {
            index = i;
        }
    }
    if(canvases.size()>0) {
            // The new face
        ofFace theFace = ofFace(_face, ofVec3f(x + (w/2), y + (h/2),0),ofVec3f(canvases[index]->cx+ofRandom(-5,5),canvases[index]->cy+ofRandom(-5,5),0),canvases[index]->width/2);
        // This creates a pointer to faces stored in canvases
        vector<ofFace>* faces = &canvases[index]->canvas; 
        faces->push_back( theFace );
            //        canvases[index].compareWithStillActive( &faces )
    }
}

/******************************************
 Core draw and update
 *******************************************/

void testApp::draw() {
        //    ofLog() << "Size of array" << ofToString(canvases.size());
    
    ofPushMatrix();
    ofScale(panel.getValueF("scaleWindow"),panel.getValueF("scaleWindow"));
    for(int i=0; i<canvases.size(); i++) {
        if(i!=0) ofTranslate(canvases[i-1]->width*2);
        canvases[i]->draw();
            //        ofLog() << "Drawing canvases" << ofToString(i);
    }
    ofPopMatrix();
    
    
    
    
    GLboolean isDepthTesting;
    glGetBooleanv(GL_DEPTH_TEST, &isDepthTesting);
    if(isDepthTesting == GL_TRUE)
        glDisable(GL_DEPTH_TEST);
}


void testApp::configure() {
    
}

void testApp::reset() {
    
}

/******************************************
 Events and interface
 *******************************************/             

void testApp::mousePressed(int x, int y, int button) {
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
            //        for(int i=0;i<canvases->size();i++) {
            //            ofVec3f loc = canvases->at(i).pos;
            //            if(loc.x < x && loc.x+canvases->at(i).width > x) {
            //                if(loc.y < y && loc.y+canvases->at(i).height > y) {
            //                    canvases->at(i).select();
            //                }
            //            }
            //        }
    }
}

void testApp::mouseDragged(int x, int y, int button) {
    if(movingPoint) {
            //   curPoint->set(x, y);
    }
    if(isConfigCanvases) {
            //        for(int i=0;i<canvases->size();i++) {
            //            ofVec3f loc = canvases->at(i).pos;
            //            if(loc.x < x && loc.x+canvases->at(i).width > x) {
            //                if(loc.y < y && loc.y+canvases->at(i).height > y) {
            //                    canvases->at(i).select();
            //                }
            //            }
            //        }
    }
}

int testApp::numberOfCanvases() {
    return canvases.size();
}

void testApp::mouseReleased(int x, int y, int button) {
    movingPoint = false;
}

void testApp::keyPressed(int key) {
    if(key == ' ') {
        saveMatrix = true;
    }
}

/******************************************
 Homographic transforms + img change
 *******************************************/
void testApp::updateHomography() {
    
}

void testApp::saveHomography() {
    
}

bool testApp::loadHomography( string * path) {
    return false;
}

bool testApp::movePoint(vector<ofVec2f>& points, ofVec2f point) {
    for(int i = 0; i < points.size(); i++) {
        if(points[i].distance(point) < 20) {
            movingPoint = true;
            curPoint = &points[i];
            return true;
        }
    }
    return false;
}


bool testApp::loadMap(string * path) {
    ofFile previous(*path);
    if(previous.exists()) {
        FileStorage fs(ofToDataPath(*path), FileStorage::READ);
        fs[*path] >> homography;
        matrixReady = true;
    }
    return false;
}

void testApp::parseMap(ofImage * map) {
    
}

/******************************************
 Various setup functions 
 *******************************************/

void testApp::updateConditional() {
    if(scaleFactor != panel.getValueF("faceScale")) {
        graySmall.allocate(cam.getWidth() * panel.getValueF("faceScale"), cam.getHeight() * panel.getValueF("faceScale"), OF_IMAGE_GRAYSCALE);
    }
    
    scaleFactor = panel.getValueF("faceScale");
    
    if(panel.getValueB("debug")) setDebug(true);
    else setDebug(false);
    
    if(panel.getValueB("resetFaces")) {
        faces.clear();
        panel.setValueB("resetFaces",false);
    }
    if(panel.getValueB("add100Faces")) {
        for(int i=0;i<canvases.size();i++) {
            canvases[i]->testImages();
        }
         
        panel.setValueB("add100Faces",false);
    }
    
    if(panel.hasValueChanged("circleResolution")) {
        /*        for(int i=0; canvas1->size(); i++) {
         canvas1->get(i).resolution = panel.getValueI("circleResolution");
         //                canvas1->panel.getValueI("makeInactive");
         }*/
    }
    if(panel.hasValueChanged("circleResolution")) {
            //        panel.getValueF("simplifyPolyline");
    }
    
    panel.getValueI("makeInactive");
    
    panel.getValueF("faceDetectSmothing");
    
    panel.getValueI("distanceThresh");    
}

void testApp::setupPanel() {
        //Setup panel
    panelWidth = 200;
    panel.setup(panelWidth, 800);
    panel.addPanel("Tracking Bits");
    panel.addLabel("Main Window");
    panel.addSlider("scaleWindow", 1.0, 0.05, 2.0, false);
    panel.addSlider("scaleTop", 1.0, 0.05, 1.0, false);
    panel.addSlider("scaleBottom", 1.0, 0.05, 1.0, false);
    panel.addLabel("Image Processing");
    panel.addSlider("faceScale", .2, 0.05, 1.0, false);
    panel.addSlider("minAreaRadius", 7, 0, 640, true);
    panel.addSlider("maxAreaRadius", 100, 0, 640, true);
    
    panel.addLabel("DetectMultiScale tweak");
    panel.addSlider("scaleFactor",1.06, 1.01,2.0,false);
    panel.addSlider("minNeighbors",1, 1, 5,true);
    panel.addSlider("minSize",0, 1, 400,true);
    panel.addSlider("maxSize",100, 1, 400,true);
    
    panel.addLabel("Debug switches");
    panel.addToggle("resetFaces", false);
    panel.addToggle("add100Faces", false);
    panel.addToggle("debug",false);
    
    panel.addPanel("Global Tweaks");
    panel.addSlider("circleResolution", 64, 4, 128, true);
    panel.addSlider("simplifyPolyline",1.,0.01,1.0,false);
    
    panel.addPanel("Behaviour Tweaks");
    panel.addSlider("makeInactive", 30, 1, 120, true);
    panel.addSlider("faceDetectSmothing", 64, 1, 128, true);
    panel.addSlider("distanceThresh",15,1,100,true);    
}

void testApp::setupType() {
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofTrueTypeFont::setGlobalDpi(72);
    raleway.loadFont("Raleway-Thin.ttf", 32, true, true);
    raleway.setLineHeight(18.0f);
    raleway.setLetterSpacing(1.037);
}

void testApp::updateCamera() {
    cam.update();
    if(cam.isFrameNew()) {
        
        convertColor(cam, gray, CV_RGB2GRAY);
        resize(gray, graySmall);
        Mat graySmallMat = toCv(graySmall);
        if(ofGetMousePressed()) {
            equalizeHist(graySmallMat, graySmallMat);
        }
        graySmall.update();
        
        classifier.detectMultiScale(graySmallMat, objects, panel.getValueF("scaleFactor"), panel.getValueI("minNeighbors"), 0, cv::Size(panel.getValueI("minSize"),panel.getValueI("minSize")), cv::Size(panel.getValueI("maxSize"),panel.getValueI("maxSize")));
        newFrame.setFromPixels(cam.getPixels(), cam.width, cam.height, OF_IMAGE_COLOR);
        
    }
}

void testApp::setDebug(bool _debug) {
    for(int i=0;i<canvases.size();i++) {
        canvases[i]->debug = _debug;
    }
}