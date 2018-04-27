#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RTClib.h>

// #define NDEBUG

// IDs 1-127 are senders
// IDs 128-160 are receivers
const uint8_t SYSTEM_ID = 128;
const float RADIO_FREQUENCY = 900.0f; //mHz
const char* LOG_FILENAME = "log.txt";
const char* DATA_FILENAME = "data";
const uint16_t LISTENING_TIMEOUT = 60000; //ms
const uint16_t SENDING_TIMEOUT = 5000; //ms

char packet[10];
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t pingData = '.';

RTC_PCF8523 rtc;
DateTime now;

const int RADIO_LED = 9;
const int RADIO_CS = 8;
const int RADIO_IRQ = 3;
RH_RF95 radio(RADIO_CS, RADIO_IRQ);
RHReliableDatagram datagram(radio, SYSTEM_ID);

const int SD_CS = 10;

void logMessage(String message) {
  enableSD();
  now = rtc.now();
  File logfile = SD.open(LOG_FILENAME, FILE_WRITE);
  if(logfile) {
    char timestamp[21];
    sprintf(timestamp, "%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
            now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());
    logfile.print(timestamp);
    logfile.print(' ');
    logfile.println(message);
    logfile.close();
#ifndef NDEBUG
    Serial.println(message);
  }
  else {
    Serial.println("Could not open log file");
#endif
  }
}

void writePacket() {
  enableSD();
  File datafile = SD.open(DATA_FILENAME, FILE_WRITE);
  if(datafile) {
    datafile.write(packet, sizeof(packet));
    datafile.close();
  }
#ifndef NDEBUG
  else {
    logMessage("Could not open data file");
  }
#endif

#ifndef NDEBUG
  Serial.println("Written packet");
  Serial.println(packet);
#endif  
}

void enableRadio() {
  digitalWrite(SD_CS, HIGH);
  digitalWrite(RADIO_CS, LOW);
}

void enableSD() {
  digitalWrite(RADIO_CS, HIGH);
  digitalWrite(SD_CS, LOW);
}

void initializeRadio() {
  pinMode(RADIO_LED, OUTPUT);
  pinMode(RADIO_CS, OUTPUT);
  enableRadio();
  if(!datagram.init()){
#ifndef NDEBUG
    logMessage("Couldn't find radio");
#endif
    while(1) delay(60000);
  }
  radio.setFrequency(RADIO_FREQUENCY);
}

void initializeRTC() {
  if(!rtc.begin()) {
#ifndef NDEBUG
    logMessage("Couln't find RTC");
#endif
    while (1) delay(60000);
  }
  if(!rtc.initialized()) {
#ifndef NDEBUG
    logMessage("RTC is not running");
#endif
    // set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }  
}

void initializeSD() {
  pinMode(SD_CS, OUTPUT);
  enableSD();
  if (!SD.begin(SD_CS)) {
#ifndef NDEBUG
  Serial.println("SD initialization failed");
#endif
    while (1) delay(60000);
  } else {
#ifndef NDEBUG
    Serial.println("SD card is present.");
#endif
  }  
}

#ifndef NDEBUG
void initializeSerial() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing...");
}
#endif

void tryReceive(uint8_t id) {
  String message = "Pinging ";
  message += String(id, DEC);
  logMessage(message);
  enableRadio();
  digitalWrite(LED_BUILTIN, HIGH);
  if(datagram.sendtoWait(&pingData, sizeof(pingData), id)) {
    uint8_t len = sizeof(packet);
    uint8_t from;
    if (datagram.recvfromAckTimeout((uint8_t*)packet, &len, 2000, &from)) {
      message = "Received packet from ";
      message += String(from, DEC);
      logMessage(message);
      enableRadio();
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
#ifndef NDEBUG
  initializeSerial();
#endif
  initializeSD();
  initializeRadio();
  initializeRTC();
  logMessage("Initializing receiver complete");
}

void loop() {
  tryReceive(1);
  writePacket();
  //delay(1000);
}
