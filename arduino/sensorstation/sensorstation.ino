#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RH_RF95.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const int SENSOR_PERIOD = 10000; // ms
const float RADIO_FREQUENCY = 900.0f; //mHz
const char* LOG_FILENAME = "log.txt";

// #define NDEBUG

// In forced mode, the sensor must be told to take a measurement
// After the measurement, it goes to sleep mode
// In normal mode, the sensor takes continuous measurements
#define SENSOR_FORCED_MODE

float temperature;
float humidity;
float pressure;
Adafruit_BME280 sensor;

RTC_PCF8523 rtc;
DateTime now;

const int RADIO_LED = 9;
const int RADIO_CS = 8;
const int RADIO_IRQ = 3;
RH_RF95 rf95(RADIO_CS, RADIO_IRQ);

const int SD_CS = 10;

void logMessage(String message) {
  now = rtc.now();
  File logfile = SD.open(LOG_FILENAME, FILE_WRITE);
  if(logfile) {
    char timestamp[20];
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

void getSensor() {
#ifdef SENSOR_FORCED_MODE
  sensor.takeForcedMeasurement();
#endif
  now = rtc.now();
  temperature = sensor.readTemperature();
  humidity = sensor.readHumidity();
  pressure = sensor.readPressure()/100.0f;
#ifndef NDEBUG
  Serial.print(now.unixtime());
  Serial.print(' ');
  Serial.print(temperature);
  Serial.print(' ');
  Serial.print(humidity);
  Serial.print(' ');
  Serial.println(pressure);
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

void initializeSensor() {
  if(!sensor.begin()) {
#ifndef NDEBUG
    logMessage("Coulnd't find sensor");
    Serial.println("Couldn't find sensor");
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

void initializeRadio() {
  pinMode(RADIO_LED, OUTPUT);
  pinMode(RADIO_CS, OUTPUT);
  enableRadio();
  if(!rf95.init()){
#ifndef NDEBUG
    logMessage("Couldn't find radio");
    Serial.println("Couldn't find radio");
#endif
    while(1) delay(60000);
  }
  rf95.setFrequency(RADIO_FREQUENCY); 
}

void initializeRTC() {
  if(!rtc.begin()) {
#ifndef NDEBUG
    logMessage("Couln't find RTC");
    Serial.println("Couldn't find RTC");
#endif
    while (1) delay(60000);
  }
  if(!rtc.initialized()) {
#ifndef NDEBUG
    logMessage("RTC is not running");
    Serial.println("RTC is not running");
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
  logMessage("SD initialization failed");
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

void setup() {
#ifndef NDEBUG
  initializeSerial();
#endif
  initializeSensor();
  initializeRadio();
  initializeRTC();
  initializeSD();
  logMessage("Initialization complete");
}

void loop() {
  getSensor();
  delay(SENSOR_PERIOD);
}
