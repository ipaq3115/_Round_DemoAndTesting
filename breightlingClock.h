

#ifndef BREIGHTLING_CLOCK_PAGE_
#define BREIGHTLING_CLOCK_PAGE_

#define cacheFilename "breitCH.gci"

class BreightlingClockPage : public Page {
    
public:

int currentHour = -1;
int currentMinute = -1;
int currentSecond = -1;

BreightlingClockPage CONSTRUCTOR_MACRO

SdFile secondsFile;

elapsedMillis time;

void initalize() {
    
    if(D) USB.println("initalize BreightlingClockPage");

    if(secondsFile.isOpen()) secondsFile.close();
    
    if(!secondsFile.open("bentlyS.gci",O_RDWR)) USB.println("File open fail");
    
    // mergeBackground(now());
    
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
    if(!backfile.open("bently.gci",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("bentlyM.gci",O_RDWR)) Serial.println("blumin didn't load");
    if(!newfile.open("tmp.gci",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,minute(tmpTime));
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    if(!backfile.open("tmp.gci",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("bentlyH.gci",O_RDWR)) Serial.println("bluhour didn't load");
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

    if(time > 10) {
        time = 0;
        
        time_t newTime = now();
        tmElements_t timeHolder;
        
        breakTime(newTime, timeHolder);
        
        bool somethingPrinted = false;
        int timeA = micros();
        
        if(timeHolder.Second == 59 || !lcd->backgroundInfo.file.isOpen()) {
        
            mergeBackground(newTime + 1);
           
            somethingPrinted = true;
        
        }
    
        if(timeHolder.Second != currentSecond) {
        
            secondsTimeToPrint = millis() + 800;
            secondsValueToPrint = timeHolder.Second + 1;
            if(secondsValueToPrint >= 60) secondsValueToPrint -= 60;
        
            currentSecond = timeHolder.Second;
        
            // lcd->printImage(&lcd->backgroundInfo);
            // 
            // lcd->printGci(secondsFile,0,0,timeHolder.Second);
            // 
            // somethingPrinted = true;
        
        }
        
        int timeB = micros();

        if(somethingPrinted) Serial.printf("time total %d\r\n",timeB-timeA);
        
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
























