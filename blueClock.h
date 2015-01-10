


#ifndef BlUE_CLOCK_PAGE_
#define BlUE_CLOCK_PAGE_

#include "roundUtil.h"

class BlueClockPage : public Page {

public:

time_t editTime;

int touchIndex = -1;

int touches[10];
bool repeatTouch[10];

image_info numbFile;
image_info ampmFile;
image_info weekdayFile;
image_info monthFile;
image_info dayAFile;
image_info dayBFile;
image_info yearFile;

image_info backgroundImageFile;

enum time_array {

    TIME_HOUR,
    TIME_MINUTE_A,
    TIME_MINUTE_B,
    TIME_AMPM,
    TIME_MONTH,
    TIME_DAY,
    TIME_YEAR,
    TIME_WEEKDAY,

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



BlueClockPage CONSTRUCTOR_MACRO


void initalize() {

    if(D) USB.println("initalize");

    // if(backgroundImageFile.isOpen()) backgroundImageFile.close();
    // if(numbFile.isOpen()) numbFile.close();
    // if(ampmFile.isOpen()) ampmFile.close();
    // if(weekdayFile.isOpen()) weekdayFile.close();
    // if(monthFile.isOpen()) monthFile.close();
    // if(dayAFile.isOpen()) dayAFile.close();
    // if(dayBFile.isOpen()) dayBFile.close();
    // if(yearFile.isOpen()) yearFile.close();

    // if(!backgroundImageFile      .open("blue.Gci",O_RDWR)) USB.println("File open fail");
    // if(!numbFile                                  .open("blueNum.gci",O_RDWR)) USB.println("File open fail");
    // if(!ampmFile                     
    // if(!weekdayFile                           .open("blueDays.gci",O_RDWR)) USB.println("File open fail");
    // if(!monthFile                                     .open("blueMon.gci",O_RDWR))    USB.println("File open fail");
    // if(!dayAFile                              .open("blueDayA.gci",O_RDWR))    USB.println("File open fail");
    // if(!dayBFile                              .open("blueDayB.gci",O_RDWR))    USB.println("File open fail");
    // if(!yearFile                              .open("blueyear.gci",O_RDWR))    USB.println("File open fail");

    watch->loadImage("blue.Gci",     &backgroundImageFile);
    watch->loadImage("blueNum.gci",  &numbFile           );
    watch->loadImage("blueAmPm.gci", &ampmFile           );
    watch->loadImage("blueDays.gci", &weekdayFile        );
    watch->loadImage("blueMon.gci",  &monthFile          );
    watch->loadImage("blueDayA.gci", &dayAFile           );
    watch->loadImage("blueDayB.gci", &dayBFile           );
    watch->loadImage("blueyear.gci", &yearFile           );
    
    pageMode = MODE_MAIN;
    // pageMode = SET;
    
    init();

}

void init() {
    
    fori(10) touches[i] = -1;
    
    printBack();
    
    fori(TIME_TOTAL) timeArray[i] = -1;
    
    // timeArray[TIME_HOUR]        = -1;
    // timeArray[TIME_MINUTE_A]    = -1;
    // timeArray[TIME_MINUTE_B]    = -1;
    // timeArray[TIME_AMPM]        = -1;
    // timeArray[TIME_MONTH]       = -1;
    // timeArray[TIME_DAY]         = -1;
    // timeArray[TIME_YEAR]        = -1;

    if(pageMode == MODE_SET) {
        
        currentEdit = 0;
        currentEditOld = 0;
    
        printTime(editTime);
        
    } else {
        
        printTime(now());
        
    }
    
    // if(D) USB.printf("curTime %d\r\n",curTime.hour,curTime.minute,curTime.second,curTime.hundredth);
    
}

void leavingPage() {

}

void loop() {

    switch(pageMode) {
        case MODE_MAIN: mainLoop(); break;
        case MODE_SET:  setLoop();  break;
    }

}

void mainLoop() {

    // static elapsedMillis updateTime;
    // if(updateTime > 100) {
        
        // updateTime = 0;

        // if(D) USB.printf("curTime %03d %03d %03d %03d\r\n",curTime.hour,curTime.minute,curTime.second,curTime.hundredth);
        
        // printTime(curTime.second,curTime.hundredth);
        // printTime(curTime.minute,curTime.second);
        
        printTime(now());
        
        // printTime(curTime.hour,curTime.minute);
        // printDate(curTime.day,curTime.month,curTime.year);
        
    // }

}

void lowPowerLoop() {
    
    static long lastTime = 0;
    
    if(micros() - lastTime > 100000) {
    
        printTime(now());
        
        lastTime = micros();
    
    }
    
}

void setLoop() {

    if(blinkTime > 500) {
        
        blinkTime = 0;
        
        blinkState = !blinkState;
        
        printSetItem(currentEdit,blinkState);
        
    }

}

void printSetItem(int index,bool state) {

    switch(index) {
    
        case TIME_HOUR:     printHour(state ? timeArray[TIME_HOUR] : -1); break;
        case TIME_MINUTE_A: watch->printImage(&numbFile,128,77,11 * 2 + (!state ? 10 : timeArray[TIME_MINUTE_A])); break;
        case TIME_MINUTE_B: watch->printImage(&numbFile,160,77,11 * 3 + (!state ? 10 : timeArray[TIME_MINUTE_B])); break;
        case TIME_AMPM:
            if(state)   watch->printImage(&ampmFile,190,104,timeArray[TIME_AMPM]); 
            else        watch->printImage(&backgroundImageFile,0,0,1,190,104,211,117);
            break;
        case TIME_MONTH:
            if(state)   printMonth(timeArray[TIME_MONTH]);
            else        watch->printImage(&backgroundImageFile,0,0,1,45,157,141,175);
            break;
        case TIME_DAY:
            if(state)   printDay(timeArray[TIME_DAY]);
            else        watch->printImage(&backgroundImageFile,0,0,1,144,158,165,171);
            break;
        case TIME_YEAR:
            if(state)   printYear(timeArray[TIME_YEAR]);
            else        watch->printImage(&backgroundImageFile,0,0,1,89,179,132,192);
            break;
    }

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    static int touchPressedTime = 0;
    
    if(touchType != MOVING) if(D) USB.printf("touch() finePos %d\r\n",finePos);

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

void buttonPress(int btn,bool longPress,bool repeatPress) {

    if(btn == -1) {
        if(D) USB.println("Invalid button");
        return;
    }
    
    if(D) USB.printf("buttonPress %s longPress %d repeatPress %d\r\n",buttonNames[btn],longPress,repeatPress);

    switch(btn) {
    
        case MAIN_SET: 
            
            if(longPress) {
                
                editTime = now();
                
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
        
            // Make sure the time is alligned on the minute
            editTime -= editTime % 60;
        
            Teensy3Clock.set(editTime);
            setTime(editTime);
        
        case SET_CANCEL: 
            if(!repeatPress || longPress) {
                pageMode = MODE_MAIN; 
                init();
            }
            break;
        
    }

}

void currentEditIncrement(int dir) {

    currentEditIncrement(dir,false);

}

void currentEditIncrement(int dir,bool force) {

    if(dir == UP) {
        currentEdit++; 
        if(currentEdit >= TIME_TOTAL) currentEdit = 0;
    } else if(dir == DOWN) {
        currentEdit--; 
        if(currentEdit < 0) currentEdit = TIME_TOTAL - 1;
    }
    
    if(currentEditOld != currentEdit || force) {
    
        printSetItem(currentEditOld,true);
        printSetItem(currentEdit,false);
        
        blinkState = false;
        
        currentEditOld = currentEdit;
        
        blinkTime = 250;
        
    }
    
}

void editIncrement(bool dir) {

    tmElements_t time;

    breakTime(editTime,time);

    int incr = dir ? 1 : -1;
    
    #define INCREMENT(VAR,INC,MAX,MIN) { int _a = VAR; _a += incr * (INC); if((_a) < (MIN)) _a += (MAX) - (MIN); if((_a) >= (MAX)) _a -= (MAX) - (MIN); VAR = _a; }
    
    if(currentEdit == TIME_HOUR)            INCREMENT(time.Hour,    1,  24, 0)
    else if(currentEdit == TIME_AMPM)       INCREMENT(time.Hour,   12,  24, 0)
    else if(currentEdit == TIME_MINUTE_A)   INCREMENT(time.Minute, 10,  60, 0)
    else if(currentEdit == TIME_MINUTE_B)   INCREMENT(time.Minute,  1,  time.Minute-(time.Minute%10)+10,    time.Minute-(time.Minute%10))
    else if(currentEdit == TIME_DAY)        INCREMENT(time.Day,     1,  monthLength(editTime), 0)
    else if(currentEdit == TIME_YEAR)       INCREMENT(time.Year,    1,  2105-1970, 0)
    else if(currentEdit == TIME_MONTH)      INCREMENT(time.Month,   1,  13, 1)
    
    editTime = makeTime(time);
    
    printTime(editTime);
    
    blinkState = true;
    
    blinkTime = 250;
    
    
    /*
    
    int incr = dir ? 1 : -1;
    
    if(currentEdit == TIME_HOUR)            editTime += incr * SECS_PER_HOUR;
    else if(currentEdit == TIME_AMPM)       editTime += (isPM(editTime) ? -1 : 1) * 12 * SECS_PER_HOUR;
    else if(currentEdit == TIME_MINUTE_A)   editTime += incr * SECS_PER_MIN * 10; 
    else if(currentEdit == TIME_MINUTE_B)   editTime += incr * SECS_PER_MIN;
    else if(currentEdit == TIME_DAY)        editTime += incr * SECS_PER_DAY; 
    else if(currentEdit == TIME_YEAR)       editTime += incr * SECS_PER_YEAR;
    else if(currentEdit == TIME_MONTH)      editTime += incr * SECS_PER_DAY * monthLength(month(editTime),year(editTime));
    
    printTime(editTime);
    
    blinkState = true;
    
    blinkTime = 250;
    
    */
    
    
    /*
    
//     int incr = dir ? 1 : -1;
//     
//     timeArray[currentEdit] += incr;
//     
//     if(currentEdit == TIME_HOUR) {
//     
//         if(timeArray[currentEdit] > 12) timeArray[currentEdit] = 1;
//         if(timeArray[currentEdit] < 1) timeArray[currentEdit] = 12;
//         
//     } else if(currentEdit == TIME_AMPM) {
//     
//         if(timeArray[currentEdit] > 1) timeArray[currentEdit] = 0;
//         if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 1;
//         
//     } else if(currentEdit == TIME_MINUTE_A) {
//     
//         if(timeArray[currentEdit] > 5) timeArray[currentEdit] = 0;
//         if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 5;
//     
//     } else if(currentEdit == TIME_MINUTE_B) {
//     
//         if(timeArray[currentEdit] > 9) timeArray[currentEdit] = 0;
//         if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 9;
//     
//     } else if(currentEdit == TIME_MONTH) {
//     
//         if(timeArray[currentEdit] > 12) timeArray[currentEdit] = 1;
//         if(timeArray[currentEdit] < 1) timeArray[currentEdit] = 12;
//     
//     } else if(currentEdit == TIME_DAY) {
//     
//         int monthLen = monthLength(timeArray[TIME_MONTH],curTime.year);
//     
//         if(timeArray[currentEdit] > monthLen + 1) timeArray[currentEdit] = 1;
//         if(timeArray[currentEdit] < 1) timeArray[currentEdit] = monthLen + 1;
//     
//     } else if(currentEdit == TIME_YEAR) {
//     
//         if(timeArray[currentEdit] > 9999) timeArray[currentEdit] = 9999;
//         if(timeArray[currentEdit] < 0) timeArray[currentEdit] = 0;
//     
//     }
//     
//     printSetItem(currentEdit,true);
//     
//     blinkState = true;
//     
//     blinkTime = 250;
//     
//     if(timeArray[TIME_DAY] > monthLength(timeArray[TIME_MONTH],curTime.year) + 1) {
//         
//         timeArray[TIME_DAY] = monthLength(timeArray[TIME_MONTH],curTime.year) + 1;
//         printSetItem(TIME_DAY,true);
//     
//     }
//     
//     if(currentEdit == TIME_DAY || currentEdit == TIME_MONTH || currentEdit == TIME_YEAR) {
//     
//         printWeekday(timeArray[TIME_DAY],timeArray[TIME_MONTH],timeArray[TIME_YEAR]);
//     
//     }
//     

*/

}

int buttonID(int d) {

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

void button(int dir,int index) {

}

void printTime(time_t tmpTime) {
    
    // Hour
    if(timeArray[TIME_HOUR] != hour(tmpTime)) printHour(hour(tmpTime));

    // Minute
    byte minuteA = minute(tmpTime) / 10;
    byte minuteB = minute(tmpTime) - minuteA * 10;
    
    if(minuteA != timeArray[TIME_MINUTE_A]) watch->printImage(&numbFile,128,77,11 * 2 + minuteA);
    if(minuteB != timeArray[TIME_MINUTE_B]) watch->printImage(&numbFile,160,77,11 * 3 + minuteB);
    
    timeArray[TIME_MINUTE_A] = minuteA; 
    timeArray[TIME_MINUTE_B] = minuteB; 
    
    // AM/PM
    byte ampm = isPM(tmpTime);
    
    if(timeArray[TIME_AMPM] != ampm) watch->printImage(&ampmFile,190,104,ampm);
    
    timeArray[TIME_AMPM] = ampm;

    printDate(tmpTime);
   
}

void printHour(int hours) {
    
    if(D) db.printf("printHour %d\r\n",hours);
    
    if(hours == -1) {
    
        watch->printImage(&numbFile,32,77,11 * 0 + 10);
        watch->printImage(&numbFile,64,77,11 * 1 + 10);
        return;
        
    }

    timeArray[TIME_HOUR] = hours;

    if(hours > 12) hours -= 12;
    
    byte hourA = hours / 10;
    byte hourB = hours - hourA * 10;

    watch->printImage(&numbFile,32,77,11 * 0 + (hourA == 0 ? 10 : hourA));

    watch->printImage(&numbFile,64,77,11 * 1 + hourB);
    
}

void printDate(time_t tmpTime) {

    if(timeArray[TIME_WEEKDAY] != weekday(tmpTime))     printWeekday(weekday(tmpTime));
    if(timeArray[TIME_MONTH] != month(tmpTime))         printMonth(month(tmpTime));
    if(timeArray[TIME_DAY] != day(tmpTime))             printDay(day(tmpTime));
    if(timeArray[TIME_YEAR] != year(tmpTime))           printYear(year(tmpTime));
    
}

void printDay(int day) {

    if(day == -1) {
    
        watch->printImage(&dayAFile,144,158,10);
        watch->printImage(&dayBFile,155,158,10);
        
    }

    byte dayA = day / 10;
    byte dayB = day - dayA * 10;

    if(day > 9) watch->printImage(&dayAFile,144,158,dayA);
    else        watch->printImage(&backgroundImageFile,0,0,1,144,158,154,171);
    watch->printImage(&dayBFile,155,158,dayB);
    
    timeArray[TIME_DAY] = day;
    
}

void printMonth(int month) {

    // if(D) USB.printf("printMonth %d\r\n",month);

    watch->printImage(&monthFile,45,157,month - 1);
    
    timeArray[TIME_MONTH] = month;
    
}

void printWeekday(int wkday) {
    
    timeArray[TIME_WEEKDAY] = wkday;

    // This number is 1 indexed and it starts at Sunday whereas the file is 
    // zero indexed and it starts with monday, therefore we subtract two here
    wkday -= 2;
    
    if(wkday < 0) wkday += 7;

    watch->printImage(&weekdayFile,60,136,wkday);
    
}

void printYear(int year) {

    char yearstring[10];
    
    inttostring(year,yearstring,0,4,'0');
    
    yearstring[4] = 0;
    if(D) db.printf("yearstring %s\r\n",yearstring);
    
    fori(4) watch->printImage(&yearFile,89,179,yearstring[i] + 1 - '0',i * 11,0,(i + 1) * 11 - 2,12);
    
    timeArray[TIME_YEAR] = year;
    
}

void printBack() {

    switch(pageMode) {
    
        case MODE_SET: watch->printImage(&backgroundImageFile,0,0,1); break;
        case MODE_STOPWATCH:
        case MODE_TIMER:
        case MODE_MAIN: watch->printImage(&backgroundImageFile,0,0,0); break;
    
    }
    
}


};

#endif
























