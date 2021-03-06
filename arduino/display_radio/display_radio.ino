  // put your setup code here, to run once:
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LSM303_U.h>
#include <SPI.h>
#include <RH_RF95.h>

#define OLED_RESET 5
Adafruit_SSD1306 display(OLED_RESET);

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

RH_RF95 rf95(8, 3);
const int led = 9;
unsigned long count = 0;

void drawlogo(const unsigned char* bitmap, int width, int height) {
  // draw the logo centered on the screen
  int xoffset = (SSD1306_LCDWIDTH-width)/2;
  int yoffset = (SSD1306_LCDHEIGHT-height)/2;
  display.drawBitmap(xoffset, yoffset, bitmap, width, height, WHITE);
}

void setup() {
  // display splash screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  drawlogo(ksu_logo_bits, ksu_logo_width, ksu_logo_height);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println(" Kentucky\n   State\nUniversity");
  display.display();
  delay(2000);
  display.clearDisplay();
  drawlogo(verizon_logo_bits, verizon_logo_width, verizon_logo_height);
  display.display();
  delay(2000);
  
  // Initialize the radio
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init()){
    display.clearDisplay();
    display.setCursor(0,10);
    display.println("No radio\ndetected.");
    display.display();
    while(1);
  }
  rf95.setFrequency(900.0f);
}

void loop() {
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      count++;
      digitalWrite(led, HIGH);
//      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(led, LOW);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Packet: ");
  display.println(count);
  display.display();
}

