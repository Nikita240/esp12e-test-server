#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

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


	//called when the url is not defined here
	//use it to load content from SPIFFS
	server.onNotFound([](){
		if(!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.begin();
	Serial.println("HTTP server started");
}

void loop(void){
	server.handleClient();
}

String getContentType(String filename){
	if(server.hasArg("download")) return "application/octet-stream";
	else if(filename.endsWith(".htm")) return "text/html";
	else if(filename.endsWith(".html")) return "text/html";
	else if(filename.endsWith(".css")) return "text/css";
	else if(filename.endsWith(".js")) return "application/javascript";
	else if(filename.endsWith(".png")) return "image/png";
	else if(filename.endsWith(".gif")) return "image/gif";
	else if(filename.endsWith(".jpg")) return "image/jpeg";
	else if(filename.endsWith(".ico")) return "image/x-icon";
	else if(filename.endsWith(".xml")) return "text/xml";
	else if(filename.endsWith(".pdf")) return "application/x-pdf";
	else if(filename.endsWith(".zip")) return "application/x-zip";
	else if(filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path){
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);
	if(path.endsWith("/")) path += "index.htm";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if(SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}
