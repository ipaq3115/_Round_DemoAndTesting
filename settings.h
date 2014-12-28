
#ifndef SETTINGS_PAGE_
#define SETTINGS_PAGE_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class SettingsPage : public Page {

    protected:
    
        SdFile 
        backgroundImageFile,
        brightnessFile,
        batteryBarFile,
        batteryPercentFile;
        
        int firstTouch = -1;
        // int brightness;

    public:

        SettingsPage() {}
        
        SettingsPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        void printBrightness(int percent);
        void printBattery(int percent);

};

void SettingsPage::printBrightness(int percent) {

    int value = 1.44 * percent;
    // int value = 144 * ((float)percent/(float)100);
    
    if(D) USB.printf("printBrightness value %d percent %d\r\n",value,percent);

    watch->printRaw(brightnessFile,7,38,0,0,0,45,144 - value,false);
    watch->printRaw(brightnessFile,7,38,1,0,144 - value + 1,45,144,false);
    
}

void SettingsPage::printBattery(int percent) {

    int value = 0.83 * percent;
    
    if(D) USB.printf("printBattery value %d percent %d\r\n",value,percent);

    watch->printRaw(batteryBarFile,68,7,1,0,0,value,23,false);
    watch->printRaw(batteryBarFile,68,7,0,value + 1,0,83,23,false);
    
    watch->printRaw(batteryPercentFile,74,25,percent);
    
}

void SettingsPage::initalize() {
    
    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    if(brightnessFile.isOpen()) brightnessFile.close();
    if(batteryBarFile.isOpen()) batteryBarFile.close();
    if(batteryPercentFile.isOpen()) batteryPercentFile.close();
    
    if(!backgroundImageFile.open("settings.bmp",O_RDWR)) USB.println("File open fail");
    if(!brightnessFile.open("setBrgt.Gci",O_RDWR)) USB.println("File open fail");
    if(!batteryBarFile.open("setBatB.Gci",O_RDWR)) USB.println("File open fail");
    if(!batteryPercentFile.open("setBatP.Gci",O_RDWR)) USB.println("File open fail");
    
    watch->printBitmap(backgroundImageFile,0,0);
    
    printBrightness(watch->getBrightness());
    printBattery(42);
    
    firstTouch = -1;
    
    if(D) USB.println("apps page initalize is done");
    
}

void SettingsPage::leavingPage() {

}

void SettingsPage::loop() {

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

void SettingsPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
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

void SettingsPage::button(int dir,int index) {

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