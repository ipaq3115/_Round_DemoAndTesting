

#ifndef STARGATE_PAGE_H_
#define STARGATE_PAGE_H_

class StargatePage : public Page {
public:

StargatePage CONSTRUCTOR_MACRO

image_info back;

void initalize() {

    watch->loadImage("sgBack.raw",&back);
    
    back.x = 0; back.y = 0;    
    watch->printImage(&back,1);
    
}

void redraw() { initalize(); }

void leavingPage() { }

void bootup() {}

void loop() { }

void serviceLoop() {}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) { }
void button(int dir,int index) { }
void bluetoothCallback(bt_event event) { }


// void drawStargateLight(int value) {
// 
//     static int currentLight = -1;
//     
//     // x and y boundaries of the chevron lights in the stargate image. Format: [x left,y top,x right,y bottom]
//     int chevPos[15][4] = {
//         {100,  2,125, 24},
//         {165, 24,189, 47},
//         {196, 83,217,106},
//         {185,150,207,175},
//         {133,192,159,215},
//         { 61,194, 88,215},
//         { 13,150, 35,176},
//         {  3, 81, 24,104},
//         { 36, 24, 59, 45}
//     };
// 
//     if(value != currentLight) {
// 
//         if(currentLight != -1) 
//             watch.printBitmap(sgDarkFile,10,0,chevPos[currentLight][0],chevPos[currentLight][1],chevPos[currentLight][2],chevPos[currentLight][3]);
//         
//         currentLight = value;
//         
//         watch.printBitmap(sgLightFile,10,0,chevPos[currentLight][0],chevPos[currentLight][1],chevPos[currentLight][2],chevPos[currentLight][3]);
//     
//     }
//     
// }


};

#endif

