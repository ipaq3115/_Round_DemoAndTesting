






#if 0

namespace FREQUENCY_ENTRY_PAGE_VARS {

    int pressIndex = 0;
    bool pressedState = false;
    char frequency[10];
    int frequencyLength = 0;

}

void frequencyEntryInitialize() {

    using namespace FREQUENCY_ENTRY_PAGE_VARS;
    
    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    
    if(!backgroundImageFile.open("numpad.bmp",O_RDWR)) USB.println("File open fail");
    
    lcd.printBitmap(backgroundImageFile,0,0);
    
    printFrequency(frequency);
    
}

void frequencyEntryTouch(int touchType,int finePos,int activeTouches,int touchIndex) {

    using namespace FREQUENCY_ENTRY_PAGE_VARS;

    switch(touchType) {
    
        case PRESSED: 
            
            pressedState = true;
            pressIndex = activeTouches;
        
            break;
        case MOVING: 
            break;
        case RELEASED: 
            
            if(pressedState) {
            
                pressedState = false;
                
                if(D) USB.printf("activeTouches %d pressIndex %d\r\n",activeTouches,pressIndex);
            
                // Add digit
                if(activeTouches == pressIndex) {
                
                    if(D) USB.printf("Button #%d pressed\r\n",activeTouches);
                
                    if(frequencyLength < 8) {
                    
                        frequency[frequencyLength] = activeTouches + '0';
                        frequency[frequencyLength + 1] = 0;
                        
                        frequencyLength++;
                        
                        printFrequency(frequency);
                        
                    }
                    
                    digitalWrite(PIN::VIBRATOR, HIGH);
                    delay(100);
                    digitalWrite(PIN::VIBRATOR, LOW);
                
                // Delete
                } else if((activeTouches == 5 || activeTouches == 6) && (pressIndex == 3 || pressIndex == 4)) {
                
                    if(D) USB.println("Erase");
                
                    if(frequencyLength > 0) {
                        
                        frequency[--frequencyLength] = 0;
                        
                        printFrequency(frequency);
                    
                    }
                
                // Erase all
                } else if(activeTouches == 5 && pressIndex == 1) {
                
                    frequencyLength = 0;
                    frequency[frequencyLength] = 0;
                    
                    printFrequency(frequency);
                    
                // Decimal point
                } else if((activeTouches == 5 || activeTouches == 6) && (pressIndex == 7 || pressIndex == 8)) {
                
                    if(frequencyLength < 4) {
                
                        int distance = 4 - frequencyLength;
                        
                        for(int i=0;i<4;i++) {
                        
                            int index = frequencyLength - 1 - i;
                        
                            if(index >= 0) {
                            
                                frequency[3 - i] = frequency[index];
                            
                            } else {
                            
                                frequency[3 - i] = '0';
                            
                            }
                        
                        }
                        
                        frequencyLength = 4;
                        frequency[frequencyLength] = 0;
                        
                        printFrequency(frequency);
                        
                    }
            
                }
            
            }
        
            break;
    
    }

}

void printFrequency(char * frequency) {

    using namespace FREQUENCY_ENTRY_PAGE_VARS;

    static int lastPrintLength = 0;

    char tmpFreq[15];

    int writePtr = 0;
    for(int i=0;i<8;i++) {

        if(frequency[i]==0) break;

        if(i==4) tmpFreq[writePtr++] = '.';

        tmpFreq[writePtr++] = frequency[i];

    }

    tmpFreq[writePtr] = 0;

    if(D) USB.printf("printFrequency %s\r\n",tmpFreq);
    
    lcd.setColor(0,0,0);
    lcd.fillRect(110 - 16 * lastPrintLength / 2,110 - 8,110 + 16 * lastPrintLength / 2,110 + 8);

    lcd.setColor(0xFF,0xFF,0xFF);
    lcd.print(tmpFreq,110 - 16 * writePtr / 2,110 - 8);
    
    lastPrintLength = writePtr;

}

void frequencyEntryButtonEvent(int dir,int index) {

    using namespace FREQUENCY_ENTRY_PAGE_VARS;

    using namespace BUTTON;
    
    static bool screenState = 0;

    switch(index) {
    
        case POWER_BUTTON:
        
            if(dir) {
            
                screenState = !screenState;
                
                RUtils::rampBrightness(screenState);
            
            }
        
            break;
    
    }

}

#endif