

#ifndef VIDEO_PLAYER_
#define VIDEO_PLAYER_

#include "roundUtil.h"

#define VIDEO_IDLE 0
#define VIDEO_PLAYING 1
#define VIDEO_PAUSED 2

class VideoPlayerPage : public Page {

protected:

int mode;

SdFile videoFile;

int videoFrameTotal = 0;
int currentFrame = 0;
int videoState = VIDEO_IDLE;

int firstTouchID = -1;
bool seekTouch = false;
int touchTime = 0;
int lastMovePos = -1;

int vidX,vidY;

int oldResponseRate = 0;

public:

VideoPlayerPage() {}

VideoPlayerPage CONSTRUCTOR_MACRO

void initalize() {

    // if(!videoFile.open("shipVid.Gci",O_READ)) if(D) USB.println("Error: video file didn't open"); 
    
    if(videoState != VIDEO_PLAYING) {
        
        loadVideo("rally",0,0);
        // loadVideo("ep28",0,0);
        // loadVideo("shipVid",0,0);
        // loadVideo("drift",0,110-62);
        // loadVideo("drift2",0,110-62);
        
    }
    
    // Touch
    firstTouchID = -1;
    seekTouch = false;
    touchTime = 0;
    lastMovePos = -1;
    
    // Max response rate on the touch for buttery smoothness while navigating the video
    oldResponseRate = touchCtrl->getResponseRateRaw();
    touchCtrl->setResponseRate(100);
    
}

void leavingPage() {

    // Return the response rate to the previous condition
    touchCtrl->setResponseRateRaw(oldResponseRate);
    
    stopAction();

}

void loadVideo(char * name,int x,int y) {

    const char ext[] = ".Gci";
    int extC = 0;
    char videoFileName[15];
    bool nameDone = false;
    
    fori(12) {
    
        if(name[i] == 0) nameDone = true;
        
        if(!nameDone) {
        
            videoFileName[i] = name[i];
        
        } else {
        
            if(ext[extC] == 0) {
                
                videoFileName[i] = 0;
                break;
        
            } else {
            
                videoFileName[i] = ext[extC++];
            
            }
        
        }
    
    }
    
    if(videoFile.isOpen()) videoFile.close();

    if(!videoFile.open(videoFileName,O_READ)) {
        if(D) USB.println("Error: video file didn't open"); 
        return;
    }
    
    // startPlay("drift2");
    // startPlay(name);

    videoFrameTotal = lcd->loadVideo(videoFile,0,0);
    currentFrame = 0;
    
    // lcd->clrScr();
    lcd->videoFrame(x,y,0);
    
    vidX = x;
    vidY = y;
    
    videoState = VIDEO_PLAYING;

}

void loop() {

    static elapsedMillis frameUpdateTime;

    // if(videoState == VIDEO_PLAYING && firstTouchID == -1 && frameUpdateTime > 40) {
    if(videoState == VIDEO_PLAYING && firstTouchID == -1 && frameUpdateTime > 67) {
        
        frameUpdateTime = 0;
        
        lcd->videoFrame(currentFrame,vidX,vidY);
        
        currentFrame++;
        if(currentFrame >= videoFrameTotal) {
        
            currentFrame = 0;
            // videoState = VIDEO_IDLE;
        
        }
        
    }
    
    // if(firstTouchID != -1 && !seekTouch && millis() - touchTime > 500) seekTouch = true;

}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {
    
    if(D && touchType != MOVING) USB.printf("touch\r\n");
    
    switch(touchType) {
    
        case PRESSED:
            if(firstTouchID == -1) {
            
                firstTouchID = touchIndex;
                
                touchTime = millis();
                
                lastMovePos = -1;
                
                // seekTouch = false;
            
            }
            break;
        case MOVING:
        
            // if(seekTouch) {
            // if(firstTouchID == touchIndex && seekTouch) {
            // if(firstTouchID == touchIndex) {
            
                if(lastMovePos != -1) {
                
                    if(finePos - lastMovePos > 180) lastMovePos += 360;
                    if(lastMovePos - finePos > 180) lastMovePos -= 360;

                    int difference = finePos - lastMovePos;
                    
                    currentFrame += difference * 3 / 2;
                    
                    if(currentFrame >= videoFrameTotal) currentFrame -= videoFrameTotal;
                    if(currentFrame < 0) currentFrame += videoFrameTotal;
                    
                    lcd->videoFrame(currentFrame,0,0);
                    
                }
                
                lastMovePos = finePos;
                
            // }
            
            break;
        case RELEASED:
            
            if(firstTouchID == touchIndex) {
            
                // if(millis() - touchTime < 500) {
                // 
                //     if(videoState == VIDEO_IDLE) {
                //     
                //        videoState = VIDEO_PLAYING;
                //        // currentFrame = 0;
                //        seekTouch = false;
                //     
                //     } else if(videoState == VIDEO_PLAYING) {
                //     
                //        videoState = VIDEO_IDLE;
                //        seekTouch = true;
                //     
                //     }
                // 
                // }
                
                firstTouchID = -1;
            
            }
            
            
            break;
    
    }

}

void button(int dir,int index) {

}


};


#endif
