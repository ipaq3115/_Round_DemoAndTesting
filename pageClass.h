

#ifndef PAGE_
#define PAGE_

#include <BC127.h>
#include <SdFat.h>
// #include <PiTouch.h>
#include <PiWatch.h>

class Page {
    
    public:
    
        Page() { }
    
        BC127* bluetooth;
        LSM303_custom* compass;
        PiWatch* watch;
        int D = false;
        int E = false;
        
        Page(BC127* tmpBluetooth,LSM303_custom* tmpCompass,PiWatch* tmpWatch,int tmpD,int tmpE) {
        
            bluetooth = tmpBluetooth;
            compass = tmpCompass;
            watch = tmpWatch;
            D = tmpD;
            E = tmpE;
            
        }
        
        // Functions that subclasses need to inherit
        
        virtual void initalize() = 0;
        // virtual void initalize(int mode,char * data) = 0;
        virtual void initalize(int mode,char * data) { initalize(); }
        virtual void leavingPage() = 0;
        
        virtual void redraw() { initalize(); }
        
        virtual void loop() = 0;
        
        virtual void touch(int touchType,int finePos,int activeTouches,int touchIndex) = 0;
        virtual void button(int dir,int index) = 0;
        
    
    protected:

};

#endif
