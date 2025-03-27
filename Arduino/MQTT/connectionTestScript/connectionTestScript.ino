#include <Ethernet.h>
#include <PubSubClient.h>

byte server[] = { 10,6,0,21 }; // MQTT-palvelimen IP-osoite
unsigned int Port = 1883;  // MQTT-palvelimen portti
EthernetClient ethClient; // Ethernet-kirjaston client-olio
PubSubClient client(server, Port, ethClient); // PubSubClient-olion luominen

#define outTopic   "ICT4_out_2020" // Aihe, jolle viesti lähetetään

static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x62 }; // MAC-osoite Ethernet-liitäntää varten

char* clientId = "a731fsd4"; // MQTT-clientin tunniste

void setup() {
    Serial.begin(9600); // Sarjaportin alustaminen
    fetch_IP(); // Kutsutaan IP-osoitteen haku-funktiota
}

void loop() {
    send_MQTT_message(); // Kutsutaan MQTT-viestin lähettämis-funktiota
    delay(5000); // 5 sekunnin viive
}

void fetch_IP() {
    bool connectionSuccess = Ethernet.begin(mymac); // Yhdistäminen Ethernet-verkkoon ja tallennetaan yhteyden tila
    if (!connectionSuccess) {
        Serial.println("Failed to access Ethernet controller"); // Jos yhteys ei onnistunut -> yhteysvirheilmoitus
    } else {
        Serial.println("Connected with IP: " + Ethernet.localIP()); // Onnistuessa tulostetaan IP-osoite
    }
}

void send_MQTT_message() {
    if (!client.connected()) { // Tarkistetaan onko yhteys MQTT-brokeriin muodostettu
        connect_MQTT_server(); // Jos yhteyttä ei ollut, kutsutaan yhdistä -funktiota
    }
    if (client.connected()) { // Jos yhteys on muodostettu
        client.publish(outTopic, "Hello from MQTT!"); // Lähetetään viesti MQTT-brokerille
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