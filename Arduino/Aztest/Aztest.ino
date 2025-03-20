
#include <LiquidCrystal.h> 

const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int analogPin = A1;
count step = 0;

void setup()
{ 
  Serial.begin(9600);
  lcd.begin(16,2);
  initSpecialChar();
}

void loop()
{
  for(int i = 65; i < 91; i++)
  {
    lcd.clear();
    lcd.write(i);
    
  }
}

void initSpecialChar()
{
  byte AwithRing[8] = {
  B00100,
  B01010,
  B01110,
  B00001,
  B01111,
  B10001,
  B01111,
  };
  
  byte AwithDots[8] = {
  B01010,
  B00000,
  B01110,
  B00001,
  B01111,
  B10001,
  B01111,
  };
  
  byte OwithDots[8] = {
  B01010,
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  };
  
  byte CapitalAwithRing[8] = {
  B00100,
  B01010,
  B01110,
  B10001,
  B11111,
  B10001,
  B10001,
  };
  
  byte CapitalAwithDots[8] = {
  B01010,
  B00000,
  B01110,
  B10001,
  B11111,
  B10001,
  B10001,
  };
  
  byte CapitalOwithDots[8] = {
  B01010,
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  };
  
  lcd.createChar(1, AwithRing);
  lcd.createChar(2, AwithDots);
  lcd.createChar(3, OwithDots);
  lcd.createChar(4, CapitalAwithRing);
  lcd.createChar(5, CapitalAwithDots);
  lcd.createChar(6, CapitalOwithDots);
}
