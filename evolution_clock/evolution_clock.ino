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

uint16_t outMapSeg [12] = {0x0001, 0x0800, 0x0C00, 0x0E00 , 0x0F00 , 0x0F80 , 0x0FC0 , 0x0FE0 , 0x0FF0 , 0x0FF8 , 0x0FFC , 0x0FFE  };


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*
 * 0 - 59 
 * 
 * 
*/
uint8_t convertMinuteIndexSegment (uint8_t minutes, uint8_t timeAdvance) {
  uint8_t indexSegment = 255; /* used to return */

  indexSegment = (minutes + timeAdvance) / 5;

  if (indexSegment > 11)  {
    indexSegment = 0;
  }
  
  return indexSegment;
}

uint8_t convertHourIndexSegment (uint8_t hours, uint8_t timeAdvance) {
  uint8_t indexSegment = 255; /* used to return */

  if (hours > 11) {
    hours = hours - 12;
  }

  indexSegment = (hours + timeAdvance);

  if (indexSegment > 11)  {
    indexSegment = 0;
  }
  
  return indexSegment;
}


uint8_t decodeDecOutLCD(LiquidCrystal_I2C lcd, uint8_t indexSegLCD) {
    if ( indexSegLCD > 3 ) {
    return 255;
    }
    
    switch (indexSegLCD) {
      case 0:
      lcd.write(4);
      break;
      
      case 1:
      lcd.write(5);
      break;
      
      case 2:
      lcd.write(6);
      break;
      
      case 3:
      lcd.write(7);
      break;
      
      }
}

uint8_t decodeBinOutLCD(LiquidCrystal_I2C lcd, uint8_t indexUpper, uint8_t indexLower) {
    if ( (indexUpper > 1) || (indexLower > 1)) {
    return 255;
  }
  uint8_t indexSegLCD = indexUpper;
  indexSegLCD = indexSegLCD << 1;
  indexSegLCD = indexSegLCD | indexLower;

  decodeDecOutLCD( lcd,  indexSegLCD);
  
}

uint8_t showOutLCD (LiquidCrystal_I2C lcd, uint8_t indexUpper, uint8_t indexLower) {
  if ( (indexUpper > 12) || (indexLower > 12)) {
    return 255;
  }
  
  lcd.setCursor(0, 1);
  lcd.print("  ");
  
  uint8_t contSegLCD = 0;

  for (contSegLCD = 12; contSegLCD > 0; contSegLCD--) {

      decodeBinOutLCD( lcd,  (1 & (outMapSeg[indexUpper] >> (contSegLCD - 1))),  (1 & (outMapSeg[indexLower] >> (contSegLCD - 1))));
  }
 
return 0;

}


String ReadSerialData() {
  String myString = "";
  char receivedChar;
  uint8_t contChar = 0;
  
  while (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar != '\n') {
      myString.concat(receivedChar);
      }
  }

  return myString;
  
}

/*
 * 
 *  SETD 31/12/2018
 *  SETH 14:50:10
 *  
 *  
 */


void SetParameter(RTC_DS3231 rtc) {
  String StringDataReceived = "";
  String commandType = "";
  String aux = "";
  uint16_t IntDataArray[10];
  DateTime nowDateTime;
  
  StringDataReceived = ReadSerialData();
  StringDataReceived.toUpperCase();
  StringDataReceived [20];

  uint8_t cont;
  for (cont = 0;cont < 4;cont++) {
    commandType.concat(StringDataReceived[cont]);
  }

  if (commandType == "SETD") {

    nowDateTime = rtc.now();

    Serial.println(StringDataReceived);
    Serial.print(nowDateTime.year(), DEC);
    Serial.print('/');
    Serial.print(nowDateTime.month(), DEC);
    Serial.print('/');
    Serial.print(nowDateTime.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[nowDateTime.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(nowDateTime.hour(), DEC);
    Serial.print(':');
    Serial.print(nowDateTime.minute(), DEC);
    Serial.print(':');
    Serial.print(nowDateTime.second(), DEC);
    Serial.println();

    aux = String (StringDataReceived[5]);
    aux += String (StringDataReceived[6]);
    Serial.println(aux);
    Serial.println(aux.toInt());
    IntDataArray[0] = aux.toInt(); //day
    aux = String (StringDataReceived[8]);
    aux += String (StringDataReceived[9]);
//    aux = String ((char)StringDataReceived[8] + (char)StringDataReceived[9]);
    Serial.println(aux);
    IntDataArray[1] = aux.toInt(); //month
    aux = String (StringDataReceived[11]);
    aux += String (StringDataReceived[12]);
    aux += String (StringDataReceived[13]);
    aux += String (StringDataReceived[14]);
//    aux = String ((char)StringDataReceived[11] + (char)StringDataReceived[12] + (char)StringDataReceived[13] + (char)StringDataReceived[14]);
    Serial.println(aux);
    IntDataArray[2] = aux.toInt(); //year
    rtc.adjust(DateTime(IntDataArray[2], IntDataArray[1], IntDataArray[0], nowDateTime.hour(), nowDateTime.minute(), nowDateTime.second()));

    Serial.println(IntDataArray[0]);
    Serial.println(IntDataArray[1]);
    Serial.println(IntDataArray[2]);
     
  }

  if (commandType == "SETH") {
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  if (commandType == "TEST") {
    
  }
}
/**********************************************************************

                  888                             .d88      88b.   
                  888                            d88P"      "Y88b  
                  888                           d88P          Y88b 
.d8888b   .d88b.  888888 888  888 88888b.       888            888 
88K      d8P  Y8b 888    888  888 888 "88b      888            888 
"Y8888b. 88888888 888    888  888 888  888      Y88b          d88P 
     X88 Y8b.     Y88b.  Y88b 888 888 d88P       Y88b.      .d88P  
 88888P'  "Y8888   "Y888  "Y88888 88888P"         "Y88      88P"   
                                  888                              
                                  888                              
                                  888                                                            
                                    
**********************************************************************/




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
  lcd.setCursor(0, 1);
  decodeDecOutLCD( lcd,  0);
  decodeDecOutLCD( lcd,  1);
  decodeDecOutLCD( lcd,  2);
  decodeDecOutLCD( lcd,  3);
  decodeBinOutLCD( lcd,  0,  0);
  decodeBinOutLCD( lcd,  0,  1);
  decodeBinOutLCD( lcd,  1,  0);
  decodeBinOutLCD( lcd,  1,  1);
  
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.home();

  uint8_t contIcon = 0;
  uint8_t contMinutes = 0;
  uint8_t contHours = 0;
  
  
  while(1) {

    if(1) {
       DateTime mynow = DateTime( 199, 12,01, 12,59,33);
//       String myString = "";
//       char receivedChar;
//
//       while (Serial.available() > 0) {
//        receivedChar = Serial.read();
//
//        if (receivedChar != '\n') {
//          myString.concat(receivedChar);
//        }
//       }
//       Serial.println(myString);
//        Serial.print(ReadSerialData());
        SetParameter(rtc);
       
       Serial.print(mynow.year(), DEC);
        Serial.print('/');
        Serial.print(mynow.month(), DEC);
        Serial.print('/');
        Serial.print(mynow.day(), DEC);
        Serial.print(" (");
        Serial.print(daysOfTheWeek[mynow.dayOfTheWeek()]);
        Serial.print(") ");
        Serial.print(mynow.hour(), DEC);
        Serial.print(':');
        Serial.print(mynow.minute(), DEC);
        Serial.print(':');
        Serial.print(mynow.second(), DEC);
        Serial.println();
//        delay (1000);
    }
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


//    contMinutes = now.second();
//    contHours = now.minute();
//    showOutLCD ( lcd, convertMinuteIndexSegment( contHours , 0),  convertMinuteIndexSegment( contMinutes , 0));

    contMinutes = now.minute();
    contHours = now.hour();
    showOutLCD ( lcd, convertHourIndexSegment( contHours , 0),  convertMinuteIndexSegment( contMinutes , 0));
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
    

    
    
    delay(900);
  }
  
}

/**********************************************************************


888                                   .d88      88b.   
888                                  d88P"      "Y88b  
888                                 d88P          Y88b 
888  .d88b.   .d88b.  88888b.       888            888 
888 d88""88b d88""88b 888 "88b      888            888 
888 888  888 888  888 888  888      Y88b          d88P 
888 Y88..88P Y88..88P 888 d88P       Y88b.      .d88P  
888  "Y88P"   "Y88P"  88888P"         "Y88      88P"   
                      888                              
                      888                              
                      888                              
 
**********************************************************************/

void loop() {
  // put your main code here, to run repeatedly:

}
