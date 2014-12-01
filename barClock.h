

#ifndef BAR_CLOCK_PAGE_
#define BAR_CLOCK_PAGE_

#include "roundUtil.h"

class BarClockPage : public Page {

    protected:
    
        int touchIndex = -1;
        
        SdFile 
        
        ringFile,
        ringA,
        ringB,
        ringC;
    
        elapsedMillis updateTime;
        
        int sec=0,minu=0,hour=0;
        int secO=0,minuO=0,hourO=0;
        
    public:

        BarClockPage() {}
        
        BarClockPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        // void printTime(int hours,int minutes);
        // void printDate(int day,int month,int year);
        
        
};

void BarClockPage::initalize() {

    if(!ringFile.isOpen()) if(!ringFile.open("ringBack.Gci",O_RDWR)) if(D) USB.println("Couldn't open file");
    if(!ringA.isOpen())    if(!ringA.open("ringA.Gci",O_RDWR)) if(D) USB.println("Couldn't open file");
    if(!ringB.isOpen())    if(!ringB.open("ringB.Gci",O_RDWR)) if(D) USB.println("Couldn't open file");
    if(!ringC.isOpen())    if(!ringC.open("ringC.Gci",O_RDWR)) if(D) USB.println("Couldn't open file");
    
    lcd->printGci(ringFile,0,0);
    
    secO = -1;
    minuO = -1;
    hourO = -1;
    
    touchIndex = -1;
    
    updateTime = 101;
    
    loop();

}

void BarClockPage::leavingPage() {

}

void BarClockPage::loop() {

    if(updateTime > 100) {
        
        updateTime = 0;
        
        sec  = curTime.second * 2;
        minu = curTime.minute * 2;
        hour = curTime.hour * 5;
     
        if(sec != secO)  lcd->printGci(ringA,0,0,sec);
        if(minu != minuO) lcd->printGci(ringB,0,0,minu);
        if(hour != hourO) lcd->printGci(ringC,0,0,hour);
        
        secO = sec;
        minuO = minu;
        hourO = hour;

    }
    
}

void BarClockPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    switch(touchType) {
    
        case PRESSED: 
            // if(this->touchIndex == -1) this->touchIndex = touchIndex;
            break;
        case MOVING: 
            break;
        case RELEASED: 
            // if(this->touchIndex == touchIndex) goPage(PAGE::HOME); 
            break;
    
    }

}

void BarClockPage::button(int dir,int index) {

}

#endif
