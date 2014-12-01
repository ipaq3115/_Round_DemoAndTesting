
#ifndef SETTINGS_PAGE_B_
#define SETTINGS_PAGE_B_

#include <SdFat.h>
#include "pageClass.h"
#include "roundUtil.h"

class SettingsPageB : public Page {

public:

    SdFile 
    backgroundImageFile,
    knobFile,
    vibrateFile,
    volumeFile;

    int firstTouch = -1;
    // int brightness;
    int knobPos = 0;
    int iconID = 0;

    int volume = 0;

SettingsPageB() {}

SettingsPageB CONSTRUCTOR_MACRO

void initalize() {
    
    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    if(knobFile.isOpen()) knobFile.close();
    if(vibrateFile.isOpen()) vibrateFile.close();
    if(volumeFile.isOpen()) volumeFile.close();
    
    if(!backgroundImageFile.open("setB.bmp",O_RDWR)) USB.println("File open fail");
    if(!knobFile.open("knob.Gci",O_RDWR)) USB.println("File open fail");
    if(!vibrateFile.open("setvib.Gci",O_RDWR)) USB.println("File open fail");
    if(!volumeFile.open("setvol.Gci",O_RDWR)) USB.println("File open fail");
    
    lcd->printBitmap(backgroundImageFile,0,0);
    
    // printBrightness(brightness);
    // printBattery(42);
    
    firstTouch = -1;
    knobPos = 115;
    iconID = 2;
    
    if(D) USB.println("SettingsPageB page initalize is done");
    
}

void leavingPage() {

    backgroundImageFile.close();
    knobFile.close();
    vibrateFile.close();
    volumeFile.close();

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
        
        lcd->printGci(vibrateFile,19,126,count);
        // lcd->printGci(knobFile,44,44,count);
        
        // printBrightness(count);
        // printBattery(count);
    
    }

}

void updateVolume(int dir) {

    if(dir == UP)   volume++;
    else            volume--;

    if(volume > 4) volume = 4;
    if(volume < 0) volume = 0;
    
    lcd->printGci(volumeFile,170,53,volume ? volume + 1 : 0);
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    if(D && touchType != MOVING) USB.printf("homePageTouch\r\n");
    
    int const iconPosions[5] {0,50,115,175,229};
    
    static int lastTouchPos;
    static int touchMode = -1;
    static int touchTime = 0;
    static bool repeat = false;
    
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
            
            if(firstTouch == touchIndex) {
            
                if(touchMode == 1) {
                
                    if(finePos - lastTouchPos > 180) lastTouchPos += 360;
                    if(lastTouchPos - finePos > 180) lastTouchPos -= 360;
                    
                    // if(D) USB.printf("mv %d %d\r\n",finePos,lastTouchPos);
                    
                    // brightness -= (finePos - lastTouchPos);
                    knobPos += (finePos - lastTouchPos);
                    // brightness -= (finePos - lastTouchPos) * 3 / 4;
                    lastTouchPos = finePos;
                    
                    if(knobPos > 229) knobPos = 229;
                    if(knobPos < 0) knobPos = 0;
                    // if(brightness < 0) brightness = 0;
                    
                    lcd->printGci(knobFile,44,44,knobPos);
                    
                    // printBrightness(brightness);
                    // printBrightness(brightness/5*5);
                    
                    // checkBrightness();                
                    
                } else {
                
                    // Volume
                    if(iconID == 3 && millis() - touchTime > 750 || (repeat && millis()-touchTime>20)) {
                    
                        updateVolume(finePos > 180 ? DOWN : UP);
                    
                        repeat = true;
                        
                        touchTime = millis();
                        
                    
                    // Brightness
                    } else if(iconID == 2 && millis() - touchTime > 750 || (repeat && millis()-touchTime>20)) {
                    
                        // if(finePos - lastTouchPos > 180) lastTouchPos += 360;
                        // if(lastTouchPos - finePos > 180) lastTouchPos -= 360;
                        // 
                        // // if(D) USB.printf("mv %d %d\r\n",finePos,lastTouchPos);
                        // 
                        // // brightness -= (finePos - lastTouchPos) / 2;
                        // // brightness -= (finePos - lastTouchPos) * 3 / 4;
                        // lastTouchPos = finePos;
                        // 
                        // if(knobPos > 100) knobPos = 100;
                        // if(brightness < 0) brightness = 0;
                        // 
                        // // printBrightness(brightness);
                        // // printBrightness(brightness/5*5);
                        // 
                        // checkBrightness(); 
                        
                        watch->setBrightness(watch->getBrightness() + (finePos > 180 ? -5 : 5));
                        
                        repeat = true;
                        
                        touchTime = millis();
                        
                    }
                
                
                }
                
            }

            break;
        case RELEASED: 
            
            if(firstTouch == touchIndex) {
            
                if(touchMode == 1) {
            
                    if(millis() - touchTime < 100) {
                    
                        knobPos = finePos + 115;
                        if(knobPos >= 360) knobPos -= 360;
                        
                    }
                    
                    touchMode = -1;
                    
                    int lastDistance = 0;
                    fori(5) {
                        
                        int distance = abs(knobPos - iconPosions[i]);
                        
                        if(lastDistance < distance && i != 0) {
                        
                            
                        
                        } else {
                            
                            iconID = i;
                            lastDistance = distance;
                        
                        }
                    
                    }
                    
                    knobPos = iconPosions[iconID];
                    lcd->printGci(knobFile,44,44,knobPos);
                    
                } else {
                
                    if(iconID == 2 && millis() - touchTime < 200) watch->setBrightness(watch->getBrightness() + (finePos > 180 ? -10 : 10));
                    
                    if(iconID == 3) updateVolume(finePos > 180 ? DOWN : UP);
                    
                }
                
                firstTouch = -1;
                
            }
       
            break;
    
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