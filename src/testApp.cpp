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

    panel.addPanel("Global Tweaks");
    panel.addSlider("circleResolution", 64, 4, 128, true);
    panel.addSlider("simplifyPolyline",1.,0.01,1.0,false);

    panel.addPanel("Behaviour Tweaks");
    panel.addSlider("makeInactive", 30, 1, 120, true);
    panel.addSlider("faceDetectSmothing", 64, 1, 128, true);
    panel.addSlider("distanceThresh",15,1,100,true);    
    
    ofBackground(0);
    
    // Allocate and setup openCV methods
    cam.initGrabber(640, 480);
    thresh.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    bgThresh.allocate(640,480,OF_IMAGE_GRAYSCALE);
    scaleFactor = panel.getValueF("faceScale");
    classifier.load(ofToDataPath("haarcascade_frontalface_alt2.xml"));
    graySmall.allocate(cam.getWidth() * scaleFactor, cam.getHeight() * scaleFactor, OF_IMAGE_GRAYSCALE);
    debug = false;
    
    ofImage jacket;
    jacket.loadImage("test_thresh.jpg");
    ofVec3f loc = ofVec3f(0,0,0);
    ofxProjectionManager manager = *new ofxProjectionManager();
    manager.add(&loc, &jacket);
}

void testApp::update() {
    // Reallocates and scales the image being passed to the classifier
    if(scaleFactor != panel.getValueF("faceScale")) {
            graySmall.allocate(cam.getWidth() * panel.getValueF("faceScale"), cam.getHeight() * panel.getValueF("faceScale"), OF_IMAGE_GRAYSCALE);
    }
    
        //scaleFactor = manager->scaleFactor = panel.getValueF("faceScale");
    
    if(panel.getValueB("debug")) debug = true;
    else debug = false;
    
    if(panel.getValueB("resetFaces")) {
        faces.clear();
        panel.setValueB("resetFaces",false);
    }
    if(panel.getValueB("add100Faces")) {
        for(int i=0;i<manager->numberOfCanvases();i++) {
                //    manager->canvases->at(i).testImages();
        }
            //        canvas->testImages();
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
            // saveFace();
        ofImage newFrame;
        newFrame.setFromPixels(cam.getPixels(), cam.width, cam.height, OF_IMAGE_COLOR);
        manager->update(&newFrame,&objects);
            ///        canvas1->update();   
    }
}

void testApp::draw() {
    
        //ofSetBackgroundAuto(true);
    ofBackground(100);
    ofPushMatrix();
        //ofTranslate(panelWidth, 0);
        ofSetColor(255);
        debugDraw();
        ofNoFill();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    manager->draw();
    ofPopMatrix(); // For panel
}

void testApp::debugDraw() {
    if(debug) {
        cam.draw(0, 0);
        //contourFinder.draw();
        
        graySmall.draw(cam.width, 0, 2, 256,192);
            //testMap.draw(cam.width, 192, 2, 256,192);
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
            //        saveFace();
    }
}

void testApp::mouseMoved(int x, int y ){
    
}

void testApp::saveFace() {
/*    faces.clear();
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
    }*/
}


