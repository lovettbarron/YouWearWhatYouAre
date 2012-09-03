#include "testApp.h"

using namespace cv;
using namespace ofxCv;

void testApp::setup() {
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofTrueTypeFont::setGlobalDpi(72);
    raleway.loadFont("Raleway-Thin.ttf", 32, true, true);
    raleway.setLineHeight(18.0f);
    raleway.setLetterSpacing(1.037);
    
        //Setup panel
    panelWidth = 200;
    panel.setup(panelWidth, 800);
    panel.addPanel("Tracking Bits");
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
    
    ofBackground(0);
    
    // Allocate and setup openCV methods
    cam.initGrabber(640, 480);
    thresh.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    bgThresh.allocate(640,480,OF_IMAGE_GRAYSCALE);
    scaleFactor = panel.getValueF("faceScale");
    classifier.load(ofToDataPath("haarcascade_frontalface_alt2.xml"));
    graySmall.allocate(cam.getWidth() * scaleFactor, cam.getHeight() * scaleFactor, OF_IMAGE_GRAYSCALE);
    debug = false;
    
    testMap.loadImage("test_thresh.jpg");
    ofImage testMapGray;
    ofImage testMapCopy = testMap;
    testMapGray.allocate(testMap.width, testMap.height, OF_IMAGE_GRAYSCALE);
    int grayIndex = 0;
    // Create the grayscale image based off the 100% green
/*    for (int i = 0; i < testMap.getPixelsRef().size()-3; i+=3){
        if(testMap.getPixelsRef()[i+1] >= 250) {
            testMap.getPixelsRef()[(i)] = 255;
            testMap.getPixelsRef()[(i+1)] = 255;
            testMap.getPixelsRef()[(i+2)] = 255;
            
        } else {
            testMap.getPixelsRef()[(i)] = 0;
            testMap.getPixelsRef()[(i+1)] = 0;
            testMap.getPixelsRef()[(i+2)] = 0;
        }
    }*/
    contourFinder.setTargetColor(ofColor(0,255,0), TRACK_COLOR_RGB);
    contourFinder.setInvert(true);
    contourFinder.setThreshold(127);
        //    contourFinder.setTargetColor(ofColor(0,255,0));
        //    cv::Mat contourImage;
        //    cv::threshold(testMap, contourImage);
        //    convertColor(testMap,contourImage,CV_RGB2GRAY);
        //    autothreshold(contourImage);
    contourFinder.findContours(testMap);
    ofLog() << "Number of polylines: " << ofToString(contourFinder.size());
    if(contourFinder.size() != 0 ) {
        vector<ofPolyline> polylines;
        polylines = contourFinder.getPolylines();
        for(int i=0; i<polylines.size(); i++) {
            ofLog() << "Polyline" << ofToString(i) << " has " << ofToString(polylines[i].size());
            if(i==0) testPoly = polylines[i];
            if(polylines[i].size() >= testPoly.size()) testPoly = polylines[i];
        }
        ofLog() << "Found contours: " << ofToString(testPoly.size());
    } else {
        ofLog() << "Defaulting to image box";
        testPoly.addVertex(ofVec2f(0,0));
        testPoly.addVertex(ofVec2f(ofGetWidth(),0));
        testPoly.addVertex(ofVec2f(ofGetWidth(), ofGetHeight()));
        testPoly.addVertex(ofVec2f(0, ofGetHeight()));
    }
    
    canvas1 = new ofCanvas(ofVec3f(0,0,0), testMap, testPoly);
}

void testApp::update() {
    // Reallocates and scales the image being passed to the classifier
    if(scaleFactor != panel.getValueF("faceScale")) {
            graySmall.allocate(cam.getWidth() * panel.getValueF("faceScale"), cam.getHeight() * panel.getValueF("faceScale"), OF_IMAGE_GRAYSCALE);
    }
    
    scaleFactor = panel.getValueF("faceScale");
    
    if(panel.getValueB("debug")) debug = true;
    else debug = false;
    
    if(panel.getValueB("resetFaces")) {
        faces.clear();
        panel.setValueB("resetFaces",false);
    }
    if(panel.getValueB("add100Faces")) {
        canvas1->testImages();
        panel.setValueB("add100Faces",false);
    }
    //Updates camera
    cam.update();
    if(cam.isFrameNew()) {

        //Face Tracking stuff
        convertColor(cam, gray, CV_RGB2GRAY);
        resize(gray, graySmall);
        Mat graySmallMat = toCv(graySmall);
        if(ofGetMousePressed()) {
            equalizeHist(graySmallMat, graySmallMat);
        }
        graySmall.update();
        
        /*classifier.detectMultiScale(graySmallMat, objects, 1.06, 1,
                                    //CascadeClassifier::DO_CANNY_PRUNING |
                                    //CascadeClassifier::FIND_BIGGEST_OBJECT |
                                    //CascadeClassifier::DO_ROUGH_SEARCH |
                                    0); */
        classifier.detectMultiScale(graySmallMat, objects, panel.getValueF("scaleFactor"), panel.getValueI("minNeighbors"), 0, cv::Size(panel.getValueI("minSize"),panel.getValueI("minSize")), cv::Size(panel.getValueI("maxSize"),panel.getValueI("maxSize")));
        saveFace();
        canvas1->update();   
    }
}

void testApp::draw() {
    
        //ofSetBackgroundAuto(true);
    ofBackground(100);
    ofPushMatrix();
        ofTranslate(panelWidth, 0);
        ofSetColor(255);
        debugDraw();
        ofNoFill();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
        canvas1->draw(0,0);
    ofPopMatrix(); // For panel
}

void testApp::debugDraw() {
    if(debug) {
        cam.draw(0, 0);
        //contourFinder.draw();
        
        graySmall.draw(cam.width, 0, 2, 256,192);
        testMap.draw(cam.width, 192, 2, 256,192);
        ofNoFill();
        ofPushMatrix();
        ofScale(1 / scaleFactor, 1 / scaleFactor);
        for(int i = 0; i < objects.size(); i++) {
            ofLog() << "Drawing face #" << ofToString(i);
            ofRect(toOf(objects[i]));
        }
        ofPopMatrix();
    }
}

void testApp::keyPressed(int key) {
    if(key == ' ') {
        saveFace();
    }
}

void testApp::mouseMoved(int x, int y ){
    
}

void testApp::saveFace() {
    faces.clear();
    for(int i=0; i < objects.size(); i++) {   
        
        ofImage newFace;
        newFace.allocate((int)objects[i].width * (1 / scaleFactor), (int)objects[i].height * (1 / scaleFactor), OF_IMAGE_COLOR);
        int x = objects[i].x * (1 / scaleFactor);
        int y = objects[i].y * (1 / scaleFactor);
        int w = objects[i].width * (1 / scaleFactor);
        int h = objects[i].height * (1 / scaleFactor);
            //ofLog() << "x" << ofToString(x) << " y" << ofToString(y) << " w" << ofToString(w) << " h" << ofToString(h);
        
        ofImage pixels;
        pixels.setFromPixels(cam.getPixels(), cam.width, cam.height, OF_IMAGE_COLOR);
        newFace.cropFrom(pixels, x, y, w, h );
        newFace.reloadTexture();
        
        ofFace theFace = ofFace(newFace, ofVec3f(x + (w/2), y + (h/2),0),ofVec3f(canvas1->width+ofRandom(-5,5),canvas1->height+ofRandom(-5,5),0));
        faces.push_back( theFace );
        
        canvas1->compareWithStillActive( &faces );
        
            // canvas1->add( &theFace );
    }
}


