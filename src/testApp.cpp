#include "testApp.h"

using namespace ofxCv;
using namespace cv;


void testApp::setup() {
    setupType();
    setupPanel();
    ofBackground(0);
    scaleFactor = panel.getValueF("faceScale");
        // Allocate and setup openCV methods
#ifdef _TWOCAM
    cam1.listDevices();
    cam1.setDeviceID(2);
    cam1.initGrabber(320, 240);
    cam2.setDeviceID(3);
    cam2.initGrabber(cam1.getWidth(),cam1.getHeight());
    
    cam.allocate(cam1.getWidth()+cam2.getWidth(),cam1.getHeight(), OF_IMAGE_COLOR);
#else
    cam.initGrabber(320,240);
#endif
    graySmall.allocate(cam.getWidth() * scaleFactor, cam.getHeight() * scaleFactor, OF_IMAGE_GRAYSCALE);
    
    classifier.load(ofToDataPath("haarcascade_frontalface_alt2.xml"));
    
    debug = false;
    smoothingCounter = 3;

    background.setLearningTime(900);
    background.setThresholdValue(10);

    
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
    canvases.push_back( new ofCanvas( this, ofVec3f(0,0,0), _map) );
    ofLog() << "Canvases added, size " << ofToString(canvases.size());
}


    // Update takes in new frame data and passes it to the canvas
void testApp::update() {
    updateConditional();
    updateCamera();
    
    // Using Lucas Kanade method to track faces across time
    // would be ideal for this application, maybe later?
    
    /*convertColor(graySmall, curCVFrame,CV_8UC1);
    int width = cam.getWidth() * scaleFactor;
    int height = cam.getHeight() * scaleFactor;
    cv::Mat velX = cvCreateImage( cvSize( width ,height ), IPL_DEPTH_32F, 1  );
    cv::Mat velY = cvCreateImage( cvSize( width ,height ), IPL_DEPTH_32F, 1  );
    
    buildOpticalFlowPyramid(graySmall)
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
    
    // Check to see if it's a previous face location
    newFace.cropFrom(newFrame, x, y, w, h );
    newFace.reloadTexture();
    ofVec3f _loc = ofVec3f(x+w/2, y+h/2,0);
    bool isNew = true;
    for(int i=0;i<canvases.size();i++) {
        // Don't run if false, because a face can only be "new" once
        if(isNew != false)
            isNew = canvases[i]->compareWithStillActive( &newFrame, &_loc );
    }
    if(isNew) {
        delegateToCanvas(newFace,x,y,w,h);
    }
}

void testApp::delegateToCanvas(ofImage _face, int x, int y, int w, int h) {
    int index = 0;
    for(int i=0;i<canvases.size();i++) {
        if(canvases[i]->size() < canvases[index]->size()) {
            index = i;
        }
    }
    if(canvases.size()>0) {
        // Create the new face from the passed image
        ofFace theFace = ofFace(_face
                                , ofVec3f(x + (w/2), y + (h/2),0)
                                ,ofVec3f(canvases[index]->cx+ofRandom(-5,5)
                                ,canvases[index]->cy+ofRandom(-5,5),0)
                                ,20*4);
        
        canvases[index]->canvas.push_back( theFace );
    }
}

/******************************************
 Core draw and update
 *******************************************/

void testApp::draw() {
        //    ofBackground(1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    ofPushMatrix();
        ofTranslate(panX,panY);
        ofScale(panel.getValueF("scaleWindow"),panel.getValueF("scaleWindow"));
        for(int i=0; i<canvases.size(); i++) {
            if(i!=0) ofTranslate(canvases[i-1]->width*2);
            canvases[i]->draw();
        }
    ofPopMatrix();
    
    if(debug) {
        ofPushMatrix();
            ofNoFill();
            ofTranslate(ofGetWidth()-cam.width,0);
            cam.draw(0,0);
            graySmall.draw(0,cam.height);
            ofScale(1 / scaleFactor, 1 / scaleFactor);
            for(int i = 0; i < objects.size(); i++) {
                    //ofLog() << "Drawing face #" << ofToString(i);
                ofRect(toOf(objects[i]));
            }
        ofPopMatrix();
    }
       
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
        background.reset();
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
    
    panX = panel.getValueF("panX");
    panY = panel.getValueF("panY");
    
    if(panel.getValueB("debug")) setDebug(true);
    else setDebug(false);
    
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
    panel.addLabel("Debug switches");
    panel.addToggle("add100Faces", false);
    panel.addToggle("debug",false);
    
    panel.addLabel("Main Window");
    panel.addSlider("scaleWindow", 1.0, 0.005, 1.0, false);
    panel.addSlider("scaleTop", 1.0, 0.05, 1.0, false);
    panel.addSlider("scaleBottom", 1.0, 0.05, 1.0, false);
    panel.addLabel("Image Processing");
    panel.addSlider("faceScale", .2, 0.05, 1.0, false);
    
    panel.addLabel("DetectMultiScale tweak");
    panel.addSlider("scaleFactor",1.06, 1.01,2.0,false);
    panel.addSlider("minNeighbors",1, 1, 5,true);
    panel.addSlider("minSize",0, 1, 400,true);
    panel.addSlider("maxSize",100, 1, 400,true);
    
    panel.addPanel("Global Tweaks");
    panel.addSlider("circleResolution", 64, 4, 128, true);
    panel.addSlider("simplifyPolyline",1.,0.01,1.0,false);
    
    panel.addPanel("Behaviour Tweaks");
    panel.addSlider("makeInactive", 30, 1, 120, true);
    panel.addSlider("faceDetectSmothing", 64, 1, 128, true);
    panel.addSlider("distanceThresh",15,1,100,true);    
    
    panel.addPanel("Screen Move");
    panel.addSlider("panX", 0, -1000, 1000, true);
    panel.addSlider("panY", 0, -1000, 1000, true);
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
#ifdef _TWOCAM
    cam1.update();
    cam2.update();
    if(cam1.isFrameNew()) {
        for(int x=0;x<cam1.getWidth()-1;x++) {
            for(int y=0;y<cam1.getHeight()-2;y++) {
                int index = (x + ( y * cam1.getWidth() ) ) * 3;
                int index1 = (( x + ( y * cam1.getWidth() ) ) + (y*cam2.getWidth())) *3;
                    // (( x + (y * cam1.getWidth())) + (y*cam2.getWidth())) * 3;
                int index2 = (((y+1) * cam1.getWidth()) + (x + ( y * cam2.getWidth()) ) ) * 3;
                    // (( ( x + cam1.getWidth() ) + cam1.getWidth() ) + ( (y+1) * cam1.getWidth() )) * 3;
                
                
                cam.getPixelsRef()[ index1 ] = cam1.getPixelsRef()[index];
                cam.getPixelsRef()[ index1+1 ] = cam1.getPixelsRef()[index+1];
                cam.getPixelsRef()[ index1+2 ] = cam1.getPixelsRef()[index+2];
                
                cam.getPixelsRef()[ index2 ] = cam2.getPixelsRef()[index];
                cam.getPixelsRef()[ index2+1 ] = cam2.getPixelsRef()[index+1];
                cam.getPixelsRef()[ index2+2 ] = cam2.getPixelsRef()[index+2];
                
                
            }
        }
        
        cam.update();

#else
    cam.update();
    if(cam.isFrameNew()) {
#endif
               //        for(int
        
        convertColor(cam, gray, CV_RGB2GRAY);//CV_RGB2GRAY);
        resize(gray, graySmall);
        graySmall.update();
        Mat graySmallMat = toCv(graySmall);
        equalizeHist(graySmallMat, graySmallMat);
        //        if(ofGetMousePressed()) {
        //            equalizeHist(graySmallMat, graySmallMat);
        //        }
        //imitate(graySmallMat, graySmall);
        
        classifier.detectMultiScale(
                                    graySmallMat
                                    , unfilteredObjects
                                    , panel.getValueF("scaleFactor")
                                    , panel.getValueI("minNeighbors")
                                    , 0
                                    , cv::Size(panel.getValueI("minSize")
                                               , panel.getValueI("minSize"))
                                    , cv::Size(panel.getValueI("maxSize")
                                               ,panel.getValueI("maxSize")));
        
        
        // Smoothing detected face rectangles across three frames
        objectSmoothingThresh = 3;
        if(smoothingCounter>0) {
            for(int i=0;i<unfilteredObjects.size();i++) {
                cv::Rect & obj = unfilteredObjects[i];
                ofVec3f * c1 = new ofVec3f(obj.x + obj.width/2, obj.y + obj.height/2,0);
                for(int j=0;j<smoothedObjects.size();j++) {
                    cv::Rect & comobj = smoothedObjects[j];
                    ofVec3f * c2 = new ofVec3f(comobj.x + comobj.width/2, comobj.y + comobj.height/2,0);

                    if(c1->squareDistance(*c2)<objectSmoothingThresh) {
                        //Averages the two assumed equal faces
                        //  smoothedObjects[j].x = (smoothedObjects[j].x + unfilteredObjects[j].x) / 2;
                            // smoothedObjects[j].y = (smoothedObjects[j].y + unfilteredObjects[j].y) / 2;
                            // smoothedObjects[j].width = (smoothedObjects[j].width + unfilteredObjects[j].width) / 2;
                            //smoothedObjects[j].height = (smoothedObjects[j].height + unfilteredObjects[j].height) / 2;
                        
                    } else {
                        // Assumes a new face and pushes it to the smoothed queue
                        smoothedObjects.push_back(unfilteredObjects[i]);
                    }
                }
                if(smoothedObjects.size() == 0)
                    smoothedObjects.push_back(unfilteredObjects[i]);
            }
                
            
            smoothingCounter -= 1;
        } else {
            smoothingCounter = 3;
            objects = smoothedObjects;
            smoothedObjects.clear();
        }
        
        newFrame.setFromPixels(cam.getPixels(), cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
            //resize(gray, graySmall);
        
    }
}

void testApp::setDebug(bool _debug) {
    debug = _debug;
    for(int i=0;i<canvases.size();i++) {
        canvases[i]->debug = _debug;
    }
}