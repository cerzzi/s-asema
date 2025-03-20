
#include <LiquidCrystal.h> 

const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int analogPin = A1;
float voltage = 0;
float rawValue = 0;
float temperature = 0;
int count = 0;
int row = 0;
bool lol = false;

unsigned long duration = 10000;

void setup()
{ 
  Serial.begin(9600);
  lcd.begin(16,2);
  initSpecialChar();
  
  while(true)
}

  delay(2000);

  lcd.clear();
  lcd.setCursor(0,1);
  for(int i = 0; i < 4; i++)
  {
     lcd.clear();
     lcd.print("Miettii.");
     delay(700);
     lcd.clear();
     lcd.print("Miettii..");
     delay(700);
     lcd.clear();
     lcd.print("Miettii...");
     delay(700);
  }
}

void loop()
{
  unsigned long startTime = millis();
  rawValue = 0;

  while(millis() - startTime < duration)
  {
    int temp = analogRead(analogPin);
    
    if(rawValue < temp )
    {
      rawValue = temp;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(rawValue);
      lcd.print("   ");
      lcd.print(rawValue * (5.0/1024.0));
      lcd.print("v"); 
    }
  }

  //Jännite arvo
  voltage = rawValue * (5.0/1024.0);

  // lämpötila arvo
  temperature = convertVoltageToTemperature(voltage);
  printValue(temperature);
  delay(2000);
}

void printValue(float value)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");

  lcd.setCursor(0, 1);
  lcd.print(value);
  lcd.print(" C");

  return;
}

// Muunna jännite taulukon lämpötila arvoon
float convertVoltageToTemperature(float voltage)
{
  if(voltage < 1.5)
  {
    return -10;
  }

  else if(voltage < 2.5)
  {
    return -10 + (voltage-1.5) * (10.0/1.0);
  }

  else if(voltage < 3.6)
  {
    return (voltage - 2.5) * (10.0/1.1);
  }

  else if(voltage < 4.5)
  {
    return 10 + (voltage -3.6) * (10.0/0.9);
  }

  else if (voltage <= 4.95)
  {
     return 20 + (voltage-4.5) * (5.0/0.45);
  }

  else
  {
    return 25; //max value
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
