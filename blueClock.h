

#ifndef BlUE_CLOCK_PAGE_
#define BlUE_CLOCK_PAGE_

#include "roundUtil.h"

class BlueClockPage : public Page {

    protected:
    
        int touchIndex = -1;
        
        int touches[10];
        bool repeatTouch[10];
        
        SdFile numbFile;
        SdFile ampmFile;
        SdFile weekdayFile;
        SdFile monthFile;
        SdFile dayAFile;
        SdFile dayBFile;
        SdFile yearFile;
        
        SdFile backgroundImageFile;
    
        enum time_array {
            
            TIME_HOUR,
            TIME_MINUTE_A,
            TIME_MINUTE_B,
            TIME_AMPM,
            TIME_MONTH,
            TIME_DAY,
            TIME_YEAR,
            
            TIME_TOTAL
        
        };
    
        int timeArray[TIME_TOTAL];
    
        // Set Page vars
        int currentEdit = 0;
        int currentEditOld = 0;
        bool blinkState = false;
        elapsedMillis blinkTime;
    
        enum pgmode_t {
            
            MODE_MAIN,
            MODE_SET,
            MODE_STOPWATCH,
            MODE_TIMER
        
        } pageMode = MODE_MAIN;
        
        enum btn_t {
        
            MAIN_SET,
            MAIN_TIMER,
            MAIN_STOPWATCH,
            MAIN_HOME,
            
            SET_UP,
            SET_DONE,
            SET_RIGHT,
            SET_DOWN,
            SET_LEFT,
            SET_CANCEL,
            
        };
        
        const char buttonNames[10][20] {
        
            "MAIN_SET",
            "MAIN_STOPWATCH",
            "MAIN_TIMER",
            "MAIN_HOME",
            
            "SET_UP",
            "SET_DONE",
            "SET_RIGHT",
            "SET_DOWN",
            "SET_LEFT",
            "SET_CANCEL",
        
        };
        
    public:

        BlueClockPage CONSTRUCTOR_MACRO
        
        void initalize();
        void initalize(int,int);
        void leavingPage();
        void loop();
        void touch(int touchType,int finePos,int activeTouches,int touchIndex);
        void button(int dir,int index);
        
        void mainLoop();
        void setLoop();
        
        void printSetItem(int index,bool state);
        
        void buttonPress(int btn,bool longPress,bool repeatPress);
        
        void printTime(int hours,int minutes);
        void printHour(int hours);
        void printDate(int day,int month,int year);
        void printDay(int day);
        void printMonth(int month);
        void printWeekday(int day,int month,int year);
        void printYear(int year);
        
        int  buttonID(int finePos);
        void init();
        void currentEditIncrement(int dir);
        void currentEditIncrement(int dir,bool force);
        void editIncrement(bool dir);
        void printBack() {
        
            // using namespace pgmode_t;
        
            switch(pageMode) {
            
                case MODE_SET: lcd->printGci(backgroundImageFile,0,0,1); break;
                case MODE_STOPWATCH:
                case MODE_TIMER:
                case MODE_MAIN: lcd->printGci(backgroundImageFile,0,0,0); break;
            
            }
            
        }
        
        
};

void BlueClockPage::initalize() {

    if(D) USB.println("BlueClockPage::initalize");

    if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    if(numbFile.isOpen()) numbFile.close();
    if(ampmFile.isOpen()) ampmFile.close();
    if(weekdayFile.isOpen()) weekdayFile.close();
    if(monthFile.isOpen()) monthFile.close();
    if(dayAFile.isOpen()) dayAFile.close();
    if(dayBFile.isOpen()) dayBFile.close();
    if(yearFile.isOpen()) yearFile.close();

    if(!backgroundImageFile.open("blue.Gci",O_RDWR)) USB.println("File open fail");
    if(!numbFile.open("blueNum.gci",O_RDWR)) USB.println("File open fail");
    if(!ampmFile.open("blueAmPm.gci",O_RDWR)) USB.println("File open fail");
    if(!weekdayFile.open("blueDays.gci",O_RDWR)) USB.println("File open fail");
    if(!monthFile.open("blueMon.gci",O_RDWR)) USB.println("File open fail");
    if(!dayAFile.open("blueDayA.gci",O_RDWR)) USB.println("File open fail");
    if(!dayBFile.open("blueDayB.gci",O_RDWR)) USB.println("File open fail");
    if(!yearFile.open("blueyear.gci",O_RDWR)) USB.println("File open fail");
    
    pageMode = MODE_MAIN;
    // pageMode = SET;
    
    init();

}

void BlueClockPage::init() {
    
    fori(10) touches[i] = -1;
    
    printBack();
    
    timeArray[TIME_HOUR]        = -1;
    timeArray[TIME_MINUTE_A]    = -1;
    timeArray[TIME_MINUTE_B]    = -1;
    timeArray[TIME_AMPM]        = -1;
    timeArray[TIME_MONTH]       = -1;
    timeArray[TIME_DAY]         = -1;
    timeArray[TIME_YEAR]        = -1;

    printTime(curTime.hour,curTime.minute);
    printDate(curTime.day,curTime.month,curTime.year);
    
    if(pageMode == MODE_SET) {
        
        currentEdit = 0;
        currentEditOld = 0;
    
    }
    
    // if(D) USB.printf("curTime %d\r\n",curTime.hour,curTime.minute,curTime.second,curTime.hundredth);
    
}

void BlueClockPage::leavingPage() {

}

void BlueClockPage::loop() {

    switch(pageMode) {
        case MODE_MAIN: mainLoop(); break;
        case MODE_SET:  setLoop();  break;
    }

}

void BlueClockPage::mainLoop() {

    static elapsedMillis updateTime;
    if(updateTime > 100) {
        
        updateTime = 0;

        // if(D) USB.printf("curTime %03d %03d %03d %03d\r\n",curTime.hour,curTime.minute,curTime.second,curTime.hundredth);
        
        // printTime(curTime.second,curTime.hundredth);
        // printTime(curTime.minute,curTime.second);
        printTime(curTime.hour,curTime.minute);
        
        printDate(curTime.day,curTime.month,curTime.year);
        
    }

}

void BlueClockPage::setLoop() {

    if(blinkTime > 500) {
        
        blinkTime = 0;
        
        blinkState = !blinkState;
        
        printSetItem(currentEdit,blinkState);
        
    }

}

void BlueClockPage::printSetItem(int index,bool state) {

    switch(index) {
    
        case TIME_HOUR:     printHour(state ? timeArray[TIME_HOUR] : -1); break;
        case TIME_MINUTE_A: lcd->printGci(numbFile,128,77,11 * 2 + (!state ? 10 : timeArray[TIME_MINUTE_A])); break;
        case TIME_MINUTE_B: lcd->printGci(numbFile,160,77,11 * 3 + (!state ? 10 : timeArray[TIME_MINUTE_B])); break;
        case TIME_AMPM:     
            if(state)   lcd->printGci(ampmFile,190,104,timeArray[TIME_AMPM]); 
            else        lcd->printGci(backgroundImageFile,0,0,1,190,104,211,117,false);
            break;
        case TIME_MONTH:
            if(state)   printMonth(timeArray[TIME_MONTH]);
            else        lcd->printGci(backgroundImageFile,0,0,1,45,157,141,175,false);
            break;
        case TIME_DAY:
            if(state)   printDay(timeArray[TIME_DAY]);
            else        lcd->printGci(backgroundImageFile,0,0,1,144,158,165,171,false);
            break;
        case TIME_YEAR:
            if(state)   printYear(timeArray[TIME_YEAR]);
            else        lcd->printGci(backgroundImageFile,0,0,1,89,179,132,192);
            break;
    }

}

void BlueClockPage::touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    static int touchPressedTime = 0;
    
    if(touchType != MOVING) if(D) USB.printf("BlueClockPage::touch() finePos %d\r\n",finePos);

    switch(touchType) {
    
        case PRESSED: 
        
            if(touches[touchIndex] == -1) {
                
                touches[touchIndex] = buttonID(finePos);
                
                repeatTouch[touchIndex] = false;
                
                touchPressedTime = millis();
                
            }
        
            break;
        case MOVING:
            
            if(!repeatTouch[touchIndex] && 
            millis() - touchPressedTime > 750 && 
            touches[touchIndex] == buttonID(finePos)) {
            
                touchPressedTime = millis();
                
                repeatTouch[touchIndex] = true;
            
                buttonPress(buttonID(finePos),true,false);
            
            }
            
            if(repeatTouch[touchIndex] && 
            millis() - touchPressedTime > 200 && 
            touches[touchIndex] == buttonID(finePos)) {
            
                touchPressedTime = millis();
            
                buttonPress(buttonID(finePos),false,true);
            
            }
        
            break;
        case RELEASED: 
            
            if(touches[touchIndex] == buttonID(finePos)) {
            
                touches[touchIndex] = -1;
                
                buttonPress(buttonID(finePos),false,false);
            
            }
            
            break;
    
    }

}

void BlueClockPage::buttonPress(int btn,bool longPress,bool repeatPress) {

    if(btn == -1) {
        if(D) USB.println("Invalid button");
        return;
    }
    
    if(D) USB.printf("buttonPress %s longPress %d repeatPress %d\r\n",buttonNames[btn],longPress,repeatPress);

    switch(btn) {
    
        case MAIN_SET: 
            
            if(longPress) {
                
                pageMode = MODE_SET; 
                init(); 
            
            }
            
            break;

        case MAIN_STOPWATCH: break;
        case MAIN_TIMER: break;
        case MAIN_HOME: 
            goPage(PAGE::HOME);
            break;
    
        case SET_UP:    editIncrement(UP);       break;
        case SET_DOWN:  editIncrement(DOWN);     break;
        case SET_RIGHT: currentEditIncrement(UP); break;
        case SET_LEFT:  currentEditIncrement(DOWN); break;
        
        case SET_DONE:
        
            curTime.month = timeArray[TIME_MONTH];
            curTime.day = timeArray[TIME_DAY];
            curTime.year = timeArray[TIME_YEAR];
            
            curTime.hour = timeArray[TIME_HOUR];
            if(timeArray[TIME_AMPM] == 1) curTime.hour += 12;
            
            curTime.minute = timeArray[TIME_MINUTE_A] * 10;
            curTime.minute += timeArray[TIME_MINUTE_B];
            curTime.second = 0;
            curTime.hundredth = 0;
            
        case SET_CANCEL: 
            if(!repeatPress || longPress) {
                pageMode = MODE_MAIN; 
                init();
            }
            break;
        
    }

}

void BlueClockPage::currentEditIncrement(int dir) {

    currentEditIncrement(dir,false);

}

void BlueClockPage::currentEditIncrement(int dir,bool force) {

    if(dir == UP) {
        currentEdit++; if(currentEdit >= TIME_TOTAL) currentEdit = 0;
    } else if(dir == DOWN) {
        currentEdit--; if(currentEdit < 0) currentEdit = TIME_TOTAL - 1;
    }
    
    if(currentEditOld != currentEdit || force) {
    
        printSetItem(currentEditOld,true);
        printSetItem(currentEdit,false);
        
        blinkState = false;
        
        currentEditOld = currentEdit;
    
        blinkTime = 250;
        
    }
    
}

void BlueClockPage::editIncrement(bool dir) {

    int incr = dir ? 1 : -1;
    
    timeArray[currentEdit] += incr;
    
    if(currentEdit == TIME_HOUR) {
    
        if(timeArray[currentEdit] > 12) timeArray[currentEdit] = 1;
        if(timeArray[currentEdit] < 1) timeArray[currentEdit] = 12;
        
    } else if(currentEdit == TIME_AMPM) {
    
        if(timeArray[currentEdit] > 1) timeArray[currentEdit] = 0;
        if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 1;
        
    } else if(currentEdit == TIME_MINUTE_A) {
    
        if(timeArray[currentEdit] > 5) timeArray[currentEdit] = 0;
        if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 5;
    
    } else if(currentEdit == TIME_MINUTE_B) {
    
        if(timeArray[currentEdit] > 9) timeArray[currentEdit] = 0;
        if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 9;
    
    } else if(currentEdit == TIME_MONTH) {
    
        if(timeArray[currentEdit] > 12) timeArray[currentEdit] = 1;
        if(timeArray[currentEdit] < 1) timeArray[currentEdit] = 12;
    
    } else if(currentEdit == TIME_DAY) {
    
        int monthLen = monthLength(timeArray[TIME_MONTH],curTime.year);
    
        if(timeArray[currentEdit] > monthLen + 1) timeArray[currentEdit] = 1;
        if(timeArray[currentEdit] < 1) timeArray[currentEdit] = monthLen + 1;
    
    } else if(currentEdit == TIME_YEAR) {
    
        if(timeArray[currentEdit] > 9999) timeArray[currentEdit] = 9999;
        if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 0;
    
    }
    
    printSetItem(currentEdit,true);
    
    blinkState = true;
    
    blinkTime = 250;
    
    if(timeArray[TIME_DAY] > monthLength(timeArray[TIME_MONTH],curTime.year) + 1) {
        
        timeArray[TIME_DAY] = monthLength(timeArray[TIME_MONTH],curTime.year) + 1;
        printSetItem(TIME_DAY,true);
    
    }
    
    if(currentEdit == TIME_DAY || currentEdit == TIME_MONTH || currentEdit == TIME_YEAR) {
    
        printWeekday(timeArray[TIME_DAY],timeArray[TIME_MONTH],timeArray[TIME_YEAR]);
    
    }
    
}

int BlueClockPage::buttonID(int d) {

    // if(D) 

    #define inrg(x,y) (d >= min(x,y) && d < max(x,y))
    
    int splitpoints[20];
    int buttons = 0;
    int buttonZero = 0;
    
    switch(pageMode) {
    
        case MODE_MAIN: 
            
            buttons = 0;
        
            splitpoints[buttons++] = 270; // MAIN_SET
            splitpoints[buttons++] =   0; // MAIN_TIMER
            splitpoints[buttons++] =  90; // MAIN_STOPWATCH
            splitpoints[buttons++] = 180; // MAIN_HOME
            
            // if(     inrg(270,360)) return MAIN_SET;
            // else if(inrg(  0, 90)) return MAIN_TIMER;
            // else if(inrg( 91,180)) return MAIN_STOPWATCH;
            // else                   return MAIN_HOME;
            
            buttonZero = MAIN_SET;
            
            break;
        case MODE_SET:
        
            buttons = 0;
        
            splitpoints[buttons++] = 339; // SET_UP
            splitpoints[buttons++] =  23; // SET_DONE 
            splitpoints[buttons++] =  85; // SET_RIGHT 
            splitpoints[buttons++] = 144; // SET_DOWN 
            splitpoints[buttons++] = 234; // SET_LEFT 
            splitpoints[buttons++] = 275; // SET_CANCEL 
            
            buttonZero = SET_UP;
            
            break;
        case MODE_STOPWATCH: break;
        case MODE_TIMER: break;
    
    
    }
    
    for(int i=0;i<buttons;i++) {
    
        int lo = splitpoints[i];
        int hi = splitpoints[(i+1==buttons) ? 0 : (i+1)];
    
        if((d >= lo && d < hi) || (hi < lo && (d >= lo || d < hi))) return buttonZero + i;
    
    }
    
    #undef inrg(x,y)
    
    return -1;
    
}

void BlueClockPage::button(int dir,int index) {

}

void BlueClockPage::printTime(int hours,int minutes) {
    
    // Hour
    if(timeArray[TIME_HOUR] != hours) printHour(hours);

    // Minute
    byte minuteA = minutes / 10;
    byte minuteB = minutes - minuteA * 10;
    
    if(minuteA != timeArray[TIME_MINUTE_A]) lcd->printGci(numbFile,128,77,11 * 2 + minuteA);
    if(minuteB != timeArray[TIME_MINUTE_B]) lcd->printGci(numbFile,160,77,11 * 3 + minuteB);
    
    timeArray[TIME_MINUTE_A] = minuteA; 
    timeArray[TIME_MINUTE_B] = minuteB; 
    
    // AM/PM
    byte ampm = 0;
    if(hours > 12) ampm = 1;
    
    if(timeArray[TIME_AMPM] != ampm) lcd->printGci(ampmFile,190,104,ampm);
    
    timeArray[TIME_AMPM] = ampm;

}

void BlueClockPage::printHour(int hours) {

    if(hours == -1) {
    
        lcd->printGci(numbFile,32,77,11 * 0 + 10);
        lcd->printGci(numbFile,64,77,11 * 1 + 10);
        return;
        
    }

    if(hours > 12) hours -= 12;
    
    byte hourA = hours / 10;
    byte hourB = hours - hourA * 10;

    lcd->printGci(numbFile,32,77,11 * 0 + (hourA == 0 ? 10 : hourA));

    lcd->printGci(numbFile,64,77,11 * 1 + hourB);
    
    timeArray[TIME_HOUR] = hours;
    
}

void BlueClockPage::printDate(int day,int month,int year) {

    printWeekday(day,month,year);
    printMonth(month);
    printDay(day);
    if(timeArray[TIME_YEAR] != year) printYear(year);
   
}

void BlueClockPage::printDay(int day) {

    if(day == -1) {
    
        lcd->printGci(dayAFile,144,158,10);
        lcd->printGci(dayBFile,155,158,10);
        
    }

    byte dayA = day / 10;
    byte dayB = day - dayA * 10;

    if(day > 9) lcd->printGci(dayAFile,144,158,dayA);
    else        lcd->printGci(backgroundImageFile,0,0,1,144,158,154,171,false);
    lcd->printGci(dayBFile,155,158,dayB);
    
    timeArray[TIME_DAY] = day;
    
}

void BlueClockPage::printMonth(int month) {

    lcd->printGci(monthFile,45,157,month - 1);
    
    timeArray[TIME_MONTH] = month;
    
}

void BlueClockPage::printWeekday(int day,int month,int year) {

    int weekDay = getDayOfTheWeek(day,month - 1,year) - 1;
    if(weekDay < 0) weekDay += 7;
    
    lcd->printGci(weekdayFile,60,136,weekDay);
    
}

void BlueClockPage::printYear(int year) {

    char yearstring[10];
    
    inttostring(year,yearstring,0,4,'0');
    
    yearstring[4] = 0;
    if(D) db.printf("yearstring %s\r\n",yearstring);
    
    fori(4) lcd->printGci(yearFile,89,179,yearstring[i] + 1 - '0',i * 11,0,(i + 1) * 11 - 2,12);
    
    timeArray[TIME_YEAR] = year;
    
}

#endif
























