
#ifndef CLOCKSELCTIONPAGE_
#define CLOCKSELCTIONPAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class ClockSelectPage : public Page {

public:

int page = 0;

ClockSelectPage() {}

ClockSelectPage CONSTRUCTOR_MACRO

void initalize() {
    
    page = 0;
    
    init();
    
}

void init() {

    watch->printImage("clkBack.gci",0,0,page);
    
}

void leavingPage() {

}

void loop() {

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    int iconIndex = finePos / 36;
    if(iconIndex == 0 || iconIndex == 9) {
        
        iconIndex = 0;
    
    } else {
    
        iconIndex--;
    
        iconIndex /= 2;
        
        iconIndex++;
    
    }
    
    if(D && touchType != MOVING) USB.printf("homePageTouch iconIndex %d\r\n",iconIndex);
    
    static bool iconState[5] {false,false,false,false,false};

    switch(touchType) {
    
        case PRESSED:
        
            iconState[iconIndex] = true;
        
            break;
        case MOVING:
            break;
        case RELEASED: 
        
            if(iconState[iconIndex]) {
            
                iconState[iconIndex] = false;
                
                    if(iconIndex == 0) {
                        page = !page; 
                        init(); 
                        return;
                    }
                    
                    if(page == 0) {
                        
                        switch(iconIndex) {
                        
                            case 1: goPage(PAGE::XKCD_CLOCK); return;
                            case 2: goPage(PAGE::BLUE_CLOCK); return;
                            case 3: goPage(PAGE::BAR_CLOCK); return;
                            case 4: goPage(PAGE::KICKSTARTER_CLOCK); return;
                            
                        }
                    
                    } else if(page == 1) {
                     
                        switch(iconIndex) {
                        
                            case 1: goPage(PAGE::BREIGHTLING_CLOCK); return;
                            case 2: /* this clock face still needs to be created */ return;
                            case 3: goPage(PAGE::RADIAN_CLOCK); return;
                            case 4: goPage(PAGE::BLUISH_CLOCK); return;
                            
                        }
                    
                    }
                
            }
            
            break;
    
    }
}

void button(int dir,int index) {

    using namespace BUTTON;
    
    static bool screenState = 0;

    switch(index) {
    
        case POWER_BUTTON:
        
            if(dir) {
            
                screenState = !screenState;
                
                watch->rampBrightness(screenState);
            
            }
        
            break;
    
    }

}

};



#endif