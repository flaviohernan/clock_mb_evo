#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* Date and time functions using a DS3231 RTC connected via I2C and Wire lib
 *  
 */
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


uint8_t clockIcon3H[8] = {0x00, 0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00};
uint8_t clockIcon6H[8] = {0x00, 0x00, 0x0e, 0x15, 0x15, 0x15, 0x0e, 0x00};
uint8_t clockIcon9H[8] = {0x00, 0x00, 0x0e, 0x15, 0x1D, 0x11, 0x0e, 0x00};
uint8_t clockIcon12H[8] = {0x00, 0x00, 0x0e, 0x15, 0x15, 0x11, 0x0e, 0x00};

uint8_t outU0L0 [8] = {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};
uint8_t outU0L1 [8] = {0x1F, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
uint8_t outU1L0 [8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x11, 0x11, 0x11, 0x1F};
uint8_t outU1L1 [8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*
 * 0 - 59 
 * 
 * 
*/
uint8_t convertMinuteIndexSegment (uint8_t minute, uint8_t timeAdvance) {
  uint8_t indexSegment = 255; /* used to return */

  indexSegment = (minute + timeAdvance) / 5;

  if (indexSegment > 11)  {
    indexSegment = 0;
  }
  
  return indexSegment;
}

uint8_t showOutLCD (LiquidCrystal_I2C lcd, uint8_t indexHour, uint8_t indexMinute) {
  if ( (indexHour > 12) || (indexMinute > 12)) {
    return -1;
  }
  uint8_t indexHourMinute = indexHour*12 + indexMinute; 
  
  lcd.setCursor(0, 1);
  lcd.print("  ");

  switch (indexHourMinute) {
    case 0:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    break;

    case 1:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(4);
    break;

    case 2:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 3:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 4:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 5:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 6:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 7:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 8:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 9:
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 10:
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 11:
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(4);
    break;

    case 12:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(6);
    lcd.write(5);
    break;

    case 13:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(7);
    lcd.write(4);
    break;

    case 14:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 15:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 16:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 17:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 18:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 19:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 20:
    lcd.write(4);
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 21:
    lcd.write(4);
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 22:
    lcd.write(4);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;

    case 23:
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(5);
    lcd.write(7);
    lcd.write(4);
    break;
    
  }
  
}

void setup() {
  
  Serial.begin(9600);
  
  delay(2000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  lcd.begin();
  lcd.backlight();

  lcd.createChar(0, clockIcon3H);
  lcd.createChar(1, clockIcon6H);
  lcd.createChar(2, clockIcon9H);
  lcd.createChar(3, clockIcon12H);
  lcd.createChar(4, outU0L0);
  lcd.createChar(5, outU0L1);
  lcd.createChar(6, outU1L0);
  lcd.createChar(7, outU1L1);

  lcd.home();

  lcd.print("Hello world...  ");
  delay(1000);
  lcd.home();
  lcd.print("                ");
  lcd.home();

  uint8_t contIcon = 0;
  uint8_t contMinutes = 0;
  
  while(1) {
    lcd.setCursor(0, 0);
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    lcd.print((now.hour() < 10) ? "0" : "" );
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print((now.minute() < 10) ? "0" : "" );
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print((now.second() < 10) ? "0" : "" );
    lcd.print(now.second(), DEC);
    lcd.print(" ");
    
    lcd.write(contIcon);
    
    lcd.print(" ");
    lcd.print((now.day() < 10) ? "0" : "" );
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print((now.month() < 10) ? "0" : "" );
    lcd.print(now.month(), DEC);
    
//    lcd.print('/');
//    lcd.print(now.year(), DEC);
    
    
    
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println();
    
//    lcd.setCursor(0, 1);
//    lcd.write(contIcon);
    contIcon ++;
    if (contIcon > 3) {
      contIcon = 0;
    }


    contMinutes = now.second();
    showOutLCD ( lcd, 0,  convertMinuteIndexSegment( contMinutes , 0));
//
//
    
//    lcd.print ("   ");
//    lcd.setCursor(2, 1);
//    if (contMinutes < 10) {
//      lcd.print (0);
//    }
//    lcd.print (contMinutes);
//
//    lcd.print ("   ");
//    lcd.setCursor(5, 1);
////    if (convertMinuteIndexSegment( contMinutes , 0) < 10) {
//      lcd.print ((convertMinuteIndexSegment( contMinutes , 0) < 10) ? "0" : "");
////    }
//    lcd.print(convertMinuteIndexSegment( contMinutes , 0));
////    contMinutes ++;
//    if (contMinutes > 59) {
//      contMinutes = 0;
//    }
    

    
    
    delay(1000);
  }
  
}


void loop() {
  // put your main code here, to run repeatedly:

}
