

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
        if(D) db.printf("pos %d logEntries %d\r\n",(logEntries - 1) * LENGTH + OFFSET + FLAGS,logEntries);
        
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

    if(D) db.println("##### printGraph #####");

    int const GRAPH_WIDTH = 150;

    // Get the time for the first collection in this log file
    batteryLog.seekSet(0 * LENGTH + OFFSET + TIME);
    time_t startTime = batteryLog.read(4);
    
    // Get the time for the last collection in this log file
    batteryLog.seekSet((logEntries - 1) * LENGTH + OFFSET + TIME);
    time_t endTime = batteryLog.read(4);
    
    if(logEntries == -1) {
    
        if(D) db.printf("logEntries is -1 abort printGraph!\r\n");
        return;
    
    }
    
    // Print start and stop times on the graph
    
    char str[20];
    
    watch->setFont(SmallFont);
    watch->setColor(WHITE);
    watch->setBackColor(TRANSPARENT);
    
    snprintf(str,20,"%02lu:%02lu:%02lu - %02lu:%02lu:%02lu",hourFormat12(startTime),minute(startTime),second(startTime),hourFormat12(endTime),minute(endTime),second(endTime));
    watch->print(str,CENTER,35);
    
    // The time in seconds that the graph will represent
    time_t timeTotal = endTime - startTime;
    
    // Calculate the seconds per pixel on the 150px graph
    // This should give a window of time for each pixel so that collections that belong here can be easily found
    unsigned long long millisPerPx = (double)timeTotal / (double)GRAPH_WIDTH * 1000.0; if(millisPerPx < 1) millisPerPx = 1;
    
    if(D) db.printf("startTime %lu endTime %lu timeTotal %lu millisPerPx %llu\r\n",
    startTime, endTime, timeTotal, millisPerPx);
    
    // Go through the file (maybe in buffered chunks) and average all the times within the window for that pixel
    // If the time gets set during a log file this could cause a problem. Maybe just detect time going backwards
    // and count negative time difference as zero seconds
    
    char buf[LENGTH];
    int graphPx = 0;
    int voltAvg = 0,voltAvgCount = 0;
    int chrgAvg = 0,plugAvg = 0;
    int time,value;
    int lastPixelValue = -1;
    int lastXvalue;
    
    int minBat=9999,maxBat=0;
    int minPos=0,maxPos=0;
    
    if(D) db.printf("logEntries %d\r\n",logEntries);
    
    // Loop through every one of the samples in the log file
    for(int i=0;i<logEntries;i++) {
    
        if(D) db.printf("i %d\r\n", i);
    
        // Read all the data for this sample into buf
        batteryLog.seekSet(i * LENGTH + OFFSET);
        
        int bytesRead = batteryLog.read(buf, LENGTH);
        // if(D) db.printf("bytesRead %d LENGTH %d at %d\r\n",bytesRead,LENGTH,i * LENGTH + OFFSET);
        
        // if(D) {
        //     db.printf("buf %d: ",i * LENGTH + OFFSET);
        //     for(int k=0;k<LENGTH;k++) db.printf("%02X ",buf[k]);
        //     db.println();
        // }
        
        // Get the time of this sample
        time = 0; for(int k=0;k<4;k++) { time <<= 8; time += buf[k]; }
        
        // if(D) db.printf("time %lu\r\n",time);
    
        retry:
    
        if(D) db.printf("retry\r\n");

        // Current sample is ahead of or equal to the minium time of the window
        if(time >= startTime + graphPx * millisPerPx / 1000) {

            if(D) db.printf("a\r\n");

            // Current sample is also behind the max value for this window (last element just includes all)
            if(time < startTime + (graphPx + 1) * millisPerPx / 1000 || graphPx == GRAPH_WIDTH - 1) {
                
                if(D) db.printf("b\r\n");

                chrgAvg += bitRead(buf[FLAGS], CHARGING) ? 1 : -1;
                plugAvg += bitRead(buf[FLAGS], PLUGGED_IN) ? 1 : -1;
            
                voltAvg += (buf[VOLTAGE] << 8) + buf[VOLTAGE + 1];
                voltAvgCount++;
            
                // if(D) db.printf("Take average %d vold %d chrg %d plug %d %d %d\r\n",voltAvgCount,voltAvg,chrgAvg,plugAvg,bitRead(buf[FLAGS], CHARGING),bitRead(buf[FLAGS], PLUGGED_IN));
            
                if(i == logEntries - 1) goto printit;
            
            // This means new sample is out of the window for the current pixel so we want to print this 
            // window and retry this sample in the next pixel window
            } else {

                printit:
                
                if(D) db.printf("c\r\n");
                
                // if(D) db.printf("Window finish graphPx %d voltAvgCount %d\r\n",graphPx,voltAvgCount);
            
                // There might be entire windows that are empty, for instance times when the watch is off
                // In that case we just want to skip and leave blanks in our graph, might fill these in with a different color eventually
                if(voltAvgCount > 0) {
            
                    // Finish the averages
                    value = voltAvg / voltAvgCount; 
                    if(value > 5000) value = 5000;
                    if(value < 2500) value = 2500;
                    chrgAvg = chrgAvg >= 0 ? true : false;
                    plugAvg = plugAvg >= 0 ? true : false;
                    
                    // if(D) db.printf("Averages value %d chrgAvg %d plugAvg %d\r\n",value,chrgAvg,plugAvg);
                
                    // Max/Min tracking
                    if(value < minBat) { minBat = value; minPos = graphPx; }
                    if(value > maxBat) { maxBat = value; maxPos = graphPx; }
                
                    // Set color
                    
                    // Charging
                    if(chrgAvg) watch->setColor(180,180,180);
                    
                    // Plugged in but not charging [charged]
                    else if(plugAvg) watch->setColor(WHITE);
                    
                    // Discharging
                    else {
                        
                        if(value > 4000) watch->setColor(0,255,0); // Green
                        else if(value > 3800) watch->setColor(YELLOW);
                        else if(value > 3600) watch->setColor(0xFF, 0xA5, 0x00); // Orange
                        else watch->setColor(RED);
                        
                    }
                    
                    // Scale for the graph
                    value = (value - 2500) / 20; if(value<0) value = 0;
                    
                    if(lastPixelValue != -1) {
                        
                        // Draw a line from the last voltage to the new voltage
                        watch->drawLine(
                        lastXvalue,   169 - lastPixelValue,
                        graphPx + 33, 169 - value
                        );
                        // if(D) db.printf("drawLine x %d y %d x %d y %d\r\n",graphPx + 33 + 1, 169 - value, graphPx + 33, 169 - lastPixelValue);
                        
                    }
                
                    lastXvalue = graphPx + 33;
                    lastPixelValue = value;
                
                }
            
                voltAvg = 0;
                voltAvgCount = 0;
                chrgAvg = 0; plugAvg = 0;
                
                graphPx++;
                // if(D) db.printf("graphPx %d\r\n",graphPx);
                
                if(graphPx > GRAPH_WIDTH) goto theEnd;
                
                goto retry;
                
            }
        
        // Current sample is behind the minimum time of our window so either
        // time travel has been invented or the user set the time in between
        // this sample and the last
        } else {
        
            // TODO
        
        }

    }
    
    theEnd:

    if(D) db.printf("d\r\n");
    
    return; // For some reason the complier wants a ';' after the above label
    
}

void readBatteryConfigFile() {

    if(D) db.printf("readBatteryConfigFile\r\n");
    
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

    if(D) db.printf("loadLogFile\r\n");
    
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