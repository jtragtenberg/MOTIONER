//
//  ramInspectorUI.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/3/13.
//
//

#include "ramInspectorUI.h"

#include "ramSkeleton.h"
#include "ramSkeletonManager.h"

#include "ofMain.h"
#include "ofxUI.h"

#include "ofxEvent.h"
#include "ramEvent.h"


using namespace ram;


//----------------------------------------------------------------------------------------
InspectorUI::InspectorUI() :

mIsInited(false),
mSettingsFilePath(""),

mSkeletonList(NULL),
mDeviceHostNameLabel(NULL),
mSettingFileNameLabel(NULL),
mSkeletonNameInput(NULL),

mJointNameLabel(NULL),
mJointName("not selected"),
mDialerX(NULL),
mDialerY(NULL),
mDialerZ(NULL),

mResetPositionXSlider(NULL),
mResetPositionZSlider(NULL),
mEnableJointToggle(NULL)
{
}

//----------------------------------------------------------------------------------------
InspectorUI::~InspectorUI()
{
}

//----------------------------------------------------------------------------------------
void InspectorUI::setup()
{
    if (mIsInited) return;
    mIsInited = true;
    
    mSize.set(0.0f, 0.0f, 180.0f, ofGetHeight());
    
    ofxUICanvas *canvas = NULL;
    ofxUIWidget *widget = NULL;
    ofColor outlineColor;
    outlineColor.setHex(0x494949);
    
    /// create gui objects
    const float space = OFX_UI_GLOBAL_WIDGET_SPACING;
    const float w = mSize.width+space;
    
    mTabs.clear();
    
    for (int i=0; i<N_TABS; i++) {
        
        ofSetLogLevel(OF_LOG_FATAL_ERROR); /// shout ofxUI out
        ofxUICanvas *canvas = new ofxUICanvas();
        canvas->setWidth(w);
        
        ofSetLogLevel(DEFAULT_LOG_LEVEL);
        canvas->setFont(GUI_FONT, false); /// aliased
        mTabs.push_back(canvas);
    }
    
    /// setup guis
    const float hw = mSize.width-OFX_UI_GLOBAL_WIDGET_SPACING;
    
    const float buttonSize = 20.0f;
    
    //const float range = 10000.0f;
    
    //--------------------------------------------------//
    //------------ TAB_GENERAL_SETTINGS ----------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_GENERAL_SETTINGS);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    canvas->addLabel("Skeleton ID:", OFX_UI_FONT_SMALL);
    
    /// TAB_GENERAL_SETTINGS - NAME label
    //--------------------------------------------------
    {
        mDeviceHostNameLabel = canvas->addLabel("No Device Found", OFX_UI_FONT_SMALL);
        canvas->addSpacer(hw, 1.0f);
    }
    
    /// TAB_GENERAL_SETTINGS - Basic settings
    //--------------------------------------------------
    {
        canvas->addLabel("Name:", OFX_UI_FONT_SMALL);
        mSkeletonNameInput = canvas->addTextInput("Actor Name",
                                                  "Enter Name",
                                                  hw,
                                                  0,
                                                  0,
                                                  0,
                                                  OFX_UI_FONT_SMALL);
        mSkeletonNameInput->setAutoClear(false);
        mSkeletonNameInput->setDrawOutline(true);
        mSkeletonNameInput->setColorOutline(outlineColor);
        
        widget = canvas->addLabelButton("Calibration", false, hw);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
        canvas->addSpacer(hw, 1.0f);
        
        mEnableOscOutToggle = canvas->addToggle("OSC Out",
                                                true,
                                                buttonSize,
                                                buttonSize);
        mEnableOscOutToggle->setDrawOutline(true);
        mEnableOscOutToggle->setColorOutline(outlineColor);
        
        canvas->addSpacer(hw, 1.0f);
    }
    
    /// TAB_GENERAL_SETTINGS - Skeleton managment
    //--------------------------------------------------
    {
        canvas->addLabel("Loaded Setting:", OFX_UI_FONT_SMALL);
        mSettingFileNameLabel = canvas->addLabel("", OFX_UI_FONT_SMALL);
        //canvas->addLabel(" ");
        addImageButton(canvas, buttonSize, "images/open.png", "Load", true);
        addImageButton(canvas, buttonSize, "images/save.png", "Save", false);
        addImageButton(canvas, buttonSize, "images/saveas.png", "Save As...", false);
        //canvas->addSpacer(hw, 1.0f);
    }
    
    canvas->autoSizeToFitWidgets();
    canvas->setWidth(w);

    //--------------------------------------------------//
    //---------------- JOINT EDITOR --------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_JOINT_EDITOR);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    /// JOINT EDITOR - JOINT NAME label
    //--------------------------------------------------
    {
        canvas->addLabel("Joint:", OFX_UI_FONT_SMALL);
        mJointNameLabel =new ofxUILabel("No Joint Selected", OFX_UI_FONT_SMALL);
        canvas->addWidgetDown(mJointNameLabel);
    }
    
    /// JOINT EDITOR - X number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("X:", OFX_UI_FONT_SMALL);
        
        mDialerX = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "Offset X", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerX);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    /// JOINT EDITOR - Y number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("Y:");
        mDialerY = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "Offset Y", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerY);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    /// JOINT EDITOR - Z number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("Z:");
        mDialerZ = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "Offset Z", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerZ);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    {
        mEnableJointToggle = canvas->addToggle("Enable Joint",
                                               false,
                                               buttonSize,
                                               buttonSize);
        mEnableJointToggle->setDrawOutline(true);
        mEnableJointToggle->setColorOutline(outlineColor);
    }
    
    canvas->autoSizeToFitWidgets();
    canvas->setWidth(w);
    
    //--------------------------------------------------//
    //-------------- POSITION EDITOR -------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_POSITION_EDITOR);
    canvas->setDrawOutline(true);
    canvas->setDrawPadding(false);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    /// POSITION EDITOR - SLIDES
    //--------------------------------------------------
    {
        canvas->addLabel("Position Offset:", OFX_UI_FONT_SMALL);
        
        mResetPositionXSlider = canvas->addSlider("Centter X",
                                                  -1500.0f,
                                                  1500.0f,
                                                  0.0f,
                                                  hw,
                                                  10.0f);
        mResetPositionXSlider->setDrawOutline(true);
        mResetPositionXSlider->setColorOutline(outlineColor);
        
        mResetPositionZSlider = canvas->addSlider("Centter Z",
                                                  -1500.0f,
                                                  1500.0f,
                                                  0.0f,
                                                  hw,
                                                  10.0f);
        mResetPositionZSlider->setDrawOutline(true);
        mResetPositionZSlider->setColorOutline(outlineColor);
        
        widget = canvas->addLabelButton("Reset", false, hw);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    for (int i=0; i<N_TABS; i++) {
        mTabs.at(i)->setDrawWidgetPadding(false);
        ofAddListener(mTabs.at(i)->newGUIEvent, this, &InspectorUI::guiEvent);
    }
    
    canvas->autoSizeToFitWidgets();
    canvas->setWidth(w);

    //--------------------------------------------------//
    //----------------- TAB_COLOR ----------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_COLOR_EDITOR);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    {
        /// colors
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
        
        /// TAB_OPTIONS - options
        //--------------------------------------------------
        {
            
            mColorRSlider = canvas->addSlider("Color R", 0, 255, 255);
            mColorRSlider->setDrawOutline(true);
            mColorRSlider->setColorOutline(outlineColor);
            mColorRSlider->setColorFill(ofColor(255, 100, 100));
            
            mColorGSlider = canvas->addSlider("Color G", 0, 255, 255);
            mColorGSlider->setDrawOutline(true);
            mColorGSlider->setColorOutline(outlineColor);
            mColorGSlider->setColorFill(ofColor(100, 255, 100));
            
            mColorBSlider = canvas->addSlider("Color B", 0, 255, 255);
            mColorBSlider->setDrawOutline(true);
            mColorBSlider->setColorOutline(outlineColor);
            mColorBSlider->setColorFill(ofColor(100, 100, 255));
        }
    }
    
    canvas->autoSizeToFitWidgets();
    canvas->setWidth(w);
    
    //--------------------------------------------------//
    //----------------- TAB_OPTIONS --------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_OPTIONS);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    {
        /// colors
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
        
        /// TAB_OPTIONS - options
        //--------------------------------------------------
        {
            widget = canvas->addLabelButton("Delete", false, hw);
            widget->setDrawOutline(true);
            widget->setColorOutline(outlineColor);
        }
    }
    
    canvas->autoSizeToFitWidgets();
    canvas->setWidth(w);
    
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    ofSetLogLevel(OF_LOG_FATAL_ERROR);
    mTabbedCanvas = new  ofxUITabbedCanvas();
    mTabbedCanvas->setup(GUI_FONT);
    for (int i=0; i<N_TABS; i++) {
        mTabbedCanvas->add(mTabs.at(i));
    }
    mTabbedCanvas->loadSettings(INSPECTOR_XML);
    
    mTabbedCanvas->setDrawOutline(true);
    mTabbedCanvas->setDrawBack(false);
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        mTabbedCanvas->setColorBack(b);
        mTabbedCanvas->setColorFill(f);
        mTabbedCanvas->setColorOutline(o);
    }
    
    const float guiw = mSize.width+space;
    const float guiMgnX = 10.0f;
    const float guiMgnY = 10.0f;
    mTabbedCanvas->setPosition(ofVec2f(ofGetWidth()-guiw-guiMgnX, guiMgnY));
    //mTabbedCanvas->setVisible(false);
    ofSetLogLevel(DEFAULT_LOG_LEVEL);
    
    ofAddListener(ofxEvent(), this, &InspectorUI::onMessageReceived);
}

void InspectorUI::update()
{
//    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
//    bool visible;
//    skl ? visible = true : visible = false;
//    if (mTabbedCanvas)
//        mTabbedCanvas->setVisible(visible);
}

//----------------------------------------------------------------------------------------
void InspectorUI::exit()
{
    ofRemoveListener(ofxEvent(), this, &InspectorUI::onMessageReceived);
    
    for (int i=0; i<N_TABS; i++) {
        ofRemoveListener(mTabs.at(i)->newGUIEvent, this, &InspectorUI::guiEvent);
    }
    
    if (mTabbedCanvas) {
        //mTabbedCanvas->saveSettings(INSPECTOR_XML);
        delete mTabbedCanvas;
        mTabbedCanvas = NULL;
    }
}

/// GUI event callback
//----------------------------------------------------------------------------------------
void InspectorUI::guiEvent(ofxUIEventArgs &e)
{
    checkError();
    
    OFX_BEGIN_EXCEPTION_HANDLING
    
    const string name = e.widget->getName();
    //ofLogNotice("Inspector") << name;
    //--------------------
    if(name == "Actor Name") {
        ofxUITextInput *ti = static_cast<ofxUITextInput *>(e.widget);
        
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl)
            skl->setName(ti->getTextString());
        else
            ofxThrowException(ofxException, "Skeleton not found!");
    }
    else if(name == "Calibration") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->calibrate();
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "Reset") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            ofVec3f pos(mResetPositionXSlider->getScaledValue(),
                        0.0f,
                        mResetPositionZSlider->getScaledValue());
            //ofLogNotice() << pos;
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->resetPosition(pos);
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "Load") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl) {
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a xml file",
                                                                       false);
                if (openFileResult.bSuccess) {
                    skl->loadSettings(openFileResult.filePath);
                    setSettingsFilePath(openFileResult.filePath);
                }
            }
            else {
                ofxThrowException(ofxException, "Skeleton not found!");
            }
        }
    }
    else if(name == "Save") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->saveSettings();
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "Save As...") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl) {
                ofFileDialogResult saveFileResult =
                ofSystemSaveDialog("Untitled.xml", "Save current skeleton settings");
                if (saveFileResult.bSuccess) {
                    skl->saveSettings(saveFileResult.filePath);
                    mSettingFileNameLabel->setLabel(getFileName(skl->getSettingsFileName()));
                }
            }
            else {
                ofxThrowException(ofxException, "Skeleton not found!");
            }
        }
    }
    else if (name == "Offset X") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 0);
    }
    else if (name == "Offset Y") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 1);
    }
    else if (name == "Offset Z") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 2);
    }
    else if (name == "Enable Joint") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        
        if (skl) {
            const skeleton::Node &n = skl->getJoint(mJointName);
            if (!n.name.empty()) {
                ofxUILabelToggle *lt = static_cast<ofxUILabelToggle *>(e.widget);
                skl->setEnableJoint(skl->getJointIndexFromName(mJointName), lt->getValue());
            }
            else {
                ofxThrowException(ofxException, "Node not found!");
            }
        }
        else {
            ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if (name == "Color R") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl) {
            skl->setColorR(static_cast<ofxUISlider*>(e.widget)->getScaledValue());
        }
    }
    else if (name == "Color G") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl) {
            skl->setColorG(static_cast<ofxUISlider*>(e.widget)->getScaledValue());
        }
    }
    else if (name == "Color B") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl) {
            skl->setColorB(static_cast<ofxUISlider*>(e.widget)->getScaledValue());
        }
    }
    else if (name == "Delete") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl) {
            const string host = skl->getHostName();
            skeleton::SkeletonManager::getInstance().removeSkeleton(host);
        }
    }
    else if (name == "OSC Out") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl)
            skl->setEnableOscOut(static_cast<ofxUIToggle *>(e.widget)->getValue());
    }
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void InspectorUI::checkError()
{
    if (!mIsInited)
        ofxThrowException(ofxException, "UI dosen't inited!");
}

//----------------------------------------------------------------------------------------
void InspectorUI::addSkeletonFromXML(const string &fileName)
{
    skeleton::SkeletonManager::getInstance().addSkeleton(fileName);
}

//----------------------------------------------------------------------------------------
void InspectorUI::performSelectListItems(ofxUIDropDownList *ddList,
                                         void (InspectorUI::*func)(const string &))
{
    checkError();
    
    vector<ofxUIWidget *> &selected = ddList->getSelected();
    for(size_t i = 0; i < selected.size(); i++) {
        //cout << "SELECTED: " << selected[i]->getName() << endl;
        const string &name = selected.at(i)->getName();
        (this->*func)(name);
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setDevice(const string &hostName)
{
    checkError();
    
    SkeletonMap *skeletonMap = &skeleton::SkeletonManager::getInstance().getSkeletons();
    
    for (SkeletonMap::iterator it=skeletonMap->begin(); it!=skeletonMap->end(); ++it)
        it->second->setState(skeleton::Skeleton::STATE_IDLE);
    
    SkeletonMap::iterator result = skeletonMap->find(hostName);
    if (result == skeletonMap->end()) {
        ofLogWarning("UI") << "No such skelton!";
        return;
    }
    mSkeleton = result->second; /// mSkeleton is weak_ptr
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    if (skl) {
        /// set host name
        mDeviceHostNameLabel->setLabel(skl->getHostName());
        
        /// set name
        mSkeletonNameInput->setTextString(skl->getName());
        
        mSettingFileNameLabel->setLabel(getFileName(skl->getSettingsFileName()));
                
        mResetPositionXSlider->setValue(skl->getResetPosition().x);
        mResetPositionZSlider->setValue(skl->getResetPosition().z);
        
        mEnableOscOutToggle->setValue(skl->getEnableOscOut());
        
        skl->setActilveJoint(JOINT_HIPS);
        mEnableJointToggle->setValue(skl->getEnableJoint(JOINT_HIPS));
        
        mColorRSlider->setValue(skl->getColor().r);
        mColorGSlider->setValue(skl->getColor().g);
        mColorBSlider->setValue(skl->getColor().b);
        
        /// mark as selected
        skl->setState(skeleton::Skeleton::STATE_SELECTED);
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setSettingsFilePath(const string &path)
{
    mSettingsFilePath = path;
    
    if (mSettingFileNameLabel) {
        const string name = getLastPathCompornent(path);
        mSettingFileNameLabel->setLabel(name);
    }
    else {
        ofxThrowException(ofxException, "mSettingFileNameLabel isn't allocated!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setJoint(int index)
{
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    
    string name;
    
    if (skl) {
        name = skl->getJoint(index).name;
    }
    
    setJoint(name);
}

//----------------------------------------------------------------------------------------
void InspectorUI::setJoint(const string &name)
{
    checkError();
    
    mJointNameLabel->setLabel(name);
    mJointName = name;
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    
    if (skl) {
        const skeleton::Node &n = skl->getJoint(name);
        skl->setActilveJoint(skl->getJointIndexFromName(name));
        mEnableJointToggle->setValue(skl->getEnableJoint(skl->getJointIndexFromName(name)));
        
        if (!n.name.empty()) {
            ofLogNotice() << n.name << ": " << n.getPosition();//debug
            mDialerX->setValue(n.getPosition().x);
            mDialerY->setValue(n.getPosition().y);
            mDialerZ->setValue(n.getPosition().z);
        }
        else {
            //ofxThrowException(ofxException, "Node not found!");
            ofLogWarning("InspectorUI") << "Node not found!";
        }
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setName(const string &name)
{
    mSkeletonNameInput->setTextString(name);
}

//----------------------------------------------------------------------------------------
void InspectorUI::editOffset(ofxUINumberDialer *nd, int vec3fIndex)
{
    checkError();
    
    const bool inRange = ofInRange((float)vec3fIndex, 0.0f, (float)ofVec3f::DIM);
    if (!inRange)
        ofxThrowException(ofxException, "vec3fIndex out of range!");
    
    const float f = nd->getValue();
    
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    
    if (skl) {
        const skeleton::Node &n = skl->getJoint(mJointName);
        
        if (!n.name.empty()) {
            ofVec3f vec = n.getPosition();
            vec[vec3fIndex] = f;
            skl->editHierarchy(n.id, vec);
        }
        else {
            ofxThrowException(ofxException, "Node not found!");
        }
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::onMessageReceived(ofxEventMessage &m)
{
    const string addr = m.getAddress();
    if (addr==event::ADDRESS_PICKED_SKELETON) {
        setDevice(m.getArgAsString(0));
    }
    else if (addr==event::ADDRESS_PICKED_JOINT) {
        setDevice(m.getArgAsString(0));
        setJoint(m.getArgAsInt32(1));
    }
    else if (addr==event::ADDRESS_CHANGE_SKELETON_NAME) {
        setName(m.getArgAsString(0));
    }
}

//----------------------------------------------------------------------------------------
ofxUIImageButton *InspectorUI::addImageButton(ofxUICanvas *canvas,
                                              int dimension,
                                              const string &path,
                                              const string &name,
                                              bool bNewLine)
{
    ofxUIWidget *widget = NULL;
    if (!bNewLine)
        widget = canvas->addWidgetRight(new ofxUIImageButton(dimension,
                                                             dimension,
                                                             false,
                                                             path,
                                                             name));
    else
        widget = canvas->addWidgetDown(new ofxUIImageButton(dimension,
                                                            dimension,
                                                            false,
                                                            path,
                                                            name));
    ofColor deactive, outline;
    deactive.setHex(COLOR_M);
    outline.setHex(COLOR_HILIGHT);
    widget->setColorBack(deactive);
    widget->setDrawOutline(false);
    //widget->setColorOutline(outline);
    return static_cast<ofxUIImageButton *>(widget);
}

//----------------------------------------------------------------------------------------
ofxUIImageToggle *InspectorUI::addImageToggle(ofxUICanvas *canvas,
                                              int dimension,
                                              const string &path,
                                              const string &name,
                                              bool bNewLine)
{
    ofxUIWidget *widget = NULL;
    if (!bNewLine)
        widget = canvas->addWidgetRight(new ofxUIImageToggle(dimension,
                                                             dimension,
                                                             false,
                                                             path,
                                                             name));
    else
        widget = canvas->addWidgetDown(new ofxUIImageToggle(dimension,
                                                            dimension,
                                                            false,
                                                            path,
                                                            name));
    ofColor deactive, hilight;
    deactive.setHex(COLOR_ML);
    hilight.setHex(COLOR_H);
    widget->setColorBack(deactive);
    widget->setDrawOutline(false);
    widget->setColorFill(hilight);
    widget->setColorOutlineHighlight(COLOR_M);
    return static_cast<ofxUIImageToggle *>(widget);
}