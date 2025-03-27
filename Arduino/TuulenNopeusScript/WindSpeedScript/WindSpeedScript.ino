// #include <LiquidCrystal.h> 

// const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int signalPin = 7;  // Digital input pin for signal

void setup() {
  Serial.begin(9600);
  pinMode(signalPin, INPUT);
}

void loop() {
  // Measure HIGH duration
  unsigned long highTime = pulseIn(signalPin, HIGH);
  // Measure LOW duration
  unsigned long lowTime = pulseIn(signalPin, LOW);
  
  // Calculate period (HIGH + LOW time)
  unsigned long period = highTime + lowTime;

  if (period > 0) {  // Avoid division by zero
    float frequency = 1000000.0 / period;  // Convert to Hz (microseconds to seconds)
    float windSpeed = -0.24 + frequency * 0.699;
    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");

    Serial.print("Windspeed: ");
    Serial.print(windSpeed);
    Serial.println(" m/s");

  }
  
  delay(500); // Small delay before next measurement
}
