

#ifndef PHONE_
#define PHONE_


#include "roundUtil.h"
#include "contacts.h"

class PhonePage : public Page {

    protected:
    
        int currentPhoneState;
        int pressIndex = 0;
        int pressIndexFine = 0;
        bool pressedState = false;
        
        SdFile callScreensImage;
        bool callScreen = false;

    public:

        PhonePage() {}
        
        PhonePage CONSTRUCTOR_MACRO

        void initalize();
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);


};

void PhonePage::initalize() {
    
    SdFile backgroundImageFile;

    if(!backgroundImageFile.isOpen()) {
        
        if(!backgroundImageFile.open("callBack.gci",O_RDWR)) USB.println("File open fail");

    }
    
    watch->printRaw(backgroundImageFile,0,0,4);
    
}

void PhonePage::leavingPage() {

}

void PhonePage::loop() {

    if(currentPhoneState != bluetooth->phoneState) {
    
        initalize();
    
    }

}

void PhonePage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    int iconIndex = finePos / 36;
    if(iconIndex == 0 || iconIndex == 9) {
        
        iconIndex = 0;
    
    } else {
    
        iconIndex--;
    
        iconIndex /= 2;
        
        iconIndex++;
    
    }
    
    if(D && touchType != MOVING) USB.printf("homePageTouch iconIndex %d\r\n",iconIndex);
    
    static bool iconState[5] {false,false,false,false,false};

    switch(touchType) {
    
        case PRESSED:
        
            iconState[iconIndex] = true;
        
            break;
        case MOVING:
            break;
        case RELEASED: 
        
            if(iconState[iconIndex]) {
            
                iconState[iconIndex] = false;
                
                switch(iconIndex) {
                
                    case 0: return;
                    case 1: goPage(PAGE::CONTACTS,CONTACTS_ALL); return;
                    case 2: goPage(PAGE::NUMBER_PAD); return;
                    case 3: goPage(PAGE::CONTACTS,CONTACTS_RECENT); return;
                    case 4: goPage(PAGE::CONTACTS,CONTACTS_FAVORITES); return;
                
                }
            
            }
            
            break;
    
    }

}

void PhonePage::button(int dir,int index) {

}

#endif
