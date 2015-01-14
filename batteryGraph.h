

#ifndef BATTERY_GRAPH_H
#define BATTERY_GRAPH_H

// Stored in one byte at the moment so the highest this number should
// be without modifying the code is 255 
// NOTE: If this value is 1 it could try to erase a file while that file
// is still open. Might cause issues. 
#define MAX_BATTERY_LOG_HISTORY 100

class BatteryGraphPage : public Page {
public:

struct bat_cfg {

    byte batteryLogIndex;
    
    // Variable init
    bat_cfg () : 
    batteryLogIndex(0) 
    {}

    // array-style access
    unsigned char& operator[](size_t i) { return reinterpret_cast<unsigned char*>(this)[i]; }
    
} batConfig;

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

bool newData = false;

// System calls

void bootup() {
    
    // Read data into the batConfig struct
    readBatteryConfigFile();

    // Get/create the current log file based on index obtained from readBatteryConfigFile()
    loadLogFile();

}

void initalize() {
    
    // Print the background image
    watch->printImage("batGraph.raw",0,0);
    
    // Print the graph [Since the battery is always being logged the files are already open]
    printGraph();
    
}

void leavingPage() {

}

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
    
    // Save a sample every 10 seconds
    if(logTimer > 10000) {
    
        logTimer = 0;
        
        byte flags;
        
        // Pull the flags from the last sample in this file
        batteryLog.seekSet((logEntries - 1) * LENGTH + OFFSET + FLAGS);
        flags = batteryLog.read();
        
        if(D) db.printf("flags %d plugged in %d charging %d charged %d\r\n",flags,bluetooth->powerPluggedIn(),bluetooth->batteryCharging(),(bluetooth->powerPluggedIn() && !bluetooth->batteryCharging()));
        
        // old state is "charged" and new state is "not charged"
        if(flags == 2 && !(bluetooth->powerPluggedIn() && !bluetooth->batteryCharging())) {
        
            if(D) db.println("New log File");
        
            // Increment the file index pointer
            // This moves us into a new file
            batConfig.batteryLogIndex++;
            if(batConfig.batteryLogIndex >= MAX_BATTERY_LOG_HISTORY) batConfig.batteryLogIndex = 0;
            
            // Update the batteryLogIndex in the bat.cfg file
            writeBatteryConfigFile();
            
            // Check to see if there is already a log file here and erase it if nessesary
            eraseLogFile(batConfig.batteryLogIndex);

            // Load up the new log file
            loadLogFile();
        
        }
    
        // Don't want rollover issues!
        if(logEntries >= 0x7FFFFFFF) return;
        
        // Seeking to the beginning of the new entry in the file
        batteryLog.seekSet(logEntries * LENGTH + OFFSET);

        // Time
        batteryLog.write(now(), 4);
        
        // Battery voltage in millivolts
        batteryLog.write(bluetooth->getBatteryVoltage(), 2);
        
        // Set flags
        flags = 0;
        bitWrite(flags,CHARGING,    bluetooth->batteryCharging());
        bitWrite(flags,PLUGGED_IN,  bluetooth->powerPluggedIn());
        
        // Flags
        batteryLog.write(flags);
        
        // The state of the low power mode at the time of the log
        batteryLog.write(byte(0));
        
        // Brightness
        batteryLog.write(byte(watch->getBrightness()));

        // Increment the total
        logEntries++;
        
        // Write the Total
        batteryLog.seekSet(0);
        batteryLog.write(logEntries, 4);
        
        // Sync changes
        batteryLog.sync();
        
        if(D) db.printf("Saved battery point for %d at %d\r\n",logEntries - 1,bluetooth->getBatteryVoltage());
        
        newData = true;

    }
    
}

void touch(int touchType,int finePos,int activeTouches,int touchIndex) {}
void button(int dir,int index) {}

// Page specific functions

/** Print battery history data from the current log file */
void printGraph() {

    // The scale of the graph, higher means more time is shown in total
    int const scale = 1;
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
    byte flags = 0;
    
    int minBat=9999,maxBat=0;
    int minPos=0,maxPos=0;
    
    for(int i=scale;i<total;i+=scale) {
    
        // Read the voltage for this position on the graph
        batteryLog.seekSet((startIndex + i) * LENGTH + OFFSET + VOLTAGE);
        newVoltage = batteryLog.read(2);
        
        // Read the voltage for the last position [should just save this from the last loop]
        batteryLog.seekSet((startIndex + i - scale) * LENGTH + OFFSET + VOLTAGE);
        oldVoltage = batteryLog.read(2);

        // Max/Min tracking
        if(newVoltage < minBat) { minBat = newVoltage; minPos = i; }
        if(newVoltage > maxBat) { maxBat = newVoltage; maxPos = i; }
        
        // Read the flags byte
        batteryLog.seekSet((startIndex + i) * LENGTH + OFFSET + FLAGS);
        flags = batteryLog.read();
        
        // Charging
        if(bitRead(flags, CHARGING)) {
        
            watch->setColor(180,180,180);
        
        // Plugged in but not charging [charged]
        } else if(bitRead(flags, PLUGGED_IN)) {
        
            watch->setColor(WHITE);
        
        // Discharging
        } else {
            
            if(newVoltage > 4000) watch->setColor(0,255,0); // Green
            else if(newVoltage > 3800) watch->setColor(YELLOW);
            else if(newVoltage > 3600) watch->setColor(0xFF, 0xA5, 0x00); // Orange
            else watch->setColor(RED);
            
        }
        
        // Scale for the graph
        newVoltage = (newVoltage - 2500) / 20; if(newVoltage<0) newVoltage = 0;
        oldVoltage = (oldVoltage - 2500) / 20; if(oldVoltage<0) oldVoltage = 0;
        
        // Draw a line from the last voltage to the new voltage
        watch->drawLine(
        i/scale + 33,       169 - oldVoltage,
        i/scale + 33 + 1,   169 - newVoltage
        );
    
    }
    
    watch->setColor(WHITE);
    watch->setBackColor(TRANSPARENT);
    watch->setFont(SmallFont);
    
    int xpos;
    
    // Draw circle at the lowest voltage on the graph
    int tminBat = (minBat - 2500) / 20; if(minBat < 0) minBat = 0;
    watch->drawCircle(minPos/scale + 33 + 1, 169 - tminBat, 3);
    
    // Print min voltage number
    xpos = minPos/scale + 33 + 1 - 20; if(xpos < 35) xpos = 35; if(xpos > 150) xpos = 150;
    watch->printNumF((double)minBat / 1000.0,3, xpos, 169 - tminBat + 5);
    
    // Draw circle at the highest voltage on the graph
    int tmaxBat = (maxBat - 2500) / 20; if(maxBat < 0) maxBat = 0;
    watch->drawCircle(maxPos/scale + 33 + 1, 169 - tmaxBat, 3);
    
    // Print max voltage number
    xpos = maxPos/scale + 33 + 1 - 20;  if(xpos < 35) xpos = 35;  if(xpos > 150) xpos = 150;
    watch->printNumF((double)maxBat / 1000.0,3, xpos, 169 - tmaxBat - 15);
    
}

void readBatteryConfigFile() {

    if(sd.exists("bat.cfg")) {

        SdFile batteryConfigFile;
        
        if(!batteryConfigFile.open("bat.cfg",O_RDWR | O_CREAT)) if(E) db.println("bat.cfg couldn't be opened");
    
        if(D) db.printf("bat.cfg read\r\n");
    
        batteryConfigFile.seekSet(0);
        batteryConfigFile.read((byte *)&batConfig, sizeof(batConfig));
    
        if(D) db.printf("batConfig.batteryLogIndex %d\r\n",batConfig.batteryLogIndex);
        
        batteryConfigFile.close();
    
    } else {

        writeBatteryConfigFile();
        
    }

}

void writeBatteryConfigFile() {

    SdFile batteryConfigFile;
    
    if(!batteryConfigFile.open("bat.cfg",O_RDWR | O_CREAT)) if(E) db.println("bat.cfg couldn't be opened");

    if(D) db.printf("bat.cfg write\r\n");

    batteryConfigFile.seekSet(0);
    batteryConfigFile.write((byte *)&batConfig, sizeof(batConfig));
    
    batteryConfigFile.close();

}

void eraseLogFile(int index) {

    char filename[20];
    
    snprintf(filename,20,"bat%02X.log",index);
    
    if(sd.exists(filename)) {
    
        sd.remove(filename);
    
    }

}

void loadLogFile() {

    char filename[20];
    
    // Create the filename based on the current index
    snprintf(filename,20,"bat%02X.log",batConfig.batteryLogIndex);
    
    // Want to know if the file exists before we do a open() which will create the file if it doesn't exist
    bool fileExists = sd.exists(filename);
    
    // Want to close any old file that is loaded into batteryLog
    if(batteryLog.isOpen()) batteryLog.close();

    // Open the log file
    if(!batteryLog.open(filename,O_RDWR | O_CREAT)) if(E) db.printf("%s couldn't be opened\r\n",filename);

    // Get the existing number of entries
    if(fileExists) {
        
        batteryLog.seekSet(0);
        logEntries = batteryLog.read(4);
        
    // New file, set the number of entries to zero
    } else {
    
        logEntries = 0;
        
        batteryLog.seekSet(0);
        batteryLog.write(logEntries, 4);
        
        batteryLog.sync();
    
    }
    
    if(D) db.printf("logEntries %d\r\n",logEntries);

}

};

#endif