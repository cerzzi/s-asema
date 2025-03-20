#include <LiquidCrystal.h> 

const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int row = 0;
int count = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  initSpecialChar();
  lcd.setCursor(0,0);
}

void loop() {
  
  for(int i = 65; i < 94; i++)
  {
    delay(250);
    lcd.clear();
    lcd.setCursor(count,row);
    Serial.println(count);

    if(i > 90)
    {
      lcd.write(i - 90);
    }
    
    else
    {
      lcd.write(i);
    }

    if(row == 0 && count == 15)
    {
      row = 1;
      Serial.println("juu");
      count++;
    }

    else if(row == 0)
    {
      Serial.println("ei");
      count++;
    }

    if(row == 1 && count == 0)
    {
      row = 0;
    }
    
    else if(row == 1)
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
  
  lcd.createChar(1, CapitalAwithRing);
  lcd.createChar(2, CapitalAwithDots);
  lcd.createChar(3, CapitalOwithDots);
  lcd.createChar(4, AwithRing);
  lcd.createChar(5, AwithDots);
  lcd.createChar(6, OwithDots);
}