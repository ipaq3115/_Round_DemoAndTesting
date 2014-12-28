

// #if 1

#include "roundUtil.h"

class XKCDclockPage : public Page {

    protected:
    
        int firstTouchID = -1;
        int lastPrint = -1;
        
        SdFile img;
        
    public:

        XKCDclockPage() {}
        
        XKCDclockPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        
};

void XKCDclockPage::initalize() {
    
    // if(!img.open("needleA.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    if(!img.open("xkcdnowb.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    
    watch->loadVideo(img,0,0);
    
    watch->videoFrame(0);
    
    firstTouchID = -1;
    lastPrint = -1;
    
}

void XKCDclockPage::leavingPage() {

}

void XKCDclockPage::loop() {

}

void XKCDclockPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    switch(touchType) {
    
        case PRESSED: 
            if(firstTouchID == -1) firstTouchID = touchIndex;
            break;
        case MOVING:
            // if(firstTouchID == touchIndex) {
                
                if(abs(lastPrint - finePos) > 3) {
                
                    watch->videoFrame(finePos);
                    lastPrint = finePos;
                
                }
            
            // }
            break;
        case RELEASED: 
            if(firstTouchID == touchIndex) firstTouchID = -1;
            break;
    
    }

}

void XKCDclockPage::button(int dir,int index) {

}
