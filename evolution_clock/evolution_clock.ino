#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* Date and time functions using a DS3231 RTC connected via I2C and Wire lib
 *  
 */

#include "RTClib.h"

RTC_DS3231 rtc;
DateTime now;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"};


//LCD custom character
// clock icon
uint8_t clockIcon3H[8] = {0x00, 0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00};
uint8_t clockIcon6H[8] = {0x00, 0x00, 0x0e, 0x15, 0x15, 0x15, 0x0e, 0x00};
uint8_t clockIcon9H[8] = {0x00, 0x00, 0x0e, 0x15, 0x1D, 0x11, 0x0e, 0x00};
uint8_t clockIcon12H[8] = {0x00, 0x00, 0x0e, 0x15, 0x15, 0x11, 0x0e, 0x00};

//LCD custom character
// output indicator
uint8_t outU0L0 [8] = {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};
uint8_t outU0L1 [8] = {0x1F, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
uint8_t outU1L0 [8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x11, 0x11, 0x11, 0x1F};
uint8_t outU1L1 [8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

//output bitmask S1 S2 S3 ... S11 S12
uint16_t outMapSeg [12] = {0x0001, 0x0800, 0x0C00, 0x0E00 , 0x0F00 , 0x0F80 , 0x0FC0 , 0x0FE0 , 0x0FF0 , 0x0FF8 , 0x0FFC , 0x0FFE  };


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*
 * 0 - 59 
 * 
 * 
 * Recebe MINUTO e converte para um index
 * saidas
 * 0 a 4 = 0
 * 5 a 9 = 1
 * 10 a 14 = 2
 * 
 * o retorno dessa fincao, sera usado como indice do vetor outMapSeg[]
*/
uint8_t convertMinuteIndexSegment (uint8_t minutes, uint8_t timeAdvance) {
  
  uint8_t indexSegment = 255; /* used to return */

  indexSegment = (minutes + timeAdvance) / 5;

  if (indexSegment > 11)  {
    indexSegment = 0;
  }
  
  return indexSegment;
}

/*
 * 
 * Recebe HORA e converte para um index
 * saidas
 * 0 a 4 = 0
 * 5 a 9 = 1
 * 10 a 14 = 2
 * 
 *o retorno dessa fincao, sera usado como indice do vetor outMapSeg[]
 */
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


/*
 * Usada para escrever no LCD os caracteres que representam o estado das saidas
 * recebe um número de 0 a 3
 * 
 */
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

/*
 * Funcao responsável por unir os bit de de cada saida.
 * 0 e 0 = 0
 * 0 e 1 = 1
 * 1 e 0 = 2
 * 1 e 1 = 3
 * 
 * Recebe 0 ou 1 no campo indexUpper
 * Recebe 0 ou 1 no campo indexLower
 * 
 * O retorno dessa funcao sera usado como parametro para decodeDecOutLCD
 */
uint8_t decodeBinOutLCD(LiquidCrystal_I2C lcd, uint8_t indexUpper, uint8_t indexLower) {
    if ( (indexUpper > 1) || (indexLower > 1)) {
    return 255;
  }
  uint8_t indexSegLCD = indexUpper;
  indexSegLCD = indexSegLCD << 1;
  indexSegLCD = indexSegLCD | indexLower;

  decodeDecOutLCD( lcd,  indexSegLCD);
  
}

/*
 * Funcao usada para exibir no LCD, o estado de todas as saidas
 * 
 */
uint8_t showOutLCD (LiquidCrystal_I2C lcd, uint8_t indexUpper, uint8_t indexLower) {
  if ( (indexUpper > 12) || (indexLower > 12)) {
    return 255;
  }
  
  lcd.setCursor(0, 1);
  lcd.print("  ");
  
  uint8_t contSegLCD = 0;

/*
 * para exibir o estado de cada saida no LCD
 * 
 * eh escrito de forma reversa no LCD
 */
  for (contSegLCD = 12; contSegLCD > 0; contSegLCD--) {

      decodeBinOutLCD( lcd,  (1 & (outMapSeg[indexUpper] >> (contSegLCD - 1))),  (1 & (outMapSeg[indexLower] >> (contSegLCD - 1))));
  }
 
return 0;

}


/*
 * Funcao usada para controlar o estado logico dos pinos
 * 
 */
uint8_t showOutPins ( uint8_t indexUpper, uint8_t indexLower) {
  // if ( (indexUpper > 12) || (indexLower > 12)) {
  //   return EXIT_FAILURE;
  // }
  
  #ifdef __AVR_ATmega328P__

  //PORTD 4 a 7 = S1 a S4
  //PORTB 0 a 3 = S5 a S8
  //PORTC 0 a 3 = S9 a S12
  // {0x0001, 0x0800, 0x0C00, 0x0E00 , 0x0F00 , 0x0F80 , 0x0FC0 , 0x0FE0 , 0x0FF0 , 0x0FF8 , 0x0FFC , 0x0FFE  };

  PORTD &= 0x0F;
  PORTD |= 0xF0 & (outMapSeg[indexLower] > 4);
  PORTB &= 0xF0;
  PORTB |= 0x0F & (outMapSeg[indexLower] > 8);
  PORTC &= 0xF0;
  PORTC |= 0x0F & outMapSeg[indexLower];


  // PORTD |= 0xF0;

  // if ((PIND & 0xF0) == 0xF0 ) {
  //   PORTD &= ~0xF0;
  // } else {
  //   PORTD |= 0xF0;
  // }
  


  #endif
  

 
return EXIT_SUCCESS;

}


/*
 * Le uma string pela Serial, ate o caracter de nove linha
 */
String ReadSerialData() {
  String myString = "";
  char receivedChar;
  uint8_t contChar = 0;

  if (Serial.available() > 4) {
    
    while (Serial.available() > 0) {
      
      receivedChar = Serial.read();
      
      if (receivedChar != '\n') {
        myString.concat(receivedChar);

      }
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


uint8_t SetParameter(RTC_DS3231& rtc) {
  String StringDataReceived = "";
  String commandType = "";
  String aux = "";
  uint16_t IntDataArray[10];
  DateTime nowDateTime;
  
  StringDataReceived = ReadSerialData();
  StringDataReceived.toUpperCase();
  StringDataReceived [20];
  // Serial.println(StringDataReceived);

  uint8_t cont;
  for (cont = 0;cont < 4;cont++) {
    commandType.concat(StringDataReceived[cont]);
  }

  if (commandType == "SETD") {

    nowDateTime = rtc.now();

    aux = String (StringDataReceived[5]);
    aux += String (StringDataReceived[6]);

    IntDataArray[0] = aux.toInt(); //day
    if ( (IntDataArray[0] < 1) || (IntDataArray[0] > 31) ) {
      return EXIT_FAILURE;
    }
    aux = String (StringDataReceived[8]);
    aux += String (StringDataReceived[9]);

    IntDataArray[1] = aux.toInt(); //month
    if ( (IntDataArray[1] < 1) || (IntDataArray[1] > 12) ) {
      return EXIT_FAILURE;
    }
    aux = String (StringDataReceived[11]);
    aux += String (StringDataReceived[12]);
    aux += String (StringDataReceived[13]);
    aux += String (StringDataReceived[14]);

    IntDataArray[2] = aux.toInt(); //year
    if ( (IntDataArray[2] < 2000) || (IntDataArray[2] > 2099) ) {
      return EXIT_FAILURE;
    }
    rtc.adjust(DateTime(IntDataArray[2], IntDataArray[1], IntDataArray[0], nowDateTime.hour(), nowDateTime.minute(), nowDateTime.second()));
     
  }

  if (commandType == "SETH") {
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    nowDateTime = rtc.now();

    aux = String (StringDataReceived[5]);
    aux += String (StringDataReceived[6]);
    IntDataArray[0] = aux.toInt(); //hour

    if ( (IntDataArray[0] < 0) || (IntDataArray[0] > 23) ) {
      return EXIT_FAILURE;
    }
    
    aux = String (StringDataReceived[8]);
    aux += String (StringDataReceived[9]);
    IntDataArray[1] = aux.toInt(); //minute

    if ( (IntDataArray[1] < 0) || (IntDataArray[1] > 59) ) {
      return EXIT_FAILURE;
    }
    
    aux = String (StringDataReceived[11]);
    aux += String (StringDataReceived[12]);
    IntDataArray[2] = aux.toInt(); //second

    if ( (IntDataArray[2] < 0) || (IntDataArray[2] > 59) ) {
      return EXIT_FAILURE;
    }
    
    rtc.adjust(DateTime(nowDateTime.year(),nowDateTime.month(),nowDateTime.day() , IntDataArray[0], IntDataArray[1], IntDataArray[2]));

  }
  if (commandType == "TEST") {
    
  }

  return EXIT_SUCCESS;
}

void showDateTimeLCD (RTC_DS3231& rtc, DateTime& now) {
    lcd.setCursor(0, 0);

    // now = rtc.now();

    lcd.print((now.hour() < 10) ? "0" : "" );
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print((now.minute() < 10) ? "0" : "" );
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print((now.second() < 10) ? "0" : "" );
    lcd.print(now.second(), DEC);
    lcd.print(" ");

    lcd.write(now.second() & 0x03);
    
    lcd.print(" ");
    lcd.print((now.day() < 10) ? "0" : "" );
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print((now.month() < 10) ? "0" : "" );
    lcd.print(now.month(), DEC);
}

void showDateTimeSerial ( DateTime& now) {

    Serial.print((now.hour() < 10) ? "0" : "" );
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print((now.minute() < 10) ? "0" : "" );
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print((now.second() < 10) ? "0" : "" );
    Serial.print(now.second(), DEC);
    Serial.print(' ');
    Serial.print((now.day() < 10) ? "0" : "" );
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print((now.month() < 10) ? "0" : "" );
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print (rtc.getTemperature());
    Serial.println ("°C");
    // Serial.println();

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

#define LEDboard 13
#define pinInt 2


void setup() {

  #ifdef __AVR_ATmega328P__

  //PORTD 4 a 7 = S1 a S4
  //PORTB 0 a 3 = S5 a S8
  //PORTC 0 a 3 = S9 a S12

  DDRD |= 0xF0;
  DDRB |= 0x0F;
  DDRC |= 0x0F;

  PORTD |= 0xF0;
  delay(500);
  PORTD &= ~0xF0;

  PORTB |= 0x0F;
  delay(500);
  PORTB &= ~0x0F;

  PORTC |= 0x0F;
  delay(500);
  PORTC &= ~0x0F;

  #endif

  pinMode(LEDboard, OUTPUT);
  pinMode (pinInt, INPUT_PULLUP);
//  pinMode (pinInt, INPUT);
  
  Serial.begin(9600);

  if ( rtc.begin()) {
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

  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  DateTime nowAlarm1;
  nowAlarm1 = DateTime (F(__DATE__), F(__TIME__));
  rtc.disableAlarm1();
  rtc.clearFlagAlarm1();
  /*
  DS3231_ALARM_TYPES_t :
    DS3231_ALM1_EVERY_SECOND      //every_second
    DS3231_ALM1_MATCH_SECONDS     //match seconds
    DS3231_ALM1_MATCH_MINUTES     //match minutes *and* seconds
    DS3231_ALM1_MATCH_HOURS       //match hours *and* minutes, seconds
    DS3231_ALM1_MATCH_DATE        //match date *and* hours, minutes, seconds
    DS3231_ALM1_MATCH_DAY         //match day *and* hours, minutes, seconds
  */
  rtc.adjustAlarm1(nowAlarm1, DS3231_ALM1_EVERY_SECOND);
  rtc.enableAlarm1();
  
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

  lcd.print("Clock Evolution ");
  lcd.setCursor(0, 1);
  lcd.print("Ver 1.0 06/2018 ");
  
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.home();
  
  
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
  
  SetParameter(rtc);
  
  if (!digitalRead(2)) {

    rtc.clearFlagAlarm1();

    digitalWrite(LEDboard,!digitalRead(LEDboard));

    now = rtc.now();
    
    showDateTimeLCD (rtc, now);

    showDateTimeSerial ( now );

    showOutLCD ( lcd, convertHourIndexSegment( now.hour() , 0),  convertMinuteIndexSegment( now.minute() , 0));
    showOutPins ( convertHourIndexSegment( now.hour() , 0), convertMinuteIndexSegment( now.minute() , 0));
    
    }

  delay(50);
}
