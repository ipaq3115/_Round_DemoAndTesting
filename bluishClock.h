

#ifndef BLUISH_CLOCK_PAGE_
#define BLUISH_CLOCK_PAGE_

#define cacheFilename "bluishCH.gci"

class BluishClockPage : public Page {

public:

int currentHour = -1;
int currentMinute = -1;
int currentSecond = -1;

BluishClockPage CONSTRUCTOR_MACRO

SdFile secondsFile;

elapsedMillis time;

void initalize() {

    if(D) USB.println("initalize BluishClockPage");

    if(secondsFile.isOpen()) secondsFile.close();

    // if(!secondsFile.open("blusec.gci",O_RDWR)) USB.println("File open fail");
    if(!secondsFile.open("bentlyS.gci",O_RDWR)) USB.println("File open fail");
    
    // lcd->backgroundInfo.file.close();
    // lcd->setBackgroundImage("test.gci",0,0);
    // // lcd->setBackgroundImage("bluish.bmp",0,0);
    
    mergeBackground(now());
    
    lcd->setBackgroundImage(cacheFilename,0,0);
    lcd->printImage(&lcd->backgroundInfo);
    
    // lcd->printGci(secondsFile,0,0,second());
    
    currentHour = -1;
    currentMinute = -1;
    currentSecond = -1;
    
    time = 1000;
    
    // loop();
    
}

void leavingPage() {

    if(secondsFile.isOpen()) secondsFile.close();

}

void mergeBackground(time_t tmpTime) {

    lcd->backgroundInfo.file.close();
    
    SdFile backfile,frontfile,newfile;
    
    sd.remove(cacheFilename);
    
    // if(!backfile.open("PiSplash.gci",O_RDWR)) Serial.println("bluish didn't load");
    // if(!backfile.open("bluish.gci",O_RDWR)) Serial.println("bluish didn't load");
    // if(!backfile.open("radBack.gci",O_RDWR)) Serial.println("radBack didn't load");
    if(!backfile.open("kickBack.gci",O_RDWR)) Serial.println("kickBack didn't load");
    if(!frontfile.open("blumin.gci",O_RDWR)) Serial.println("blumin didn't load");
    if(!newfile.open("tmp.gci",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,minute(tmpTime));
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    if(!backfile.open("tmp.gci",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("bluhour.gci",O_RDWR)) Serial.println("bluhour didn't load");
    if(!newfile.open(cacheFilename,O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,(hour(tmpTime) % 12)  * 5);
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    sd.remove("tmp.gci");
    
    lcd->setBackgroundImage(cacheFilename,0,0);
    

}

void loop() {

    static int secondsValueToPrint = -1;
    static unsigned long secondsTimeToPrint = 0;

    if(time > 1000) {
        
        time = 0;
        
        time_t newTime = now();
        tmElements_t timeHolder;
        
        breakTime(newTime, timeHolder);
        
        if(timeHolder.Second == 59) {
        
            mergeBackground(newTime + 1);
        
        }
    
        if(timeHolder.Second != currentSecond) {
        
            secondsTimeToPrint = millis() + 800;
            secondsValueToPrint = timeHolder.Second + 1;
            if(secondsValueToPrint >= 60) secondsValueToPrint -= 60;
        
            currentSecond = timeHolder.Second;
            
            // lcd->printImage(&lcd->backgroundInfo);

            // lcd->printGci(secondsFile,0,0,countc);
            
        }
        
    }
    
    if(secondsTimeToPrint != 0 && millis() >= secondsTimeToPrint) {
        
        secondsTimeToPrint = 0;
    
        lcd->printImage(&lcd->backgroundInfo);
        
        lcd->printGci(secondsFile,0,0,secondsValueToPrint);
        
    }

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(touchType != MOVING) if(D) USB.printf("touch() finePos %d\r\n",finePos);

    switch(touchType) {
    
        case PRESSED: break;
        case MOVING: break;
        case RELEASED: break;
    
    }

}

void button(int dir,int index) {

}

};

#endif
























