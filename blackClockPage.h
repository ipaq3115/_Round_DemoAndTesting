
#ifndef BLACK_CLOCK_H_
#define BLACK_CLOCK_H_

class BlackClockPage : public Page {
public:

BlackClockPage CONSTRUCTOR_MACRO

image_info smallDigitFile,largeDigitFile;

int lastPrintedHour = -1;
int lastPrintedMinute = -1;
int lastPrintedSecond = -1;
    
void bootup() {

}

void initalize() {

    watch->loadImage("sbSec.raw",&smallDigitFile);
    watch->loadImage("sbLg.raw", &largeDigitFile);

    lastPrintedHour = -1;
    lastPrintedMinute = -1;
    lastPrintedSecond = -1;
    
    watch->printImage("sbBack.raw",0,0);
    
    printTime(now());
    
}

void leavingPage() {

    smallDigitFile.file.close();
    largeDigitFile.file.close();

}

void loop() {

    static time_t lastTime = 0;
    time_t t = now();
    
    if(lastTime != t) {

        printTime(t);
    
        lastTime = t;
    
    }
    

}

void printTime(time_t tmpTime) {

    printHour(tmpTime);
    printMinute(tmpTime);
    printSecond(tmpTime);

}

void printHour(time_t tmpTime) {

    if(lastPrintedHour == hourFormat12(tmpTime)) return;

    largeDigitFile.y = 78;
    
    largeDigitFile.x = 2;
    watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),1));
    // watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),1) == 0 ? 10 : decDigit(hourFormat12(tmpTime),1));
    
    largeDigitFile.x = 50;
    watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),0));
    
}

void printMinute(time_t tmpTime) {

    if(lastPrintedMinute == minute(tmpTime)) return;

    largeDigitFile.y = 78;
    
    largeDigitFile.x = 123;
    watch->printImage(&largeDigitFile,decDigit(minute(tmpTime),1));
    
    largeDigitFile.x = 172;
    watch->printImage(&largeDigitFile,decDigit(minute(tmpTime),0));

}

void printSecond(time_t tmpTime) {

    if(lastPrintedSecond == second(tmpTime)) return;

    smallDigitFile.y = 40;
    
    smallDigitFile.x = 90;
    watch->printImage(&smallDigitFile,decDigit(second(tmpTime),1));
    
    smallDigitFile.x = 111;
    watch->printImage(&smallDigitFile,decDigit(second(tmpTime),0));

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

}

void button(int dir,int index) {

}

};


#endif