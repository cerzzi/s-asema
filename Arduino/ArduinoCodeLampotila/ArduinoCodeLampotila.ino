  
  const int analogPin = A1;
  float rawValue = 0;
  float voltageValue = 0;
  float temperatureValue = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  rawValue = 0;
  for(int i = 0; i <= 10; i++)
  {
    int temp = analogRead(analogPin);
    if(rawValue < temp)
    {
      rawValue = temp;
      Serial.println("New High Score!"+ String(rawValue));
    }
      Serial.println(temp);
      delay(1000);
  }
  
  Serial.println("=========================================");

  Serial.println(rawValue);
  //Jännite arvo
  voltageValue = convertInputToVoltage(rawValue);
  Serial.println(String(voltageValue) + "V");

  // lämpötila arvo
  temperatureValue = convertVoltageToTemperature(voltageValue);
  
  // Kaikki hyvin
  if(temperatureValue != 999)
  {
    Serial.println("Temperatue " + String(temperatureValue) + "C");
  }

  Serial.println("=========================================");
}

// Muunna raaka arvo jännitteekis
float convertInputToVoltage(int input)
{
  return input * (5.0/1024.0); 
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
    return 0 + (voltage - 2.5) * (10.0/1.1);
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
