

#ifndef BATTERY_GRAPH_H
#define BATTERY_GRAPH_H

class BatteryGraphPage : public Page {
public:

BatteryGraphPage() {}

BatteryGraphPage CONSTRUCTOR_MACRO

SdFile batteryLog;

/*
Battery Log format

Header
bytes description
04    number of samples

Sample
bytes description
04    timestamp
02    voltage
01    flags
01    low power
01    screen brightness



*/



static const int

OFFSET = 4,
LENGTH = 9,

TIME        = 0,
VOLTAGE     = 4,
FLAGS       = 6,
LOW_POWER   = 7,
BRIGHTNESS  = 8,

// Flags
CHARGING    = 0,
PLUGGED_IN  = 1;

unsigned long logEntries = 0;

void bootup() {

    bool fileExists = sd.exists("bat.log");

    if(!batteryLog.open("bat.log",O_RDWR | O_CREAT)) if(E) db.println("bat.log couldn't be opened");

    if(fileExists) {
        
        batteryLog.seekSet(0);
        
        logEntries  = batteryLog.read() << 24;
        logEntries += batteryLog.read() << 16;
        logEntries += batteryLog.read() <<  8;
        logEntries += batteryLog.read();
    
    } else {
    
        logEntries = 0;
        
        batteryLog.seekSet(0);
        
        batteryLog.write(byte(logEntries >> 24));
        batteryLog.write(byte(logEntries >> 16));
        batteryLog.write(byte(logEntries >>  8));
        batteryLog.write(byte(logEntries));
        
        batteryLog.sync();
    
    }
    
    if(D) db.printf("logEntries %d\r\n",logEntries);

}

void initalize() {

    watch->printImage("batGraph.raw",0,0);
    
    printGraph();
    
}

void printGraph() {

    // char buf[900];

    int const scale = 3;
    // int const scale = 80;
    
    int startIndex = logEntries - 150 * scale;
    if(startIndex < 0) startIndex = 0;
    
    // batteryLog.seekSet(startIndex * LENGTH + OFFSET);
    
    int total = logEntries - startIndex;
    
    if(D) db.printf("total %d logEntries %d startIndex %d\r\n",total,logEntries,startIndex);
    
    // total = batteryLog.read(buf, total * LENGTH) / LENGTH;

    if(D) db.printf("total %d\r\n",total);
    
    int newVoltage = 0;
    int oldVoltage = 0;
    
    watch->setColor(VGA_WHITE);
    
    for(int i=scale;i<total;i+=scale) {
    
        batteryLog.seekSet((startIndex + i) * LENGTH + OFFSET + VOLTAGE);
        newVoltage = (batteryLog.read() << 8) + batteryLog.read();
    
        batteryLog.seekSet((startIndex + i - scale) * LENGTH + OFFSET + VOLTAGE);
        oldVoltage = (batteryLog.read() << 8) + batteryLog.read();

        newVoltage = (newVoltage - 2500) / 20; if(newVoltage<0) newVoltage = 0;
        oldVoltage = (oldVoltage - 2500) / 20; if(oldVoltage<0) oldVoltage = 0;
        
        // newVoltage = (buf[i * LENGTH + VOLTAGE] << 8) + buf[i * LENGTH + VOLTAGE + 1];
        // oldVoltage = (buf[(i - 1) * LENGTH + VOLTAGE] << 8) + buf[(i - 1) * LENGTH + VOLTAGE + 1];

        // if(D) db.printf("new %d old %d\r\n",newVoltage,oldVoltage);
        // if(D) db.printf("%d %d %d\r\n",i,LENGTH,VOLTAGE);
        // if(D) db.printf("%d %d %d %d\r\n",i * LENGTH + VOLTAGE,i * LENGTH + VOLTAGE + 1,(i - 1) * LENGTH + VOLTAGE,(i - 1) * LENGTH + VOLTAGE + 1);
        // if(D) db.printf("%d %d %d %d\r\n",buf[i * LENGTH + VOLTAGE],buf[i * LENGTH + VOLTAGE + 1],buf[(i - 1) * LENGTH + VOLTAGE],buf[(i - 1) * LENGTH + VOLTAGE + 1]);
        
        watch->drawLine(
        i/scale + 33,
        169 - oldVoltage,
        i/scale + 33 + 1,
        169 - newVoltage
        );
    
    }
    if(D) db.printf("done\r\n");
    
}

void leavingPage() {

}

bool newData = false;

void loop() {
    
    // static elapsedMillis time;
    // if(time > 20000) {
    // time = 0;
    
    if(newData) {
    
        newData = false;
        
        watch->printImage("batGraph.raw",0,0);
        
        printGraph();
    
    }

}

void serviceLoop() {

    static elapsedMillis logTimer;
    
    if(logTimer > 10000) {
    
        logTimer = 0;

        int bat = bluetooth->getBatteryVoltage();
    
        if(logEntries >= 0x7FFFFFFF) return;

        batteryLog.seekSet(logEntries * LENGTH + OFFSET);

        time_t t = now();
        
        // Time
        batteryLog.write(byte(t >> 24));
        batteryLog.write(byte(t >> 16));
        batteryLog.write(byte(t >>  8));
        batteryLog.write(byte(t));
        
        // Voltage
        batteryLog.write(byte(bat >> 8));
        batteryLog.write(byte(bat));
        
        byte flags = 0;
        
        bitWrite(flags,CHARGING,    bluetooth->batteryCharging());
        bitWrite(flags,PLUGGED_IN,  bluetooth->powerPluggedIn());
        
        // Flags
        batteryLog.write(flags);
        
        // Low power
        batteryLog.write(byte(0));
        
        // Brightness
        batteryLog.write(byte(watch->getBrightness()));
      
        logEntries++;
        
        batteryLog.seekSet(0);
        
        batteryLog.write(byte(logEntries >> 24));
        batteryLog.write(byte(logEntries >> 16));
        batteryLog.write(byte(logEntries >>  8));
        batteryLog.write(byte(logEntries));
        
        batteryLog.sync();
        
        if(D) db.printf("Saved battery point for %d at %d\r\n",logEntries,bat);
        
        newData = true;

    }
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {}
void button(int dir,int index) {}

};

#endif