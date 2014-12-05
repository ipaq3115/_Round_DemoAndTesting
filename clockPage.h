
#ifndef CLOCKSELCTIONPAGE_
#define CLOCKSELCTIONPAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class ClockSelectPage : public Page {

    protected:
    
    public:

        ClockSelectPage() {}
        
        ClockSelectPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);

};


void ClockSelectPage::initalize() {
    
    SdFile backgroundImageFile;
    
    if(!backgroundImageFile.open("clkBack.bmp",O_RDWR)) if(D) USB.println("File open fail");
    
    lcd->printBitmap(backgroundImageFile,0,0);
    
    backgroundImageFile.close();
    
}

void ClockSelectPage::leavingPage() {

}

void ClockSelectPage::loop() {

}

void ClockSelectPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

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
                
                switch(iconIndex) {
                
                    case 0: goPage(PAGE::BREIGHTLING_CLOCK); return;
                    case 1: goPage(PAGE::XKCD_CLOCK); return;
                    case 2: goPage(PAGE::BLUE_CLOCK); return;
                    case 3: goPage(PAGE::BAR_CLOCK); return;
                    case 4: goPage(PAGE::BLUISH_CLOCK); return;
                
                }
            
            }
            
            break;
    
    }
}

void ClockSelectPage::button(int dir,int index) {

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


#endif