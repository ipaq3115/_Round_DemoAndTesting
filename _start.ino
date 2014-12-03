
void setup() {

    // Make sure this is the first thing you do because it sets
    // pin states up for the device
    watch.init();
    
    USB.begin(300000); 
    // while(!USB); // Wait for PC to open the USB serial port before running this program
    delay(100);
    
    if(D) USB.println("## SETUP START ##");
    
    // while(true);

    loadCompileTime();
    
    Wire.begin(I2C_MASTER,0,0,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_400);
    // Wire.begin(I2C_MASTER,0,0,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_800);
    
    // setupCompass();
    compass.init(LSM303C_DEVICE);
    
    btooth.init();
    btooth.discoverable(ON);
    
    lcd.InitLCD(currentRotation);
    touchCtrl.setOrientation(currentRotation);
    
    loadSdCard();
    
    pageArray[PAGE::HOME]               = new HomePage(ARGS_MACRO);
    pageArray[PAGE::GRAVITY_BALL]       = new GravityBallPage(ARGS_MACRO);
    pageArray[PAGE::CONTROL_SIMPLE]     = new ControlSimplePage(ARGS_MACRO);
    pageArray[PAGE::NUMBER_PAD]         = new NumberPadPage(ARGS_MACRO);
    pageArray[PAGE::PHONE]              = new PhonePage(ARGS_MACRO);
    pageArray[PAGE::CALL]               = new CallPage(ARGS_MACRO);
    pageArray[PAGE::TOUCH_DEMO]         = new TouchDemoPage(ARGS_MACRO);
    pageArray[PAGE::CONTACTS]           = new ContactsPage(ARGS_MACRO);
    pageArray[PAGE::BLUE_CLOCK]         = new BlueClockPage(ARGS_MACRO);
    pageArray[PAGE::XKCD_CLOCK]         = new XKCDclockPage(ARGS_MACRO);
    pageArray[PAGE::BAR_CLOCK]          = new BarClockPage(ARGS_MACRO);
    pageArray[PAGE::CLOCKS]             = new ClockSelectPage(ARGS_MACRO);
    pageArray[PAGE::VIDEO]              = new VideoPlayerPage(ARGS_MACRO);
    pageArray[PAGE::APPS]               = new AppsPage(ARGS_MACRO);
    pageArray[PAGE::SETTINGS]           = new SettingsPage(ARGS_MACRO);
    pageArray[PAGE::SETTINGS_B]         = new SettingsPageB(ARGS_MACRO);
    pageArray[PAGE::BLUISH_CLOCK]       = new BluishClockPage(ARGS_MACRO);
    
    showSplash();
    
    // goPage(PAGE::VIDEO);
    // goPage(PAGE::TOUCH_DEMO);
    goPage(PAGE::BLUE_CLOCK);
    // goPage(PAGE::HOME);
    // goPage(PAGE::SETTINGS);
    
    // animateNotificationTest();
    
    fillDemoContacts();
    
    // analogReference(INTERNAL);
    analogReadResolution(8);
    
    if(D) USB.println("## LOOP START ##");
    
    analogWriteResolution(8);
    
    // startPlay("rally");
    // startPlay("rdkill");
    // startPlay("rdkill3");
    // startPlay("drift");
    // startPlay("drift2");
    
    btooth.power(OFF);

    touchCtrl.init(touch);
    
}

void animateNotificationTest() {

    for(int i=0;i<70;i++) {
    
        for(int n=0;n<5;n++) {
        
        
        }
    
        lcd.setColor(0xFFFF);
        lcd.fillRect(0,219 - i,219,219);
        
        lcd.setColor(0);
        lcd.setBackColor(0xFFFF);
        lcd.setFont(BigFont);
        lcd.print("Hello",CENTER,219 - i + 10);
        lcd.print("World!",CENTER,219 - i + 25);
        
        delay(2);
        
    }
    
    while(1);

}

void loop() {

    touchCtrl.loop();

    pollButtons();
    
    btooth.loop();
    
    pageArray[page]->loop();
    
    checkPhoneState();
    
    updateTime();
    
    audioLoop();
    
    // lowPowerTimeout();

    checkOrientation();
    
}

void checkOrientation() {
    
    static elapsedMillis mytime;
    if(mytime > 10) {
    
        mytime = 0;
        
        compass.read();
        
        int lastRotation = currentRotation;
        
        #ifdef HARDWARE_REVB
            
            if(page == PAGE::TOUCH_DEMO || page == PAGE::GRAVITY_BALL)     currentRotation = PORTRAIT;
            else if(compass.a.y < -1000 && currentRotation != PORTRAIT)    currentRotation = PORTRAIT;
            else if(compass.a.y > 1000 && currentRotation != PORTRAIT_R)   currentRotation = PORTRAIT_R;
            else if(compass.a.x < -1000 && currentRotation != LANDSCAPE)   currentRotation = LANDSCAPE;
            else if(compass.a.x > 1000 && currentRotation != LANDSCAPE_R)  currentRotation = LANDSCAPE_R;
            
        #else
            
            if(page == PAGE::TOUCH_DEMO || page == PAGE::GRAVITY_BALL)     currentRotation = PORTRAIT;
            else if(compass.a.x > 1000 && currentRotation != PORTRAIT)     currentRotation = PORTRAIT;
            else if(compass.a.x < -1000 && currentRotation != PORTRAIT_R)  currentRotation = PORTRAIT_R;
            else if(compass.a.y > 1000 && currentRotation != LANDSCAPE)    currentRotation = LANDSCAPE;
            else if(compass.a.y < -1000 && currentRotation != LANDSCAPE_R) currentRotation = LANDSCAPE_R;
        
        #endif
        
        if(lastRotation != currentRotation) {
        
            lcd.setOrientation(currentRotation);
            touchCtrl.setOrientation(currentRotation);
            pageArray[page]->initalize();
        
        }
        
    }

}

#if 0 // Low power stuff

void lowPowerTimeout() {

    IntervalTimer_LP pwmTimerStart;
    IntervalTimer_LP pwmTimerEnd;

    if(millis() - lastTouchTime > 5000) {

        TOUCH_READ::timer.end();

        // delay(1000);

        // btooth.power(OFF);

        LP.CPU(TWO_MHZ);

        // delay(5000);

        // analogWrite(PIN::LCD_BACKLIGHT, 127);
        pinMode(PIN::LCD_BACKLIGHT, OUTPUT);
        digitalWrite(PIN::LCD_BACKLIGHT, LOW);

        pwmTimerStart.begin(pwmON,2000);

        while(1) {

            for(int i=0;i<10;i++) if(touchRead(TOUCH_READ::touchPin[i]) > calValue[i] + 1000) goto out;

        }

        out:

        pwmTimerStart.end();

        LP.CPU(F_CPU);

        digitalWriteFast(PIN::LCD_BACKLIGHT, HIGH);

        // LP.DeepSleep(TSI_WAKE, TOUCH_READ::touchPin[5], calValue[5] + 1000);

        startSample();

        lastTouchTime = millis();

    }

}

void pwmON() {

    pwmTimerEnd.begin(pwmOFF, 100);
    
    digitalWriteFast(PIN::LCD_BACKLIGHT, HIGH);
    
}

void pwmOFF() {

    pwmTimerEnd.end();
    
    digitalWriteFast(PIN::LCD_BACKLIGHT, LOW);

}

#endif

void checkPhoneState() {

    static int currentPhoneState = -1;
    if(currentPhoneState != btooth.phoneState) {
    
        currentPhoneState = btooth.phoneState;
    
        switch(currentPhoneState) {
            
            case PHONE_RINGING:   
            case PHONE_IN_CALL:    
            case PHONE_DIALING:
                goPage(PAGE::CALL);
                break;
        
        }
    
    }

}

void updateTime() {

    static int lastUpdateTime = 0;
    int timeSinceLastUpdate = millis() - lastUpdateTime;
    if(timeSinceLastUpdate > 10) {
        lastUpdateTime = millis();
        
        // Add the hundreth's of a second
        curTime.hundredth += timeSinceLastUpdate / 10;
        
        // Check for overflow and add to the seconds
        while(curTime.hundredth > 99) {
        
            curTime.hundredth -= 100;
            curTime.second += 1;
        
        }
        
        // Check for overflow and add to the minutes
        while(curTime.second > 59) {
        
            curTime.second -= 60;
            curTime.minute += 1;
        
        }
        
        // Check for overflow and add to the hours
        while(curTime.minute > 59) {
        
            curTime.minute -= 60;
            curTime.hour += 1;
        
        }
        
        // Check for overflow and add to the days
        while(curTime.hour > 23) {
        
            curTime.hour -= 24;
            curTime.day += 1;
        
        }
    
        while(curTime.day >= monthLength(curTime.month,curTime.year + 2000)) {
        
            curTime.day -= monthLength(curTime.month,curTime.year + 2000);
            curTime.month += 1;
        
        }
        
        while(curTime.month > 12) {
        
            curTime.month -= 12;
            curTime.year += 1;
        
        }
    
    }

}

int monthLength(int month,int year) {

    int const 
    
    JANUARY         =  1,
    FEBRUARY        =  2,
    MARCH           =  3,
    APRIL           =  4,
    MAY             =  5,
    JUNE            =  6,
    JULY            =  7,
    AUGUST          =  8,
    SEPTEMBER       =  9,
    OCTOBER         = 10,
    NOVEMBER        = 11,
    DECEMBER        = 12;
    
    switch(month) {
    
        case JANUARY:   return 31;
        case FEBRUARY:  
            if(year % 4)            return 28;
            else if(year % 100)     return 29;
            else if(year % 400)     return 28;
            else                    return 29;
        case MARCH:     return 31;
        case APRIL:     return 30;
        case MAY:       return 31;
        case JUNE:      return 30;
        case JULY:      return 31;
        case AUGUST:    return 31;
        case SEPTEMBER: return 30;
        case OCTOBER:   return 31;
        case NOVEMBER:  return 30;
        case DECEMBER:  return 31;
    
    }
    
}

int loadCompileTime() {

    // Making time and date string more cryptic
    char workString[50]; // "Mmm dd yyyyhh:mm:ss"
    int wsC=0;
    
    for(int i=0;i<11;i++) workString[wsC++] = __DATE__[i];
    for(int i=0;i<8;i++) workString[wsC++] = __TIME__[i]; workString[wsC++] = 0;
    
    // Month
    int month = 0;
    if(sMatch(workString,"Jan")==0)      month = 0x0;
    else if(sMatch(workString,"Feb")==0) month = 0x1;
    else if(sMatch(workString,"Mar")==0) month = 0x2;
    else if(sMatch(workString,"Apr")==0) month = 0x3;
    else if(sMatch(workString,"May")==0) month = 0x4;
    else if(sMatch(workString,"Jun")==0) month = 0x5;
    else if(sMatch(workString,"Jul")==0) month = 0x6;
    else if(sMatch(workString,"Aug")==0) month = 0x7;
    else if(sMatch(workString,"Sep")==0) month = 0x8;
    else if(sMatch(workString,"Oct")==0) month = 0x9;
    else if(sMatch(workString,"Nov")==0) month = 0xA;
    else if(sMatch(workString,"Dec")==0) month = 0xB;

    // Day
    int day = 0x00;
    if(!stringtoint(workString,4,5,day)) day = 0x00;
    
    // Year
    int year = 0x0000;
    if(!stringtoint(workString,7,10,year)) year = 0x0000;
    
    // Hour
    int hour = 0x00;
    if(!stringtoint(workString,11,12,hour)) hour = 0x00;
    
    // Minute
    int minute = 0x00;
    if(!stringtoint(workString,14,15,minute)) minute = 0x00;
    
    // Second
    int second = 0x00;
    if(!stringtoint(workString,17,18,second)) second = 0x00;
    
    if(D) USB.printf("Compile time %d %d %d %d %d %d\r\n",year,month,day,hour,minute,second);
    
    curTime.year = year;
    curTime.month = month + 1;
    curTime.day = day;
    curTime.hour = hour;
    curTime.minute = minute;
    curTime.second = second;
    curTime.hundredth = 0;

}

// Init

void fillDemoContacts() {

    Contact test("myname","15407884622");
    
    if(D) USB.printf("sizeof(test) %d\r\n",sizeof(test));
    
    SdFile testFile;
    
    if(!testFile.open(contactsFilename,O_RDWR | O_CREAT)) USB.printf("Opening %s failed\r\n",contactsFilename);
    else {
    
        testFile.seekSet(0);

        testFile.write(byte(0));
        testFile.write(byte(50));
        
        for(int i=0;i<50;i++) {
    
            test.name[6] = i + 'A';
        
            testFile.write(test.b,sizeof(test.b));
        
        }
    
    }
    
    testFile.close();
    
}

void loadSdCard() {

    // SPI.setMOSI(7);
    // SPI.setSCK(13);

    // SPI.setMOSI(7);
    // SPI.setSCK(14);

    retry1:
    if(sd.begin(PIN::SD_CHIP_SELECT, SPI_FULL_SPEED)) {
        
        if(D) USB.println("SD card mounted");
        
    } else {
    
        lcd.print("No sdcard",CENTER,100);
        
        if(D) USB.println("No sdcard");
        
        delay(750);
        
        lcd.clrScr();
        
        goto retry1;
        
    }
  
}

void showSplash() {

    SdFile splashImage;
    
    if(!splashImage.open("PiSplash.bmp",O_RDWR)) USB.println("PiSplash not opened");
    
    lcd.printBitmap(splashImage,0,0);
    
    watch.rampBrightness(UP);
    
    splashImage.close();
    
    // delay(2000);
    
}

// Polling routines

void pollButtons() {

    using namespace BUTTON;
    
    // Button states
    const int
    STATE_DOWN = 0,
    STATE_UP = 1,
    GOING_DOWN = 2,
    GOING_UP = 3;

    const int DEBOUNCE_TIME = 200;
    
    static int buttonStates[TOTAL] {-1};
    static int lastButtonTime[TOTAL] {0};
    bool instantButtonState[TOTAL];
    
    int value = analogRead(PIN::POWER_BUTTON);
    
    instantButtonState[POWER_BUTTON] = value > 250;
    
    // if(D) USB.printf("%d\r\n",value);
    
    for(int i=0;i<TOTAL;i++) {
    
        if(buttonStates[i] == -1 && !instantButtonState[i]) {
        
            buttonStates[i] = STATE_DOWN;
        
        } else if(buttonStates[i] == STATE_DOWN && instantButtonState[i]) {
        
            if(D) USB.printf("GOING_UP %d\r\n",value);
        
            buttonStates[i] = GOING_UP;
            lastButtonTime[i] = millis();
    
        } else if(buttonStates[i] == STATE_UP && !instantButtonState[i]) {
        
            if(D) USB.println("GOING_DOWN");
        
            buttonStates[i] = GOING_DOWN;
            lastButtonTime[i] = millis();
        
        // if the state is pressed and the debounce curTime is over
        } else if(buttonStates[i] >= GOING_DOWN && millis() - lastButtonTime[i] > DEBOUNCE_TIME) {
        
            if(buttonStates[i] != instantButtonState[i]) {
            
                buttonStates[i] = instantButtonState[i];
                
                buttonEvent(buttonStates[i],i);
            
            }
        
        }
    
    
    
    }
    
}





void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(D && touchType != MOVING) {
        
        USB.print("touch ");
        switch(touchType) {
            case PRESSED:  USB.print("PRESSED "); break;
            case MOVING:   USB.print("MOVING  "); break;
            case RELEASED: USB.print("RELEASED"); break;
        }
        USB.printf(" finePos %03d activeTouches %d touchIndex %d\r\n",
        finePos,activeTouches,touchIndex);
        
    }

    int activatedTouches = 0;
    bool leftTouch = false,rightTouch = false;
    int const touchWidth = 72;
    static bool backActivated = false;
    static int lastTouchPos[10] {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    static long pressTime = 0;
    
    switch(touchType) {
    
        case PRESSED: 
            watch.vibrate(75);
            pressTime = millis();
        case MOVING: 
            lastTouchPos[touchIndex] = finePos; 
            break;
        case RELEASED: 
            lastTouchPos[touchIndex] = -1; 
            if(!watch.getVibrateState() && millis() - pressTime > 500) watch.vibrate(75); 
            break;
    
    }
    
    // Search through all of the current touches 
    // looking for two simultaneous touches in the 
    // right position to constitute a back gesture
    for(int i=0;i<10;i++) {
        
        if(lastTouchPos[i] != -1) {
        
            activatedTouches++;
            
            if(lastTouchPos[i] <= 360 && lastTouchPos[i] >= 360 - touchWidth) leftTouch = true;
            if(lastTouchPos[i] <= touchWidth && lastTouchPos[i] >= 0) rightTouch = true;

            if(leftTouch && rightTouch) backActivated = true;

        }

    }
    
    // Don't send touches if the back gesture has been noticed
    if(!backActivated) pageArray[page]->touch(touchType,finePos,activeTouches,touchIndex);

    // When all of the touches are gone and if the back was
    // activated send a back gesture now
    if(activatedTouches == 0 && backActivated) {
    
        backActivated = false;
        
        goBack();
    
    }
    
}

void buttonEvent(int dir,int index) {
   
    if(D) USB.printf("buttonEvent dir %d index %d\r\n",dir,index);
    
    switch(index) {
    
        case BUTTON::POWER_BUTTON:
        
            if(!dir) {
            
                watch.rampBrightness(DOWN);
                
                watch.powerDown();
            
            }
        
            break;
    
    }

    // switch(page) {
    // 
    //     case PAGE::HOME: homePageButtonEvent(dir,index); break;
    //     case PAGE::NUMBER_PAD: frequencyEntryButtonEvent(dir,index); break;
    // 
    // }
    
}

// Page management

void goBack() {

    if(D) USB.println("goBack");

    goPage(pageTrailRemove(),true,0,0);

}

void goBack(int mode,char * data) {

    if(D) USB.println("goBack");

    goPage(pageTrailRemove(),true,mode,data);

}

void goPage(int pg) {

    goPage(pg,false,0,0);

}

void goPage(int pg,int mode) {

    goPage(pg,false,mode,0);

}

void goPage(int pg,int mode,char * data) {

    goPage(pg,false,mode,data);

}

void goPage(int pg,bool goingback,int mode,char * data) {

    if(D) USB.printf("goPage %s goingback %d mode %d\r\n",PAGE::names[pg],goingback,mode);
    
    if(pg >= PAGE::TOTAL || pg < 0) {
    
        if(E) db.printf("Invalid page number %d\r\n",pg);
        return;
    
    }

    // No change
    if(page == pg) { if(D) USB.println("Page is the same"); return; }
    
    if(page != -1) pageArray[page]->leavingPage();
    
    page = pg;
    
    if(!goingback) pageTrailAdd(page);
    
    watch.rampBrightness(DOWN);
    
    pageArray[page]->initalize(mode,data);
    
    watch.rampBrightness(UP);

}

void pageTrailAdd(int tPage) {
  
    if(D) USB.printf("pageTrailAdd %d\r\n",tPage);
    
    pageTrail[pageTrailLocation] = tPage;
    
    pageTrailLocation++;  
    pageTrailLength++;   
    
    if(pageTrailLocation >= PAGE_TRAIL_SIZE)  pageTrailLocation = 0;
    if(pageTrailLength > PAGE_TRAIL_SIZE)     pageTrailLength = PAGE_TRAIL_SIZE;

}

int pageTrailRemove() {

    if(D) USB.printf("pageTrailRemove pageTrailLength %d pageTrailLocation %d\r\n",
    pageTrailLength,pageTrailLocation);
    
    if(pageTrailLength > 1) {
    
        pageTrailLength--;
        pageTrailLocation--; 
        
        if(pageTrailLocation < 0) pageTrailLocation = PAGE_TRAIL_SIZE - 1;
        
        if(D) USB.printf("Removed %d\r\n",pageTrail[pageTrailLocation]);
        
        pageTrail[pageTrailLocation] = 0xFF;
        
    }
    
    int tempPageTrailLocation = pageTrailLocation - 1;
    if(tempPageTrailLocation < 0) tempPageTrailLocation = PAGE_TRAIL_SIZE - 1;
    
    if(D) USB.printf("pageTrailRemove last page %d\r\n",pageTrail[tempPageTrailLocation]);
    
    return pageTrail[tempPageTrailLocation];

}


// Gague Page

void printNeedleFast(int frame,int undrawFrame) {

    if(frame < 0) frame = 0; if(frame >= 138) frame = 137;
    if(undrawFrame < 0) undrawFrame = 0; if(undrawFrame >= 138) undrawFrame = 137;
    
    int xbound,ybound;
    
    int tmp;
    
    tmp = 220 + ((138 - frame) * 2); 
    while(tmp >= 360) tmp -= 360;
    
    MyUtils::orbitPoint(120,110,tmp,90,xbound,ybound);
    
    int xboundold,yboundold;
    
    tmp = 220 + ((138 - undrawFrame) * 2); 
    while(tmp >= 360) tmp -= 360;
    
    MyUtils::orbitPoint(120,110,tmp,90,xboundold,yboundold);
    
    // Creates a little buffer for the needle (imagine it pointing straight up) alse doesn't let the value go off screen (<10)
    
    xbound += xbound < 119 ? -10 : 10;
    
    if(xbound < 10) xbound = 10; 
    if(xbound > 229) xbound = 229;
    
    xboundold += xboundold < 119 ? -10 : 10;

    if(xboundold < 10) xboundold = 10; 
    if(xboundold > 229) xboundold = 229;
    
    // Do the same for the y values
    ybound += ybound < 109 ? -10 : 10;
    
    if(ybound < 0) ybound = 0; 
    if(ybound > 219) ybound = 219;
    
    yboundold += yboundold < 109 ? -10 : 10;
    
    if(yboundold < 0) yboundold = 0; 
    if(yboundold > 219) yboundold = 219;

    int xmax = max(130,max(xbound,xboundold));
    int ymax = max(120,max(ybound,yboundold)); // if(ymax == 109) ymax += 10;
    int xmin = min(110,min(xbound,xboundold));
    int ymin = min(100,min(ybound,yboundold)); // if(ymin == 109) ymax -= 10;
    
    // lcd.setColor(0xFF,0xFF,0xFF);
    // lcd.fillRect(xmin,ymin,xmax,ymax);
    
    lcd.printGci(carGagueFile,10,0,frame,xmin-10,ymin,xmax-10,ymax,false);
    
    // lcd.setColor(0xFF,0xFF,0xFF);
    // lcd.fillCircle(xbound,ybound,5);
    
    // delay(50);
    
}

// Stargate Page

void drawStargateLight(int value) {

    static int currentLight = -1;
    
    // x and y boundaries of the chevron lights in the stargate image. Format: [x left,y top,x right,y bottom]
    int chevPos[15][4] = {
        {100,  2,125, 24},
        {165, 24,189, 47},
        {196, 83,217,106},
        {185,150,207,175},
        {133,192,159,215},
        { 61,194, 88,215},
        { 13,150, 35,176},
        {  3, 81, 24,104},
        { 36, 24, 59, 45}
    };

    
    if(value != currentLight) {

        if(currentLight != -1) lcd.printBitmap(sgDarkFile,10,0,chevPos[currentLight][0],chevPos[currentLight][1],chevPos[currentLight][2],chevPos[currentLight][3],false);
        
        currentLight = value;
        
        lcd.printBitmap(sgLightFile,10,0,chevPos[currentLight][0],chevPos[currentLight][1],chevPos[currentLight][2],chevPos[currentLight][3],false);
    
    }
    
}

void updateStargate(bool onoff) {

    for(int i=0;i<9;i++) {
    
    
    }

}

// Compass Circles and orbit calculations

void partyCircles() {

    static int count = 0;
    static int spin = 0;
    static int spinB = 0;
    static int spinC = 234;
    static int spinD = 65;

    static int angleA = 0;
    static int angleB = 0;
    
    // Undraw last circles
    // drawCompass(120,110,angleA,0);
    drawpointer(110,110,90,spinB,0);
    drawpointer(110,110,90,spinC,0);
    drawpointer(110,110,90,spinD,0);
    drawpointer(110,110,90,spin,0);
    
    // Increment pointers
    spin+=5; if(spin>=360) spin-=360;
    spinB-=2; if(spinB<0) spinB=359;
    spinC+=4; if(spinC>=360) spinC=0;
    spinD-=6; if(spinD<0) spinD=359;
    // angleA-=10; if(angleA<0) angleA += 360;
    
    // lcd.setColor(random(255), random(255), random(255));
    
    // Build number string for one of the pointers
    // char tempString[20];
    // inttostring(spin,tempString,0,3);
    // tempString[3]=0;
    
    // Build number string for one of the pointers
    // char tempString2[20];
    // inttostring(angleA,tempString2,0,3);
    // tempString2[3]=0;
    
    // Print number string to the screen
    // lcd.print(tempString,70,60);
    
    // Print number string to the screen
    // lcd.print(tempString2,70,110);
    
    // Draw new circles
    // drawCompass(120,110,angleA,1);
    drawpointer(110,110,90,spin,1);
    drawpointer(110,110,90,spinB,1);
    drawpointer(110,110,90,spinC,1); 
    drawpointer(110,110,90,spinD,1);
    
    
    // Don't loop too fast
    delay(7);


}

void drawpointer(int centerx,int centery,int radius,int angle,int color) {

    // if(color) { lcd.setColor(random(255), random(255), random(255)); } 
    if(color) { lcd.setColor(255, 255, 255); }
    else { lcd.setColor(0, 0, 0); }
    
    angle-=180;

    int xoffset = sin(MyUtils::degreestoradians(angle))*radius;
    int yoffset = cos(MyUtils::degreestoradians(angle))*radius;

    if(color) { lcd.fillCircle(centerx-xoffset,centery+yoffset,5); }
    // else { lcd.printBitmap(carFile,10,0,centerx-xoffset-6,centery+yoffset-6,centerx-xoffset+6,centery+yoffset+6,false); } 
    else { lcd.fillCircle(centerx-xoffset,centery+yoffset,6); }
    
}

void drawCompass(int x,int y,int angle,int enabled) {

    int polygon[10][2];
    
    MyUtils::orbitPoint(x,y,angle,       100, polygon[0][0],polygon[0][1]);
    MyUtils::orbitPoint(x,y,angle + 90,  30,  polygon[1][0],polygon[1][1]);
    MyUtils::orbitPoint(x,y,angle + 180, 100, polygon[2][0],polygon[2][1]);
    MyUtils::orbitPoint(x,y,angle - 90,  30,  polygon[3][0],polygon[3][1]);
    
    // if(enabled) { lcd.setColor(random(255), random(255), random(255)); } else { lcd.setColor(0,0,0); } 
    if(enabled) { lcd.setColor(255,255,255); } else { lcd.setColor(0,0,0); }
    for(int i=0;i<4;i++) lcd.drawLine(polygon[i][0],polygon[i][1],polygon[(i+1)%4][0],polygon[(i+1)%4][1]);
    
    // lcd.drawLine(polygon[0][0],polygon[0][1],polygon[1][0],polygon[1][1]);

}

// Bluetooth Functions

void btStrt() {

    // if(D) db << pstr("btStrt") << endl;

}

void btAvailibleDevice(long long address,int linkIndex,char * name) {

    if(D) USB.printf("btAvailibleDevice %d %s %012llX\r\n",linkIndex,name,address);

    // if(address == (long long)0x3017C88BA738) {
    
        // btooth.connect(address);
    
    // }


}

void btConnected(long long address) {

    // if(D) db << pstr("btConnected") << endl;

}

void btDisconnected(long long address) {

    // if(D) db << pstr("btDisconnected") << endl;

}

// Other

void flipBitmap(char *filename) {

    // Realized after I built this that an image can be flipped much more easily in gimp

    if(D) Serial.printf("flipBitmap %s\r\n",filename);

    /*
    Turns this:   Into this:
    p o n m       a e i m
    l k j i       b f j n 
    h g f e       c g k o 
    d c b a       d h l p 
    */
    
    // Takes a bitmap that is filled row by row and fills it column by column for faster vertical screen writes [trying to minimize seek curTime on the sdcard]

    SdFile tmpFileSrc,tmpFileDest;
    
    // Verify that this is a bitmap file
    if(MyUtils::strMatch(filename,".bmp") == -1) { if(D) Serial.println("No BMP extension"); return; }
    
    // Open the bitmap to be flipped
    if(!tmpFileSrc.open(filename,O_READ)) if(D) Serial.println("File open fail");
    
    // Verify that this file has a bitmap header
    
    // Read the header for the bitmap that needs to be flipped
    
    int imageStart;
    
    tmpFileSrc.seekSet(10);
    
    // The location where useful image data starts
    imageStart  = tmpFileSrc.read() + (tmpFileSrc.read() << 8) + (tmpFileSrc.read() << 16) + (tmpFileSrc.read() << 24);
    
    if(D) Serial.printf("imageStart %d\r\n",imageStart);
    
    int imageWidth,imageHeight;
    
    tmpFileSrc.seekSet(18);
    
    // Width
    imageWidth = tmpFileSrc.read() + (tmpFileSrc.read() << 8) + (tmpFileSrc.read() << 16) + (tmpFileSrc.read() << 24);
    
    // Height
    imageHeight = tmpFileSrc.read() + (tmpFileSrc.read() << 8) + (tmpFileSrc.read() << 16) + (tmpFileSrc.read() << 24);
    
    if(D) Serial.printf("imageWidth %d imageHeight %d\r\n",imageWidth,imageHeight);
    
    int dataBits;
    
    tmpFileSrc.seekSet(28);
    
    // Bits of data in a single image pixel
    dataBits  = ((tmpFileSrc.read()      ) & 0x00FF);
    dataBits += ((tmpFileSrc.read() <<  8) & 0xFF00);
    
    int dataBytes = dataBits/8; // works for 16 and 24 bit images
    
    // Create a new file on the sdcard with a bm2 extension
    char newFilename[15]; for(int i=0;i<12;i++) { newFilename[i] = filename[i]; if(filename[i]==0) break; }
    
    newFilename[MyUtils::strMatch(filename,".bmp")+3] = 'b';
    
    if(D) Serial.printf("new filename is %s\r\n",newFilename);
    
    if(sd.exists(newFilename)) sd.remove(newFilename);
    
    if(!tmpFileDest.open(newFilename,O_RDWR | O_CREAT)) if(D) Serial.println("File open fail");
    
    // Copy all of the header data into the new file just for functions
    tmpFileDest.seekSet(0); tmpFileSrc.seekSet(0);
    for(int i=0;i<imageStart;i++) tmpFileDest.write(tmpFileSrc.read());
    
    if(D) Serial.println("header written");
    
    // Change a data field in the bitmap header somewhere to indicate the flipped format
    
    // Seek to the start of the destination file, should already be here but this makes sure
    tmpFileDest.seekSet(imageStart);
    
    int const COL_TO_BUFFER = 100;
    
    byte buffer[COL_TO_BUFFER*imageHeight*dataBytes];
    
    // Iterate through the columns of the image
    for(int col=0;col<imageWidth;col+=COL_TO_BUFFER) {
        
        int total = COL_TO_BUFFER;
        if(col + (total-1) > imageWidth) total = imageWidth - col;
    
        if(D) Serial.printf("col %d\r\n",col);
    
        // Iterate through all the rows in a column and write them to the new file in the new order
        for(int row=0;row<imageHeight;row++) {
        
            tmpFileSrc.seekSet(imageStart + ((imageHeight - 1 - row) * imageWidth*dataBytes) + (imageWidth - 1 - col)*dataBytes);
            
            for(int i=total-1;i>=0;i--) {
            
                buffer[i] = tmpFileSrc.read();
                buffer[i] = tmpFileSrc.read();
            
            }
    
        }
        
        for(int i=0;i<total;i++) {
        
            for(int row=0;row<imageHeight;row++) {
        
                tmpFileDest.write(buffer[ (row*total*dataBytes) + (i*dataBytes) ]);
                tmpFileDest.write(buffer[ (row*total*dataBytes) + (i*dataBytes) + 1 ]);

            }
            
        }
        
    }
    
    // Close files
    tmpFileDest.close();
    tmpFileSrc.close();
    
    if(D) Serial.println("images closed... done");
    
}

void calculateCircle() {

    static int curve[300][2];

    for(int i=0;i<300;i++) for(int k=0;k<2;k++) curve[i][k] = -1;

    int x[300],y[300];
    
    int angle = 0;
    
    lcd.setColor(255, 255, 255);
    
    int tmpx,tmpy;
    
    // Get values for 360 degrees around the circle
    for(int i=0;i<360;i++) {
    
        // Get an x and a y from the sin and cos functions
        tmpx = (sin(MyUtils::degreestoradians(i))*100)+120;
        tmpy = (cos(MyUtils::degreestoradians(i))*100)+110;
        
        // if(D) db << pstr("X: ") << dec << tmpx << pstr(" Y: ") << dec << tmpy << endl;
        
        // Storing a high and a low value of x for every y value
        if(curve[tmpy][0] == -1) {
        
            curve[tmpy][0] = tmpx;
            
        } else if(curve[tmpy][1] == -1) {
        
            curve[tmpy][1] = tmpx;
            
            // Swap the values of the high and low if they are backwards
            if(curve[tmpy][0] > curve[tmpy][1]) {
            
                int temp = curve[tmpy][0];
                curve[tmpy][0] = curve[tmpy][1];
                curve[tmpy][1] = temp;
            
            }
        
        }
        
        //lcd.drawPixel(120+x[i],110+y[i]);
        
    }
    
    curve[0][1] = 120;
    curve[0][1] = 121;
    
    for(int i=0;i<220;i++) {
    
        bool continueflg = false;
        
        for(int k=0;k<2;k++) {
            if(curve[i][k] == 0 || curve[i][k] == -1) {
            
                if(curve[i-1][k] == 0 || curve[i-1][k] == -1) {
                
                    if(continueflg == false) {
                        i-=2;
                        continueflg = true;
                    }
                
                } else {
                
                    curve[i][k] = curve[i-1][k];
                
                }
            
            }
        }
        if(continueflg) continue;
    
        lcd.drawPixel(curve[i][0],i);
        lcd.drawPixel(curve[i][1],i);
        
        // if(D) db << pstr("X: ") << dec << curve[i][0] << pstr(" X: ") << dec << curve[i][1] << pstr(" Y: ") << dec << i << endl;
    
    }

}

int analogReadR(int index){

    switch(index){
        case  0: return analogRead( A0); break;
        case  1: return analogRead( A1); break;
        case  2: return analogRead( A2); break;
        case  3: return analogRead( A3); break;
        case  4: return analogRead( A4); break;
        case  5: return analogRead( A5); break;
        case  6: return analogRead( A6); break;
        case  7: return analogRead( A7); break;
        case  8: return analogRead( A8); break;
        case  9: return analogRead( A9); break;
        case 10: return analogRead(A10); break;
        case 11: return analogRead(A11); break;
        case 12: return analogRead(A12); break;
        case 13: return analogRead(A13); break;
        case 14: return analogRead(A14); break;
        case 15: return analogRead(A15); break;
        case 16: return analogRead(A16); break;
        case 17: return analogRead(A17); break;
        case 18: return analogRead(A18); break;
        case 19: return analogRead(A19); break;
        case 20: return analogRead(A20); break;
    }

    return -1;

}

int getDayOfTheWeek(int day,int month,int year) {

    // 1-based day, 0-based month, year since 1900
    std::tm time_in = { 0, 0, 0, day, month, year - 1900 }; 

    std::time_t time_temp = std::mktime( & time_in );

    std::tm const *time_out = std::localtime( & time_temp );

    return time_out->tm_wday;

}


// Debug

int lastUsbCommandSentTime = 0;
bool saveD = false,saveE = false;
bool debugHold = false;

int const MAX_DEBUG_COMMANDS = 5;
int const MAX_DEBUG_COMMAND_LENGTH = 30;

char const debugCommandStrings[MAX_DEBUG_COMMANDS][MAX_DEBUG_COMMAND_LENGTH] = {

"help",
"d",
"pbook",
"resetbt",
"getconfigbt"

};

int const DEBUG_MSG_HELP        = 0;
int const DEBUG_MSG_DEBUG        = 1;
int const DEBUG_MSG_PBOOK        = 2;
int const DEBUG_MSG_RESET_BT    = 3;
int const DEBUG_MSG_CONFIG_BT    = 4;

bool debugLocked = false;

void usbCommand() {

    int inChar = Serial.read();
    
    if(debugHold) {

        if(millis() > (lastUsbCommandSentTime+1500)) {
        
            debugHold = false;
            D = saveD; E = saveE;
        
        }
    
    }
    
    if(inChar != -1) newDebugChar(inChar);
    
}

void newDebugChar(char tmpChar) {

    int const USB_MESSAGE_BUFFER_SIZE = 100;
    int static msgLength = -1;
    char static msgString[USB_MESSAGE_BUFFER_SIZE];
    
    if(debugLocked) {
    
        char password[] = "zer0koolaid";
        
        if(msgLength == -1 && tmpChar == password[0]) {
    
            msgLength = 1;
        
        } else if(tmpChar == password[msgLength]) {
        
            msgLength++;
            
            // End of password
            if(password[msgLength] == 0) {
                
                debugLocked = false;
                
                msgLength = -1;
            
            }
            
        } else { msgLength = -1; }
    
    } else if(tmpChar == '\r') {
        
        if(msgLength>0) usbMessage(msgString,msgLength);
        msgLength = -1;
        
        USB.println(">");
    
    } else if(tmpChar != '\n') {
    
        if(msgLength == -1) msgLength = 0;
        
        msgString[msgLength] = tmpChar;
        
        USB.write(tmpChar);
        
        msgLength++;
        
        msgString[msgLength] = 0;
        
        if(msgLength == USB_MESSAGE_BUFFER_SIZE) {
            usbMessage(msgString,msgLength);
            msgLength = -1;
        }
    
    }

}

void usbMessage(char *msgStr,int msgLen) {

    int msgID = -1;

    for(int count=0;count<MAX_DEBUG_COMMANDS;count++) { if(MyUtils::strMatchC(msgStr,debugCommandStrings[count])==0 && msgLen==MyUtils::strLengthC(debugCommandStrings[count])) { msgID = count;  break; } }

    //usb << pstr("msgID ") << dec << msgID << endl;
    
    USB.println();
    
    if(msgID == DEBUG_MSG_HELP) {
    
        for(int i=0;i<MAX_DEBUG_COMMANDS;i++) USB.printf("%03d %s\r\n",i+1,debugCommandStrings[i]);
        
    } else if(msgID == DEBUG_MSG_DEBUG) {
    
        if(debugHold) {

            saveD = !saveD;
            
            USB.printf("Debug %s\r\n",saveD ? "ON" : "OFF");
            
        } else {

            D = !D;
            
            USB.printf("Debug %s\r\n",D ? "ON" : "OFF");
            
        }
        
    } else if(msgID == DEBUG_MSG_PBOOK) {
    
        // btooth.getPbook();
        
    } else if(msgID == DEBUG_MSG_RESET_BT) {
    
        // btooth.reset();
        
    } else if(msgID == DEBUG_MSG_CONFIG_BT) {
    
        // btooth.getAllConfig();
    
    }
    
    lastUsbCommandSentTime = millis();
    if(debugHold == false) {
        saveD = D; saveE = E;
        D = false; //E = false;
        debugHold = true;
    }
    
    
    
    

}


// Error

void err(char * msgStr) {

    errD(msgStr,false);

}

void err(char * msgStr,int arga) {

    errD(msgStr,arga,false);

}

void err(char * msgStr,int arga,int argb) {

    errD(msgStr,arga,argb,false);

}

void err(char * msgStr,int arga,int argb,int argc) {

    errD(msgStr,arga,argb,argc,false);

}


void errH(char * msgStr) {

    errD(msgStr,true);

}

void errH(char * msgStr,int arga) {

    errD(msgStr,arga,true);

}

void errH(char * msgStr,int arga,int argb) {

    errD(msgStr,arga,argb,true);

}

void errH(char * msgStr,int arga,int argb,int argc) {

    errD(msgStr,arga,argb,argc,true);

}


void err(char * msgStr,char * arga) {

    errS(msgStr,arga,false);

}

void err(char * msgStr,char * arga,char * argb) {

    errS(msgStr,arga,argb,false);

}

void err(char * msgStr,char * arga,char * argb,char * argc) {

    errS(msgStr,arga,argb,argc,false);

}


void errH(char * msgStr,char * arga) {

    errS(msgStr,arga,true);

}

void errH(char * msgStr,char * arga,char * argb) {

    errS(msgStr,arga,argb,true);

}

void errH(char * msgStr,char * arga,char * argb,char * argc) {

    errS(msgStr,arga,argb,argc,true);

}

// Main error functions for numbers

void errD(char * msgStr,bool hard) {

    if(E) db.println(msgStr);
    
    if(H && hard) {
    
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.println(msgStr);
        }
    
    }

}

void errD(char * msgStr,int arga,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga); 
        db.println(); 
    
    }
    
    if(H && hard) {
        
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga);
            db.println();
        }
    
    }

}

void errD(char * msgStr,int arga,int argb,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga,argb); 
        db.println(); 
    
    }
    
    if(H && hard) {
    
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga,argb);
            db.println();
        }
    
    }

}

void errD(char * msgStr,int arga,int argb,int argc,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga,argb,argc); 
        db.println(); 
        
    }
    
    if(H && hard) {
    
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga,argb,argc);
            db.println();
        }
    
    }

}


// Main error functions for strings

void errS(char * msgStr,char * arga,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga); 
        db.println(); 
    
    }
    
    if(H && hard) {
        
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga);
            db.println();
        }
    
    }

}

void errS(char * msgStr,char * arga,char * argb,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga,argb); 
        db.println(); 
    
    }
    
    if(H && hard) {
    
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga,argb);
            db.println();
        }
    
    }

}

void errS(char * msgStr,char * arga,char * argb,char * argc,bool hard) {

    if(E) { 
    
        db.print("ERROR: ");
        db.printf(msgStr,arga,argb,argc); 
        db.println(); 
        
    }
    
    if(H && hard) {
    
        while(true) {
            delay(500);
            db.print("ERROR: ");
            db.printf(msgStr,arga,argb,argc);
            db.println();
        }
    
    }

}


void setupOld() {

    // curTime.day = 29;
    // curTime.month = 10;
    // curTime.year = 2014;
    
    // digitalWrite(PIN::LCD_BACKLIGHT, HIGH);
    
    // testb();
    
    // while(true);
    
    // int day = getDayOfTheWeek(29,9,2014);
    
    // USB.printf("Day of the week %d\r\n",day);
    
    // for(int i=5;i<=360;i+=10) test(1000,1000,i);
    
    // while(true);
    
    // Wait for a start character before running this program
    // if(D) { static int tmpTime = millis() + 1000; while(Serial.read() == -1) { if(millis() > tmpTime) { db << pstr("Waiting on start command...") << endl; tmpTime = millis() + 1000; } } }
    
    // while(true) {
    // 
    //     COM.println("RESET");
    //     delay(2000);+
    // 
    // }
    
    // showSplash();
    
    // lcd.clrScr();
    
    // watch.rampBrightness(UP);
    
    // SdFile tmpFile;
    
    // if(!tmpFile.open("PiSplash.bmp",O_RDWR)) USB.println("File open fail");
    
    // lcd.printBitmap(tmpFile,0,0);
    
    // while(true);
 
    // SdFile A,B;
    // 
    // if(!A.open("needleA.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // if(!B.open("xkcdnowb.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // 
    // delay(750);
    // 
    // watch.rampBrightness(DOWN);
    // 
    // lcd.printGciTransparent(0x001F,A,0,0,45,B,0,0,0); 
    // 
    // watch.rampBrightness(UP);
    // 
    // int count = 0;
    // while(true) {
    //     
    //     count+=15;
    //     if(count >= 360) count -= 360;
    // 
    //     // lcd.printGci(A,0,0,count); 
    //     lcd.printGciTransparent(0x001F,A,0,0,count,B,0,0,0); 
    //     // lcd.printGciTransparent(0x001F,A,0,0,count,B,0,0,359 - count); 
    //     
    // 
    // 
    // }

    // SdFile video,video2;
    
    // if(!video.open("shipVid.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    
    // while(true) lcd.playGci(video,0,0,0);
    
    
    // // if(!video.open("test.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // if(!video.open("xkcdnow.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // if(!video2.open("xkcdnowb.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // // if(!video2.open("xkcdnowi.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    // 
    
    // analogWrite(PIN::LCD_BACKLIGHT, 50);
    
    
    // lcd.loadVideo(video2,0,0);
    // 
    // int count = 0;
    // while(true) {
    //     
    //     count+=10;
    //     if(count > 359) count -= 360;
    //     lcd.videoFrame(count);
    //     // lcd.playGci(video2,0,0,0);
    //     
    //     // delay(5000);
    //     
    //     // lcd.playGci(video,0,0,0);
    //     
    //     // delay(5000);
    // 
    // }
    
    
    // for(int i=0;i<1236;i++) {
    // 
    //     int timeA = micros();
    // 
    //     lcd.printGci(video,0,0,i);
    //  
    //     int timeB = micros();
    //     
    //     if(D) USB.printf("fps %05.3f\r\n",(double)1000000/(timeB-timeA));
    //     // if(D) USB.printf("curTime %05d\r\n",timeB-timeA);
    //     
    // }
    /*
    
    SdFile back;
    
    if(!back.open("blue.bmp",O_RDWR)) if(D) USB.println("Couldn't open video");
    
    lcd.printBitmap(back,0,0);
    
    SdFile numbers;
    
    if(!numbers.open("tst.Gci",O_RDWR)) if(D) USB.println("Couldn't open video");
    
    while(true) {
    
        lcd.printGci(numbers,32,77,1);
        lcd.printGci(numbers,64,77,1 * 11 + 1);
        // lcd.printGci(numbers,32,77,9);
        
        delay(500);
        
        // lcd.printBitmap(back,0,0);
        lcd.printGci(numbers,32,77,10);
        lcd.printGci(numbers,64,77,1 * 11 + 10);
        
        delay(500);
        
        // for(int x=0;x<11;x++) {
        
            // lcd.printGci(numbers,32,77,x);
            
            // delay(500);
        
        // }
    }
    */
    // pageArray[PAGE::HOME] = new HomePage(&btooth,&lcd);
    
    // if(D) USB.printf("freeMemory %d\r\n",mallinfo().uordblks);
    
    
    // if(!backgroundImageFile.open("numpad.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
    
    // SdFile backgroundImageFile;
    // 
    // if(!backgroundImageFile.open("numpad.bmp",O_RDWR)) {
    // 
    //     Serial.println("File open fail");
    //     
    // } else {
    // 
    //     Serial.println("Background file is opened");
    // 
    // }
    
    // myHomePageTest.initalize();
    // pageArray[page]->initalize(backgroundImageFile);
    
    
    // if(!backgroundImageFile.open("testTem.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;

//     
//     
    // if(!backgroundImageFile.open("car2.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!transparentStarBitmapFile.open("tstar.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!transparentGradientFile.open("gradient.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     
//     if(!carGagueFile.open("needles.gci",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!carGagueInvFile.open("needlesR.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     
//     // printGci(carGagueFile,10,0);
//     
//     if(!wheelFile.open("wheelB.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!wheelInvFile.open("wheelR.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     // lcd.printBitmap(wheelFile,10,0);
//     
//     if(!carFile.open("car2.bmp",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!carInvFile.open("car2.bmf",O_RDWR)) if(D) db << pstr("File open fail") << endl;
//     if(!sgDarkFile.open("sgdark.bmp",O_READ)) if(D) db << pstr("File open fail") << endl;
//     if(!sgLightFile.open("sglight.bmp",O_READ)) if(D) db << pstr("File open fail") << endl;
//     if(!sgLightInvFile.open("sglightR.bmp",O_READ)) if(D) db << pstr("File open fail") << endl;
//     
//     if(D) db << pstr("Files open") << endl;
//     
//     
//     
    // lcd.printBitmapBackGround(backgroundImageFile,10,0);
    // lcd.printBitmap(backgroundImageFile,10,0);
    // lcd.printBitmap(sgLightFile,10,0);
//     lcd.printBitmap(transparentStarBitmapFile,50,40);
//     lcd.printBitmap(transparentGradientFile,50,120);
    
    // while(1);
    
    // while(1);
    
    // timeA = micros()-timeA;
    // if(D) db << pstr("Time: ") << dec << timeA << endl;
    
    // delay(1000);
    
    // int chevC=0;
    
    // while(1) {

        // lcd.printBitmap(imageFile,10,0,chevPos[chevC][0],chevPos[chevC][1],chevPos[chevC][2],chevPos[chevC][3]);
        
        // chevC++; if(chevC>=9) chevC=0;
        
        // lcd.printBitmap(imageFileB,10,0,chevPos[chevC][0],chevPos[chevC][1],chevPos[chevC][2],chevPos[chevC][3]);
        
        // delay(111);

    // }
    
    // if(D) db << pstr("top to bottom") << endl;
    
    // for(int i=0;i<220;i+=4) { lcd.printBitmap(imageFile,10,0,0,i,220,i+3); delay(15); }
    
}
