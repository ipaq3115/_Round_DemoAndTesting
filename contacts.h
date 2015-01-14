

#ifndef CONTACTS_
#define CONTACTS_

#include "roundUtil.h"

const int 
CONTACTS_ALL = 1,
CONTACTS_FAVORITES = 2,
CONTACTS_RECENT = 3;

int const BUFFER_SIZE = 20;

union Contact {
    
    byte b[28];
    
    struct {

        char name[16];
        char number[12];

    };
    
    Contact() {}
    Contact(char * initName,char * initNumber) {
        bool stringDone = false;
        for(int i=0;i<16;i++) {
            if(initName[i] == 0 || i >= 15) stringDone = true;
            name[i] = stringDone ? 0 : initName[i];
        }
        stringDone = false;
        for(int i=0;i<12;i++)  {
            if(initNumber[i] == 0 || i >= 11) stringDone = true;
            number[i] = stringDone ? 0 : initNumber[i];
        }
    }
    
};

class ContactsPage : public Page {

public:

ContactsPage CONSTRUCTOR_MACRO

int mode;

SdFile contactsFile;
SdFile ptrFile;

int totalContacts = 0;

Contact contactsBuff[BUFFER_SIZE];
uint16_t contactsBuffIndex[BUFFER_SIZE];

int listPos = 0;
int lastPrintPos = -1;
int lastTouchPos = 0;
int firstTouchIndex = -1;
bool touchSpinning = false;
int oldIndexStart = -1;
int lastDirection = -1;
int touchTime = 0;

int currentSelected = -1;

void leavingPage() {};

void initalize() { 

    initalize(CONTACTS_ALL,0); 

}

void initalize(int mode,char * data) {

    this->mode = mode;
    
    // Black screen
    watch->clrScr();
    
    // Close the file if it's already open
    if(contactsFile.isOpen()) contactsFile.close();
    
    // Init variables
    totalContacts = 0;
    listPos = 0;
    lastPrintPos = -1;
    lastTouchPos = 0;
    firstTouchIndex = -1;
    touchSpinning = false;
    oldIndexStart = -1;
    lastDirection = -1;
    currentSelected = -1;

    // Open the contacts file
    if(contactsFile.open(contactsFilename,O_RDWR)) {

        contactsFile.seekSet(0);
        
        totalContacts = (contactsFile.read() << 8) + contactsFile.read(); 

        byte tmpBuffer[30];
        for(int i=0;i<min(totalContacts,BUFFER_SIZE);i++) {
            
            contactsBuffIndex[i] = i;
        
            contactsFile.read(tmpBuffer,sizeof(Contact));
            for(int k=0;k<sizeof(Contact);k++) contactsBuff[i].b[k] = tmpBuffer[k];
            
            if(D) {
                for(int k=0;k<sizeof(Contact);k++) USB.write(tmpBuffer[k]);
                USB.println();
            }
        
        }

    } else if(D) USB.printf("Error: couldn't open %s\r\n",contactsFilename);

    // Do mode specific things

    // Print the list of contacts to the screen
    printList(0,0xFFFF);
    
}

void loop() {

    // Make these static so it doesn't have to be created and destroyed 
    // on each revolution of the loop
    // Maybe this would save curTime? Needs to be researched.
    static int indexStart;
    static int index;
    static int absIndex;
    static byte tmpBuffer[sizeof(Contact)];
    
    // Set the index that top of the list starts at
    if(listPos > 0) indexStart = 0;
    else indexStart = (-listPos) / 18;
    
    // Search through the buffer and find things that need new data
    for(int i=0;i<BUFFER_SIZE;i++) {
        
        // Find the absolute value of this index in the buffer and
        // subtract 5 so that the buffer starts 5 entries before what 
        // we actually need, allows for a scroll buffer
        absIndex = i + indexStart - 5; 
        if(absIndex < 0) absIndex = 0;
        
        // Get the index in the buffer from the absolute index
        index = absIndex % BUFFER_SIZE;
        
        if(absIndex != contactsBuffIndex[index]) {
            
            // Read data from the sdcard
            contactsFile.seekSet(absIndex * sizeof(Contact) + 2);
            contactsFile.read(tmpBuffer,sizeof(Contact));
            
            // Put the data into an contact object in the array
            // Could skip this and have the sd read function go straight to this array
            for(int i=0;i<sizeof(Contact);i++) contactsBuff[index].b[i] = tmpBuffer[i];
    
            if(D) {
                USB.printf("Updated buffer pos %03d which is absolute pos %03d\r\n",index,absIndex);
                for(int i=0;i<sizeof(Contact);i++) USB.write(tmpBuffer[i]);
                USB.println();
            }
            
            // Print the new data
            watch->setColor(0xFFFF);
            watch->print(contactsBuff[index].name,35,lastPrintPos + i * 18);
            
            // Update the absolute index that this Contact holds
            contactsBuffIndex[index] = absIndex;

            // Only do one per loop cycle, trying to let the loop service the touch 
            // as well while possibly updating a lot of these
            break;
        
        }
    
    }
    
    static elapsedMillis pressHoldTimer;
    
    if(firstTouchIndex != -1 && millis() - touchTime > 500) {
    
        if(pressHoldTimer > 250) {
    
            if(lastDirection != -1) incrementListPosition(lastDirection);
            
        }
    
    }

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    // if(D && touchType != MOVING) USB.printf("touch\r\n");
    
    static int lastUpdateTime = 0;
    
    switch(touchType) {
    
        case PRESSED:
        
            if(firstTouchIndex == -1) {
            
                if(finePos < 135 && finePos > 45) touchSpinning = true;
            
                firstTouchIndex = touchIndex;
                lastTouchPos = finePos;
                touchTime = millis();
                
                
                if(abs(finePos - 180) < 45) {

                    lastDirection = UP;
                
                } else if(finePos < 45 || finePos > 315) {

                    lastDirection = DOWN;
                
                } else {
                
                    lastDirection = -1;
                
                }
                
            }
            
            break;
        case MOVING:
        
            if(firstTouchIndex == touchIndex && touchSpinning) {
                
                if(finePos - lastTouchPos > 180) lastTouchPos += 360;
                if(lastTouchPos - finePos > 180) lastTouchPos -= 360;
                
                listPos += (finePos - lastTouchPos) * 3 / 4;
                lastTouchPos = finePos;

                if(listPos > 100) listPos = 100;
                if(listPos < totalContacts * 18 * -1 + 100) listPos = totalContacts * 18 * -1 + 100;
                
                printList(listPos,0xFFFF);
                
            }
            
            break;
        case RELEASED:
        
            if(firstTouchIndex == touchIndex) {
                
                int time = millis() - touchTime;
                if(time < 300 && time > 5) {
                
                    if(abs(finePos - 180) < 45) {

                        incrementListPosition(UP);
                    
                    } else if(finePos < 45 || finePos > 315) {

                        incrementListPosition(DOWN);
                    
                    }
                
                }
                
                firstTouchIndex = -1;
                touchSpinning = false;
                
            }
            
            break;
    
    }

}

void incrementListPosition(int dir) {

    if(dir == DOWN) listPos += 18;
    if(dir == UP)   listPos -= 18;

    if(listPos > 100) listPos = 100;
    if(listPos < totalContacts * 18 * -1 + 100) listPos = totalContacts * 18 * -1 + 100;
    
    printList(listPos,0xFFFF);

}

void button(int dir,int index) {

}

void printList(int yPos,int color) {

    if(D) USB.printf("printList %d %d\r\n",yPos,lastPrintPos);
    
    watch->setFont(BigFont);
    
    int indexStart;

    // The top of the screen is zero so this happens whenever the top 
    // of the list is below the top of the screen
    if(yPos > 0) {
        
        indexStart = 0;
    
    } else {
        
        indexStart = (-yPos) / 18;
        yPos += indexStart * 18;
        
    }
    
    if(D) USB.printf("indexStart %d\r\n",indexStart);

    if(yPos < lastPrintPos || indexStart < oldIndexStart) {

        // Print top to bottom
        for(int i=0;i<20;i++) printListItemNewOld(i,indexStart,color,yPos);
        
    } else if(yPos > lastPrintPos || indexStart > oldIndexStart) {
    
        // Print bottom to top
        for(int i=19;i>=0;i--) printListItemNewOld(i,indexStart,color,yPos);
        
    }

    lastPrintPos = yPos;
    oldIndexStart = indexStart;
    
}

void printListItemNewOld(int i,int indexStart,int color,int yPos) {

    int absIndex = i + indexStart;
    int index = absIndex % BUFFER_SIZE;
    if(absIndex < 0) return;
    
    // Unprint the old version of this
    printListItem(lastPrintPos,i,index,color,absIndex,indexStart,true);
    
    if(absIndex >= totalContacts) return;
    
    // Print the new version of this
    printListItem(yPos,i,index,color,absIndex,indexStart,false);
    
}

void printListItem(int yPos,int i,int index,int color,int absIndex,int indexStart,bool old) {

    int y = yPos + i * 18;
    if(y > 87) y += 10;
    if(y > 115) y += 10;

    int x = 35;
    // if(absIndex - indexStart == 5) x = 25;
    if(y > 87 && y <=115) x = 25;
    
    if(old) {
        
        watch->setColor(0);
        watch->fillRect(x-20,y,220,y + watch->getFontYsize());
    
    } else {
    
        watch->setColor(color);
        char str[2] {i + 'A',0};
        watch->print(str,x-20,y);
        watch->print(contactsBuff[index].name,x,y);
        
        currentSelected = absIndex;

    }
}



};

#endif
