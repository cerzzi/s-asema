#include <LiquidCrystal.h> 
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TimerOne.h>

byte server[] = { 10,6,0,23 }; // MQTT-palvelimen IP-osoite
unsigned int Port = 1883;  // MQTT-palvelimen portti
EthernetClient ethClient; // Ethernet-kirjaston client-olio
PubSubClient client(server, Port, ethClient); // PubSubClient-olion luominen

#define outTopic   "ICT4_out_2020" // Aihe, jolle viesti lähetetään

static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x62 }; // MAC-osoite Ethernet-liitäntää varten

char* clientId = "a731fsd4"; // MQTT-clientin tunniste

// LCD pin configuration
const int rs = 12, en = 11, d4 = 5, d5 = 3, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Analog pin for temperature sensor
const int analogPin = A1;
const int digitalPin = 7;

float temperature = 0;
float windSpeed = 0;

bool swap = true;

void setup()
{ 
  pinMode(digitalPin, INPUT);
  Serial.begin(9600);
  fetch_IP(); // Kutsutaan IP-osoitteen haku-funktiota
  lcd.begin(16,2);
  initSpecialChar();  // Initialize special characters for LCD
  lcd.setCursor(4,0);  
  lcd.print("Starting");
  lcd.setCursor(7,1); 
  lcd.print("up");
  Timer1.initialize(10000000);
  Timer1.attachInterrupt(My_timer_int_routine);
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
    temperature = convertVoltageToTemperature(voltage);  // Convert to temperature
    
    float frequency = getAverageFrequencyReading(); // Convert to Hz (microseconds to seconds)
    windSpeed = -0.24 + frequency * 0.699;
    printValue(temperature, voltage, rawValue, frequency, windSpeed);
}

// Function to display temperature on LCD
void printValue(float value ,float voltage, float rawValue, float frequency, float windSpeed) 
{
  lcd.clear();  // Clear display before updating
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); lcd.print(value); lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("M/S: "); lcd.print(windSpeed);
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

void My_timer_int_routine()
{
  send_MQTT_message(temperature, windSpeed); // Kutsutaan MQTT-viestin lähettämis-funktiota
  Serial.println("PAM");
}



void fetch_IP() {
    bool connectionSuccess = Ethernet.begin(mymac); // Yhdistäminen Ethernet-verkkoon ja tallennetaan yhteyden tila
    if (!connectionSuccess) {
        Serial.println("Failed to access Ethernet controller"); // Jos yhteys ei onnistunut -> yhteysvirheilmoitus
    } else {
        Serial.println("Connected with IP: " + Ethernet.localIP()); // Onnistuessa tulostetaan IP-osoite
    }
}

void send_MQTT_message(float temp, float speed) 
{
    static char S_value[15];
    String S_name = "";

    if(swap)
    {
      S_name = "NJS_Temperature";
      dtostrf(temp,5,3, S_value);
      swap = false;
      Serial.println("temp");
    }

    else
    {
      S_name = "NJS_WindSpeed";
      dtostrf(speed,5,3, S_value);
      swap = true;
      Serial.println("wind");
    }

    char bufa[50];
    sprintf(bufa, "IOTJS={\"S_name\":\"%s\",\"S_value\":%s}", S_name.c_str(), S_value);

    if (!client.connected()) { // Tarkistetaan onko yhteys MQTT-brokeriin muodostettu
        connect_MQTT_server(); // Jos yhteyttä ei ollut, kutsutaan yhdistä -funktiota
    }
    if (client.connected()) { // Jos yhteys on muodostettu
        client.publish(outTopic, bufa); // Lähetetään viesti MQTT-brokerille
        Serial.println("Message sent to MQTT server."); // Tulostetaan viesti onnistuneesta lähettämisestä
    } else {
        Serial.println("Failed to send message: not connected to MQTT server."); // Ei yhteyttä -> Yhteysvirheilmoitus
    }
}

void connect_MQTT_server() { 
    Serial.println("Connecting to MQTT"); // Tulostetaan vähän info-viestiä
    if (client.connect(clientId)) { // Tarkistetaan saadaanko yhteys MQTT-brokeriin
        Serial.println("Connected OK"); // Yhdistetty onnistuneesti
    } else {
        Serial.println("Connection failed."); // Yhdistäminen epäonnistui
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