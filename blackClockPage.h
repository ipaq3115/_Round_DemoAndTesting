
#ifndef BLACK_CLOCK_H_
#define BLACK_CLOCK_H_

class BlackClockPage : public Page {
public:

BlackClockPage CONSTRUCTOR_MACRO

image_info smallDigitFile,largeDigitFile;

int lastPrintedHour = -1;
int lastPrintedMinute = -1;
int lastPrintedSecond = -1;
int batVoltOld = -1;
    
void bootup() {

}

void initalize() {

    watch->loadImage("sbSec.raw",&smallDigitFile);
    watch->loadImage("sbLg.raw", &largeDigitFile);

    lastPrintedHour = -1;
    lastPrintedMinute = -1;
    lastPrintedSecond = -1;
    batVoltOld = -1;
    
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
    printVoltage();

}

void printHour(time_t tmpTime) {

    if(lastPrintedHour == hourFormat12(tmpTime)) return;
    
    lastPrintedHour = hourFormat12(tmpTime);

    largeDigitFile.y = 78;
    
    largeDigitFile.x = 2;
    watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),1));
    // watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),1) == 0 ? 10 : decDigit(hourFormat12(tmpTime),1));
    
    largeDigitFile.x = 50;
    watch->printImage(&largeDigitFile,decDigit(hourFormat12(tmpTime),0));
    
}

void printMinute(time_t tmpTime) {

    if(lastPrintedMinute == minute(tmpTime)) return;
    
    lastPrintedMinute = minute(tmpTime);

    largeDigitFile.y = 78;
    
    largeDigitFile.x = 123;
    watch->printImage(&largeDigitFile,decDigit(minute(tmpTime),1));
    
    largeDigitFile.x = 172;
    watch->printImage(&largeDigitFile,decDigit(minute(tmpTime),0));

}

void printSecond(time_t tmpTime) {

    if(lastPrintedSecond == second(tmpTime)) return;
    
    lastPrintedSecond = second(tmpTime);

    smallDigitFile.y = 30;
    
    smallDigitFile.x = 90;
    watch->printImage(&smallDigitFile,decDigit(second(tmpTime),1));
    
    smallDigitFile.x = 111;
    watch->printImage(&smallDigitFile,decDigit(second(tmpTime),0));

}

void printVoltage() {
    
    int batVolt = bluetooth->getBatteryVoltage();

    if(batVoltOld == batVolt) return;
    
    batVoltOld = batVolt;

    smallDigitFile.y = 160;
    
    smallDigitFile.x = 70;
    watch->printImage(&smallDigitFile,decDigit(batVolt,3));
    
    smallDigitFile.x = 90;
    watch->printImage(&smallDigitFile,decDigit(batVolt,2));

    smallDigitFile.x = 110;
    watch->printImage(&smallDigitFile,decDigit(batVolt,1));

    smallDigitFile.x = 130;
    watch->printImage(&smallDigitFile,decDigit(batVolt,0));

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    // if(touchType == RELEASED) {
    //     
    //     goPage(PAGE::HOME);
    //     
    // }
    
}

void button(int dir,int index) {

}

};


#endif