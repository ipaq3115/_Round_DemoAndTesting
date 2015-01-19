
#ifndef SETTINGS_PAGE_
#define SETTINGS_PAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class SettingsPage : public Page {

public:

image_info 
backgroundImageFile,
brightnessFile,
batteryBarFile,
batteryPercentFile;

int firstTouch = -1;
// int brightness;

SettingsPage CONSTRUCTOR_MACRO

void printBrightness(int percent) {

    int value = 1.44 * percent;
    // int value = 144 * ((float)percent/(float)100);
    
    if(D) USB.printf("printBrightness value %d percent %d\r\n",value,percent);

    watch->printImage(&brightnessFile,7,38,0,0,0,45,144 - value);
    watch->printImage(&brightnessFile,7,38,1,0,144 - value + 1,45,144);
    
}

void printBattery(int percent) {

    int value = 0.83 * percent;
    
    if(D) USB.printf("printBattery value %d percent %d\r\n",value,percent);

    watch->printImage(&batteryBarFile,68,7,1,0,0,value,23);
    watch->printImage(&batteryBarFile,68,7,0,value + 1,0,83,23);
    
    watch->printImage(&batteryPercentFile,74,25,percent);
    
}

void initalize() {
    
    watch->loadImage("settings.bmp",&backgroundImageFile);
    watch->loadImage("setBrgt.raw",&brightnessFile);
    watch->loadImage("setBatB.raw",&batteryBarFile);
    watch->loadImage("setBatP.raw",&batteryPercentFile);
    
    watch->printImage(&backgroundImageFile,0,0);
    
    printBrightness(watch->getBrightness());
    printBattery(42);
    
    firstTouch = -1;
    
    if(D) USB.println("apps page initalize is done");
    
}

void leavingPage() {

}

void loop() {

    // static elapsedMillis time;
    // static int count = 0;
    // static bool dir = true;
    
    // if(time > 20) {
    
        // time = 0;
        
        // count += dir ? 5 : -5;
        
        // if(count >= 100 || count <= 0) dir = !dir;
        
        // printBrightness(count);
        // printBattery(count);
    
    // }

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    if(D && touchType != MOVING) USB.printf("homePageTouch\r\n");
    
    static int lastTouchPos;
    
    switch(touchType) {
    
        case PRESSED:
        
            if(firstTouch == -1) {
            
                firstTouch = touchIndex;

                lastTouchPos = finePos;
        
            }
            
            break;
        case MOVING:
            
            if(firstTouch == touchIndex) {
                
                if(finePos - lastTouchPos > 180) lastTouchPos += 360;
                if(lastTouchPos - finePos > 180) lastTouchPos -= 360;
                
                // if(D) USB.printf("mv %d %d\r\n",finePos,lastTouchPos);
                
                // brightness -= (finePos - lastTouchPos);
                // brightness -= (finePos - lastTouchPos) * 3 / 4;
                watch->setBrightness(watch->getBrightness() - ((finePos - lastTouchPos) * 3 / 4));
                lastTouchPos = finePos;

                printBrightness(watch->getBrightness());          
                
            }

            break;
        case RELEASED: 
            
            if(firstTouch == touchIndex) firstTouch = -1;
       
            break;
    
    }
}

void button(int dir,int index) {

}



};

#endif