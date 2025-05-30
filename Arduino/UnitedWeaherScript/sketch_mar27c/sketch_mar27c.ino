#include <LiquidCrystal.h> 

// LCD pin configuration
const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Analog pin for temperature sensor
const int analogPin = A1;
const int digitalPin = 7;

void setup()
{ 
  pinMode(digitalPin, INPUT);
  Serial.begin(9600);
  lcd.begin(16,2);
  initSpecialChar();  // Initialize special characters for LCD
  lcd.setCursor(4,0);  
  lcd.print("Starting");
  lcd.setCursor(7,1); 
  lcd.print("up");
}

// Function to get a smoothed sensor reading using averaging
float getAverageVoltageReading()
{
  unsigned long duration = 10000;
  unsigned long startTime = millis();
  float rawValue = 0;
  int i = 0;

  while(millis() - startTime < duration)
  {
    rawValue += analogRead(analogPin);
    i++;
  }
  rawValue = rawValue/i;
  return rawValue;
}

float getAverageFrequencyReading()
{
  unsigned long duration = 10000;  // Measure for 10 seconds
  unsigned long startTime = millis();
  unsigned long period = 0;
  unsigned long periodSum = 0;
  int i = 0;

  while (millis() - startTime < duration)
  {
    // Measure HIGH and LOW duration
    unsigned long highTime = pulseIn(digitalPin, HIGH, 1000000);  // Timeout to avoid getting stuck
    unsigned long lowTime = pulseIn(digitalPin, LOW, 1000000);
    unsigned long period = highTime + lowTime;  // Total period of one cycle

    if (period > 0) {
      periodSum += period;
      i++;
    }
  }
  if (i == 0) return 0;  // Prevent division by zero
  float avgPeriod = (float)periodSum / i;  // Average period in microseconds
  float averageFrequency = 1000000.0 / avgPeriod;  // Convert to Hz
  return averageFrequency;
}

void loop()
{
    float rawValue = getAverageVoltageReading();  // Get averaged sensor value
    float voltage = rawValue * (5.0 / 1024.0);  // Convert to voltage
    float temperature = convertVoltageToTemperature(voltage);  // Convert to temperature
    
    float frequency = getAverageFrequencyReading(); // Convert to Hz (microseconds to seconds)
    float windSpeed = -0.24 + frequency * 0.699;
    printValue(temperature, voltage, rawValue, frequency, windSpeed);  // Display temperature on LCD
}

// Function to display temperature on LCD
void printValue(float value ,float voltage, float rawValue, float frequency, float windSpeed) 
{
  lcd.clear();  // Clear display before updating
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); lcd.print(value); lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("RV:"); lcd.print(rawValue);
  lcd.print(" ");
  lcd.print("U:"); lcd.print(voltage);
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("M/S: "); lcd.print(windSpeed);
  lcd.setCursor(0,1);
  lcd.print("F: "); lcd.print(frequency);
}

// Function to convert voltage to temperature
float convertVoltageToTemperature(float voltage) 
{
    if (voltage < 1.5) {
        return -10;
    } else if (voltage < 2.5) {
        return -10 + (voltage - 1.5) * (10.0 / 1.0);
    } else if (voltage < 3.6) {
        return (voltage - 2.5) * (10.0 / 1.1);
    } else if (voltage < 4.5) {
        return 10 + (voltage - 3.6) * (10.0 / 0.9);
    } else if (voltage <= 4.95) {
        return 20 + (voltage - 4.5) * (5.0 / 0.45);
    } else {
        return 25;  // Max temperature limit
    }
}

// Function to initialize special characters on LCD
void initSpecialChar() 
{
    byte AwithRing[8]        = { B00100, B01010, B01110, B00001, B01111, B10001, B01111, B00000 };
    byte AwithDots[8]        = { B01010, B00000, B01110, B00001, B01111, B10001, B01111, B00000 };
    byte OwithDots[8]        = { B01010, B00000, B01110, B10001, B10001, B10001, B01110, B00000 };
    byte CapitalAwithRing[8] = { B00100, B01010, B01110, B10001, B11111, B10001, B10001, B00000 };
    byte CapitalAwithDots[8] = { B01010, B00000, B01110, B10001, B11111, B10001, B10001, B00000 };
    byte CapitalOwithDots[8] = { B01010, B00000, B01110, B10001, B10001, B10001, B01110, B00000 };

    lcd.createChar(1, AwithRing);
    lcd.createChar(2, AwithDots);
    lcd.createChar(3, OwithDots);
    lcd.createChar(4, CapitalAwithRing);
    lcd.createChar(5, CapitalAwithDots);
    lcd.createChar(6, CapitalOwithDots);
}

