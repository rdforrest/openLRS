/*
  Based on work by Tim Senecal.
  For Nano Easy. (I have had problems with clones stopping working and requiring sketch to be reloaded.)
  Displays 2 streams of data on 0.96 inch oLED display which is visible in sunlight.
  Sends data over openLRS Passthru.
  To do this, open the openLRS configurator for TX , Set serial baudrate(bps) to 9600, data rate(bps) to 9600, Telemetry to "Yes" (This is Passthru)
  Connections to Hobbyking transmitter module:
  Identify serial port on module. (6 pins in a line.)
  Identify DTR pin. (Usually the left hand pin but it is marked on the back of the board.)
  Connect Arduino RX (Pin RXD1) --> TX pin on TX module (next to DTR pin)
  Connect Arduino Gnd --> Ground on TX module. (One of 2 pins furthest from DTR)
  Power supply: Arduino Vin --> Transmitter battery positive. (You can connect a flying lead into the TX module.)
  Arduino serial data default is, serial baud rate (9600) and 8 data bits, no parity,no stop,LSB and not inverted - the default

  Nano and display draw about 50mA

  Details of OLED display from Banggood:
  White 0.96 Inch OLED I2C IIC Communication Display 128*64 LCD Module
  VCC: 3.3-5V
  GND: Ground
  SCL: Serial Clock
  SDA: Serial Data
  Size: 0.96 inch
  Resolution: 128*64
  Color: white
  Viewing angle: >160°
  Support platform: , 51 series, MSP430 series, STM32/2, chip, etc.
  Ultra low power consumption: 0.04W during normal operation
  Wide voltage support: DC 3.3V-5V
  Working temperature: -30 ° C ~ 80 ° C
  Driver chip: SSD1306
  Communication method: IIC, only 2 I/O ports
  Backlight: OLED self-illumination, no backlight required
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

*/
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

String inString;   //  Make global
long bitt = 0;   //  Make global
/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.

  This is a page buffer example for oLED.

  Some fonts:
    // u8g2.setFont(u8g2_font_ncenB14_tr);
    // 11 Pixel height font
    //u8g2.setFont(u8g2_font_ncenB10_tr);
    // 14 Pixel font
    //u8g2.setFont(u8g2_font_profont22_mf);
    // 16 Pixel font
    //u8g2.setFont(u8g2_font_crox3hb_tn);
    // 49 Pixel Height
    //u8g2.setFont(u8g2_font_fur49_tn);
    //u8g2.drawStr(20, 55, b);
    // 62 Pixel Height
    u8g2.setFont(u8g2_font_logisoso62_tn);
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Picture Loop Page Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8G2_NULL u8g2(U8G2_R0);	// null device, a 8x8 pixel display which does nothing
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);	// Arduboy (Production, Kickstarter Edition)
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_3W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_ALT0_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// End of constructor list.RDF removed a lot of these.

void setup(void) {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Wake-up USB!");
  delay(1000); // We wait 1 second

  //Test oled
  u8g2.begin();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 15, "DATA RX");
  } while ( u8g2.nextPage() );
  delay(1000);

}
void loop(void) {
  // This is the function which receives the serial data:

  recvWithEndMarker();

  delay(200); // We wait 200 mS to print the next value

  // Convert integer to character for display ( https://www.instructables.com/id/Converting-integer-to-character/)

  long a = bitt;

  char b[3]; // define character variable

  String str; // define string variable

  str = String(a); // convert to string

  str.toCharArray(b, 3);

  // Print to oLED
  u8g2.firstPage();
  do {
    // 62 Pixel Height
    u8g2.setFont(u8g2_font_logisoso62_tn);
    u8g2.drawStr(20, 65, b);
  } while ( u8g2.nextPage() );
  delay(1000);

}

// Functions:

void recvWithEndMarker() {

  /*
    Serial connection with String to Integer conversion

    Reads a serial input string until it sees a newline, then converts the string
    to a number if the characters are digits.

    The circuit:
    - No external components needed.

    created 29 Nov 2010
    by Tom Igoe

    This example code is in the public domain.

    http://www.arduino.cc/en/Tutorial/StringToInt
  */

  inString = "";    // string to hold input

  // Open serial communications and wait for ports to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // send an intro:
  Serial.println("\n\nString to Int():");
  Serial.println();

  // Read serial1 input:
  Serial1.begin(9600);
  while (Serial1.available() > 0) {
    int inChar = Serial1.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      // Save value as a long integer.RDF
      bitt = inString.toInt();
      Serial.print("Value:");
      Serial.println(inString.toInt());
      Serial.print("String: ");
      Serial.println(inString);
      // clear the string for new input:
      inString = "";
    }
  }
}
