

#ifndef KICKSTARTER_CLOCK_PAGE_
#define KICKSTARTER_CLOCK_PAGE_

#define cacheFilename "kickCH.gci"

class KickstarterClockPage : public Page {

public:

int currentHour = -1;   
int currentMinute = -1;
int currentSecond = -1;

KickstarterClockPage CONSTRUCTOR_MACRO

image_info secondsFile;

elapsedMillis time;

void initalize() {

    if(D) USB.println("initalize KickstarterClockPage");

    // if(secondsFile.isOpen()) secondsFile.close();
    // 
    // // if(!secondsFile.open("blusec.gci",O_RDWR)) USB.println("File open fail");
    // if(!secondsFile.open("bentlyS.gci",O_RDWR)) USB.println("File open fail");
    
    watch->loadImage("bentlyS.gci",&secondsFile);
    
    // watch->clrBackground();
    // watch->setBackground("test.gci",0,0);
    // // watch->setBackground("bluish.bmp",0,0);
    
    mergeBackground(now());
    
    watch->setBackground(cacheFilename,0,0);
    watch->printBackground();
    
    watch->printImage(&secondsFile,0,0,second());
    
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

    watch->clrBackground();
    
    SdFile backfile,frontfile,newfile;
    
    sd.remove(cacheFilename);
    
    if(!backfile.open("kickBack.gci",O_RDWR)) Serial.println("kickBack didn't load");
    if(!frontfile.open("blumin.gci",O_RDWR)) Serial.println("blumin didn't load");
    if(!newfile.open("tmp.gci",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    watch->mergeImages(&newfile,&backfile,&frontfile,0,0,minute(tmpTime));
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    if(!backfile.open("tmp.gci",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("bluhour.gci",O_RDWR)) Serial.println("bluhour didn't load");
    if(!newfile.open(cacheFilename,O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    watch->mergeImages(&newfile,&backfile,&frontfile,0,0,(hour(tmpTime) % 12)  * 5 + minute(tmpTime) / 12);
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    sd.remove("tmp.gci");
    
    watch->setBackground(cacheFilename,0,0);
    

}

void loop() {

    if(time > 10) {
        
        time = 0;
        
        time_t newTime = now();
        
        if(second(newTime) != currentSecond) {
            
            currentSecond = second(newTime);
            
            watch->printBackground();

            watch->printImage(&secondsFile,0,0,second(newTime));
            
            if(second(newTime) == 59) mergeBackground(newTime + 1);
            
        }
        
    }
   
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(touchType != MOVING) if(D) USB.printf("touch() finePos %d\r\n",finePos);

    switch(touchType) {
    
        case PRESSED: break;
        case MOVING: break;
        case RELEASED: 
            goPage(PAGE::HOME);
            break;
    
    }
    

}

void button(int dir,int index) {

}

};

#endif



