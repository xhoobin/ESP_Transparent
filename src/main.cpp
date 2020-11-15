#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager/tree/development

#define MAX_SRV_CLIENTS 1

WiFiManager wifiManager;
ESP8266WebServer webServer(80);

WiFiServer server(8080);
WiFiClient serverClients[MAX_SRV_CLIENTS];

String nameString;

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  // Do a little work to get a unique-ish name. Get the
  // last two bytes of the MAC (HEX'd)":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();

  nameString = "ESP8266-" + macID;

  char nameChar[nameString.length() + 1];
  memset(nameChar, 0, nameString.length() + 1);

  for (int i = 0; i < nameString.length(); i++)
    nameChar[i] = nameString.charAt(i);

  // Serial.printf("Name: %s\n", nameChar );

  // reset settings - wipe credentials for testing
  // wifiManager.resetSettings();

  wifiManager.setConfigPortalBlocking(false);
  // wifiManager.setDebugOutput(false);
  // wifiManager.setConfigPortalTimeout(60);

  //automatically connect using saved credentials if they exist
  //If connection fails it starts an access point with the specified name
  // if(wifiManager.autoConnect(nameChar)){
  //   Serial.println("Wi-Fi connected");
  // }
  // else {
  //   Serial.println("Wi-Fi manager portal running");
  // }


  //start UART and the server
	Serial.begin(115200);
	server.begin();
	server.setNoDelay(true);
}

void loop() {
  // put your main code here, to run repeatedly:

  	uint8_t i;
	//check if there are any new clients
	if (server.hasClient()) {
		for (i = 0; i < MAX_SRV_CLIENTS; i++) {
			//find free/disconnected spot
			if (!serverClients[i] || !serverClients[i].connected()) {
				if (serverClients[i])
					serverClients[i].stop();
				serverClients[i] = server.available();
//				Serial1.print("New client: ");
//				Serial1.print(i);
				break;
			}
		}
		//no free/disconnected spot so reject
		if (i == MAX_SRV_CLIENTS) {
			WiFiClient serverClient = server.available();
			serverClient.stop();
//			Serial1.println("Connection rejected ");
		}
	}

	//check UART for data
	while (Serial.available() > 0) {
		int n=Serial.available();
		char *data=new char[n];
		int len = Serial.readBytes(data, n);
		//push UART data to all connected telnet clients
		for (i = 0; i < MAX_SRV_CLIENTS; i++) {
			if (serverClients[i] && serverClients[i].connected()) {
				serverClients[i].write(data,len);
			}
		}
		delete []data;
	}

	//check clients for data
	for (i = 0; i < MAX_SRV_CLIENTS; i++) {
		if (serverClients[i] && serverClients[i].connected()) {
			if (serverClients[i].available()) {
				//get data from the telnet client and push it to the UART
				while (serverClients[i].available())
					Serial.write(serverClients[i].read());
			}
		}
	}

}