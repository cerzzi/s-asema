#include <LiquidCrystal.h> 

const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int row = 0;
int count = 0;

void setup() {
  lcd.begin(16,2);
  initSpecialChar();
  lcd.setCursor(0,0);
}

void loop() {
  
  for(int i = 65; i < 94; i++)
  {
    lcd.setCursor(row,count);
    lcd.clear();
    lcd.write(i);

    if(row == 0 && count == 15)
    {
      row = 1;
    }

    if(row == 1 && count == 0)
    {
      row = 0;
    }
    
    if(row == 0);
    {
      count++;
    }

    if(row == 1);
    {
      count--;
    }
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
  lcd.createChar(91, CapitalAwithRing);
  lcd.createChar(92, CapitalAwithDots);
  lcd.createChar(93, CapitalOwithDots);
}