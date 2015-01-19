
#include "roundUtil.h"

class NumberPadPage : public Page {

protected:

int pressIndex = 0;
int pressIndexFine = 0;
bool pressedState = false;
char numberStr[15] {0};
int numberLength = 0;

image_info callScreensImage;
bool callScreen = false;

public:

NumberPadPage CONSTRUCTOR_MACRO

void initalize() {
    
    watch->printImage("numpad.bmp",0,0);
    
    watch->loadImage("callBack.raw",&callScreensImage);
    
    printNumber(numberStr);
    
}

void leavingPage() {

    callScreensImage.file.close();

}

void loop() {

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    int padIndex = finePos + 18;
    if(padIndex >= 360) padIndex -= 360;
    padIndex /= 36;

    switch(touchType) {
    
        case PRESSED: 
            
            pressedState = true;
            pressIndexFine = finePos;

            pressIndex = padIndex;
            
            break;
        case MOVING: 
            break;
        case RELEASED: 
            
            if(pressedState) {
            
                pressedState = false;
                
                // Vibrate for 100 milliseconds (this is a non-blocking call)
                watch->vibrate(100);
                
                if(D) USB.printf("padIndex %d pressIndex %d pressIndexFine %d\r\n",padIndex,pressIndex,pressIndexFine);
                
                if(callScreen) {
                
                    // Call button
                    if(pressIndexFine <= 180 && pressIndexFine >= 90 && finePos <= 180 && finePos >= 90) sendCall();
                    
                    // Back button
                    else if(pressIndexFine <= 270 && pressIndexFine > 180 && finePos <= 270 && finePos > 180) backButton();
                    
                }
                
                // Add digit
                else if(padIndex == pressIndex) newDigit(padIndex);
                
                // Delete
                else if((padIndex == 5 || padIndex == 6) && (pressIndex == 3 || pressIndex == 4)) backspace();
                
                // Erase all
                else if(padIndex == 5 && pressIndex == 1) clearNumbers();
            
            }
        
            break;
    
    }

}

void sendCall() {

    bluetooth->callNumber(numberStr);

    numberStr[0] = 0;
    numberLength = 0;

    callScreen = false;

    initalize();

}

void backButton() {

    callScreen = false;

    initalize();

}

void backspace() {

    if(D) USB.println("Erase");

    if(numberLength > 0) {
        
        numberStr[--numberLength] = 0;
        
        printNumber(numberStr);
    
    }

}

void clearNumbers() {

    numberLength = 0;
    numberStr[numberLength] = 0;

    printNumber(numberStr);

}

void newDigit(char value) {

    if(D) USB.printf("newDigit %d\r\n",value);

    if(numberLength < 10) {
    
        if(value < 0 || value > 9) {
        
            if(E) db.printf("newDigit value is invalid %d\r\n",value);
            return;
        
        }
    
        numberStr[numberLength] = value + '0';
        numberStr[numberLength + 1] = 0;
        
        numberLength++;
        
        printNumber(numberStr);
        
        if(numberLength == 10) {
        
            callScreen = true;
        
            // Print the new background image
        
            watch->printImage(&callScreensImage,0,0,0);
            
            // Print the phone number
            
            char numberPrintStr[20];

            int writePtr = 0;
            for(int i=0;i<10;i++) {
                
                if(i == 0) numberPrintStr[writePtr++] = '(';
                if(i == 6) numberPrintStr[writePtr++] = '-';
            
                numberPrintStr[writePtr++] = numberStr[i];
                
                if(i == 2) numberPrintStr[writePtr++] = ')';
            
            }
            
            numberPrintStr[writePtr++] = 0;
            
            image_info fontFile;
            
            watch->loadImage("dsm22b.raw",&fontFile);
            
            // if(!fontFile.open("dsm22b.raw",O_RDWR)) Serial.println("Error: no dsm22b.raw");
            // if(!fontFile.open("font22.raw",O_RDWR)) Serial.println("Error: no font22.raw");
            
            watch->print(numberPrintStr,&fontFile,CENTER,80,&callScreensImage,0,0,0);
            // watch->print(numberPrintStr,fontFile,CENTER,80,callScreensImage,0,0,0,0x001F);
            
            fontFile.file.close();
            
        }
        
    }

}

void button(int dir,int index) {

}

void printNumber(char * string) {

    static int lastPrintLength = 0;

    if(D) USB.printf("printNumber %s\r\n",string);
    
    int printLength = MyUtils::strLength(string);
    
    watch->setFont(BigFont);
    
    watch->setColor(0,0,0);
    watch->fillRect(110 - 16 * lastPrintLength / 2,110 - 8,110 + 16 * lastPrintLength / 2,110 + 8);

    watch->setColor(0xFF,0xFF,0xFF);
    watch->print(string,110 - 16 * printLength / 2,110 - 8);
    
    lastPrintLength = printLength;

}

};
