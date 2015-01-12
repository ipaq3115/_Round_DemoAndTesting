

#ifndef BAR_CLOCK_PAGE_
#define BAR_CLOCK_PAGE_

#include "roundUtil.h"

class BarClockPage : public Page {

public:

int touchIndex = -1;

image_info

ringFile,
ringA,
ringB,
ringC;

elapsedMillis updateTime;

int curSec=0,curMin=0,curHour=0;
int curSecO=0,curMinO=0,curHourO=0;

int lastSecondUpdate = -1;

BarClockPage() {}

BarClockPage CONSTRUCTOR_MACRO

void initalize() {

    watch->loadImage("ringBack.Gci",&ringFile);
    watch->loadImage("ringA.Gci",&ringA);
    watch->loadImage("ringB.Gci",&ringB);
    watch->loadImage("ringC.Gci",&ringC);
    
    watch->printImage(&ringFile,0,0);
    
    curSecO = -1;
    curMinO = -1;
    curHourO = -1;
    
    lastSecondUpdate = -1;
    
    touchIndex = -1;
    
    updateTime = 101;
    
    loop();

}

void leavingPage() {

}

void loop() {
    
    if(lastSecondUpdate != -1 && millis() - lastSecondUpdate > 500) {
    
        lastSecondUpdate = -1;
    
        int tmp = curSec + 1;
        if(tmp >= 119) tmp -= 119;
        watch->printImage(&ringA,0,0,tmp);
    
    }

    if(updateTime > 10) {
        
        updateTime = 0;
        
        time_t newTime = now();
        
        // tmElements_t timeHolder;
        // breakTime(newTime, timeHolder);
        
        curSec  = second(newTime) * 2;
        curMin  = minute(newTime) * 2;
        curHour = hour(newTime) * 5; if(curHour >= 119) curHour = 119;
     
        if(curSec != curSecO) {
         
            watch->printImage(&ringA,0,0,curSec);

            lastSecondUpdate = millis();
            
        }
        if(curMin != curMinO) watch->printImage(&ringB,0,0,curMin);
        if(curHour != curHourO) watch->printImage(&ringC,0,0,curHour);
        
        curSecO = curSec;
        curMinO = curMin;
        curHourO = curHour;
        
    }
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    switch(touchType) {
    
        case PRESSED: 
            // if(this->touchIndex == -1) this->touchIndex = touchIndex;
            break;
        case MOVING: 
            break;
        case RELEASED: 
            // if(this->touchIndex == touchIndex) goPage(PAGE::HOME); 
            break;
    
    }

}

void button(int dir,int index) {

}

};

#endif
