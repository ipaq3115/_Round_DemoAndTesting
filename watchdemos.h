

#ifndef WATCH_DEMOS_
#define WATCH_DEMOS_

#include "roundUtil.h"

class ClockDemoPage : public Page {

    protected:
    
        int mode = 7;
        int oldMode = -1;
        
    public:

        ClockDemoPage() {}
        
        ClockDemoPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        
};

void ClockDemoPage::initalize() {
    
    SdFile backgroundImageFile;

    if(backgroundImageFile.isOpen()) backgroundImageFile.close();

    switch(mode) {
    
        case 0: if(!backgroundImageFile.open("bently2.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 1: if(!backgroundImageFile.open("bently.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 2: if(!backgroundImageFile.open("blue.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 3: if(!backgroundImageFile.open("blueneon.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 4: if(!backgroundImageFile.open("redneon.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 5: if(!backgroundImageFile.open("radian.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 6: if(!backgroundImageFile.open("radian2.bmp",O_RDWR)) USB.println("File open fail"); break;
        case 7: if(!backgroundImageFile.open("black.bmp",O_RDWR)) USB.println("File open fail"); break;
        
    }
    
    lcd->printBitmap(backgroundImageFile,0,0);

    oldMode = mode;

}

void ClockDemoPage::leavingPage() {

}

void ClockDemoPage::loop() {

    if(oldMode != mode) initalize();

}

void ClockDemoPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    switch(touchType) {
    
        case PRESSED: 
            break;
        case MOVING: 
            break;
        case RELEASED:

            // Left
            if(finePos > 180) { if(++mode > 7) mode = 0; } 
            
            // Right
            else { if(--mode < 0) mode = 7; }
            
            break;
    
    }

}

void ClockDemoPage::button(int dir,int index) {

}

#endif
