//
//  ramRenderer.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#include "ramRenderer.h"
#include "ramSkeleton.h"
#include "ramConstants.h"

using namespace ram;
using namespace ram::skeleton;

bool Renderer::sInited = false;
ofImage Renderer::sImageL;
ofImage Renderer::sImageR;

//----------------------------------------------------------------------------------------
static void billboard()
{
	ofMatrix4x4 m;
	glGetFloatv(GL_MODELVIEW_MATRIX, m.getPtr());
	
	ofVec3f s = m.getScale();
	
	m(0, 0) = s.x;
	m(0, 1) = 0;
	m(0, 2) = 0;
	
	m(1, 0) = 0;
	m(1, 1) = s.y;
	m(1, 2) = 0;
	
	m(2, 0) = 0;
	m(2, 1) = 0;
	m(2, 2) = s.z;
	
	glLoadMatrixf(m.getPtr());
}

void Renderer::initialize()
{
    if (!sInited) {
        ofDisableArbTex();
        sImageL.loadImage("images/L.png");
        sImageR.loadImage("images/R.png");
        sImageL.mirror(true, false);
        sImageR.mirror(true, false);
        ofEnableArbTex();
        sInited = true;
    }
}

//----------------------------------------------------------------------------------------
Renderer::Renderer() :
mState(0),
mActiveJoint(JOINT_HIPS),
mJointScreenCoords(NUM_JOINTS)
{
}

//----------------------------------------------------------------------------------------
Renderer::~Renderer()
{
    
}

//----------------------------------------------------------------------------------------
void Renderer::update(Skeleton *skleton)
{
    
}

//----------------------------------------------------------------------------------------
void Renderer::draw(Skeleton *skeleton) const
{
    ofxPushAll();
        
    vector<Node> &joints = skeleton->mJoints;
    
    ofNoFill();
    
    ofColor color = skeleton->mColor;
    
    for (size_t i=0; i<joints.size(); i++) {
        ofPushStyle();
        ofFill();
        
        ofSetLineWidth(1.f);
        Node &n = joints.at(i);
        isEndSite(n.id) || n.id == JOINT_HIPS ? n.size = 120.f : n.size = 80.f;
        
        if (n.enable == false)
            ofSetColor(150);
        else if (n.error && ofGetFrameNum() % 40 < 20)
            ofSetColor(255, 100, 100);
        else if (n.error && ofGetFrameNum() % 40 >= 20)
            ofSetColor(100);
        else
            ofSetColor(color);
        
        n.draw();
        
        if (n.id == JOINT_HEAD) {
            ofPushStyle();
            ofSetColor(200);
            ofPushMatrix();
            ofMultMatrix(n.getGlobalTransformMatrix());
            ofTranslate(0.f, 0.f, n.size * 0.75f);
            ofDrawBox(ofVec3f::zero(),
                      n.size * 0.25f,
                      n.size * 0.25f,
                      n.size * 0.5f);
            ofPopMatrix();
            ofPopStyle();
        }
        
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255, 100);
        if (n.id == JOINT_LEFT_HAND || n.id == JOINT_LEFT_TOE) {
            sImageL.bind();
            const float ps = n.size;
            n.size = ps * 1.1f;
            n.draw();
            n.size = ps;
            sImageL.unbind();
        }
        else if (n.id == JOINT_RIGHT_HAND || n.id == JOINT_RIGHT_TOE) {
            sImageR.bind();
            const float ps = n.size;
            n.size = ps * 1.1f;
            n.draw();
            n.size = ps;
            sImageR.unbind();
        }
        ofPopStyle();
        
        const ofVec3f& pos = n.getGlobalPosition();
        
        GLdouble modelview[16];
        GLdouble projection[16];
        GLint viewport[4];
        
        /// use current modelview/projection matrix and viewport
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        
        GLdouble winX, winY, winZ;
        
        gluProject(static_cast<GLdouble>(pos.x),
                   static_cast<GLdouble>(pos.y),
                   static_cast<GLdouble>(pos.z),
                   modelview,
                   projection,
                   viewport,
                   &winX,
                   &winY,
                   &winZ);
        
        const ofVec3f screen(winX, ofGetHeight()-winY, 0.f);
        mJointScreenCoords.at(i) = screen;
        
        ofSetColor(color);
        if (!n.getParent()) continue;
        
        const ofVec3f v0 = n.getGlobalPosition();
        const ofVec3f v1 = n.getParent()->getGlobalPosition();
        
        ofPushStyle();
        ofSetLineWidth(2.f);
        ofSetColor(255);
        ofLine(v0, v1);
        ofPopStyle();
        
        int d = v0.distance(v1);
        const int s = 60.f;
        if (d%s < s /10 || d%s > s - s / 10)
            d += s / 5;
        const int repeat = d/s;
        for (int j=0; j<repeat; j++) {
            const float t = j/(float)repeat;
            ofSetColor(220);
            ofPushMatrix();
            ofTranslate(v0.interpolated(v1, t));
            ofRotateY(45.f);
            ofDrawBox(ofVec3f::zero(), s*0.8f);
            ofPopMatrix();
        }
         ofPopStyle();
    }
    
    ofxPopAll();
}

void Renderer::drawHUD(Skeleton *skeleton) const
{
    vector<Node> &joints = skeleton->mJoints;
    
    ofPushStyle();
    ofFill();
    
    ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);
    
    ofPushMatrix();
    ofTranslate(mJointScreenCoords.at(JOINT_HEAD));
    
     (mState==Skeleton::STATE_SELECTED) ? ofSetHexColor(0xFF0000) : ofSetHexColor(0x000000);
    ofDrawBitmapString(skeleton->getHostName()+"\n"+skeleton->getName(),
                       ofPoint(0.0f, -32.0f));
    ofPopMatrix();
    
    float t = ::fmodf(ofGetElapsedTimef() * 1.f, 1.f);
    
    for (int i=0; i<joints.size(); i++) {
        ofPushMatrix();
        ofTranslate(mJointScreenCoords.at(i));
        if (i==mActiveJoint && mState==Skeleton::STATE_SELECTED) {
            ofSetColor(255, 50, 50, 80 - t * 50.f);
            ofCircle(ofVec3f::zero(), 13.0f + t * 20.f);
            
            (mState==Skeleton::STATE_SELECTED) ? ofSetHexColor(0xFF0000) : ofSetHexColor(0x000000);
            ofDrawBitmapString(joints.at(i).name, ofPoint::zero());
        }
        ofPopMatrix();
    }
    
    ofPopStyle();
}

//----------------------------------------------------------------------------------------
void Renderer::setState(int state)
{
    //ofLogNotice("Renderer") << "state:" << state;
    mState = state;
}

//----------------------------------------------------------------------------------------
int Renderer::getState() const
{
    return mState;
}

//----------------------------------------------------------------------------------------
void Renderer::setActiveJoint(int joint)
{
    mActiveJoint = joint;
}
//----------------------------------------------------------------------------------------
int Renderer::getActiveJoint() const
{
    return mActiveJoint;
}
