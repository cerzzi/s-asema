#include <LiquidCrystal.h> 

// LCD pin configuration
const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Analog pin for temperature sensor
const int analogPin = A1;

void setup() { 
    Serial.begin(9600);
    lcd.begin(16,2);
    initSpecialChar();  // Initialize special characters for LCD

    delay(2000);
    lcd.clear();
    lcd.setCursor(0,1);
}

// Function to get a smoothed sensor reading using averaging
float getAverageReading() {
    float sum = 0;  // Sum of readings

    for (int i = 0; i < 10; i++) {  // Collect 10 readings
        sum += analogRead(analogPin);
        delay(500);  // Short delay between readings
    }

    return sum / 10.0;  // Return average reading
}

void loop() {
    float rawValue = getAverageReading();  // Get averaged sensor value
    float voltage = rawValue * (5.0 / 1024.0);  // Convert to voltage
    float temperature = convertVoltageToTemperature(voltage);  // Convert to temperature

    printValue(temperature, voltage, rawValue);  // Display temperature on LCD
    delay(2000);  // Wait before next reading
}

// Function to display temperature on LCD
void printValue(float value ,float voltage, float rawValue) {
    lcd.clear();  // Clear display before updating

    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(value);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("RV:");
    lcd.print(rawValue);
    lcd.print(" ");
    lcd.print("U:");
    lcd.print(voltage);
    


}

// Function to convert voltage to temperature
float convertVoltageToTemperature(float voltage) {
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

