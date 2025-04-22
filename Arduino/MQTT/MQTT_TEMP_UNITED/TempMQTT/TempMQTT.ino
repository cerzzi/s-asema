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
const int rs = 9, en = 6, d4 = 5, d5 = 3, d6 = 4, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Analog pin for temperature sensor
const int analogPin = A1;
const int digitalPin = 7;

float rawValue = 0;
float averageFrequency = 0;

float voltage = 0;
float temperature = 0;
float windSpeed = 0;
int press = 1;
float frec = 0;
bool swap = false;
bool swap1 = false;

void setup()
{ 
  pinMode(digitalPin, INPUT);
  Serial.begin(9600);
  fetch_IP(); // Kutsutaan IP-osoitteen haku-funktiota
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleButtonPress, FALLING);
  lcd.begin(16,2);
  initSpecialChar();  // Initialize special characters for LCD
  lcd.setCursor(4,0);  
  lcd.print("Starting");
  lcd.setCursor(7,1); 
  lcd.print("up");
}

void onButtonPress()
{
  press++;
  if(press == 5) press = 1;
  Serial.print(press);
  Serial.println(".NAPPIASDASDA");
  printValue();

  if(press == 3)
  {
    if(!swap1) swap1 = true;
    else swap1 = false;
  }
}

void handleButtonPress() {
  onButtonPress();
}

void getAverageReadings()
{
  unsigned long duration = 10000;  // Measure for 10 seconds
  unsigned long startTime = millis();
  rawValue = 0;
  averageFrequency = 0;

  unsigned long periodSum = 0;
  int fI = 0;
  int vI = 0;

  while (millis() - startTime < duration)
  {
    // Measure HIGH and LOW duration
    unsigned long highTime = pulseIn(digitalPin, HIGH, 1000000);  // Timeout to avoid getting stuck
    unsigned long lowTime = pulseIn(digitalPin, LOW, 1000000);
    unsigned long period = highTime + lowTime;  // Total period of one cycle
    
    Serial.print("Period:");
    Serial.println(period);
    if (period > 0)
    {
      periodSum += period;
      fI++;
    }
    rawValue += analogRead(analogPin);
    vI++;
  }

  if (fI == 0) averageFrequency = 0; // Prevent division by zero

  else
  {
    float avgPeriod = periodSum / fI;  // Average period in microseconds
    averageFrequency = 1000000.0 / avgPeriod;  // Convert to Hz
  }
  rawValue = rawValue/vI;
  Serial.println(fI);
  Serial.print("average frec:");
  Serial.println(averageFrequency);
}

void loop()
{
  getAverageReadings();  // Get averaged sensor values
  frec = averageFrequency;
  voltage = rawValue * (5.0 / 1024.0);  // Convert to voltage
  temperature = convertVoltageToTemperature(voltage);  // Convert to temperature
    
  windSpeed = -0.24 + frec * 0.699;

  printValue();
  send_MQTT_message(temperature, windSpeed); // Kutsutaan MQTT-viestin lähettämis-funktiota
  Serial.println("PAM");
}

// Function to display temperature on LCD
void printValue() 
{
  switch (press) {
  case 1:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Ethernet.localIP());
    break;
  case 2:
    lcd.clear();  // Clear display before updating
    lcd.setCursor(0, 0);
    lcd.print("Temp: "); lcd.print(temperature); lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("M/S: "); lcd.print(windSpeed);
    break;
  case 3:
    lcd.clear();  // Clear display before updating
    lcd.setCursor(0, 0);
    
    if(swap1)
    {
      lcd.print("Voltage: "); lcd.print(voltage);
    }
    else
    {
      lcd.print("F: "); lcd.print(frec);
    }
    break;
  case 4:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Toimmii :)");
    break;
  default:
    lcd.clear();
    lcd.print("ERROR :)");
}
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