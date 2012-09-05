#include "ofMain.h"
#include "canvas.h"

ofCanvas::ofCanvas(ofVec3f _pos, ofImage _map, ofPolyline _border)
{
    ofLog() << "'Lo, a new canvas is born";
//    std::memcpy(&pos,&_pos,sizeof *pos);
//    std::memcpy(&border,&_border,sizeof *border);
//    std::memcpy(&map,&_map,sizeof *map);
    pos = _pos;
    border = _border;
    map = _map;
    width = map.width;
    height = map.height;
    cx = width/2;
    cy = height/2;
    padding = 0.1;
    newFaceTimer = 0;
    debug = false;
    scale  = 2.0;
    limit = 50;
    newFaceTimerThresh = 30;
        //frame.allocate(width, height, GL_RGBA);
 }

ofCanvas::~ofCanvas() 
{
    
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

bool ofCanvas::compare(ofFace* face1, ofFace* face2) {    
    ofVec3f * center = new ofVec3f(cx,cy,0);
    float d1 = face1->distance(*center);
    float d2 = face2->distance(*center);
    if (d1 < d2)
        return 1;
    else if (d1 > d2)
        return -1;
    else return 0;
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
        // std::sort(canvas.begin(), canvas.end(), &compare);
    newFaceTimer--;
    int offsetx=0;
    int offsety=0;
    int maxHeight=0;
    for(int i=0;i<canvas.size();i++) {
        canvas[i].debug = debug;
        canvas[i].update();
        canvas[i].x = offsetx;
        canvas[i].y = offsety;
        canvas[i].scaleToMap(&map);
        offsetx += canvas[i].radius*2;
        if(maxHeight<canvas[i].radius*2) maxHeight = canvas[i].radius*2;
        if(offsetx>width) {
            offsetx=0;
            offsety+=maxHeight;
            maxHeight=0;
        }
    }
    
    /*
    if(canvas.size() != 0) {
        float sepSq = .4 * .4;
        for (int i = 0; i < canvas.size() - 1; i++)
        {
            canvas[i].debug = debug;
            canvas[i].update();
            if(&map == NULL) ofLog() << "Null!";
            canvas[i].scaleToMap(&map);
                //ofLog() << "Scale " << ofToString(canvas[i].scale);
            for (int j = i + 1; j < canvas.size(); j++)
            {
                    
                if (i == j)
                    continue;
                
                ofVec2f AB = canvas[j].center - canvas[i].center;
                
                float r = canvas[i].getRadius() + canvas[j].getRadius();
                
                
                // Length squared = (dx * dx) + (dy * dy);
                float d = distance(canvas[i], canvas[j]);
                float minSepSq = ofClamp(d, 0, sepSq);
                d -= minSepSq;
                bool succeed;
                float forceFromCenter = (canvas[i].distance(*new ofVec3f(cx,cy,0)) / (width/2));
                if (d < (r * r) - 0.01 )
                {
                    AB.normalize();
                    
                    AB *= (float)((r - sqrt(d)) * 0.5f);
                    int ax = canvas[j].x + AB.x;
                    int ay = canvas[j].y + AB.y;
                    int bx = canvas[i].x - AB.x;
                    int by = canvas[i].y - AB.y;
                    // Checks if the face is within the polyLine
                    if(border.inside(ax, ay)) { // && border.inside(ax-r, ay-r)
                        canvas[j].x += AB.x;
                        canvas[j].y += AB.y;
                        succeed = true;
                    } else {
                        canvas[j].x -= AB.x*forceFromCenter;
                        canvas[j].y -= AB.y*forceFromCenter;
                        succeed = false;
                    }
                    if(border.inside(bx,by)) { // && border.inside(bx-r, by-r)
                        canvas[i].x -= AB.x;
                        canvas[i].y -= AB.y;
                        succeed = true;
                    } else {
                        canvas[i].x += AB.x*forceFromCenter;
                        canvas[i].y += AB.y*forceFromCenter;
                        succeed = false;
                    }
                    
                }
                // This pulls these guys closer to the center
            }
         }
    }*/
    
    // Draw to FBO
//    frame.begin();
//    glClear(GL_COLOR_BUFFER_BIT);
//    glPushAttrib(GL_ALL_ATTRIB_BITS);
//    
//    glEnable(GL_BLEND);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
//    map.draw(0,0);
//    for(int i=0;i<canvas.size();i++) {
//        canvas[i].draw();
//    }
//    border.draw();
//    glDisable(GL_BLEND);
//    glPopAttrib();
//    frame.end();
}

void ofCanvas::draw(int _x, int _y) {
    ofPushMatrix();
    ofTranslate(0,0);
    ofScale(scale,scale);
        //    ofScale(.4,.4);
        //    ofEnableAlphaBlending();
        //    frame.draw(0,0);
    if(debug) {
        map.reloadTexture();
        map.draw(0,0);
    }
    for(int i=0;i<canvas.size();i++) {
        canvas[i].draw();
    }
    if(debug) {
        drawBoundingLines();
    }
    
        //    ofDisableAlphaBlending();
    ofPopMatrix();
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
        ofFace newFace = ofFace(random, ofVec3f(pos.x + (width/2), pos.y + (height/2),0),ofVec3f(cx+ofRandom(-15,15),cy+ofRandom(-15,15),0), width/2);
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