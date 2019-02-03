#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 8
#define DHTTYPE DHT22


#include <SPI.h>
#include <OneWire.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <Time.h>
#define cs 10
#define dc 9
//uint32_t delayMS;

DHT_Unified dht(DHTPIN, DHTTYPE);

//DHT dht(DHTPIN, DHTTYPE);
//const int ledPin = 7; // pin number where led is connected to
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 35, 5);
IPAddress subnet(255,255,255,0);
IPAddress myDns(210, 220, 163, 82);
IPAddress gateway(192,168,35,1);
EthernetClient client;
sensors_event_t event;

IPAddress server(143,248,55,74);  // numeric IP for server
//char server[] = "mazing.mli.kr";
String data;
String station_name = "하진역";
int ClientID = 0;
int Period = 30; // in seconds

void setup() {


  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initialize Ethernet with DHCP:");
  Ethernet.init(10);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    //Ethernet.begin(mac, ip, myDns, gateway, subnet);Ethernet.begin(mac, ip, myDns, gateway, subnet);
    Serial.print("   My IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }

  dht.begin();

  delay(1000);
  data = "";
}

void loop() {

  dht.temperature().getEvent(&event);
  float t = event.temperature;
  dht.humidity().getEvent(&event);
  float h = event.relative_humidity;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  data = "{\"id\": \"";
  data.concat(ClientID);
  data.concat("\", \"name\": \"");
  data.concat(station_name);
  data.concat("\", \"temperature\": \"");
  data.concat(t);
  data.concat("\", \"humidity\": \"");
  data.concat(h);
  data.concat("\"}");
  Serial.println("Send following data to server... ");
  Serial.print(data);
  Serial.println();

  int state = client.connect(server,2203);
  if (state) { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST /save_state HTTP/1.1");
    client.println("Host: mazing.mli.kr"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/json;charset=utf-8");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);

    Serial.println("** Sending Complete!");
  } else {
    Serial.println("No connection with server");
    Serial.println(state);
    delay(2000);
    return;
  }

  if (client.connected()) {
  Serial.println("** Disconnect fom the server");
  client.stop(); // DISCONNECT FROM THE SERVER
  }

  delay(1000 * Period); // WAIT "Period" seconds
}
