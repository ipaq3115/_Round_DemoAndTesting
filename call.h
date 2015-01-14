

// #if 1

#include "roundUtil.h"

class CallPage : public Page {

    protected:
    
        image_info backgroundImageFile;
        
        int currentPhoneState;
        int pressIndex = 0;
        int pressIndexFine = 0;
        bool pressedState = false;
        
        image_info callScreensImage;
        bool callScreen = false;

    public:

        CallPage() {}
        
        CallPage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        void printNumber(char * string);
        bool touchCheckDegrees(int finePosPress,int finePosRelease,int limitA,int limitB);
        void printFormattedPhoneNumber(int height);


};

void CallPage::initalize() {

    currentPhoneState = bluetooth->phoneState;
    
    switch(bluetooth->phoneState) {
    
        case PHONE_IDLE:     
        
            watch->printImage("callBack.gci",0,0,4); 
            
            break;
        case PHONE_RINGING:  
        
            watch->printImage("callBack.gci",0,0,1);
            printFormattedPhoneNumber(63);
            
            break;
        case PHONE_IN_CALL:  
        
            watch->printImage("callBack.gci",0,0,3);
            printFormattedPhoneNumber(63);
            
            break;
        case PHONE_DIALING:  
        
            watch->printImage("callBack.gci",0,0,2); 
            printFormattedPhoneNumber(63);
            
            break;
    
    }
    
}

void CallPage::printFormattedPhoneNumber(int height) {

    if(D) {
    
        USB.print("About to try to print this number: ");
        for(int i=0;i<15;i++) USB.write(bluetooth->phoneNumber[i]);
        USB.println();
    
    }

    char numberPrintStr[20];
    
    int writePtr = 0;
    for(int i=0;i<10;i++) {
        
        if(i == 0) numberPrintStr[writePtr++] = '(';
        if(i == 6) numberPrintStr[writePtr++] = '-';
    
        numberPrintStr[writePtr++] = bluetooth->phoneNumber[i];
        
        if(i == 2) numberPrintStr[writePtr++] = ')';
    
    }
    
    numberPrintStr[writePtr++] = 0;
    
    if(D) {
    
        USB.print("Result: ");
        for(int i=0;i<15;i++) USB.write(numberPrintStr[i]);
        USB.println();
    
    }
    
    image_info fontFile;
    
    watch->loadImage("font22.Gci",&fontFile);
    // if(!fontFile.open("font22.Gci",O_RDWR)) Serial.println("Error: no font22.Gci");
    
    switch(bluetooth->phoneState) {
    
        case PHONE_IDLE:     watch->print(numberPrintStr,&fontFile,CENTER,height,&backgroundImageFile,0,0,4,0x001F); break;
        case PHONE_RINGING:  watch->print(numberPrintStr,&fontFile,CENTER,height,&backgroundImageFile,0,0,1,0x001F); break;
        case PHONE_IN_CALL:  watch->print(numberPrintStr,&fontFile,CENTER,height,&backgroundImageFile,0,0,3,0x001F); break;
        case PHONE_DIALING:  watch->print(numberPrintStr,&fontFile,CENTER,height,&backgroundImageFile,0,0,2,0x001F); break;
    
    }
    
    fontFile.file.close();
    
}

void CallPage::leavingPage() {

}

void CallPage::loop() {

    if(currentPhoneState != bluetooth->phoneState) {
    
        if(bluetooth->phoneState == PHONE_IDLE) {
        
            goBack();
        
        } else {
            
            initalize();
        
        }
    
    }

}

bool CallPage::touchCheckDegrees(int finePosPress,int finePosRelease,int limitA,int limitB) {

    return(
    finePosPress <= max(limitA,limitB) && 
    finePosPress > min(limitA,limitB) && 
    finePosRelease <= max(limitA,limitB) && 
    finePosRelease > min(limitA,limitB));

}

void CallPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    int padIndex = finePos + 18;
    if(padIndex >= 360) padIndex -= 360;
    padIndex /= 36;

    switch(touchType) {
    
        case PRESSED: 
            
            pressedState = true;
            pressIndex = padIndex;
            pressIndexFine = finePos;
        
            break;
        case MOVING: 
            break;
        case RELEASED: 
            
            if(pressedState) {
            
                pressedState = false;
                
                if(D) USB.printf("padIndex %d pressIndex %d pressIndexFine %d\r\n",padIndex,pressIndex,pressIndexFine);
                
                switch(currentPhoneState) {
                
                    case PHONE_IDLE:
                    
                        // Call button
                        if(touchCheckDegrees(pressIndexFine,finePos,90,180)) {
                        
                        // Back button
                        } else if(touchCheckDegrees(pressIndexFine,finePos,180,270)) {
                        
                        }
                        
                        break;
                        
                    case PHONE_RINGING:
                    
                        // Reject
                        if(touchCheckDegrees(pressIndexFine,finePos,90,180)) {
                        
                            bluetooth->endCall();
                        
                        // Answer
                        } else if(touchCheckDegrees(pressIndexFine,finePos,180,270)) {
                        
                            bluetooth->answerCall();
                        
                        }
                    
                        break;
                        
                    case PHONE_IN_CALL:
                    
                        if(touchCheckDegrees(pressIndexFine,finePos,90,270)) {
                        
                            bluetooth->endCall();
                        
                        }
                    
                        break;
                        
                    case PHONE_DIALING:
                        break;
                
                
                }
                
                
            }
        
            break;
    
    }

}

void CallPage::button(int dir,int index) {

}


void CallPage::printNumber(char * string) {

    // using namespace FREQUENCY_ENTRY_PAGE_VARS;

    static int lastPrintLength = 0;

    if(D) USB.printf("printNumber %s\r\n",string);
    
    int printLength = MyUtils::strLength(string);
    
    watch->setColor(0,0,0);
    watch->fillRect(110 - 16 * lastPrintLength / 2,110 - 8,110 + 16 * lastPrintLength / 2,110 + 8);

    watch->setColor(0xFF,0xFF,0xFF);
    watch->print(string,110 - 16 * printLength / 2,110 - 8);
    
    lastPrintLength = printLength;

}



