
#define HARDWARE_REVA 0
#define HARDWARE_REVB 1

#include <BC127.h>

// #include <Audio.h>
#include <myUtils.h>
#include <SdFat.h>
#include <SdFatUtil.h>
// #include <Wire.h>
#include <i2c_t3.h>
#include <EEPROM.h>

#include <LSM303_custom.h>
#include "pageClass.h"
#include <customUtils.h>
#include <LowPower_Teensy3.h>

HardwareSerial_LP lp_uart = HardwareSerial_LP();

#include <Time_LP.h>

// #include <ctime>
// #define min(x,y) (x < y ? x : y)
// #define max(x,y) (x > y ? x : y)

#define fori(x) for(int i=0;i<x;i++)
#define ARGS_MACRO &bt,&compass,&watch,D,E
#define CONSTRUCTOR_MACRO (BC127* a,LSM303_custom* c,PiWatch* d2,int e,int f):Page(a,c,d2,e,f) {}

// #define ARGS_MACRO &bt,&watch,&compass,&watch,&watch,D,E
// #define CONSTRUCTOR_MACRO (BC127* a,PiScreen* b,LSM303_custom* c,PiTouch* d,PiWatch* d2,int e,int f):Page(a,b,c,d,d2,e,f) {}
    
// #include <SPI.h>
// #include <MemoryFree.h>
// #include <malloc.h>
// #include <Page.h>
// #include <HomePage.h>

#define USB Serial
#define db Serial

/* TODO:

Video player

Button for the video player

######## BUGS ########

###### FEATURES ######

Add play/pause back in

Keep audio synced when seeking 
Keep audio synced better in general

Contacts list
Call from the contacts list

RTC stuff

Setting the clock

Timer & Stopwatch

Bluetooth audio Chat

###

Create a virtual knob with a pointer and options around the perimeter of the screen.

Drag it with the touch ring, auto centers on options and maybe light them up?

Reserve the bottom as an enter button.

When you got enter switch to values around the perimeter of the knob. Examples, Bar for brightness, text for low power settings.

Use the space at the bottom that was reserved for enter as a save or back button.

Clicking sounds? Vibrator feedback?

Don't forget to take a shot at the retro dialer.

Need analog click still too.

Test new low power mode.



*/

// Declare LCD library fonts
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SmallFont[];

// Constants

#define PRESSED  0
#define MOVING   1
#define RELEASED 2

namespace PAGE {

    const uint8_t

    HOME                = 0,
    NUMBER_PAD          = 1,
    TOUCH_DEMO          = 2,
    CONTROL_SIMPLE      = 3,
    GRAVITY_BALL        = 4,
    PHONE               = 5,
    CALL                = 6,
    CONTACTS            = 7,
    BLUE_CLOCK          = 8,
    XKCD_CLOCK          = 9,
    BAR_CLOCK           = 10,
    CLOCKS              = 11,
    VIDEO               = 12,
    APPS                = 13,
    SETTINGS            = 14,
    SETTINGS_B          = 15,
    BLUISH_CLOCK        = 16,
    BREIGHTLING_CLOCK   = 17,
    KICKSTARTER_CLOCK   = 18,
    RADIAN_CLOCK        = 19,
    KICKSTARTER_DEMO    = 20,
    TEXT_ENTRY          = 21,
    BATTERY_GRAPH       = 22,
    LED_RING_CONTROL    = 23,
    
    TOTAL               = 24;
    
    const char names[TOTAL][20] {
    
        "HOME",
        "NUMBER_PAD",
        "TOUCH_DEMO",
        "CONTROL_SIMPLE",
        "GRAVITY_BALL",
        "PHONE",
        "CALL",
        "CONTACTS",
        "BLUE_CLOCK",
        "XKCD_CLOCK"
        "BAR_CLOCK",
        "CLOCKS",
        "VIDEO",
        "APPS",
        "SETTINGS",
        "SETTINGS_B",
        "BLUISH_CLOCK",
        "BREIGHTLING_CLOCK",
        "KICKSTARTER_CLOCK",
        "RADIAN_CLOCK",
        "KICKSTARTER_DEMO",
        "TEXT_ENTRY",
        "BATTERY_GRAPH",
        "LED_RING_CONTROL"
        
    };

}

namespace BUTTON {
    
    const int 
    
    POWER_BUTTON    = 0,
    
    TOTAL = 1;

}

// Globals

namespace {

    // Gague Page
    
    int lastNeedlePos = 0;
    
    // Pages

    int const STARGATE      = 0;
    int const CAR           = 1;
    int const GAGUE         = 2;
    int const WHEEL_CLOCK   = 3;

    int const PAGE_TOTAL    = 4;

    // Paging Mode

    int const NAV       = 0;
    int const SELECTED  = 1;
    
    SdFile 
    sgLightFile,
    sgDarkFile,
    sgLightInvFile,
    carFile,
    carInvFile,
    wheelFile,
    carGagueFile,
    carGagueInvFile,
    wheelInvFile,
    transparentStarBitmapFile,
    transparentGradientFile;

    char const contactsFilename[] = "contacts.db";
    char const favoritesFilename[] = "favorites.ptr";
    char const recentFilename[] = "recent.db";
    
    IntervalTimer touchInterruptTimer;
    
}

struct TIME_T {

    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int hundredth;

} curTime;

namespace { // Library inits

    PiWatch watch;
    
    BC127 bt(&Serial2,bluetoothMessage);

    SdFat sd;
    
    SdFile audioFile;
    
    LSM303_custom compass;
    
    IntervalTimer audioTimer;
    
    TEENSY3_LP LP = TEENSY3_LP();

}

namespace { // General

    int z;
    bool D = true;
    bool E = true;
    bool H = true;

    int page = -1;

    bool pagingMode = NAV;
    
    int currentHeading = 0;
    
    // int brightness = 100;
    
    int currentRotation = LANDSCAPE;
    // int currentRotation = PORTRAIT;
    
    // Page* pageArray[PAGE::TOTAL];

    // ArduinoOutStream db(Serial);
    // ArduinoOutStream dbB(Serial);

    Page* pageArray[PAGE::TOTAL];
    
    long lastTouchTime = 0;

}

namespace { // Page Handling

    int const PAGE_TRAIL_SIZE = 10;
    byte pageTrail[PAGE_TRAIL_SIZE];
    int pageTrailLength = 0;
    int pageTrailLocation = 0;

}

namespace SHARED {

    char phoneNumber[15] {0};

}

// Audio

namespace {

    // Audio states
    
    enum audio_state_t {
        AUTO,
        PLAY,
        REC, 
        PASS, 
        PLAY_PASS,
        REC_PASS,
        STANDBY,
    };

    audio_state_t audioState = STANDBY;

    volatile long playFileLength;

    // 64ms per block, 1.28 seconds for 20 blocks
    int const AUDIO_BUFFER_BLOCKS = 10; 

    volatile uint8_t audioBlock[AUDIO_BUFFER_BLOCKS][512]; // 10240 bytes
    volatile bool blockReady[AUDIO_BUFFER_BLOCKS];

    volatile int currentBlock = 0,currentByte = 0;
    volatile int currentReadBlock = 0;
    volatile uint32_t fileStartBlock,fileEndBlock;
    volatile int currentFileBlock = 0;

    volatile bool playDone = false;
    volatile bool playEntireFileRead = false;
    
    
    volatile long microSplit = 0;
    volatile bool microSplitDir = false;


}

// Audio

void stopAction(audio_state_t actionToStop = AUTO) {
  
	db.printf("stopAction: %d audioState %d\r\n",actionToStop,audioState);
	
	if(actionToStop == AUTO) actionToStop = audioState;

	if(actionToStop != audioState) return;

	switch(audioState) {

		case REC_PASS:
		case REC:
		case PLAY_PASS:
		case PLAY:

			// DISPLAYONE.println("$PIN 010 0&"); // local Audio source == bt1
			// DISPLAYTWO.println("$PIN 003 1&"); // bt2 Audio source == bt1
			
			// setAudioSrcAll(SRC_RADIO);
			
			analogWrite(A14,0);
			audioTimer.end();
			
			playFileLength = 0;
			// playFileSecondsRemaining = -1;
			
			if(audioFile.isOpen()) audioFile.close();
			
			break;
		case PASS:
			break;
		default:
			break;

	}

	if(D) { db.print("Just stopped "); printState();}

	audioState = STANDBY;
  
}

void printState() {

	if(D) {
	
		db.print("STATE: "); 

		switch(audioState) {

			case REC_PASS: 	db.println("RECORD PASSTHROUGH"); 	break;
			case REC:  		db.println("RECORD"); 				break;
			case PLAY_PASS: db.println("PLAY PASSTHROUGH"); 	break;
			case PLAY: 		db.println("PLAY"); 				break;
			case PASS: 		db.println("PASSTHROUGH");  		break;
			case STANDBY: 	db.println("STANDBY");  			break;
			default: 		db.println("NONE"); 				break; 
		
		}
	
	}
  
}

void audioLoop() {
    
    static elapsedMillis errPrntTime;

    // Playback
    if(audioState == PLAY) {
    
        cli();
    
        // Playback interrupt sets this flag if it's done playing 
        // [interrupt is checking playDone so don't switch it until AFTER 
        // stopAction has run and stopped the interrupt]
        if(playDone) { stopAction(PLAY); playDone = false; return; }
        
        // int bytesBuffered = 0;

        // Don't want to get stuck here so only allow it to handle the buffer once per goround
        for(int count=0;count<AUDIO_BUFFER_BLOCKS;count++) {

            // Ready to transfer this block
            if(!blockReady[currentReadBlock] && !playEntireFileRead) {
            
                int bytesTransferred = audioFile.read((uint8_t *)audioBlock[currentReadBlock], 512);
            
                if(bytesTransferred == -1) {
                
                    // Error
                    // if(E) { printdaterr3: db.printf("ERROR: read failed\r\n"); }
                    // if(H) { delay(500); goto printdaterr3; }
                    
                    errH("read failed");
                    
                    stopAction(PLAY);
                    
                } else if(bytesTransferred > 0) {

                    // fill in values if the end of the file is found
                    if(bytesTransferred < 512) {
                        
                        playEntireFileRead = true;
                        
                        for(int i=0;i<512-bytesTransferred;i++) audioBlock[currentReadBlock][i + bytesTransferred] = 128;
                    
                    }
                    
                    // bytesBuffered += bytesTransferred;
                    
                    // Set the flag for the play interrupt to read
                    blockReady[currentReadBlock] = true;
                    
                    errPrntTime = 0;
                
                    // Increment the block pointer
                    currentReadBlock++;
                    if(currentReadBlock >= AUDIO_BUFFER_BLOCKS) currentReadBlock = 0;
                
                }
            
            // Not ready to transfer the current block
            } else {
            
                // If we are sitting on a block that doesn't have data stop looping
                break;
            
            }
        }
        
        // if(bytesBuffered) if(D) db.printf("Play, Buffered %d bytes\r\n",bytesBuffered);        
        
        // purely to print an error when we are missing data
        if(errPrntTime > 500 && !blockReady[currentBlock] && !playEntireFileRead) {

            errPrntTime = 0;
            
            // if(E) { printdatothrerr2: db.println("ERROR: we aren't playing anything!!!"); }
            // if(H) { delay(500); goto printdatothrerr2; }
            
            errH("we aren't playing anything!!!");
        
        }
        
        sei();
    
    }
    
}

int fileLen;

void startPlay(char* tempfilename) {

    audioTimer.end();

    char filename[30];

    for(int i=0;i<30;i++) {
    
        filename[i] = tempfilename[i];
        if(i == 29) filename[i] = 0;
        if(filename[i] == 0) break;
        
    }

    stopAction(AUTO);

    if(!checkFilename(filename) && E) db.printf("ERROR: checkFilename failed on %s\r\n",filename);

    if (!audioFile.open(filename, O_READ)) { if(D) db.printf("Opening %s for read failed\r\n",filename); return; }

    if(D) db.printf("Playing: %s\r\n",filename);

    // DISPLAYONE.println("$PIN 010 1&"); // local Audio source == teensy
    // DISPLAYTWO.println("$PIN 003 0&"); // bt2 Audio source == teensy

    audioFile.seekSet(40);

    playFileLength  =  audioFile.read();
    playFileLength += (audioFile.read() << 8);
    playFileLength += (audioFile.read() << 16);
    playFileLength += (audioFile.read() << 24);
    
    fileLen = playFileLength;
    
    // Initialize all of the data ready flags to false
    for(int i=0;i<AUDIO_BUFFER_BLOCKS;i++) blockReady[i] = false;
    
    currentReadBlock = 0;
    currentBlock = 0;
    currentByte = 0;
    playEntireFileRead = false;

    audioState = PLAY;

    audioLoop();
    
    microSplit = 0;
    microSplitDir = false;
    
    audioTimer.begin(playInterrupt, 125);
    // audioTimer.begin(playInterrupt, 62);
    // audioTimer.begin(playInterrupt, 31);
    // audioTimer.begin(playInterrupt, 15);
    
    // delay(1); // Start getting values into the dac before the switch happens
    
    // setAudioSrcAll(SRC_CORTEX);
  
}

void playInterrupt() {

    if(blockReady[currentBlock] && !playDone) {

        // analogWrite(A14,audioBlock[currentBlock][currentByte + 1]);
        analogWrite(A14,(audioBlock[currentBlock][currentByte] << 8) | audioBlock[currentBlock][currentByte + 1]);

        currentByte+=2;

        playFileLength-=2;
        
        // analogWrite(A14,audioBlock[currentBlock][currentByte]);

        // currentByte++;
        
        // playFileLength--;
        
        // playFileSecondsRemaining = playFileLength/8000;
        
        if(playFileLength == 0) { audioTimer.end(); playDone = true; return; }
        
        // Check to see if we have played this whole block
        if(currentByte >= 512) {
        
            currentByte = 0;
            
            // This block now needs to be filled again (not ready to be played)
            blockReady[currentBlock] = false;
            
            // Increment Block
            currentBlock++;
            if(currentBlock >= AUDIO_BUFFER_BLOCKS) currentBlock = 0;
            
        }
        
    }
    
    microSplit++;
    if(microSplit == 100) {
    
        microSplitDir = !microSplitDir;
        
        if(microSplitDir) {
        
            audioTimer.end();
            audioTimer.begin(playInterrupt, 63);
        
        } else {
        
            audioTimer.end();
            audioTimer.begin(playInterrupt, 62);
        
        }
    
    }
    
}

bool checkFilename(char* filename) { // Adds .WAV extension and gets rid of any whitespace/illegal characters.
  
    char extension[5] = ".WAV";

    // if(D) db.printf("fixed filename: %s\r\n",filename);

    for(int i=0;i<12;i++) {
    
        while(!isAlphaNumeric(filename[i]) && filename[i] != 0) {
        
            for(int n=i;1;n++) {
            
                filename[n] = filename[n + 1];
                if(filename[n] == 0) break;
                
            }
            
        }
        
        if(filename[i] == 0) {
        
            z=0;
            for(int n=i;z<5;n++) filename[n] = extension[z++];
            break;
        
        }
        
    }

    // if(D) db.printf("fixed filename: %s\r\n",filename);

    return true;

}


#include <PiWatch.h>

#include "homePage.h"
#include "GravityBallPage.h"
#include "controlSimple.h"
#include "numberPad.h"
#include "phone.h"
#include "call.h"
#include "TouchDemo.h"
#include "contacts.h"
#include "blueClock.h"
#include "xkcdnow.h"
#include "barClock.h"
#include "clockPage.h"
#include "videoPage.h"
#include "apps.h"
#include "settings.h"
#include "settingsB.h"
#include "bluishClock.h"
#include "breightlingClock.h"
#include "kickstarterClock.h"
#include "radianClock.h"
#include "kickstarterDemo.h"
#include "textEntry.h"
#include "batteryGraph.h"
#include "ledRingControl.h"

#include "roundUtil.h"