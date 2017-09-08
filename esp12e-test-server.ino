#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "240 Life";
const char* password = "boostislife";
const char* host = "led";

ESP8266WebServer server(80);

const int led = 5;

void handleLed() {
	String ledStatus = server.arg(0);

	digitalWrite(led, ledStatus.toInt());

	server.send(200, "text/plain", "LED update successfull");
}

void connectToWifi() {
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
	  delay(500);
	  Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if(MDNS.begin(host)) {
		Serial.println("MDNS responder started");
	}
}

void setup(void){
	pinMode(led, OUTPUT);
	digitalWrite(led, 0);

	Serial.begin(115200);

	connectToWifi();

	server.on("/led", HTTP_POST, handleLed);

	server.begin();
	Serial.println("HTTP server started");
}

void loop(void){
	server.handleClient();
}
