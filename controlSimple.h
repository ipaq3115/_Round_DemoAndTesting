
#ifndef CONTROL_SIMPLE_PAGE_H_
#define CONTROL_SIMPLE_PAGE_H_

class ControlSimplePage : public Page {

public:

image_info backgroundImageFile;

int firstTouchIndex = -1;
int currentQuadrant = -1;

int oldHeading;

bool dotMode;

static int const BUFFER_SIZE = 50; 

int16_t avgary[BUFFER_SIZE];
int avgaryPtr;

ControlSimplePage CONSTRUCTOR_MACRO

void initalize() {
    
    watch->loadImage("controlS.raw",&backgroundImageFile);
    
    watch->setColor(BLACK);
    watch->fillRect(0,0,219,219);
    
    // watch->printImage(&backgroundImageFile,0,0,0);    
    
    firstTouchIndex = -1;
    dotMode = true;
    oldHeading = -1;
    
    for(int i=0;i<BUFFER_SIZE;i++) avgary[i] = 0; 
    avgaryPtr = 0;
    
}

void leavingPage() {

}

void loop() {

    if(dotMode) {
    
        int heading = compass->heading() * -1;
        
        // bool stoponthis = false;
        
        // if(oldHeading != -1 && abs(heading - oldHeading) > 180) stoponthis = true;
        
        avgaryPtr++;
        if(avgaryPtr >= BUFFER_SIZE) avgaryPtr = 0;
        avgary[avgaryPtr] = heading;
        
        int last = 0;
        
        heading = 0;
        for(int i=0;i<BUFFER_SIZE;i++) {
            
            int tmp = avgary[i];
            
            if(tmp - last >= 180) tmp -= 360;
            if(tmp - last <= -180) tmp += 360;
            
            // if(stoponthis) {
            // 
            //     db.printf("i %03d tmp %03d last %03d avgary[i] %03d\r\n", 
            //     i, tmp, last, avgary[i]);
            // 
            // }
            
            heading += tmp;
            last = tmp;
            
        }
        heading /= BUFFER_SIZE;
        

        // if(stoponthis) {
        // 
        //     while(true);
        // 
        // }
        
        if(oldHeading != heading) {
        
            int x,y;
        
            watch->setColor(BLACK);
            MyUtils::orbitPoint(110, 110, oldHeading, 90, x, y);
            watch->fillCircle(x, y, 10);

            watch->setColor(WHITE);
            MyUtils::orbitPoint(110, 110, heading, 90, x, y);
            watch->fillCircle(x, y, 10);
            
        }
        
        oldHeading = heading;
    
    } else {

        int newQuadrant = compass->heading() + 45;
        while(newQuadrant < 0) newQuadrant += 360;
        while(newQuadrant > 360) newQuadrant -= 360;
        newQuadrant /= 90;
        
        if(newQuadrant != currentQuadrant) {
        
            currentQuadrant = newQuadrant;
        
            watch->printImage(&backgroundImageFile,0,0,currentQuadrant);
            
        }

    }
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(touchType == RELEASED) {
    
        dotMode = true;
    
    }

}

void button(int dir,int index) {

}


};

#endif
