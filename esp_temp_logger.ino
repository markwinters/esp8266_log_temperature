#include <ESP8266WiFi.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// Configure WIFI 
const char* ssid         = "{WIFI NAME}";
const char* password     = "{PASSWORD}";

// Server IP
const char* serverIp     = "192.168.1.80"; 

// Configure One Wire
#define ONE_WIRE_BUS 0 // GPIO 0
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

void setup() {
  // We use the high baudrate that the esp8266 uses
  Serial.begin(115200);
  delay(10);

  // config static IP
  IPAddress ip(192, 168, 1, 67);
  IPAddress gateway(192, 168, 1, 1);
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0); 
  WiFi.config(ip, gateway, subnet);

  // Connect to wifi 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Show IP 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start temperature sensor
  sensors.begin(); 
  // Locate sesonsors
  Serial.print("Sensor count: ");
  Serial.print(sensors.getDeviceCount(), DEC);
  // Report sensor parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) {
    Serial.println("ON");
  } else { 
    Serial.println("OFF");
  }
}

void loop() {

  // Wait 1 minute
  delay(60000);

  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures();
  Serial.println("DONE"); 
  Serial.print("Temperature is: "); 
  float temp = sensors.getTempCByIndex(0);
  Serial.print(temp); 
  
  Serial.print("connecting to ");
  Serial.println(serverIp);
  WiFiClient client;
  if (!client.connect(serverIp, 80)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/sensoren/woonkamer_temperatuur.php?temp=" + String(temp);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + serverIp + "\r\n" + 
      "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}

