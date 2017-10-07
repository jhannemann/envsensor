#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi101.h>
#include <WiFiMDNSResponder.h>

// #define NDEBUG

// In forced mode, the sensor must be told to take a measurement
// After the measurement, it goes to sleep mode
// In normal mode, the sensor takes continuous measurements
// #define SENSOR_FORCED_MODE

// In forced mode, SENSOR PERIOD is recommended to be 60000 ms (1 min)
#define SENSOR_PERIOD 1000 // ms

#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define OLED_RESET_PIN 4

#define DEBOUNCE_DELAY 50 // ms

#define WIFI_CONNECT_TIMEOUT 10 // s
#define CLIENT_REFRESH 5 // s
#define CLIENT_CONNECT_TIMEOUT 250 // ms

const char ssid[] = "mynetwork";
const char pass[] = "mypassword";
char mdnsName[] = "weather00";

enum State {
  IDLE_STATE,
  DISPLAY_LAST,
  DISPLAY_UNIT,
  DISPLAY_WIFI_CONN,
  DISPLAY_WIFI
};

enum Event {
  BUTTON_1_PRESS,
  BUTTON_2_PRESS,
  SENSOR_TIMEOUT,
  NUM_EVENTS
};

State state;
bool eventFlags[NUM_EVENTS] = {false,
                               false,
                               false};

unsigned long timerStart;
unsigned long now;

float temperature;
float humidity;
float pressure;
bool metric = true;

Adafruit_BME280 sensor;
Adafruit_SSD1306 display(OLED_RESET_PIN);

bool wifiOn = false;
int wifiStatus = WL_IDLE_STATUS;
WiFiMDNSResponder mdnsResponder;
WiFiServer server(80);

#define ksu_logo_width 99
#define ksu_logo_height 55
static const unsigned char PROGMEM ksu_logo_bits[] = {
 0x00,0x00,0x00,0x00,0x01,0xb6,0xed,0xb0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x01,0xb6,0xed,0xb0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xc0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xc0,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00
,0xd6,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x0d,0x60
,0x54,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x05,0x40
,0x00,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x00,0x00
,0x7c,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x07,0xc0
,0x7c,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x07,0xc0
,0x7d,0xdb,0x6d,0xb6,0xdb,0xbc,0x07,0xbb,0x6d,0xb6,0xdb,0x77,0xc0
,0x7d,0xdb,0x6d,0xb6,0xdb,0xbc,0x07,0xbb,0x6d,0xb6,0xdb,0x77,0xc0
,0x7d,0xdb,0x6d,0xb6,0xdb,0xbc,0x07,0xbb,0x6d,0xb6,0xdb,0x77,0xc0
,0x7c,0x00,0x00,0x00,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x07,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xff,0xff,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0x81,0xf8,0x1f,0x81,0xfc,0x07,0xf0,0x3f,0x03,0xf0,0x37,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xf7,0xc0
,0x7d,0xff,0xff,0xff,0xff,0xfc,0x07,0xff,0xff,0xff,0xff,0xf7,0xc0 };

#define verizon_logo_width 128
#define verizon_logo_height 35
static const unsigned char PROGMEM verizon_logo_bits[] = {
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38
,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70
,0x00,0x00,0x00,0xfc,0x00,0x00,0x80,0x00,0x00,0x00,0x7e,0x00,0x00,0xf8,0x00,0x70
,0xfc,0x07,0xe3,0xff,0x03,0xe3,0xdf,0xbf,0xff,0xc1,0xff,0x81,0xfb,0xfc,0x70,0x60
,0x7c,0x07,0xc7,0xff,0x83,0xe7,0xdf,0xbf,0xff,0xc3,0xff,0xc1,0xfb,0xfe,0x70,0xe0
,0x7e,0x0f,0xcf,0xff,0xc3,0xff,0xdf,0xbf,0xff,0xc7,0xff,0xe1,0xff,0xff,0x38,0xc0
,0x7e,0x0f,0xcf,0xff,0xe3,0xff,0xdf,0xbf,0xff,0xcf,0xff,0xf1,0xff,0xff,0x39,0xc0
,0x3e,0x0f,0x9f,0x87,0xe3,0xff,0xdf,0xbf,0xff,0xcf,0xe3,0xf9,0xff,0xff,0x19,0xc0
,0x3e,0x0f,0x9f,0x03,0xf3,0xff,0x9f,0x80,0x1f,0x9f,0x81,0xf9,0xfc,0x3f,0x9d,0x80
,0x3f,0x1f,0xbf,0x03,0xf3,0xfc,0x1f,0x80,0x3f,0x9f,0x81,0xf9,0xfc,0x1f,0x9f,0x80
,0x3f,0x1f,0xbf,0x01,0xf3,0xf8,0x1f,0x80,0x7f,0x1f,0x00,0xfd,0xf8,0x1f,0x8f,0x80
,0x1f,0x1f,0x3f,0xff,0xf3,0xf0,0x1f,0x80,0x7e,0x1f,0x00,0xfd,0xf8,0x1f,0x8f,0x00
,0x1f,0x1f,0x3f,0xff,0xf3,0xf0,0x1f,0x80,0xfc,0x3f,0x00,0xfd,0xf8,0x1f,0x8f,0x00
,0x1f,0x9f,0x3f,0xff,0xfb,0xf0,0x1f,0x81,0xfc,0x3f,0x00,0xfd,0xf8,0x1f,0x86,0x00
,0x1f,0xbf,0x3f,0xff,0xfb,0xf0,0x1f,0x83,0xf8,0x3f,0x00,0xfd,0xf8,0x1f,0x80,0x00
,0x0f,0xbe,0x3f,0xff,0xfb,0xf0,0x1f,0x83,0xf0,0x3f,0x00,0xfd,0xf8,0x1f,0x80,0x00
,0x0f,0xfe,0x3e,0x00,0x03,0xf0,0x1f,0x87,0xe0,0x1f,0x00,0xfd,0xf8,0x1f,0x80,0x00
,0x0f,0xfe,0x3f,0x00,0x03,0xf0,0x1f,0x8f,0xe0,0x1f,0x00,0xfd,0xf8,0x1f,0x80,0x00
,0x07,0xfc,0x3f,0x01,0xf3,0xf0,0x1f,0x9f,0xc0,0x1f,0x81,0xf9,0xf8,0x1f,0x80,0x00
,0x07,0xfc,0x1f,0x83,0xf3,0xf0,0x1f,0x9f,0xc0,0x1f,0x81,0xf9,0xf8,0x1f,0x80,0x00
,0x07,0xfc,0x1f,0xc7,0xf3,0xf0,0x1f,0xbf,0xff,0xcf,0xe7,0xf9,0xf8,0x1f,0x80,0x00
,0x03,0xfc,0x0f,0xff,0xe3,0xf0,0x1f,0xbf,0xff,0xcf,0xff,0xf1,0xf8,0x1f,0x80,0x00
,0x03,0xf8,0x07,0xff,0xc3,0xf0,0x1f,0xbf,0xff,0xc7,0xff,0xe1,0xf8,0x1f,0x80,0x00
,0x03,0xf8,0x07,0xff,0xc3,0xf0,0x1f,0xbf,0xff,0xc3,0xff,0xc1,0xf8,0x1f,0x80,0x00
,0x03,0xf8,0x01,0xff,0x03,0xf0,0x1f,0xbf,0xff,0xc1,0xff,0x81,0xf8,0x1f,0x80,0x00
,0x00,0x00,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void drawlogo(const unsigned char* bitmap, int width, int height) {
  // draw the logo centered on the screen
  int xoffset = (SSD1306_LCDWIDTH-width)/2;
  int yoffset = (SSD1306_LCDHEIGHT-height)/2;
  display.drawBitmap(xoffset, yoffset, bitmap, width, height, WHITE);
}

void checkButtons() {
  // Pull-ups have inverted logic
  // true aka HIGH is open button
  static bool button1State = true;
  static bool button1LastState = true;
  static bool button2State = true;
  static bool button2LastState = true;

  // debounce buttons
  static unsigned long lastDebounceTime1 = 0;
  static unsigned long lastDebounceTime2 = 0;
  static bool prelimButton1State = true;
  static bool prelimButton2State = true;

  bool reading1 = digitalRead(BUTTON_1_PIN);
  bool reading2 = digitalRead(BUTTON_2_PIN);

  if(reading1!=prelimButton1State) {
    lastDebounceTime1 = now;
  }
  if((now-lastDebounceTime1)>DEBOUNCE_DELAY) {
    button1State = reading1;
  }
  prelimButton1State = reading1;
 
  if(reading2!=prelimButton2State) {
    lastDebounceTime2 = now;
  }
  if((now-lastDebounceTime2)>DEBOUNCE_DELAY) {
    button2State = reading2;
  }
  prelimButton2State = reading2;

  // check whether buttons have been pressed
  // rather than released
  if(button1State==false && button1LastState==true) {
    eventFlags[BUTTON_1_PRESS] = true;
#ifndef NDEBUG
    Serial.println("Button 1 pressed");
#endif
  }
  if(button2State==false && button2LastState==true) {
    eventFlags[BUTTON_2_PRESS] = true;
 #ifndef NDEBUG
    Serial.println("Button 2 pressed");
 #endif
  }

  button1LastState = button1State;
  button2LastState = button2State;
}

void checkTimer() {
  if((now-timerStart)>SENSOR_PERIOD) {
    eventFlags[SENSOR_TIMEOUT] = true;
  }
}

void clearButton1() {
  eventFlags[BUTTON_1_PRESS] = false;
}

void clearButton2() {
  eventFlags[BUTTON_2_PRESS] = false;
}

void clearButtons() {
  clearButton1();
  clearButton2();
}

void resetTimer() {
  eventFlags[SENSOR_TIMEOUT] = false;
  timerStart = now;
}

void getSensor() {
#ifdef SENSOR_FORCED_MODE
  sensor.takeForcedMeasurement();
#endif
  temperature = sensor.readTemperature();
  humidity = sensor.readHumidity();
  pressure = sensor.readPressure()/100.0f;
}

void printSensor() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if(metric) {
    display.print(temperature);
  }
  else {
    display.print(1.8f*temperature+32.0f);
  }
  // F7 is the degree symbol character code
  display.print(" \xF7"); 
  if(metric) {
    display.println('C');
  }
  else {
    display.println('F');
  }
  display.print(humidity); display.println(" %");
  if(metric) {
    display.print(pressure); 
  }
  else {
    display.print(pressure*0.02953f);
  }
  if(metric) {
    display.println(" hPa");
  }
  else {
    display.println(" inHg");
  }
  display.display();
}

void printUnit() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Units:");
  if(metric) {
    display.println("Metric");
  }
  else {
    display.println("Imperial");
  }
  display.display();
}

void connectWifi() {
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting...");
  display.print("SSID: ");
  display.println(ssid);
  display.display();
  int status = WiFi.begin(ssid, pass);
  int timeout = 0;
  while(status != WL_CONNECTED && timeout < 10) {
    status = WiFi.status();
    ++timeout;
    display.print('.');
    display.display();
    delay(1000);
  }
  display.println();
  if (status == WL_CONNECTED) {
    display.println("Success!");
    wifiOn = true;
    server.begin();
  }
  else {
    display.println("Timeout!");
    wifiOn = false;
  }
  display.display();
  delay(2000);
  display.setTextSize(2);
}

void disconnectWifi() {
  WiFi.disconnect();
  wifiOn = false;
}

void printWifi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("WiFi: ");
  if(wifiOn) {
    display.println("On\n");
    display.setTextSize(1);
    display.print("IP: ");
    IPAddress ip = WiFi.localIP();
    display.print(ip[0]);
    display.print('.');
    display.print(ip[1]);
    display.print('.');
    display.print(ip[2]);
    display.print('.');
    display.println(ip[3]);       
    display.print("Name: ");
    display.print(mdnsName);
    display.println(".local");
    display.setTextSize(2);
  }
  else {
    display.println("Off");
  }
  display.display();
}

void processEvents() {
  // process events dependent on state
  switch(state) {
    case IDLE_STATE:
      if(eventFlags[SENSOR_TIMEOUT]){
        state = DISPLAY_LAST;
      }
      if(eventFlags[BUTTON_1_PRESS]){
        state = DISPLAY_UNIT;
      }
      clearButtons();
      break;
    case DISPLAY_UNIT:
      printUnit();
      if(eventFlags[BUTTON_1_PRESS]){
        state = DISPLAY_WIFI;
      }
      if(eventFlags[BUTTON_2_PRESS]){
        metric = !metric;
      }
      clearButtons();
      resetTimer();
      break;
    case DISPLAY_WIFI:
      printWifi();
      if(eventFlags[BUTTON_1_PRESS]){
        state = DISPLAY_LAST;
      }
      if(eventFlags[BUTTON_2_PRESS]){
        if(wifiOn){
          disconnectWifi();
        }
        else{
          connectWifi();
        }
      state = DISPLAY_WIFI;
      }
      clearButtons();
      resetTimer();
      break;
    case DISPLAY_LAST:
      getSensor();
      printSensor();
      state = IDLE_STATE;
      resetTimer();
      break;
    default:
#ifndef NDEBUG
      Serial.println(F("Unknown state in processEvents()"));
#endif
      while (1) delay(1000);
  }
}

void checkConnection() {
  mdnsResponder.poll();
  WiFiClient client = server.available();
  if (client) {
#ifndef NDEBUG
    Serial.println("new client");
#endif
    unsigned long connectionTime = millis();
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    // Client connections are prone to get stuck in the "connected" state.
    // No data woule be read from the client, resulting in the server
    // getting stuck.
    // As a workaround, the client will be disconnected after a timeout period.
    while (client.connected() && millis()-connectionTime<CLIENT_CONNECT_TIMEOUT) {
#ifndef NDEBUG
      Serial.println("client connected");
#endif
      if (client.available()) {
#ifndef NDEBUG
        Serial.println("read client data");
#endif
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.print("Refresh: ");
          client.println(CLIENT_REFRESH);
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("<table>");
          client.println("<tr>");
          client.println("<td>");
          client.println("Station: ");
          client.println("</td>");
          client.println("<td>");
          client.println(mdnsName);
          client.println("</td>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td>");
          client.println("Temperature: ");
          client.println("</td>");
          client.println("<td>");
          client.println(temperature);
          client.println("</td>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td>");
          client.println("Humidity: ");
          client.println("</td>");
          client.println("<td>");
          client.println(humidity);
          client.println("</td>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td>");
          client.println("Pressure: ");
          client.println("</td>");
          client.println("<td>");
          client.println(pressure);
          client.println("</td>");
          client.println("</tr>");
          client.println("</table>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection
    client.stop();
#ifndef NDEBUG
    Serial.println("client disconnected");
#endif
  }
}

void setup() {
#ifndef NDEBUG
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing...");
#endif
  // display splash screen
  // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();
  drawlogo(ksu_logo_bits, ksu_logo_width, ksu_logo_height);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println(F(" Kentucky\n   State\nUniversity"));
  display.display();
  delay(2000);
  display.clearDisplay();
  drawlogo(verizon_logo_bits, verizon_logo_width, verizon_logo_height);
  display.display();
  delay(2000);
  // Initialize the sensor
  if(!sensor.begin()) {
    display.clearDisplay();
    display.setCursor(0,10);
    display.println(F("No sensor\ndetected."));
    display.display();
    while(1);
  }
#ifdef SENSOR_FORCED_MODE
  sensor.setSampling(Adafruit_BME280::MODE_FORCED,
                     Adafruit_BME280::SAMPLING_X1, // temperature
                     Adafruit_BME280::SAMPLING_X1, // pressure
                     Adafruit_BME280::SAMPLING_X1, // humidity
                     Adafruit_BME280::FILTER_OFF);
#endif
  // wait a while for the sensor to get ready
  // to prevent NaN on the display
  delay(100);
  state = DISPLAY_LAST;
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  now = millis();
  timerStart = now;
}

void loop() {
  now = millis();
  checkButtons();
  checkTimer();
  processEvents();
  if (wifiOn) {
    checkConnection();
  }
}
