

#ifndef BLUISH_CLOCK_PAGE_
#define BLUISH_CLOCK_PAGE_

#define cacheFilenameBluish "bluishCH.raw"

class BluishClockPage : public Page {

public:

int currentHour = -1;
int currentMinute = -1;
int currentSecond = -1;

BluishClockPage CONSTRUCTOR_MACRO

image_info secondsFile;

elapsedMillis time;

void initalize() {

    if(D) USB.println("initalize BluishClockPage");

    watch->loadImage("bentlyS.raw",&secondsFile);
    
    mergeBackground(now());
    
    watch->setBackground(cacheFilenameBluish,0,0);
    watch->printBackground();
    
    currentHour = -1;
    currentMinute = -1;
    currentSecond = -1;
    
    time = 1000;
    
}

void leavingPage() {

    secondsFile.file.close();

}

void mergeBackground(time_t tmpTime) {

    watch->clrBackground();
    
    SdFile backfile,frontfile,newfile;
    
    sd.remove(cacheFilenameBluish);
    
    if(!backfile.open("bluish.raw",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("blumin.raw",O_RDWR)) Serial.println("blumin didn't load");
    if(!newfile.open("tmp.raw",O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    watch->mergeImages(&newfile,&backfile,&frontfile,0,0,minute(tmpTime));
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    if(!backfile.open("tmp.raw",O_RDWR)) Serial.println("bluish didn't load");
    if(!frontfile.open("bluhour.raw",O_RDWR)) Serial.println("bluhour didn't load");
    if(!newfile.open(cacheFilenameBluish,O_RDWR | O_CREAT)) Serial.println("test didn't load");
    
    watch->mergeImages(&newfile,&backfile,&frontfile,0,0,(hour(tmpTime) % 12)  * 5 + minute(tmpTime) / 12);
    
    backfile.close();
    frontfile.close();
    newfile.close();
    
    sd.remove("tmp.raw");
    
    watch->setBackground(cacheFilenameBluish,0,0);
    

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
        case RELEASED: break;
    
    }

}

void button(int dir,int index) {

}

};

#endif
























