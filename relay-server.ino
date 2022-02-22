#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define led_green 2
#define led_red 4
#define relay 5

const char* ssid = "TP-Link_FF2C";
const char* password = "92925618";
int status = LOW;
WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  String message = "RELAY IS NOW: ";
  if(status == HIGH){
    message += "ON";
  } else {
    message += "OFF";
  }
  
  message += "\n\n";
  message += "<br><br>";
  message += "Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<br>";
  message += "Turn <a href=\"/RELAY=ON\">ON</a> RELAY<br>";
  server.send(200, "text/html", message);
  digitalWrite(led, 0);
}

void led_on() {
  status = HIGH;
  Serial.println("RELAY STATUS: ON");
  server.send(200, "text/plain", "RELAY STATUS: ON");
}

void led_off() {
  status = LOW;
  Serial.println("RELAY STATUS: OFF");
  server.send(200, "text/plain", "RELAY STATUS: OFF");
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(relay, OUTPUT);
  
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(relay, HIGH);
  delay(500);
  digitalWrite(relay, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(led_green, HIGH);
    delay(1000);
    digitalWrite(led_green, LOW);
  }
  digitalWrite(led_green, LOW);
  digitalWrite(led_green, HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/RELAY=ON", led_on);
  server.on("/RELAY=OFF", led_off);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  if(status == HIGH){
  digitalWrite(relay, HIGH);
  digitalWrite(led_red, HIGH);
  } else {
    digitalWrite(relay, LOW);
  digitalWrite(led_red, LOW);
  }
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
