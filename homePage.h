
#ifndef HOMEPAGE_
#define HOMEPAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class HomePage : public Page {

    protected:
    
        SdFile backgroundImageFile;

    public:

        HomePage() {}
        
        HomePage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);

};


void HomePage::initalize() {
    
    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    
    if(!backgroundImageFile.open("home.bmp",O_RDWR)) USB.println("File open fail");
    
    watch->printBitmap(backgroundImageFile,0,0);
    
    if(D) { USB.println("homepage initalize is done"); }
    
}

void HomePage::leavingPage() {
    
    if(backgroundImageFile.isOpen()) backgroundImageFile.close();

}

void HomePage::loop() {

}

void HomePage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    int iconIndex = (finePos / 45) + 1;
    if(iconIndex == 8) iconIndex = 0;
    iconIndex /= 2;
    
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
                
                if(D) USB.printf("iconIndex %d\r\n",iconIndex);
                
            
                switch(iconIndex) {
                
                    case 0: goPage(PAGE::CLOCKS); return;
                    case 1: goPage(PAGE::PHONE); return;
                    case 2: goPage(PAGE::APPS); return;
                    case 3: goPage(PAGE::SETTINGS_B); return;
                    
                }
                
            }
            
            break;
    
    }
}

void HomePage::button(int dir,int index) {

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