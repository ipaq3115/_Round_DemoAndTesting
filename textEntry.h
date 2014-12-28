
#ifndef TEXT_ENTRY_PAGE_
#define TEXT_ENTRY_PAGE_

class TextEntryPage : public Page {

public:

int static const STR_MAX_LENGTH = 15;
int strLen = 0;
char str[STR_MAX_LENGTH + 1];

// This will hold the index of the first touch on the ring
// which is the touch that selects the letter
int cursorIndex = -1;

// This will hold the index of the second touch, the 'select' touch
int selectIndex = -1;

SdFile font;

TextEntryPage() {}

TextEntryPage CONSTRUCTOR_MACRO

void initalize() {

    Serial.println("text entry init");

    watch->clrScr();
    
    drawLetters(-1,VGA_WHITE);

    cursorIndex = -1;
    selectIndex = -1;
    
    str[0] = 0;
    
    strLen = 0;
    
    if(!font.open("dsm22x41.gci",O_READ)) if(E) db.println("Couldn't load dsm22x41.gci");
    
}

void redraw() {

    watch->clrScr();
    
    drawLetters(-1,VGA_WHITE);

    printStr(VGA_WHITE);
    
}

void drawLetters(int index,int color) {
    
    int result_x,result_y;
    
    watch->setColor(color);

    for(int i=0;i<26;i++) {
    
        if(index == i && index != -1) {
        
            watch->setFont(BigFont);
        
            MyUtils::orbitPoint(110,110,13.85 * (float)i * -1,80,result_x,result_y);
            
            watch->printChar('A' + i, result_x - watch->cfont.x_size / 2, result_y - watch->cfont.y_size / 2);
            
        
        } else {
        
            watch->setFont(SmallFont);
        
            MyUtils::orbitPoint(110,110,13.85 * (float)i * -1,80,result_x,result_y);
            
            watch->printChar('A' + i, result_x - watch->cfont.x_size / 2, result_y - watch->cfont.y_size / 2);
            
        }
    
    }

}

void leavingPage() {

    font.close();

}

void loop() {
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    static int posOld = -1;
    static int cursorStartPos = -1;
    static long cursorStartTime = -1;
    static int diff = 0,lastFinePos = 0;
    
    // pos is the letter we are on starting at 'A'
    int pos = finePos + 7;
    if(pos >= 359) pos -= 359;
    pos /= 14;
    
    switch(touchType) {
    
        case PRESSED:
        
            if(cursorIndex == -1) {
            
                forceCursorTouch:
            
                cursorIndex = touchIndex;
                cursorStartPos = finePos;
                cursorStartTime = millis();
                diff = 0;
                lastFinePos = finePos;
        
            } else if(selectIndex == -1) {
            
                selectIndex = touchIndex;
            
            }
            
            break;
        case MOVING:
            
            if(cursorIndex == touchIndex) {
            
                // Tracking the touch
                int tmpdiff = finePos - lastFinePos;
                if(tmpdiff > 180) tmpdiff -= 360;
                if(tmpdiff < -180) tmpdiff += 360;
                diff += tmpdiff;
                // while(diff < 0) diff += 360;
                // while(diff > 359) diff -= 360;
                lastFinePos = finePos;
                
                if(abs(diff) > 360) {
                
                    printStr(VGA_BLACK);
                    strLen = 0;
                    str[0] = 0;
                    printStr(VGA_WHITE);
                    
                    goto forceCursorTouch;
                
                }
            
                // Detecting a change in letter that we are on
                if(pos != posOld) {
                
                    watch->printRaw(font,99, 55,'A' + pos - '!');
                    watch->printRaw(font,99,124,'A' + pos - '!');
                    
                    // watch->clrScr();
                    drawLetters(posOld,VGA_BLACK);
                    drawLetters(pos,VGA_WHITE);
                    
                    posOld = pos;
    
                }
    
            }
            
            break;
        case RELEASED: 
            
            if(cursorIndex == touchIndex) {

                watch->setColor(VGA_BLACK);
                watch->fillRect(99,55,99+22,55+41);
                watch->fillRect(99,124,99+22,124+41);
            
                drawLetters(posOld,VGA_BLACK);
                drawLetters(-1,VGA_WHITE);
                cursorIndex = -1;
                
                if(D) db.printf("diff %d\r\n",diff);
                
                if(millis() - cursorStartTime < 200) {
                
                    if(diff > 36) {
                    
                        cursorStartPos = -1;
                        
                        delChar();
                    
                    } else if(diff < 36) {
                    
                        addChar(' ');
                    
                    }
                
                }
            
            }
            
            if(selectIndex == touchIndex) {
            
                selectIndex = -1;
                
                if(cursorIndex != -1) addChar('A' + posOld);
                
            }
            
        
            break;
    
    }


}

void addChar(char tmpChar) {

    if(D) db.printf("addChar %d %c strLen %d STR_MAX_LENGTH %d\r\n",tmpChar,tmpChar,strLen,STR_MAX_LENGTH);
    
    if(strLen < STR_MAX_LENGTH) {
    
        str[strLen++] = tmpChar;
        str[strLen] = 0;
    
        printStr(VGA_WHITE);
        
    }
    
}

void delChar() {

    if(strLen > 0) {

        printStr(VGA_BLACK);
        
        str[--strLen] = 0;
        
        printStr(VGA_WHITE);
        
    }

}

void printStr(int color) {

    watch->setColor(color);
    watch->setFont(SmallFont);
    watch->print(str,(220 - strLen * watch->cfont.x_size) / 2,(220 - watch->cfont.y_size) / 2);

}

void button(int dir,int index) {

}

};

#endif
