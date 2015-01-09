
#ifndef SETTINGS_PAGE_B_
#define SETTINGS_PAGE_B_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class SettingsPageB : public Page {

public:

    enum {
    
        VIBRATE,
        TIME,
        BRIGHTNESS,
        VOLUME,
        BLUETOOTH
    
    };

    int oldResponseRate = 0;

    image_info 
    backgroundImageFile,
    knobFile,
    vibrateFile,
    volumeFile;
    
    // Touch
    int lastTouchPos;
    int touchMode = -1;
    int touchTime = 0;
    bool repeat = false;
    

    int firstTouch = -1;
    int knobPos = 0;
    int iconID = 0;

    int volume = 0;

SettingsPageB() {}

SettingsPageB CONSTRUCTOR_MACRO

void initalize() {

    watch->loadImage("setB.bmp",&backgroundImageFile);
    watch->loadImage("knob.Gci",&knobFile);
    watch->loadImage("setvib.Gci",&vibrateFile);
    watch->loadImage("setvol.Gci",&volumeFile);
    
    // if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    // if(knobFile.isOpen()) knobFile.close();
    // if(vibrateFile.isOpen()) vibrateFile.close();
    // if(volumeFile.isOpen()) volumeFile.close();
    // 
    // if(!backgroundImageFile.open("setB.bmp",O_RDWR)) USB.println("File open fail");
    // if(!knobFile.open("knob.Gci",O_RDWR)) USB.println("File open fail");
    // if(!vibrateFile.open("setvib.Gci",O_RDWR)) USB.println("File open fail");
    // if(!volumeFile.open("setvol.Gci",O_RDWR)) USB.println("File open fail");
    
    // watch->printBitmap(backgroundImageFile,0,0);
    
    watch->printImage(&backgroundImageFile,0,0);
    
    // printBrightness(brightness);
    // printBattery(42);
    
    firstTouch = -1;
    knobPos = 115;
    iconID = 2;
    
    if(D) USB.println("SettingsPageB page initalize is done");
    
    // Set response rate
    oldResponseRate = watch->getResponseRateRaw();
    watch->setResponseRate(50);
    
}

void leavingPage() {

    backgroundImageFile->file.close();
    knobFile->file.close();
    vibrateFile->file.close();
    volumeFile->file.close();

    // Return the response rate to the previous condition
    watch->setResponseRateRaw(oldResponseRate);

}

void loop() {

    static elapsedMillis time;
    static int count = 0;
    static bool dir = true;
    
    if(time > 100) {
    
        time = 0;
        
        // count += dir ? 3 : -3;
        
        // if(count >= 360 || count <= 0) dir = !dir;
        
        if(dir) {
            count++;
            // if(count == 1) count++;
            if(count == 2) dir = false;
        } else {
            count--;
            // if(count == 1) count--;
            if(count == 0) dir = true;
        }
        
        watch->printImage(&vibrateFile,19,126,count);
        // watch->printRaw(knobFile,44,44,count);
        
        // printBrightness(count);
        // printBattery(count);
    
    }

}

void updateVolume(int dir) {

    if(dir == UP)   volume++;
    else            volume--;

    if(volume > 4) volume = 4;
    if(volume < 0) volume = 0;
    
    watch->printImage(&volumeFile,170,53,volume ? volume + 1 : 0);
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    if(D && touchType != MOVING) USB.printf("homePageTouch\r\n");
    
    switch(touchType) {
    
        case PRESSED:
        
            if(firstTouch == -1) {
            
                firstTouch = touchIndex;

                lastTouchPos = finePos;
                
                touchTime = millis();
                
                repeat = false;
                
                if(finePos > 126 && finePos < 234) {
                
                    touchMode = 0;
                
                } else {
                
                    touchMode = 1;
                
                }
        
            }
            
            break;
        case MOVING:
            
            if(firstTouch == touchIndex) touchMoving(finePos);
            
            break;
        case RELEASED: 
            
            if(firstTouch == touchIndex) {
            
                touchReleased(finePos);
            
                firstTouch = -1;
        
                touchMode = -1;
                
            }
       
            break;
    
    }
}

void touchMoving(int finePos) {

    // Outside of the 'tap to change' button
    if(touchMode == 1) {
    
        // Calculate the dragging of the knob
    
        if(finePos - lastTouchPos > 180) lastTouchPos += 360;
        if(lastTouchPos - finePos > 180) lastTouchPos -= 360;
        
        knobPos += (finePos - lastTouchPos);
        lastTouchPos = finePos;
        
        if(knobPos > 229) knobPos = 229;
        if(knobPos < 0) knobPos = 0;
        
        watch->printImage(&knobFile,44,44,knobPos);
        
    // Touched the 'tap to change' button
    } else {
        
        if(millis() - touchTime > 750 || (repeat && millis()-touchTime>20)) {
        
            switch(iconID) {
                case BRIGHTNESS:    
                    watch->setBrightness(watch->getBrightness() + (finePos > 180 ? -5 : 5)); break;
                case VOLUME:        
                    updateVolume(finePos > 180 ? DOWN : UP); break;
            }
            
            repeat = true;
            
            touchTime = millis();

        }
        
    }
    
}

void touchReleased(int finePos) {

    int const iconPosions[5] {0,50,115,175,229};
    
    // Outside of the 'tap to change' button
    if(touchMode == 1) {

        // Button press, move the knob to the position that was pressed
        if(millis() - touchTime < 200) {
        
            knobPos = finePos + 115;
            if(knobPos >= 360) knobPos -= 360;
            
        }
        
        // Find the icon that the knob is closest to
        int lastDistance = 0;
        fori(5) {
            
            int distance = abs(knobPos - iconPosions[i]);
            
            // Comparing this distance and the last distance, if
            // this index is closer set the iconID and continue searching
            if(lastDistance > distance || i == 0) {
            
                iconID = i;
                lastDistance = distance;
            
            }
        
        }
        
        // Snap the knob to the icon that was determined to be the closest
        // to the position it was realesed at
        knobPos = iconPosions[iconID];
        watch->printImage(&knobFile,44,44,knobPos);
    
    // Touched the 'tap to change' button
    } else {

        // Only looking for a tap action
        if(millis() - touchTime < 200) {
            
            switch(iconID) {            
                case BRIGHTNESS:    
                    watch->setBrightness(watch->getBrightness() + (finePos > 180 ? -10 : 10)); break;
                case VOLUME:
                    updateVolume(finePos > 180 ? DOWN : UP); break;
            }

        }
        
    }

}

void button(int dir,int index) {

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

void updateVolume() {

}

};


#endif