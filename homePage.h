
#ifndef HOMEPAGE_
#define HOMEPAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class HomePage : public Page {

public:

HomePage CONSTRUCTOR_MACRO

image_info backgroundImageFile;

void initalize() {

    watch->loadImage("home.bmp",&backgroundImageFile);
    
    watch->printImage(&backgroundImageFile,0,0);
    
    // if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    // 
    // if(!backgroundImageFile.open("home.bmp",O_RDWR)) USB.println("File open fail");
    // 
    // watch->printBitmap(backgroundImageFile,0,0);
    
    if(D) { USB.println("homepage initalize is done"); }
    
}

void leavingPage() {
    
    backgroundImageFile.file.close();

}

void loop() {

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

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

void button(int dir,int index) {

}



};


#endif