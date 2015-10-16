
IntervalTimer blinktimer;

#include <EEPROM.h>

void setup() {

    // set the Time library to use Teensy 3.0's RTC to keep time
    setSyncProvider((long int (*)())Teensy3Clock.get);

    // This needs to be one of the first things done because it sets
    // pin states and keeps the watch on
    // watch.init();
    watch.init(touch);
    watch.speaker(OFF);

    USB.begin(300000); 
    // while(!USB); // Wait for PC to open the USB serial port before running this program
    delay(100);
    
    // elapsedMillis time; while(1) { if(time > 500) { Serial.println("Send char to start"); time = 0; } if(Serial.read() != -1) break; }
    
    if (timeStatus() != timeSet) {
        if(D) db.println("Unable to sync with the RTC"); 
    } else {
        if(D) db.println("RTC has set the system time");
    }
    
    loadCompileTime();
    
    Wire.begin(I2C_MASTER,0,0,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_400);
    // Wire.begin(I2C_MASTER,0,0,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_800);
    
    // setupCompass();
    compass.init(LSM303C_DEVICE);
    
    // Serial2.begin(38400);
    bt.init();
    // bt.factoryReset();
    // bt.discoverable(ON);
    // bt.getConfig(BC127::BAUD);
    // bt.power(OFF);
    
    // watch.InitLCD(currentRotation);
    watch.setOrientation(currentRotation);
    
    loadSdCard();
    
    #define initPage(name,class) pageArray[PAGE::name] = new class(ARGS_MACRO);
    
    //       Page number        Class name
    initPage(HOME,              HomePage)
    initPage(GRAVITY_BALL,      GravityBallPage)
    initPage(CONTROL_SIMPLE,    ControlSimplePage)
    initPage(NUMBER_PAD,        NumberPadPage)
    initPage(PHONE,             PhonePage)
    initPage(CALL,              CallPage)
    initPage(TOUCH_DEMO,        TouchDemoPage)
    initPage(CONTACTS,          ContactsPage)
    initPage(BLUE_CLOCK,        BlueClockPage)
    initPage(XKCD_CLOCK,        XKCDclockPage)
    initPage(BAR_CLOCK,         BarClockPage)
    initPage(CLOCKS,            ClockSelectPage)
    initPage(VIDEO,             VideoPlayerPage)
    initPage(APPS,              AppsPage)
    initPage(SETTINGS,          SettingsPage)
    initPage(SETTINGS_B,        SettingsPageB)
    initPage(BLUISH_CLOCK,      BluishClockPage)
    initPage(BREIGHTLING_CLOCK, BreightlingClockPage)
    initPage(KICKSTARTER_CLOCK, KickstarterClockPage)
    initPage(RADIAN_CLOCK,      RadianClockPage)
    initPage(KICKSTARTER_DEMO,  KickstarterDemoPage)
    initPage(TEXT_ENTRY,        TextEntryPage)
    initPage(BATTERY_GRAPH,     BatteryGraphPage)
    initPage(LED_RING_CONTROL,  ledRingControlPage)
    initPage(BLACK_CLOCK,       BlackClockPage)
    initPage(STARGATE,          StargatePage)
    initPage(TARGET_BALL,       TargetBallPage)
    
    fillDemoContacts();
    
    // analogReference(INTERNAL);
    analogReadResolution(8);
    analogWriteResolution(8);
    
    // showSplash();
    
    // Let all of the pages go through their bootup
    for(int i=0;i<PAGE::TOTAL;i++) pageArray[i]->bootup();
    
    // goPage(PAGE::TEXT_ENTRY);
    // goPage(PAGE::VIDEO);
    // goPage(PAGE::TOUCH_DEMO);
    // goPage(PAGE::KICKSTARTER_DEMO);
    // goPage(PAGE::KICKSTARTER_CLOCK);
    // goPage(PAGE::BLUE_CLOCK);
    goPage(PAGE::HOME);
    // goPage(PAGE::SETTINGS);
    // goPage(PAGE::BATTERY_GRAPH);
    // goPage(PAGE::LED_RING_CONTROL);
    // goPage(PAGE::BLACK_CLOCK);
    // goPage(PAGE::STARGATE);
    // goPage(PAGE::CONTACTS);

    // Turn up the brightness
    watch.rampBrightness(100,500);
    
    if(D) USB.println("## LOOP START ##");
    
}

void loop() {
    
    // static elapsedMillis timey;
    // if(timey > 10000) {
    // 
    //     timey = 0;
    // 
    //     bt.connect(0x20FABB01862E);
    //     // bt.connect(0x20FABB018005);
    // 
    // }
    
    // lowPowerTimeout();

    static elapsedMillis_LP timeB;

    if(timeB > 200)  {

        timeB = 0;

        watch.setFont(BigFont);
        watch.setColor(WHITE);
        watch.setBackColor(BLACK);

        watch.printNumI(analogRead(PIN::POWER_BUTTON),CENTER,100,10,'0');
        watch.printNumI(touchRead(0),CENTER,115,10,'0');

    }
    
    watch.loop();
    
    bt.loop();
    
    pageArray[page]->loop();
    
    fori(PAGE::TOTAL) pageArray[i]->serviceLoop();
    
    checkPhoneState();

    // Trying to shut down with a little left in the battery so that the RTC keeps on ticking
    if(bt.getBatteryVoltage() < 3200 && bt.getBatteryVoltage() != -1) {
    
        watch.rampBrightnessWait(0,50);
        
        watch.clrScr();
        watch.setFont(BigFont);
        watch.setColor(WHITE);
        watch.setColor(BLACK);
        watch.print("Low Battery",CENTER,90);
        watch.print("Shutting Down",CENTER,120);
        
        watch.rampBrightnessWait(100,50);
        delay(500);
        watch.rampBrightnessWait(0,1000);
    
        watch.powerDown();
    
    }
    
    if(Page::lowPower) {
    
    } else {
        
        audioLoop();
        
    }

    checkOrientation();

    // This needs to be last since it can change the low power state
    pollButtons();

}

void _init_Teensyduino_internal_LP(void) {

    int DEFAULT_FTM_MOD;
    int DEFAULT_FTM_PRESCALE;

    if(LP._cpu == 60000000) {
        DEFAULT_FTM_MOD = (61440 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    } else if(LP._cpu == 56000000) {
        DEFAULT_FTM_MOD = (57344 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    } else if(LP._cpu == 48000000) {
        DEFAULT_FTM_MOD = (49152 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    } else if(LP._cpu == 40000000) {
        DEFAULT_FTM_MOD = (40960 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    } else if(LP._cpu == 36000000) {
        DEFAULT_FTM_MOD = (36864 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    } else if(LP._cpu == 24000000) {
        DEFAULT_FTM_MOD = (49152 - 1);
        DEFAULT_FTM_PRESCALE = 0;
    } else if(LP._cpu == 16000000) {
        DEFAULT_FTM_MOD = (32768 - 1);
        DEFAULT_FTM_PRESCALE = 0;
    } else if(LP._cpu == 8000000) {
        DEFAULT_FTM_MOD = (16384 - 1);
        DEFAULT_FTM_PRESCALE = 0;
    } else if(LP._cpu == 4000000) {
        DEFAULT_FTM_MOD = (8192 - 1);
        DEFAULT_FTM_PRESCALE = 0;
    } else if(LP._cpu == 2000000) {
        DEFAULT_FTM_MOD = (4096 - 1);
        DEFAULT_FTM_PRESCALE = 0;
    } else {
        DEFAULT_FTM_MOD = (49152 - 1);
        DEFAULT_FTM_PRESCALE = 1;
    }


	// init_pin_interrupts();

	//SIM_SCGC6 |= SIM_SCGC6_FTM0;	// TODO: use bitband for atomic read-mod-write
	//SIM_SCGC6 |= SIM_SCGC6_FTM1;
	FTM0_CNT = 0;
	FTM0_MOD = DEFAULT_FTM_MOD;
	FTM0_C0SC = 0x28; // MSnB:MSnA = 10, ELSnB:ELSnA = 10
	FTM0_C1SC = 0x28;
	FTM0_C2SC = 0x28;
	FTM0_C3SC = 0x28;
	FTM0_C4SC = 0x28;
	FTM0_C5SC = 0x28;
	FTM0_C6SC = 0x28;
	FTM0_C7SC = 0x28;
	FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
	FTM1_CNT = 0;
	FTM1_MOD = DEFAULT_FTM_MOD;
	FTM1_C0SC = 0x28;
	FTM1_C1SC = 0x28;
	FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
    #if defined(__MK20DX256__)
    	FTM2_CNT = 0;
    	FTM2_MOD = DEFAULT_FTM_MOD;
    	FTM2_C0SC = 0x28;
    	FTM2_C1SC = 0x28;
    	FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
    #endif

	analog_init();
	//delay(100); // TODO: this is not necessary, right?
	delay(4);
	usb_init();
    
}

void analogWriteFrequency_LP(uint8_t pin, uint32_t frequency)
{
	uint32_t minfreq, prescale, mod;

	//serial_print("analogWriteFrequency: pin = ");
	//serial_phex(pin);
	//serial_print(", freq = ");
	//serial_phex32(frequency);
	//serial_print("\n");
	for (prescale = 0; prescale < 7; prescale++) {
		minfreq = (LP._cpu >> 16) >> prescale;
		if (frequency > minfreq) break;
	}
	//serial_print("F_BUS = ");
	//serial_phex32(F_BUS >> prescale);
	//serial_print("\n");
	//serial_print("prescale = ");
	//serial_phex(prescale);
	//serial_print("\n");
	//mod = ((F_BUS >> prescale) / frequency) - 1;
	mod = (((LP._bus >> prescale) + (frequency >> 1)) / frequency) - 1;
	if (mod > 65535) mod = 65535;
	//serial_print("mod = ");
	//serial_phex32(mod);
	//serial_print("\n");
	if (pin == 3 || pin == 4) {
		FTM1_SC = 0;
		FTM1_CNT = 0;
		FTM1_MOD = mod;
		FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(prescale);
	} else if (pin == 5 || pin == 6 || pin == 9 || pin == 10 ||
	  (pin >= 20 && pin <= 23)) {
		FTM0_SC = 0;
		FTM0_CNT = 0;
		FTM0_MOD = mod;
		FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(prescale);
	}
#if defined(__MK20DX256__)
	  else if (pin == 25 || pin == 32) {
		FTM2_SC = 0;
		FTM2_CNT = 0;
		FTM2_MOD = mod;
		FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(prescale);
	}
#endif
}


// Polling routines

void checkOrientation() {
    
    static elapsedMillis mytime;
    if(mytime > 10) {
    
        mytime = 0;
        
        compass.read();
        
        int lastRotation = currentRotation;
    
        // old
        // else if(compass.a.x >  1000) currentRotation = PORTRAIT;    
        // else if(compass.a.x < -1000) currentRotation = PORTRAIT_R;  
        // else if(compass.a.y >  1000) currentRotation = LANDSCAPE;   
        // if(     compass.a.y < -1000) currentRotation = LANDSCAPE_R; 
    
        // new
        if(     compass.a.y < -1000) currentRotation = PORTRAIT;
        else if(compass.a.y >  1000) currentRotation = PORTRAIT_R;
        else if(compass.a.x < -1000) currentRotation = LANDSCAPE;
        else if(compass.a.x >  1000) currentRotation = LANDSCAPE_R;
        
        if(lastRotation != currentRotation) {
        
            bool lpdance = false;
            if(Page::lowPower) lpdance = true;
            
            if(lpdance) lowPowerDisable(false);
        
            watch.setOrientation(currentRotation);
            pageArray[page]->redraw();
        
            if(lpdance) lowPowerEnable(false);
        
        }
        
    }

}

// #if 1 // Low power stuff

IntervalTimer_LP pwmTimerStart;
IntervalTimer_LP pwmTimerEnd;

volatile int lp_brightness = 100;

void lowPowerTimeout() {

    return;

    if(millis() - lastTouchTime > 5000) {

        watch.touchEnd();
        
        watch.setOrientation(3);
        pageArray[page]->redraw();

        // delay(1000);

        // bt.power(OFF);
        
        // time_t tempTime = now();
        // LP.CPU(TWO_MHZ);
        
        // Resync the clock!
        setTime(Teensy3Clock.get());

        lpSerial2.begin(9600);
        
        // delay(5000);

        // analogWrite(PIN::LCD_BACKLIGHT, 127);
        pinMode(PIN::LCD_BACKLIGHT, OUTPUT);
        digitalWrite(PIN::LCD_BACKLIGHT, LOW);

        // Brightness value for the interrupt driven PWM
        // TODO: Reconfigure real PWM for this job
        lp_brightness = (watch.getBrightness() - 5) * 20;
        
        // Start the emulated PWM
        pwmTimerStart.begin(pwmON,2000);

        // Disable all debug since the usb doesn't work at 2MHz
        int tD = D,tE = E,tH = H;
        // D = false; E = false; H = false;
        
        bool waitRelease = false;
        
        long lastBatQueryTime = millis();
        
        Page::lowPower = true;
        
        lpSerial2.println("LP LOOP");
        while(1) {

            if(millis() - lastBatQueryTime > 3000) {
            
                lastBatQueryTime = millis();
                
                char str[20];
                int strC = 0;
                
                strC += inttostring(bt.getBatteryVoltage(), str, strC, 4, '0');
                str[strC++] = ' ';
                str[strC++] = bt.batteryCharging() + '0';
                str[strC++] = ' ';
                str[strC++] = bt.powerPluggedIn() + '0';
                str[strC++] = 0;
                
                // watch.setColor(VGA_WHITE);
                // watch.setBackColor(VGA_BLACK);
                // watch.print(str, CENTER, 50);
                
                // if(D) db.printf("Battery voltage %d charging %d plugged in %d\r\n",bt.getBatteryVoltage(),bt.batteryCharging(),bt.powerPluggedIn());
            
            }
        
            // Need to reinit analog pins first
            // pollButtons();
            
            bt.loop();
    
            pageArray[page]->loop();
            
            // Looking for touch release
            // TODO: look into using the touch library for this, need to use the interrupt with the low power clock taken into account
            bool touched = false;
            for(int i=0;i<10;i++) if(touchRead(touchPin[i]) > watch.calValue[i] + 50) touched = true;
            if(touched) waitRelease = true; else if(waitRelease) goto out;

        }
        
        out:

        Page::lowPower = false;

        pwmTimerStart.end();
        pwmTimerEnd.end();

        LP.CPU(F_CPU);

        // Set debug stuff back to normal states
        D = tD; E = tE; H = tH;
        
        watch.setBrightness(watch.getBrightness());

        // digitalWriteFast(PIN::LCD_BACKLIGHT, HIGH);

        // LP.DeepSleep(TSI_WAKE, TOUCH_READ::touchPin[5], calValue[5] + 1000);

        watch.restartTouch();

        lastTouchTime = millis();

    }

}

void pwmON() {

    pwmTimerEnd.begin(pwmOFF, lp_brightness);
    
    digitalWriteFast(PIN::LCD_BACKLIGHT, HIGH);
    
}

void pwmOFF() {

    pwmTimerEnd.end();
    
    digitalWriteFast(PIN::LCD_BACKLIGHT, LOW);

}

// #endif

void checkPhoneState() {

    static int currentPhoneState = -1;
    if(currentPhoneState != bt.phoneState) {
    
        currentPhoneState = bt.phoneState;
    
        switch(currentPhoneState) {
            
            case PHONE_RINGING:   
            case PHONE_IN_CALL:    
            case PHONE_DIALING:
                goPage(PAGE::CALL);
                break;
        
        }
    
    }

}

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
    static int buttonTime[TOTAL];
    static bool longPressed[TOTAL];
    bool instantButtonState[TOTAL];
    
    int value = analogRead(PIN::POWER_BUTTON);
    
    instantButtonState[POWER_BUTTON] = value > 127;
    // instantButtonState[POWER_BUTTON] = value > 50;
    
    // static elapsedMillis printTM;
    // if(printTM > 100) {
    // 
    //     printTM = 0;
    // 
    //     if(D) USB.printf("%d\r\n",value);
    // 
    // }
    
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
            
            if(buttonStates[i] == GOING_DOWN) {
            
                buttonStates[i] = STATE_DOWN;
                
                if(D) USB.println("STATE_DOWN");
                
            } else if(buttonStates[i] == GOING_UP) {
                
                buttonTime[i] = millis();
            
                buttonStates[i] = STATE_UP;

                if(D) USB.println("STATE_UP");
                
            }
            
            // if(buttonStates[i] != instantButtonState[i]) {
            
                // buttonStates[i] = instantButtonState[i];
                
                if(!(longPressed[i] && buttonStates[i] == STATE_DOWN)) buttonEvent(buttonStates[i],i,false);
            
            // }
        
        }
        
        if(buttonStates[i] == STATE_UP && millis() - buttonTime[i] > 1000) {

            buttonEvent(buttonStates[i],i,true);
            
            longPressed[i] = true;
        
        }
    
    }
    
}

// Init

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
    
    // curTime.year = year;
    // curTime.month = month + 1;
    // curTime.day = day;
    // curTime.hour = hour;
    // curTime.minute = minute;
    // curTime.second = second;
    // curTime.hundredth = 0;

}

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

    bool onoff = true;
    
    int b = watch.getBrightness();

    retry1:
    if(sd.begin(PIN::SD_CHIP_SELECT, SPI_FULL_SPEED)) {
        
        if(D) USB.println("SD card mounted");
        
    } else {
    
        watch.rampBrightness(100);
        
        if(onoff) watch.print("No sdcard",CENTER,100); else watch.clrScr();
        
        onoff = !onoff;
        
        if(D) USB.println("No sdcard");
        
        goto retry1;
        
    }

    watch.rampBrightness(b);
    
}

void showSplash() {

    watch.printImage("hackaday.raw",0,0);

    // SdFile splashImage;
    
    // if(!splashImage.open("hackaday.raw",O_RDWR)) if(E) db.println("hackaday.raw not opened");
    // watch.printRaw(splashImage,0,0);
    
    // if(!splashImage.open("PiSplash.bmp",O_RDWR)) USB.println("PiSplash not opened");
    // watch.printBitmap(splashImage,0,0);
    
    watch.rampBrightness(100);
    
    // splashImage.close();
    
}

// Callback Routines

void bluetoothMessage(bt_event event) {

    using namespace BT;

    switch(event.id) {
        
        case RESET: break;
        case INQUIRY: break;
        case DISCOVERABLE: break;
        case LIST: break;
        case CONNECT: 
        
            if(D) db.printf("%s to %llu\r\n",event.error ? "Connect failed" : "Connected",event.mac);
        
            break;
        case STATUS: break;
        case CONFIG: break;
        case FACTORY: break;
        case WRITE: break;
        case GET_PBOOK: break;
        case NAME: break;
        case SET_ROLE: break;
        case PLAY_NOISE: break;
        case AUDIO_TRANSFER: break;
        case CALL_NUMBER: break;
        case SET_AUDIO: break;
        case GET_MIC_STATE: break;
        case SET_GAIN: break;
        case GET_VOLUME: break;
        case SET_VOLUME: break;
        case END_CALL: break;
        case REJECT_CALL: break;
        case ANSWER_CALL: break;
        case SET_POWER: break;
        case GET_CONFIG: 
        
            if(D) db.printf("GET_CONFIG returned %s with a raw value of %s\r\n",
            CFG::names[event.cfg.id],event.cfg.rawString);

            switch(event.cfg.id) {
            
                case CFG::BATT_THRESH:
                
                    // Note: lvl3 isn't being returned by the BC127
                    // It probably either needs to be set, or it doesn't
                    // exist at all there's an error here somewhere
                
                    if(D) db.printf(
                    "chrgLvl %d crit %d low %d lvl0 %d lvl1 %d lvl2 %d lvl3 %d\r\n",
                    event.cfg.chrgLvl,
                    event.cfg.crit, event.cfg.low,
                    event.cfg.lvl0, event.cfg.lvl1,
                    event.cfg.lvl2, event.cfg.lvl3);
                    
                    break;
                case CFG::BAUD:
                    
                    // Probably don't need the baud rate if we
                    // are talking over the UART already...
                
                    if(D) db.printf("baud %d\r\n",event.cfg.baud);
                    
                    break;
                    
            }
            
            
            break;
        case BATTERY: 
            
            if(D) db.printf("Battery updated plugged in %d charging %d voltage %d\r\n",bt.powerPluggedIn(),bt.batteryCharging(),bt.getBatteryVoltage());
            
            break;

    }
    

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    lastTouchTime = millis();

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
    bool topTouch = false,bottomTouch = false;
    int const touchWidth = 36;
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
            if(Page::lowPower) {
                lowPowerDisable();
                return;
            }
            break;
    
    }
    
    // Search through all of the current touches 
    // looking for two simultaneous touches in the 
    // right position to constitute a back gesture
    for(int i=0;i<10;i++) {
        
        if(lastTouchPos[i] != -1) {
        
            activatedTouches++;
            
            if(lastTouchPos[i] <= touchWidth || lastTouchPos[i] >= 360 - touchWidth) topTouch = true;
            if(lastTouchPos[i] <= 180 + touchWidth && lastTouchPos[i] >= 180 - touchWidth) bottomTouch = true;

            if(topTouch && bottomTouch /*&& pageTrailLength > 1*/ && !backActivated) {
                
                backActivated = true;
            
            }
            
        }

    }
    
    // Don't send touches if the back gesture has been noticed
    if(!backActivated) pageArray[page]->touch(touchType,finePos,activeTouches,touchIndex);

    // When all of the touches are gone and if the back was
    // activated send a back gesture now
    if(activatedTouches == 0 && backActivated) {
    
        goBack(); 
        
        backActivated = false;
    
    }                                                                                                                              
    
}

void buttonEvent(int dir,int index,bool longPress) {
    
    lastTouchTime = millis();

    if(D) USB.printf("buttonEvent dir %d index %d longPress %d\r\n", dir, index, longPress);

    switch(index) {
    
        case BUTTON::POWER_BUTTON:
        
            // pinMode(26, OUTPUT);
            // digitalWrite(26, dir);
        
            // if(D) { USB.printf("power button\r\n"); USB.flush(); }
        
            if(longPress) {
            
                lowPowerDisable(false);

                // if(D) { USB.printf("power down\r\n"); USB.flush(); }
                
                // watch.rampBrightness(DOWN);
                
                watch.setColor(VGA_WHITE);
                watch.fillRect(0,0,219,219);
                
                // watch.rampBrightnessWait(50);
                
                watch.setColor(VGA_BLACK);
                for(int i=0;i<110;i++) {
                
                    // watch.setColor(VGA_BLACK);
                    watch.drawHLine(0,i,219);
                    watch.drawHLine(0,219-i,219);
                    delayMicroseconds(1250);
                
                }
            
                // if(D) { USB.printf("brightness off\r\n"); USB.flush(); }
                
                watch.setBrightness(0);
            
                // Turn off power to the device [everything should die here]
                watch.powerDown();
                
                delay(1000);
                
                watch.setBrightness(100);
            
                // Uhh, why are we still here??
                watch.setColor(VGA_WHITE);
                watch.fillRect(0,0,219,219);
                
                // Speaking from the grave
                watch.setColor(BLACK);
                watch.setBackColor(VGA_WHITE);
                watch.print("Shutdown fail");
                
                while(1);
            
            } else if(!dir) {
            
                if(D) USB.printf("low power %s\r\n", (Page::lowPower) ? "disable" : "enable");
            
                // watch.setColor(VGA_RED);
                // watch.fillRect(0,0,219,219);
                // delay(250);
                
                if(Page::lowPower) lowPowerDisable();
                else               lowPowerEnable();
            
                // static int oldPage = PAGE::HOME;
                // 
                // if(page == PAGE::BLACK_CLOCK) {
                // 
                //     goPage(oldPage);
                // 
                //     lowPowerDisable();
                // 
                // } else {
                // 
                //     oldPage = page;
                //     goPage(PAGE::BLACK_CLOCK);
                //     
                //     lowPowerEnable();
                // 
                // }
            
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

bool goBack() {

    if(D) USB.println("goBack");

    return goPage(pageTrailRemove(),true,0,0);

}

bool goBack(int mode,char * data) {

    if(D) USB.println("goBack");

    return goPage(pageTrailRemove(),true,mode,data);

}

bool goPage(int pg) {

    return goPage(pg,false,0,0);

}

bool goPage(int pg,int mode) {

    return goPage(pg,false,mode,0);

}

bool goPage(int pg,int mode,char * data) {

    return goPage(pg,false,mode,data);

}

bool goPage(int pg,bool goingback,int mode,char * data) {

    if(D) USB.printf("goPage %s goingback %d mode %d\r\n",PAGE::names[pg],goingback,mode);
    
    if(pg >= PAGE::TOTAL || pg < 0) {
    
        if(E) db.printf("Invalid page number %d\r\n",pg);
        return false;
    
    }
    
    // No change
    if(page == pg) { if(D) USB.println("Page is the same"); return false; }
    
    if(page != -1) pageArray[page]->leavingPage();
    
    page = pg;
    
    if(!goingback) pageTrailAdd(page);
    
    int b = watch.getBrightness();
    
    watch.rampBrightnessWait(0,50);
    
    pageArray[page]->initalize(mode,data);
    
    watch.rampBrightnessWait(b,50);
    
    return true;

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



#if 0

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
    
    // watch.setColor(0xFF,0xFF,0xFF);
    // watch.fillRect(xmin,ymin,xmax,ymax);
    
    watch.printRaw(carGagueFile,10,0,frame,xmin-10,ymin,xmax-10,ymax,false);
    
    // watch.setColor(0xFF,0xFF,0xFF);
    // watch.fillCircle(xbound,ybound,5);
    
    // delay(50);
    
    
}

#endif

void drawCompass(int x,int y,int angle,int enabled) {

    int polygon[10][2];
    
    MyUtils::orbitPoint(x,y,angle,       100, polygon[0][0],polygon[0][1]);
    MyUtils::orbitPoint(x,y,angle + 90,  30,  polygon[1][0],polygon[1][1]);
    MyUtils::orbitPoint(x,y,angle + 180, 100, polygon[2][0],polygon[2][1]);
    MyUtils::orbitPoint(x,y,angle - 90,  30,  polygon[3][0],polygon[3][1]);
    
    if(enabled) { watch.setColor(255,255,255); } else { watch.setColor(0,0,0); }
    for(int i=0;i<4;i++) watch.drawLine(polygon[i][0],polygon[i][1],polygon[(i+1)%4][0],polygon[(i+1)%4][1]);
    
}

// Debug

int lastUsbCommandSentTime = 0;
bool saveD = false,saveE = false;
bool debugHold = false;

int const MAX_DEBUG_COMMANDS = 8;
int const MAX_DEBUG_COMMAND_LENGTH = 30;

char const debugCommandStrings[MAX_DEBUG_COMMANDS][MAX_DEBUG_COMMAND_LENGTH] = {

"help",
"d",
"pbook",
"resetbt",
"getconfigbt",
"calstart",
"calstop",
"calsave"

};

int const DEBUG_MSG_HELP        = 0;
int const DEBUG_MSG_DEBUG       = 1;
int const DEBUG_MSG_PBOOK       = 2;
int const DEBUG_MSG_RESET_BT    = 3;
int const DEBUG_MSG_CONFIG_BT   = 4;
int const DEBUG_MSG_CAL_START   = 5;
int const DEBUG_MSG_CAL_STOP    = 6;
int const DEBUG_MSG_CAL_SAVE    = 7;

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
    
        // bt.getPbook();
        
    } else if(msgID == DEBUG_MSG_RESET_BT) {
    
        // bt.reset();
        
    } else if(msgID == DEBUG_MSG_CONFIG_BT) {
    
        // bt.getAllConfig();
    
    } else if(msgID == DEBUG_MSG_CAL_START) {
    
        compass.startCal();
        
        calibrating = true;
    
    } else if(msgID == DEBUG_MSG_CAL_SAVE) {
    
        compass.saveCal();
    
        calibrating = false;
    
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

