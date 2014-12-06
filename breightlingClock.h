

#ifndef BREIGHTLING_CLOCK_PAGE_
#define BREIGHTLING_CLOCK_PAGE_

class BreightlingClockPage : public Page {
    
public:

int currentHour = -1;
int currentMinute = -1;
int currentSecond = -1;

BreightlingClockPage CONSTRUCTOR_MACRO

SdFile backFile,secondsFile;

void initalize() {
    
    // SdFile backfile,frontfile,newfile;
    // 
    // sd.remove("test.bmp");
    // 
    // backfile.close();
    // frontfile.close();
    // newfile.close();
    // 
    // if(!backfile.open("PiSplash.bmp",O_RDWR)) Serial.println("bluish didn't load");
    // // if(!backfile.open("bluish.bmp",O_RDWR)) Serial.println("bluish didn't load");
    // if(!frontfile.open("blusec.gci",O_RDWR)) Serial.println("blusec didn't load");
    // if(!newfile.open("test.bmp",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    // 
    // lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,10);
    // 
    // backfile.close();
    // frontfile.close();
    // newfile.close();
    
    // backfile.close();
    // frontfile.close();
    // newfile.close();
    // 
    // if(!backfile.open("test.bmp",O_RDWR)) Serial.println("bluish didn't load");
    // if(!frontfile.open("blumin.gci",O_RDWR)) Serial.println("blusec didn't load");
    // if(!newfile.open("test.bmp",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    // 
    // lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,10);
    // 
    // lcd->printBitmap(newfile,0,0);
    // 
    // return;
    
    
    if(D) USB.println("initalize breightling");

    // if(backFile.isOpen()) backFile.close();
    if(secondsFile.isOpen()) secondsFile.close();

    // if(!backFile.open("bluish.bmp",O_RDWR)) USB.println("File open fail");
    // if(!secondsFile.open("blusec.gci",O_RDWR)) USB.println("File open fail");
    if(!secondsFile.open("bentlyS.gci",O_RDWR)) USB.println("File open fail");
    
    // lcd->printBitmap(backFile,0,0);
    
    // image_info info;
    // lcd->loadHeader(backFile,&info);
    
    lcd->backgroundInfo.file.close();
    lcd->setBackgroundImage("test.gci",0,0);
    // lcd->setBackgroundImage("bluish.bmp",0,0);
    
    lcd->printImage(&lcd->backgroundInfo);
    
    lcd->printGci(secondsFile,0,0,0);
    
    currentHour = -1;
    currentMinute = -1;
    currentSecond = -1;
    
}

void leavingPage() {

    if(backFile.isOpen()) backFile.close();
    if(secondsFile.isOpen()) secondsFile.close();

}

void loop() {

    static int secondsValueToPrint = -1;
    static unsigned long secondsTimeToPrint = 0;

    static elapsedMillis time;
    
    if(time > 10) {
        time = 0;
        
        time_t newTime = now();
        tmElements_t timeHolder;
        
        breakTime(newTime, timeHolder);
        
        bool somethingPrinted = false;
        int timeA = micros();
        
        if(timeHolder.Second == 59) {
        
            time_t tmpTime = newTime + 1;
            tmElements_t timeHolderTwo;
            
            breakTime(tmpTime, timeHolderTwo);
        
        
        // if(timeHolder.Minute != currentMinute) {
        
            // currentMinute = timeHolder.Minute;
            
            lcd->backgroundInfo.file.close();
            
            SdFile backfile,frontfile,newfile;
            
            sd.remove("test.gci");
            
            // if(!backfile.open("bluish.bmp",O_RDWR)) Serial.println("bluish didn't load");
            // if(!frontfile.open("blusec.gci",O_RDWR)) Serial.println("blusec didn't load");
            // if(!frontfile.open("bluhour.gci",O_RDWR)) Serial.println("bluhour didn't load");
            // if(!backfile.open("PiSplash.gci",O_RDWR)) Serial.println("bluish didn't load");
            
            if(!backfile.open("bently.gci",O_RDWR)) Serial.println("bluish didn't load");
            if(!frontfile.open("bentlyM.gci",O_RDWR)) Serial.println("blumin didn't load");
            // if(!frontfile.open("blumin.gci",O_RDWR)) Serial.println("blumin didn't load");
            if(!newfile.open("tmp.gci",O_RDWR | O_CREAT)) Serial.println("test didn't load");
            
            lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,timeHolderTwo.Minute);
            
            backfile.close();
            frontfile.close();
            newfile.close();
            
            if(!backfile.open("tmp.gci",O_RDWR)) Serial.println("bluish didn't load");
            if(!frontfile.open("bentlyH.gci",O_RDWR)) Serial.println("bluhour didn't load");
            // if(!frontfile.open("bluhour.gci",O_RDWR)) Serial.println("bluhour didn't load");
            if(!newfile.open("test.gci",O_RDWR | O_CREAT)) Serial.println("test didn't load");
            
            lcd->mergeImages(&newfile,&backfile,&frontfile,0,0,(timeHolderTwo.Hour % 12)  * 5);
            
            backfile.close();
            frontfile.close();
            newfile.close();
            
            sd.remove("tmp.gci");
            
            lcd->setBackgroundImage("test.gci",0,0);
            
            // if(!frontfile.open("blumin.gci",O_RDWR)) Serial.println("blumin didn't load");
            // if(!backfile.open("bluish.bmp",O_RDWR)) Serial.println("bluish didn't load");
            // if(!frontfile.open("blusec.gci",O_RDWR)) Serial.println("blusec didn't load");
        
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
























