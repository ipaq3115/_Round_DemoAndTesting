
#ifndef TargetBallPage_
#define TargetBallPage_

#include "pageClass.h"

const char hiScoreFilename[15] = "target.scr";

class TargetBallPage : public Page {

public:

bool edgeMode = false; 

int lastPosX, lastPosY;
int currentTargetX, currentTargetY;

int lastTargetX, lastTargetY;

int score;
int oldScore;
int highScore;
int oldHighScore;

TargetBallPage() {}

TargetBallPage CONSTRUCTOR_MACRO

void writeHiScore() {

    SdFile hiScoreFile;
    if(hiScoreFile.open(hiScoreFilename, O_RDWR | O_CREAT)) {

        hiScoreFile.seekSet(0);
        
        hiScoreFile.write(highScore >> 24);
        hiScoreFile.write(highScore >> 16);
        hiScoreFile.write(highScore >>  8);
        hiScoreFile.write(highScore);
        
        hiScoreFile.sync();
    
    }
    hiScoreFile.close();
    
}

void initalize() {

    if(D) USB.println("initalize");
    
    bool hiScoreFileExists = sd.exists(hiScoreFilename);
    
    SdFile hiScoreFile;
    if(hiScoreFile.open(hiScoreFilename, O_RDWR | O_CREAT)) {
    
        if(hiScoreFileExists) {
        
            hiScoreFile.seekSet(0);
        
            highScore  = hiScoreFile.read() << 24;
            highScore += hiScoreFile.read() << 16;
            highScore += hiScoreFile.read() <<  8;
            highScore += hiScoreFile.read();
        
        } else {

            // Write zero into the high score
        
            hiScoreFile.seekSet(0);
            
            for(int i=0;i<4;i++) hiScoreFile.write((byte)0);
            
            hiScoreFile.sync();
        
        } 
    
    }
    hiScoreFile.close();
    
    watch->setColor(0,0,0);
    watch->fillRect(0,0,219,219);

    score = 0;
    oldScore = 0;
    oldHighScore = 0;
    
    newTarget();
    printScore();
    drawTarget();
    
    lastPosX = -999;
    lastPosY = -999;
    
    lastTargetX = -999;
    lastTargetY = -999;
    
}

void newTarget() {
    
    int angle, radius;
    
    angle = random(360);
    radius = random(85);
    
    MyUtils::orbitPoint(110, 110, angle, radius, currentTargetX, currentTargetY);

    // if(D) db.printf("newTarget angle %d radius %d currentTargetX %d currentTargetY %d\r\n", 
    // angle, radius, currentTargetX, currentTargetY);
    
}

void redraw() { }

void leavingPage() {

    if(D) USB.println("leavingPage");

}

elapsedMillis newTargetTimer;

void loop() {

    if(newTargetTimer > 10000) {
   
        newTargetTimer = 0;
        
        // newTarget();
        
        // drawTarget();
   
    }

    // 35.1mm diameter
    // 0.159mm per pixel
    
    // 1000 counts from the accelerometer is 9.8 meters per second per second
    // or 9800mm MS2
    // or 98mm per second per 10 milliseconds
    // or 9.8mm per second per millisecond
    
    const float mmPerSecondPerMillis = 1.80665;
    // const float mmPerSecondPerMillis = 9.80665;
    
    static int ballPosX = 0,oldBallPosX = 0; // pixels
    static int ballPosY = 0,oldBallPosY = 0; // pixels
    
    static int ballDegreePos = 0;
    static double ballDegreeSpeed = 0;
    
    static float ballSpeedX = 0,ballSpeedY = 0; // units of mm per second
    static float lastAccelerationX = 0,lastAccelerationY = 0;

    static elapsedMillis speedUpdateTime = 0;
    
    if(speedUpdateTime > 10) {
    
        // lastAcceleration = 500;
        
        // int tmpTime = speedUpdateTime;

        // if(D) USB.printf("%f %d %f\r\n",
        // (mmPerSecondPerMillis * (lastAcceleration / (float)1000)),tmpTime,ballSpeed);
        
        if(!edgeMode) {

            ballSpeedX += (mmPerSecondPerMillis * (lastAccelerationX / (float)1000)) * speedUpdateTime;
            ballSpeedY += (mmPerSecondPerMillis * (lastAccelerationY / (float)1000)) * speedUpdateTime;

            ballPosX += ((ballSpeedX / 1000) * speedUpdateTime) / 0.159;
            ballPosY += ((ballSpeedY / 1000) * speedUpdateTime) / 0.159;
            
            int const limit = 110;
            
            if(ballPosX >= limit)  { ballPosX = limit;  ballSpeedX = 0; }
            if(ballPosX <= -limit) { ballPosX = -limit; ballSpeedX = 0; }
            
            if(ballPosY >= limit)  { ballPosY = limit;  ballSpeedY = 0; }
            if(ballPosY <= -limit) { ballPosY = -limit; ballSpeedY = 0; }
            
            // int tmp = MyUtils::radianstodegrees(atan2(ballPosY,ballPosX)) + 90;
            // while(tmp > 360) tmp -= 360; while(tmp < 0) tmp += 360;
            // tmp += 180;
            // if(tmp < 0) 
            // if(D) USB.printf("angle from center %03d\r\n",tmp);
            // if(D) USB.printf("angle from center %03.1f\r\n",MyUtils::radianstodegrees(tmp));
            // int tmp = atan(abs(ballPosY)/abs(ballPosX));
            // int tmp = atan(MyUtils::degreestoradians(abs(ballPosY)/abs(ballPosX)));
            // char buff[10];
            // sprintf(buff,"%3d",tmp);
            // watch->setColor(0xFFFF);
            // watch->print(tmp,CENTER,100);
            // (double)ballPosY/sin(tmp)
            
            // if(D) USB.printf("distance from center %03.1f\r\n",);
            
            int distance = sqrt(ballPosX * ballPosX + ballPosY * ballPosY);

            if(distance > 100) {

                ballDegreePos = MyUtils::radianstodegrees(atan2(ballPosY,ballPosX)) + 90;
                
                // ballDegreePos *= -1;
                // ballDegreePos -= 180;
                
                while(ballDegreePos > 360) ballDegreePos -= 360; 
                while(ballDegreePos < 0) ballDegreePos += 360;
                
                // USB.printf("ballDegreePos %d\r\n",ballDegreePos);

                // while(true);
                
                ballDegreeSpeed = 0;
                
                edgeMode = true;
                
                score = 0;

            }
        
        } 
        
        if(edgeMode) {
        
            // int ballDegreePos = MyUtils::radianstodegrees(atan2(lastAccelerationY,lastAccelerationX)) + 90;
            // while(ballDegreePos > 360) ballDegreePos -= 360; 
            // while(ballDegreePos < 0) ballDegreePos += 360;
            
            int angleToY = 180 - ballDegreePos;
            int angleToX =  90 - ballDegreePos;
            
            // if(D) USB.printf("accelerations x %05.1f y %05.1f\r\n",lastAccelerationX,lastAccelerationY);
            
            // Force along the circle
            int xForceOnCircle = round(sin(MyUtils::degreestoradians(angleToX)) * (double)lastAccelerationX); 
            int yForceOnCircle = round(sin(MyUtils::degreestoradians(angleToY)) * (double)lastAccelerationY); 
            
            // if(D) USB.printf("x %05d y %05d\r\n",xForceOnCircle,yForceOnCircle);
            
            // Force to the inside of the circle
            int xForceInsideCircle = round(cos(MyUtils::degreestoradians(angleToX)) * (double)lastAccelerationX); 
            int yForceInsideCircle = round(cos(MyUtils::degreestoradians(angleToY)) * (double)lastAccelerationY); 
            
            double combinedForce = yForceOnCircle + xForceOnCircle;
            int combinedInsideForce = yForceInsideCircle + xForceInsideCircle;
            
            // static int counter = 0;
            // if(++counter > 20) {
            // 
            //     if(D) USB.printf("accelerations x %05.1f y %05.1f ",lastAccelerationX,lastAccelerationY);
            // 
            //     if(D) USB.printf("%05d\r\n",combinedForce);
            // 
            //     counter = 0;
            //     
            // }
            
            ballDegreeSpeed *= 0.98;
            
            // ballDegreeSpeed += (mmPerSecondPerMillis * (combinedForce / (float)1000)) * speedUpdateTime;
            ballDegreeSpeed += ((double)0.05 * (combinedForce / (double)1000)) * (double)speedUpdateTime;
            // if(ballDegreeSpeed > )
            
            // if(D) USB.printf("%05d\r\n",ballDegreeSpeed);
            
            ballDegreePos += (ballDegreeSpeed * speedUpdateTime) / 10;
            while(ballDegreePos >= 360) ballDegreePos -= 360;
            while(ballDegreePos < 0)   ballDegreePos += 360;
            
            // if(D) USB.printf("%05d %05d\r\n",ballDegreeSpeed,ballDegreePos);
            
            // USB.printf("ballAngle %04d angleToY %04d X %04d yForceOnCircle %04d x %04d combinedForce %04d\r\n",
            // ballAngle,angleToY,angleToX,yForceOnCircle,xForceOnCircle,combinedForce);
            // USB.printf("ballAngle %04d angleToY %04d X %04d yForceInsideCircle %04d x %04d combinedInsideForce %04d\r\n",
            // ballAngle,angleToY,angleToX,yForceInsideCircle,xForceInsideCircle,combinedInsideForce);

            ballPosX = sin(MyUtils::degreestoradians(-ballDegreePos - 180)) * 100;
            ballPosY = cos(MyUtils::degreestoradians(-ballDegreePos - 180)) * 100;
            
            if(combinedInsideForce < -10) { 
            
                ballPosX = sin(MyUtils::degreestoradians(-ballDegreePos - 180)) * 95;
                ballPosY = cos(MyUtils::degreestoradians(-ballDegreePos - 180)) * 95;
                
                ballSpeedX = 0;
                ballSpeedY = 0;
           
                edgeMode = false;

            }
        
        }
        
        // static elapsedMillis updateTime;
        // 
        // if(updateTime > 100) {
        // 
        //     updateTime = 0;
        // 
        //     if(D) db.printf("ballPosX %03d currentTargetX %03d ballPosY %03d currentTargetY %03d %03d %03d\r\n", 
        //     ballPosX, currentTargetX, ballPosY, currentTargetY, abs((110 + ballPosX) - currentTargetX), abs((110 + ballPosY) - currentTargetY));
        // 
        // } 
        
        if(abs((110 + ballPosX) - currentTargetX) < 10 && abs((110 + ballPosY) - currentTargetY) < 10) {
        
            // if(D) db.printf("############################ target hit\r\n");
        
            score++;
            
            if(score > highScore) {
                
                highScore = score;
                
                writeHiScore();
                
            }
            
            newTarget();
        
        }
        
        printScore();
        drawTarget();
        
        // drawBall(oldBallPosX,oldBallPosY,0);
        drawBall(ballPosX, ballPosY, 0xFFFF);
        
        oldBallPosX = ballPosX;
        oldBallPosY = ballPosY;
        
        // if(D) USB.printf("%f\r\n",ballSpeed);
        
        compass->read();
        
        lastAccelerationX = compass->a.x * -1;
        lastAccelerationY = compass->a.y * -1;
    
        speedUpdateTime = 0;
    
    }
    

    return;

    static int count = 0,lastX,lastY,lastZ;

    count++;
    
    if(count == 100) {
    
        // while(true);
    
        Serial.printf("%6.2f %d\r\n",ballSpeedX,ballPosX);
        
        // Serial.println(compass->heading());
        
        // compass->read();
        // Serial.printf("%f %f %f\r\n",compass->a.x,compass->a.y,compass->a.z);
        
        // drawAccel(lastX,lastY,lastZ,0);
        // drawAccel(compass->a.x,compass->a.y,compass->a.z,0xFFFF);
        
        // lastX = compass->a.x;
        // lastY = compass->a.y;
        // lastZ = compass->a.z;
        
        count = 0;
        
    }
    
}

void printScore() {

    char str[50];

    static int last_orientation = -1;
    
    if(score != oldScore || last_orientation != currentRotation) {
    
        if(last_orientation != currentRotation) watch->setOrientation(last_orientation);
    
        watch->setColor(BLACK);
        
        snprintf(str, 50, "%d", oldScore);
        watch->print(str, CENTER, 95);
        
        snprintf(str, 50, "HI %d", oldHighScore);
        watch->print(str, CENTER, 190);
        
        if(last_orientation != currentRotation) watch->setOrientation(currentRotation);
    
    }
    
    watch->setColor(WHITE);
    
    snprintf(str, 50, "%d", score);
    watch->print(str, CENTER, 95);
    
    snprintf(str, 50, "HI %d", highScore);
    watch->print(str, CENTER, 190);
    
    // watch->setOrientation(currentRotation);
    
    oldScore = score;
    last_orientation = currentRotation;
    oldHighScore = highScore;
    
}

void drawTarget() {

    watch->setOrientation(0);
    
    if(currentTargetX != lastTargetX || currentTargetY != lastTargetY) {
        
        watch->setColor(BLACK);
        watch->drawCircle(lastTargetX, lastTargetY, 5);

    }

    watch->setColor(RED);
    watch->drawCircle(currentTargetX, currentTargetY, 5);

    watch->setOrientation(currentRotation);
    
    lastTargetX = currentTargetX;
    lastTargetY = currentTargetY;
    
}

void drawBall(int posX,int posY,int color) {

    watch->setOrientation(0);
    
    if(posX != lastPosX || posY != lastPosY) {
        
        watch->setColor(BLACK);
        watch->fillCircle(110 + lastPosX, 110 + lastPosY, 5);
    
    }
    
    watch->setColor(color);
    watch->fillCircle(110 + posX, 110 + posY, 5);
    
    watch->setOrientation(currentRotation);
    
    lastPosX = posX;
    lastPosY = posY;
    
}

void drawAccel(int x,int y,int z,int color) {

    x /= 10;
    y /= 10;
    z /= 20;
    
    watch->setColor(color);

    // X
    watch->drawLine(110,110,110,110 + x);
    
    // Y
    watch->drawLine(110,110,110 + y,110);

    // Z
    int pointX,pointY;
    MyUtils::orbitPoint(110,110,-45,z,pointX,pointY);
    // orbitPoint(110,110,z >= 0 ? 45 : 225,z,pointX,pointY);
    watch->drawLine(110,110,pointX,pointY);


}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {

    if(D) USB.printf("TargetBallPage touch %d\r\n",touchType);
    
    switch(touchType) {
    
        case PRESSED:
        
            break;
        case MOVING:
            break;
        case RELEASED: 
            watch->clrScr();
            break;
    
    }

    if(D) { USB.printf("TargetBallPage touch %d\r\n",touchType); }
    
}

void button(int dir,int index) {

if(D) USB.println("button");

using namespace BUTTON;

static bool screenState = 0;

switch(index) {

case POWER_BUTTON:

    break;

}

}

};


#endif
