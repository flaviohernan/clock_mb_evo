#include <Wire.h>
#include <LiquidCrystal_I2C.h>

uint8_t clockIcon3H[8] = {0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00};
uint8_t clockIcon6H[8] = {0x00, 0x0e, 0x15, 0x15, 0x15, 0x0e, 0x00};
uint8_t clockIcon9H[8] = {0x00, 0x0e, 0x15, 0x1D, 0x11, 0x0e, 0x00};
uint8_t clockIcon12H[8] = {0x00, 0x0e, 0x15, 0x15, 0x11, 0x0e, 0x00};


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

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

void setup() {
  lcd.begin();
  lcd.backlight();

  lcd.createChar(0, clockIcon3H);
  lcd.createChar(1, clockIcon6H);
  lcd.createChar(2, clockIcon9H);
  lcd.createChar(3, clockIcon12H);

  lcd.home();

  lcd.print("Hello world...");

  uint8_t contIcon = 0;
  uint8_t contMinutes = 0;
  while(1) {
    lcd.setCursor(0, 1);
    lcd.write(contIcon);
    contIcon ++;
    if (contIcon > 3) {
      contIcon = 0;
    }
    
    lcd.print ("   ");
    lcd.setCursor(2, 1);
    if (contMinutes < 10) {
      lcd.print (0);
    }
    lcd.print (contMinutes);

    lcd.print ("   ");
    lcd.setCursor(5, 1);
    if (convertMinuteIndexSegment( contMinutes , 0) < 10) {
      lcd.print (0);
    }
    lcd.print(convertMinuteIndexSegment( contMinutes , 0));
    contMinutes ++;
    if (contMinutes > 59) {
      contMinutes = 0;
    }
    

    
    
    delay(1000);
  }
  
}


void loop() {
  // put your main code here, to run repeatedly:

}
