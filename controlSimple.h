

class ControlSimplePage : public Page {

    protected:
    
        SdFile backgroundImageFile;
        
        int firstTouchIndex = -1;
        int currentQuadrant = -1;

    public:

        ControlSimplePage() {}
        
        ControlSimplePage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);

};


void ControlSimplePage::initalize() {

    // if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    // 
    // if(!backgroundImageFile.open("controlS.gci",O_RDWR)) USB.println("File open fail");
    
    watch->loadImage("controlS.gci",&backgroundImageFile);
    
    watch->printImage(&backgroundImageFile,0,0,0);    
    
    firstTouchIndex = -1;
    
}

void ControlSimplePage::leavingPage() {

}

void ControlSimplePage::loop() {

    int newQuadrant = compass->heading() + 45;
    while(newQuadrant < 0) newQuadrant += 360;
    while(newQuadrant > 360) newQuadrant -= 360;
    newQuadrant /= 90;
    
    if(newQuadrant != currentQuadrant) {
    
        currentQuadrant = newQuadrant;
        
        watch->printImage(&backgroundImageFile,0,0,currentQuadrant);
        
    }

}

void ControlSimplePage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

}

void ControlSimplePage::button(int dir,int index) {

}


#if 0


namespace CONTROL_SIMPLE {

    int firstTouchIndex = -1;
    int currentQuadrant = -1;
    
}

void controlSimplePageInitialize() {

    using namespace CONTROL_SIMPLE;

    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    
    if(!backgroundImageFile.open("controlS.gci",O_RDWR)) USB.println("File open fail");
    
    watch.printImage(&backgroundImageFile,0,0,0);    
    
    firstTouchIndex = -1;
    
}

void controlSimplePageLoop() {

    using namespace CONTROL_SIMPLE;

    int newQuadrant = currentHeading + 45;
    while(newQuadrant < 0) newQuadrant += 360;
    while(newQuadrant > 360) newQuadrant -= 360;
    newQuadrant /= 90;
    
    if(newQuadrant != currentQuadrant) {
    
        currentQuadrant = newQuadrant;
        
        watch.printImage(&backgroundImageFile,0,0,currentQuadrant);
        
    }

}

void controlSimplePageTouch(int touchType,int finePos,int activeTouches,int touchIndex) {

    using namespace CONTROL_SIMPLE;

    if(D && touchType != MOVING) USB.printf("controlSimplePageTouch\r\n");
    
    return;
    
    switch(touchType) {
    
        case PRESSED:
        
            // if(D) USB.printf("Touch Pressed touchIndex %d firstTouchIndex %d\r\n",touchIndex,firstTouchIndex);
        
            if(firstTouchIndex == -1) firstTouchIndex = touchIndex;
        
            break;
        case MOVING:

            if(firstTouchIndex == -1) firstTouchIndex = touchIndex;
        
            // if(D) USB.printf("Touch Moving touchIndex %d firstTouchIndex %d\r\n",touchIndex,firstTouchIndex);
        
            if(firstTouchIndex == touchIndex) {
            
                int newQuadrant = finePos + 45;
                while(newQuadrant < 0) newQuadrant += 360;
                while(newQuadrant > 360) newQuadrant -= 360;
                newQuadrant /= 90;
                
                if(newQuadrant != currentQuadrant) {
                
                    currentQuadrant = newQuadrant;
                    
                    watch.printImage(&backgroundImageFile,0,0,currentQuadrant);
                    
                }
            
            }
            
            break;
        case RELEASED: 

            // if(D) USB.printf("Touch Released touchIndex %d firstTouchIndex %d\r\n",touchIndex,firstTouchIndex);

            if(firstTouchIndex == touchIndex) {
            
                firstTouchIndex = -1;
            
            }
        
            break;
    
    }

}

void controlSimplePageButtonEvent(int dir,int index) {

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