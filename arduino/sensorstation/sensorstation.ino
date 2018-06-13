#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#define NDEBUG

// role is either SENDER or Receiver
// #define SENDER
#define RECEIVER
// IDs 1-127 are senders
// IDs 128-160 are receivers
const uint8_t SENDER_ID = 128;
const uint8_t RECEIVER_ID = 1;
const int SENSOR_PERIOD = 10000; // ms
const float RADIO_FREQUENCY = 900.0f; //mHz
const char* LOG_FILENAME = "log.txt";
const char* DATA_FILENAME = "data";
const uint16_t LISTENING_TIMEOUT = 60000; //ms
const uint16_t SENDING_TIMEOUT = 5000; //ms

// In forced mode, the sensor must be told to take a measurement
// After the measurement, it goes to sleep mode
// In normal mode, the sensor takes continuous measurements
#define SENSOR_FORCED_MODE

float temperature;
float humidity;
float pressure;

char packet[10];
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t pingData = '.';

#ifdef SENDER
Adafruit_BME280 sensor;
#endif

RTC_PCF8523 rtc;
DateTime now;

const int RADIO_LED = 9;
const int RADIO_CS = 8;
const int RADIO_IRQ = 3;
RH_RF95 radio(RADIO_CS, RADIO_IRQ);
#ifdef SENDER
RHReliableDatagram datagram(radio, SENDER_ID);
#endif
#ifdef RECEIVER
RHReliableDatagram datagram(radio, RECEIVER_ID);
#endif

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

#ifdef SENDER
void getSensor() {
#ifdef SENSOR_FORCED_MODE
  sensor.takeForcedMeasurement();
#endif
  now = rtc.now();
  temperature = sensor.readTemperature();
  humidity = sensor.readHumidity();
  pressure = sensor.readPressure()/100.0f;
}
#endif

void writeData() {
  enableSD();
  uint32_t timestamp = now.unixtime();
  // convert temperature to fixed point, one significant decimal
  int16_t itemperature = (int16_t)(temperature*10);
  // convert humidity to integer
  uint8_t ihumidity = (uint8_t)(humidity);
  // convert pressure to fixed point, one significant decimal
  uint16_t ipressure = (uint16_t)(pressure*10);
  memcpy(&packet[0], &SENDER_ID, 1);
  memcpy(&packet[1], &timestamp, 4);
  memcpy(&packet[5], &itemperature, 2);
  memcpy(&packet[7], &ihumidity, 1);
  memcpy(&packet[8], &ipressure, 2);
  File datafile = SD.open(DATA_FILENAME, FILE_WRITE);
  if(datafile) {
    datafile.write(packet, 10);
    datafile.close();
  }
#ifndef NDEBUG
  else {
    logMessage("Could not open data file");
  }
#endif

#ifndef NDEBUG
  Serial.print(now.unixtime());
  Serial.print(' ');
  Serial.print(itemperature);
  Serial.print(' ');
  Serial.print(ihumidity);
  Serial.print(' ');
  Serial.println(ipressure);
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

#ifdef SENDER
void initializeSensor() {
  if(!sensor.begin()) {
#ifndef NDEBUG
    logMessage("Coulnd't find sensor");
#endif
    while(1) delay(60000);
  }
#ifdef SENSOR_FORCED_MODE
  sensor.setSampling(Adafruit_BME280::MODE_FORCED,
                     Adafruit_BME280::SAMPLING_X1, // temperature
                     Adafruit_BME280::SAMPLING_X1, // pressure
                     Adafruit_BME280::SAMPLING_X1, // humidity
                     Adafruit_BME280::FILTER_OFF);
#endif
  // wait a while for the sensor to get ready
  delay(100);
}
#endif

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
  //radio.sleep();
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

#ifdef SENDER
void trySend() {
  logMessage("Waiting for ping");
  enableRadio();
  if(datagram.waitAvailableTimeout(LISTENING_TIMEOUT)){
    // we have been pinged
    uint8_t len = sizeof(packet);
    uint8_t from;
    if(datagram.recvfromAck((uint8_t*)packet, &len, &from)) {
      String message = "Packet from ";
      message += String(from, DEC);
      logMessage(message);
    }
  }
  enableRadio();
  //radio.sleep();
}
#endif

#ifdef RECEIVER
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
      writeData();
      enableRadio();
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}
#endif

void setup() {
#ifndef NDEBUG
  initializeSerial();
#endif
  initializeSD();
  initializeRadio();
  initializeRTC();
#ifdef SENDER
  initializeSensor();
  logMessage("Initializing sender complete");
#endif
#ifdef RECEIVER
  logMessage("Initializing receiver complete");
#endif
}

void loop() {
#ifdef SENDER
  getSensor();
  writeData();
  trySend();
  //delay(SENSOR_PERIOD);
#endif

#ifdef RECEIVER
  tryReceive(1);
  //writeData();
  //delay(1000);
#endif
}
