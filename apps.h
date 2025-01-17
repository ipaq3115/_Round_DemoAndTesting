
#ifndef APPS_PAGE_
#define APPS_PAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class AppsPage : public Page {

    protected:
    
        image_info backgroundImageFile;

        int page = 0;
        
    public:

        AppsPage() {}
        
        AppsPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);

};

void AppsPage::initalize() {

    watch->loadImage("appBack.raw",&backgroundImageFile);
    
    watch->printImage(&backgroundImageFile,0,0,page);
    
}

void AppsPage::leavingPage() {

}

void AppsPage::loop() {

}

void AppsPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

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
                
                    page++;
                    page %= 2;
                    
                    watch->printImage(&backgroundImageFile,0,0,page);
                
                } else if(page == 0) {
                
                    switch(iconIndex) {
                    
                        // case 1: goPage(PAGE::GRAVITY_BALL); return;
                        case 1: goPage(PAGE::TARGET_BALL); return;
                        case 2: goPage(PAGE::PHONE); return;
                        case 3: goPage(PAGE::CONTROL_SIMPLE); return;
                        case 4: goPage(PAGE::BATTERY_GRAPH); return;
                    
                    }
                    
                } else if(page == 1) {
                    
                    switch(iconIndex) {
                    
                        case 1: goPage(PAGE::TOUCH_DEMO); return;
                        case 2: goPage(PAGE::VIDEO); return;
                        case 3: return;
                        case 4: goPage(PAGE::TEXT_ENTRY); return;
                    
                    }
                
                
                }
            
            }
            
            break;
    
    }
}

void AppsPage::button(int dir,int index) {

}


#endif