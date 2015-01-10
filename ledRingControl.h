

#ifndef LED_RING_CONTROL_H
#define LED_RING_CONTROL_H

#include <myUtils.h>

class ledRingControlPage : public Page {
public:

ledRingControlPage() {}

ledRingControlPage CONSTRUCTOR_MACRO

int myTouch = -1;
int myTouchPos = -1;

int const RING_LED_COUNT = 16;

int const degreesPerLED = 360 / (RING_LED_COUNT - 1);

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
    
        watch->fillCircle(pointx,pointy,15);
        
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

    
    if(time > 70) {
    
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
        
        

        int ballDegreePos = MyUtils::radianstodegrees(atan2(compass->a.x,compass->a.y)) + 90;

        while(ballDegreePos > 360) ballDegreePos -= 360; 
        while(ballDegreePos < 0) ballDegreePos += 360;

        int tmpDegreePos = ballDegreePos;

        int r;
        int g;
        int b;
        
        ballDegreePos /= 120;

        if(ballDegreePos==0) {
            r = ((ballDegreePos+1)*120 - tmpDegreePos) * 3;
            g = (tmpDegreePos - ballDegreePos*120) * 3;
            b = 0;
        } else if(ballDegreePos==1) {
            r = 0;
            g = ((ballDegreePos+1)*120 - tmpDegreePos) * 3;
            b = (tmpDegreePos - ballDegreePos*120) * 3;
        } else if(ballDegreePos==2) {
            r = (tmpDegreePos - ballDegreePos*120) * 3;
            g = 0;
            b = ((ballDegreePos+1)*120 - tmpDegreePos) * 3;
        }

        // ballDegreePos /= 60;
        // 
        // if(ballDegreePos==0) {
        //     r = 255;
        //     g = 0;
        //     b = 0;
        // } else if(ballDegreePos==2) {
        //     r = 0;
        //     g = 255;
        //     b = 0;
        // } else if(ballDegreePos==4) {
        //     r = 0;
        //     g = 0;
        //     b = 255;
        // } else if(ballDegreePos==1) {
        //     r = ((ballDegreePos+1)*60 - tmpDegreePos) * 6;
        //     g = (tmpDegreePos - ballDegreePos*60) * 6;
        //     b = 0;
        // } else if(ballDegreePos==3) {
        //     r = 0;
        //     g = ((ballDegreePos+1)*60 - tmpDegreePos) * 6;
        //     b = (tmpDegreePos - ballDegreePos*60) * 6;
        // } else if(ballDegreePos==5) {
        //     r = (tmpDegreePos - ballDegreePos*60) * 6;
        //     g = 0;
        //     b = ((ballDegreePos+1)*60 - tmpDegreePos) * 6;
        // }

        // if(D) db.printf("DEGREE POS %d\r\n",ballDegreePos);
        
        // int r = (compass->a.x + 1000) / 8;
        // int g = (compass->a.y + 1000) / 8;
        // int b = (compass->a.z + 1000) / 8;
        
        if(r<0) r = 0;
        if(g<0) g = 0;
        if(b<0) b = 0;
        
        if(r>255) r = 255;
        if(g>255) g = 255;
        if(b>255) b = 255;
        
        // updateRing(-1,VGA_BLACK);
        updateRing(myTouchPos,watch->RGBto565(r,g,b));
        // updateRing(-1,watch->RGBto565(0,0,b));
        
        r /= 10;
        g /= 10;
        b /= 10;
        
        char str[] = "COLOR FFFFFFFF";
        
        hexbytetostring(r,str,6);
        hexbytetostring(g,str,8);
        hexbytetostring(b,str,10);
        hexbytetostring(myTouchPos == -1 ? 255 : 15 - (myTouchPos / 24),str,12);
        
        bt.sendData(strlen(str),(byte*)str);
        
        // static int myTouchPosOld = -1;
        // 
        // if(myTouchPosOld != myTouchPos) {
        // 
        //     char strb[] = "POS FF";
        //     
        //     hexbytetostring(myTouchPos == -1 ? 255 : myTouchPos / 22,strb,4);
        //     
        //     bt.sendData(strlen(strb),(byte*)strb);
        // 
        //     myTouchPosOld = myTouchPos;
        // 
        // }
    
    }

}

void leavingPage() {}
void lowPowerLoop() {} // Optional
void serviceLoop() {} // Optional - Called for every page

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    // if(D) db.printf("touch rotation %d\r\n",watch->getTouchOrientation());

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