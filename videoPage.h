

#ifndef VIDEO_PLAYER_
#define VIDEO_PLAYER_

#include "roundUtil.h"

#define VIDEO_IDLE 0
#define VIDEO_PLAYING 1
#define VIDEO_PAUSED 2

char videoName[15] = "rally";
// char videoName[15] = "hobbit3";

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

    // if(videoState != VIDEO_PLAYING) {
        
        loadVideo(videoName,0,0);
        // loadVideo("hobbit3",0,0);
        // loadVideo("paul2",0,0);
        // loadVideo("rally",0,0);
        // loadVideo("ep28",0,0);0
        // loadVideo("shipVid",0,0);
        // loadVideo("drift",0,110-62);
        // loadVideo("drift2",0,110-62);
        
    // }
    
    // Touch
    firstTouchID = -1;
    seekTouch = false;
    touchTime = 0;
    lastMovePos = -1;
    
    // Max response rate on the touch for buttery smoothness while navigating the video
    oldResponseRate = watch->getResponseRateRaw();
    watch->setResponseRate(100);
    
}

void leavingPage() {

    // Return the response rate to the previous condition
    watch->setResponseRateRaw(oldResponseRate);
    
    stopAction();

}

void loadVideo(char * name,int x,int y) {

    const char ext[] = ".raw";
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

    videoFrameTotal = watch->loadVideo(videoFile,0,0);
    currentFrame = 0;
    
    
    // watch->clrScr();
    watch->videoFrame(x,y,0);
    // startPlay(name);
    
    vidX = x;
    vidY = y;
    
    videoState = VIDEO_PLAYING;

}

void loop() {

    static elapsedMillis frameUpdateTime;

    // if(videoState == VIDEO_PLAYING && firstTouchID == -1 && frameUpdateTime > 40) {
    if(videoState == VIDEO_PLAYING && firstTouchID == -1 && frameUpdateTime > 67) {
        
        frameUpdateTime = 0;
        
        watch->videoFrame(currentFrame,vidX,vidY);
        
        // cli();
        // 
        // if(blockReady[currentBlock]) {
        // 
        //     int diff = playFileLength - fileLen - 8000*currentFrame;
        //     
        //     currentByte += diff;
        //     
        //     // Check to see if we have played this whole block
        //     while(currentByte >= 512 && blockReady[currentBlock]) {
        //     
        //         currentByte -= 512;
        //         
        //         // This block now needs to be filled again (not ready to be played)
        //         blockReady[currentBlock] = false;
        //         
        //         // Increment Block
        //         currentBlock++;
        //         if(currentBlock >= AUDIO_BUFFER_BLOCKS) currentBlock = 0;
        //         
        //     }
        //     
        // }
        // 
        // sei();
    
        currentFrame++;
        if(currentFrame >= videoFrameTotal) {
        
            currentFrame = 0;
            // videoState = VIDEO_IDLE;
            loadVideo(videoName,0,0);
        
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
                    
                    watch->videoFrame(currentFrame,0,0);
                    
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
