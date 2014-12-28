
#ifndef KICKSTARTER_DEMO_H
#define KICKSTARTER_DEMO_H

class KickstarterDemoPage : public Page {

public:

SdFile file;

int frame = 0;

elapsedMillis time;

KickstarterDemoPage() {}

KickstarterDemoPage CONSTRUCTOR_MACRO

void initalize() {

    file.open("kickdemo.gci",O_RDWR);
    
    watch->loadVideo(file,0,0);
    
    frame = 55;
    
    watch->videoFrame(frame++,0,0);
    
    // watch->setColor(VGA_WHITE);
    // watch->fillRect(0,0,219,219);
    
}

void leavingPage() {}
void touch(int touchType,int finePos,int activeTouches,int touchIndex) { if(touchType == RELEASED) goPage(PAGE::HOME); }
void button(int dir,int index) {}

void loop() {

    if(time > 1000) {
    
        time = 0;
        
        watch->videoFrame(frame++,0,0);
    
        if(frame > 65) frame = 55;
    
    }

}

};


#endif



































