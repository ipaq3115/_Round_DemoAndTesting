

#ifndef BATTERY_GRAPH_H
#define BATTERY_GRAPH_H

class BatteryGraphPage : public Page {
public:

BatteryGraphPage() {}

BatteryGraphPage CONSTRUCTOR_MACRO

// SdFile backFile;
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
01    charging
01    low power
01    screen brightness



*/

static const int

OFFSET = 4,
LENGTH = 9,

TIME       = 0,
VOLTAGE    = 4,
CHARGING   = 6,
LOW_POWER  = 7,
BRIGHTNESS = 8;


unsigned long logEntries = 0;

void initalize() {

    bool fileExists = sd.exists("bat.log");

    if(batteryLog.isOpen()) batteryLog.close();
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
    
    // SdFile tmpFile;
    // if(tmpFile.isOpen()) tmpFile.close();
    // if(!tmpFile.open("batGraph.gci",O_RDWR)) if(E) db.println("bat.log couldn't be opened");
    
    // watch->printRaw(tmpFile,0,0);
    
    watch->printImage("batGraph.gci",0,0);
    
    printGraph();
    
}

void printGraph() {

    // char buf[900];

    int const scale = 20;
    
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

        newVoltage = newVoltage/3 - 233; if(newVoltage<0) newVoltage = 0;
        oldVoltage = oldVoltage/3 - 233; if(oldVoltage<0) oldVoltage = 0;
        
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

    batteryLog.close();

}

bool newData = false;

void loop() {

    // if(newData) {
    
    static elapsedMillis time;
    if(time > 20000) {
        
        time = 0;
    
        // newData = false;
        
        watch->printImage("batGraph.gci",0,0);
        
        printGraph();
    
    }


}

elapsedMillis logTimer;

void serviceLoop() {

    static int count = 0;
    static int batArray[100];
    
    if(logTimer > 10) {
    
        logTimer = 0;

        batArray[count] = watch->getBatteryRaw();
        
        count++;
        
        if(count == 100) {
            
            count = 0;

            int bat = 0;
            fori(100) bat += batArray[i]; bat /= 100;
            
            // int bat = watch->getBatteryRaw();
            
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
            
            // Charging
            batteryLog.write(byte(0));
            
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
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {}
void button(int dir,int index) {}

};

#endif