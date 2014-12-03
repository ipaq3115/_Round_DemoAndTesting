

#ifndef BLUISH_CLOCK_PAGE_
#define BLUISH_CLOCK_PAGE_

class BluishClockPage : public Page {
    
public:

BluishClockPage CONSTRUCTOR_MACRO

SdFile backFile,secondsFile;

   
void initalize() {

    if(D) USB.println("initalize");

    if(backFile.isOpen()) backFile.close();
    if(secondsFile.isOpen()) secondsFile.close();

    if(!backFile.open("bluish.bmp",O_RDWR)) USB.println("File open fail");
    if(!secondsFile.open("blusec.gci",O_RDWR)) USB.println("File open fail");
    
    lcd->printBitmap(backFile,0,0);
    
    lcd->printGci(secondsFile,0,0,0);
    
}

void leavingPage() {

    if(backFile.isOpen()) backFile.close();
    if(secondsFile.isOpen()) secondsFile.close();

}

void loop() {

    static elapsedMillis time;
    static int count = 0;
    
    if(time > 1000) {
    
        time = 0;
        
        lcd->printBitmap(backFile,0,0);
        
        lcd->printGci(secondsFile,0,0,count++ % 60);
        
    
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
























