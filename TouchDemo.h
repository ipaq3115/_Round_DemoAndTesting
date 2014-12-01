

// #if 1

#include "roundUtil.h"

class TouchDemoPage : public Page {

    protected:
    
    public:

        TouchDemoPage() {}
        
        TouchDemoPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        void drawCapacitanceValue(int sizeValue,int sensor,int color);
        void testBubbles();
        

};

void TouchDemoPage::initalize() {
    
    lcd->setColor(0); 
    
    lcd->fillRect(0,0,220,220);
    
}

void TouchDemoPage::leavingPage() {

}

void TouchDemoPage::loop() {

    testBubbles();

}

void TouchDemoPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    static int lastDrawnDots[10];

    switch(touchType) {
    
        case PRESSED: 
            break;
        case MOVING: 
            
            drawpointer(110,110,70,lastDrawnDots[touchIndex],0);
            drawpointer(110,110,70,finePos,1);

            lastDrawnDots[touchIndex] = finePos;
            
            break;
        case RELEASED: 
        
            drawpointer(110,110,70,lastDrawnDots[touchIndex],0);
         
            break;
    
    }

}

void TouchDemoPage::button(int dir,int index) {

}

void TouchDemoPage::drawCapacitanceValue(int sizeValue,int sensor,int color) {

    int angle;
    int radius = 90;
    
    while(sensor >= PI_TOUCH_PIN_TOTAL) sensor -= PI_TOUCH_PIN_TOTAL;
    while(sensor < 0) sensor += PI_TOUCH_PIN_TOTAL;
    
    angle = sensor * 36;

    int centerx = 110;
    int centery = 110;

    // if(color) { lcd.setColor(random(255), random(255), random(255)); } 
    if(color) { lcd->setColor(255, 255, 255); }
    else { lcd->setColor(0, 0, 0); }
    
    angle -= 180;

    int xoffset = sin(MyUtils::degreestoradians(angle)) * radius;
    int yoffset = cos(MyUtils::degreestoradians(angle)) * radius;

    if(color) { lcd->fillCircle(centerx - xoffset,centery + yoffset,sizeValue); }
    else { lcd->fillCircle(centerx - xoffset,centery + yoffset,sizeValue + 1); }
    
}

void TouchDemoPage::testBubbles() {

    static int lastSensorValue[PI_TOUCH_PIN_TOTAL] {0,0,0,0,0,0,0,0,0,0};
    
    int tmpTouchArray[PI_TOUCH_PIN_TOTAL];
    
    // Read touch pins
    for(int i=0;i<PI_TOUCH_PIN_TOTAL;i++) {
    
        // Read into buffer
        // tmpTouchArray[i] = touchRead(touchPin[i]);
        tmpTouchArray[i] = touchCtrl->touchVals[i];
        
        int value = tmpTouchArray[i] / 100;
        if(value > 20) value = 20;
    
        drawCapacitanceValue(lastSensorValue[i],i,0);
        
        drawCapacitanceValue(value,i,1);
        
        lastSensorValue[i] = value;
        
    }
    
}
