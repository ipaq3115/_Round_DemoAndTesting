
#ifndef HOMEPAGE_
#define HOMEPAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class HomePage : public Page {

public:

HomePage CONSTRUCTOR_MACRO

image_info backgroundImageFile;

bool first = true;

void initalize() {

    if(!first) {

        watch->loadImage("homeBack.raw",&backgroundImageFile);
        // watch->loadImage("home.bmp",&backgroundImageFile);
        
        watch->printImage(&backgroundImageFile,0,0);
        
    } else {
    
        watch->setColor(BLACK);
        watch->fillRect(0,0,219,219);
    
    }
    
    if(D) { USB.println("homepage initalize is done"); }
    
}

void leavingPage() {
    
    backgroundImageFile.file.close();

}

void loop() {

    if(first) {
        
        // watch.setBrightness(100);
        // watch.rampBrightness(100,500);
        
        image_info fade_in;
        watch->loadImage("hdyfade.raw", &fade_in);
        for(int i=0;i<20;i++) watch->printImage(&fade_in, 0, 0, i);
        
        image_info fade_in_two;
        watch->loadImage("homeFade.raw", &fade_in_two);
        for(int i=0;i<20;i+=4) watch->printImage(&fade_in_two, 0, 0, i);
    
        first = false;
    
    }

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