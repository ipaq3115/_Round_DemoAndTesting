

#ifndef LED_RING_CONTROL_H
#define LED_RING_CONTROL_H

#include <myUtils.h>

class ledRingControlPage : public Page {
public:

ledRingControlPage() {}

ledRingControlPage CONSTRUCTOR_MACRO

int myTouch = -1;
int myTouchPos = -1;

int const RING_LED_COUNT = 10;

int const degreesPerLED = 360 / RING_LED_COUNT;

void initalize(int mode,char * data) { initalize(); } // Optional

void initalize() {

    watch->clrScr();

}

void redraw() { }

void updateRing(int pos,int color) {
    
    int pointx,pointy;
    
    watch->setColor(color);
    
    int orient = watch->getLcdOrientation();
    
    watch->setLcdOrientation(3);
    
    for(int i=0;i<RING_LED_COUNT;i++) {

        MyUtils::orbitPoint(110,110,i * degreesPerLED * -1,90,pointx,pointy);
    
        watch->fillCircle(pointx,pointy,20);
        
        if(pos != -1 && pos/degreesPerLED == i) {
            
            watch->setColor(VGA_BLACK);
            watch->fillCircle(pointx,pointy,10);
            watch->setColor(color);
        
        }
    
    }
    
    // watch->setFont(SmallFont);
    // watch->printNumI(compass->a.x,CENTER,90,6,'0');
    // watch->printNumI(compass->a.y,CENTER,105,6,'0');
    // watch->printNumI(compass->a.z,CENTER,120,6,'0');

    watch->setLcdOrientation(orient);
    
}

void loop() {

    static elapsedMillis time;
    // static int xMax = -4000,yMax = -4000,zMax = -4000;
    // static int xMin =  4000,yMin =  4000,zMin =  4000;
    
    // if(time > 100) {
    
        time = 0;
    
        compass->read();
        
        // xMax = max(xMax,compass->a.x);
        // yMax = max(yMax,compass->a.y);
        // zMax = max(zMax,compass->a.z);
        
        // xMin = min(xMin,compass->a.x);
        // yMin = min(yMin,compass->a.y);
        // zMin = min(zMin,compass->a.z);
        
        // int tmpXmax = xMax - xMin;
        // int tmpYmax = yMax - yMin;
        // int tmpZmax = zMax - zMin;
        
        // int r = (compass->a.x - xMin) / ((float)tmpXmax / 255.0);
        // int g = (compass->a.y - yMin) / ((float)tmpYmax / 255.0);
        // int b = (compass->a.z - zMin) / ((float)tmpZmax / 255.0);
        
        int r = (compass->a.x + 1000) / 8;
        int g = (compass->a.y + 1000) / 8;
        int b = (compass->a.z + 1000) / 8;
        
        if(r<25) r = 0;
        if(g<25) g = 0;
        if(b<25) b = 0;
        
        if(r>225) r = 255;
        if(g>225) g = 255;
        if(b>225) b = 255;
        
        // updateRing(-1,VGA_BLACK);
        updateRing(myTouchPos,watch->RGBto565(r,g,b));
        // updateRing(-1,watch->RGBto565(0,0,b));
        
    
    // }

}

void leavingPage() {}
void lowPowerLoop() {} // Optional
void serviceLoop() {} // Optional - Called for every page

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(D) db.printf("touch rotation %d\r\n",watch->getTouchOrientation());

    finePos += watch->getTouchOrientation()+90;
    if(finePos < 0) finePos += 360;
    if(finePos >= 360) finePos -= 360;

    switch(touchType) {
    
        case PRESSED:
            if(myTouch == -1) {
                myTouch = touchIndex;
                myTouchPos = finePos + degreesPerLED/2; if(myTouchPos >= 360) myTouchPos -= 360;
            }
            break;
        case MOVING:
            if(myTouch == touchIndex) {
                myTouchPos = finePos + degreesPerLED/2; if(myTouchPos >= 360) myTouchPos -= 360;
            }
            break;
        case RELEASED:
            if(myTouch == touchIndex) {
                myTouch = -1;
                myTouchPos = -1;
            }
            break;
    
    }

}


void button(int dir,int index) {}
void bluetoothCallback(bt_event event) { }  // Optional - Called for every page on bluetooth event


};

#endif