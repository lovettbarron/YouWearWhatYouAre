#include "ofMain.h"
#include "canvas.h"

using namespace ofxCv;
using namespace cv;

ofCanvas::ofCanvas(ofBaseApp * base, ofVec3f _pos, ofImage _map)
{
    ofLog() << "'Lo, a new canvas is born";
//    std::memcpy(&pos,&_pos,sizeof *pos);
//    std::memcpy(&border,&_border,sizeof *border);
//    std::memcpy(&map,&_map,sizeof *map);
    frameScale = 4;
    ofxCv::convertColor(_map, map, CV_RGB2GRAY) ;
    width = map.width * frameScale;
    height = map.height * frameScale;
    pos = _pos;
    border = getContour(_map);
    
    for(int i=0;i<border.size();i++) {
        border[i].x *= frameScale;
        border[i].y *= frameScale;
    }
    
    cx = width/2;
    cy = height/2;
    center = ofVec3f(cx,cy,0);
    padding = 0.1;
    newFaceTimer = 0;
    debug = false;
    scale  = 2.0;
    limit = 100;
    newFaceTimerThresh = 60;
    app = base;
    frame.allocate(width, height, GL_RGBA, 1);
    ofLog() << "Frame w: " << ofToString(width) << " h: " << ofToString(height);
    shader.load("shader"); //plzplzplz don'tcrash
 }

ofCanvas::~ofCanvas() 
{
    
}

ofPolyline ofCanvas::getContour(ofImage map) {
    ofPolyline poly;
//    ofImage mapResized;
//    mapResized.allocate(width,height,OF_IMAGE_COLOR);
//    resize(map, mapResized);
    contourFinder.setTargetColor(ofColor(0,255,0), TRACK_COLOR_RGB);
    contourFinder.setInvert(true);
    contourFinder.setThreshold(127);
    contourFinder.findContours(toCv(map));
    
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
    poly.simplify(.3);    
    poly.close();    
    return poly;
}

//Translated from http://www.openprocessing.org/sketch/57395
// second try: http://www.codeproject.com/Articles/42067/2D-Circle-Packing-algorithm-ported-to-C

void ofCanvas::drawBoundingLines() {
    for(int i=0; i< (int) border.size(); i++ ) {
        bool repeatNext = i == border.size() - 1;
        
        const ofPoint& cur = border.getVertices()[i];
        const ofPoint& next = repeatNext ? border.getVertices()[0] : border.getVertices()[i + 1];
        
        float angle = atan2f(next.y - cur.y, next.x - cur.x) * RAD_TO_DEG;
        float distance = cur.squareDistance(next);
        
        if(repeatNext) {
            ofSetColor(255, 0, 255);
        }
        glPushMatrix();
            glTranslatef(cur.x, cur.y, 0);
            ofRotate(angle);
            ofLine(0, 0, 0, distance);
            ofLine(0, 0, distance, 0);
        glPopMatrix();
    }
}

bool ofCanvas::compareWithStillActive( ofImage * _img, ofVec3f * _loc) {
    for(int i=0;i<canvas.size();i++){
        if( canvas[i].isActive() ) {            
                if(canvas[i].isWithinRange(*_loc)) {
                    canvas[i].updateFace(*_img, *_loc);
                    return false; // A face was found and updated
                } else {
                    return true; // No face was found on this canvas.
                    }
                }
    }
    return true; // Default to adding face
}


void ofCanvas::update() {
    checkSize();
    mouse.x = (app->mouseX + pos.x) * scale;
    mouse.y = (app->mouseY + pos.y) * scale;
        // std::sort(canvas.begin(), canvas.end(), &compare);
    newFaceTimer--;
    
    //sort(canvas.begin(), canvas.end());
    
    if(canvas.size() != 0) {
        float sepSq = .4 * .4;
        for (int i = 0; i < canvas.size() - 1; i++)
        {
            if(mouse.squareDistance(canvas[i].loc) <= canvas[i].radius) {
                canvas[i].selected = true;
            } else {
                canvas[i].selected = false;
            }
            canvas[i].debug = debug;
            canvas[i].update();
            if(&map == NULL) ofLog() << "Null!";
                //            canvas[i].scaleToMap(&map);
            canvas[i].scaleToCount(canvas.size());
                //ofLog() << "Scale " << ofToString(canvas[i].scale);
            for (int j = i + 1; j < canvas.size(); j++)
            {
                    
                if (i == j)
                    continue;
                
                ofVec3f diff = canvas[i].loc - canvas[j].loc;
                float r = canvas[i].radius + canvas[j].radius;
                float d = (diff.x*diff.x) + (diff.y*diff.y) + (diff.z*diff.z);
                
                    // APPLY FORCE IF REQUIRED
                if (d < (r * r) - 0.01 ){
                    ofVec3f * force = &diff;
                    force->normalize();
                    *force *=  ( r - sqrt(d) ) * 0.5;
                    canvas[i].loc += *force;
                    canvas[j].loc -= *force;
                    if(!border.inside( canvas[i].loc ) ) {
                        canvas[i].loc -= center;
                        canvas[i].loc -= *force;
                        canvas[i].scale *= .9;
                    }
                    if(!border.inside( canvas[j].loc ) ) {
                        canvas[j].loc -= center;
                        canvas[i].loc -= *force;
                        canvas[i].scale *= .9;
                    }                    
                }
            }
         }
    }
    
    for (int i = 0; i < canvas.size(); i++) {
        float damping = 0.1/(float)i;
        canvas[i].loc;
        ofVec3f diff;
        diff = canvas[i].loc - center;
        diff *= damping;
        canvas[i].x -= diff.x;
        canvas[i].x -= diff.y;
    }
    
}

void ofCanvas::draw(int _x, int _y) {
    
    // Draw to FBO
    frame.begin();
        ofPushMatrix();
            ofClear(0,0,0);  
            glDisable(GL_DEPTH_TEST);
        
            if(debug) {
                map.reloadTexture();
                map.draw(0,0,width,height);
                drawBoundingLines();
            }
        
            shader.begin();
            // ofScale(frameScale,frameScale);
                for(int i=0;i<canvas.size();i++) {
                    canvas[i].draw();
                }
            shader.end();

            ofClearAlpha();
        ofPopMatrix();
    frame.end();
        //    ofScale(.4,.4);
        //ofEnableAlphaBlending();
    ofPushMatrix();
        ofScale(scale,scale);
        glEnable(GL_BLEND);  
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  
        frame.draw(0,0);
        glDisable(GL_BLEND);  
    ofPopMatrix();
    
    if(debug) {
        ofPushMatrix();
        ofTranslate(0,0,0.0f);
            //ofScale(0.6,0.6);
        string stats;
        stats = "pos: " + ofToString(pos);
        ofDrawBitmapString(stats,0, 0);
        stats = "width: " + ofToString(width) + " height: " + ofToString(height);
        ofDrawBitmapString(stats,0, 12);
        stats = "faces: " + ofToString(canvas.size());
        ofDrawBitmapString(stats,0, 24);
        stats = "cx: " + ofToString(center.x) + " cy: " + ofToString(center.y);
        ofDrawBitmapString(stats,0, 36);
        ofPopMatrix();
    }
    
}

void ofCanvas::draw() {
    draw(0,0);
}

void ofCanvas::configure() {

}

void ofCanvas::add(ofFace * face) {
    canvas.push_back(*face);
}


void ofCanvas::reset() {

}

void ofCanvas::testImages() {
    for(int i=0;i<20;i++) {
        ofImage random;
        random = canvas[ofRandom(0,canvas.size()-1)].theFace;
        ofFace newFace = ofFace(random
                                , ofVec3f(pos.x + (width/2)
                                , pos.y + (height/2),0)
                                , ofVec3f(cx+ofRandom(-15,15)
                                , cy+ofRandom(-15,15),0)
                                , 20);
        canvas.push_back(newFace);
        
    }
}

int ofCanvas::size() {
    return canvas.size();
}

void ofCanvas::checkSize() {
    if(canvas.size()>limit) {
        canvas.erase(canvas.begin());
    }
}

ofFace & ofCanvas::get( int index ) {
    return canvas.at(index);
}

float ofCanvas::distance(ofFace & face1, ofFace & face2) {
    return (face1.x - face2.x) * (face1.x - face2.x) + (face1.y - face2.y) * (face1.y - face2.y);

}

float ofCanvas::distance(ofPoint pt1, ofPoint pt2) {
    return (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y);
    
}
    // via http://forum.openframeworks.cc/index.php?topic=886.0