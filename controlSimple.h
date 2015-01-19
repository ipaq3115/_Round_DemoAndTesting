
#ifndef CONTROL_SIMPLE_PAGE_H_
#define CONTROL_SIMPLE_PAGE_H_

class ControlSimplePage : public Page {

public:

image_info backgroundImageFile;

int firstTouchIndex = -1;
int currentQuadrant = -1;

ControlSimplePage CONSTRUCTOR_MACRO

void initalize() {
    
    watch->loadImage("controlS.raw",&backgroundImageFile);
    
    watch->printImage(&backgroundImageFile,0,0,0);    
    
    firstTouchIndex = -1;
    
}

void leavingPage() {

}

void loop() {

    int newQuadrant = compass->heading() + 45;
    while(newQuadrant < 0) newQuadrant += 360;
    while(newQuadrant > 360) newQuadrant -= 360;
    newQuadrant /= 90;
    
    if(newQuadrant != currentQuadrant) {
    
        currentQuadrant = newQuadrant;
        
        watch->printImage(&backgroundImageFile,0,0,currentQuadrant);
        
    }

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

}

void button(int dir,int index) {

}


};

#endif
