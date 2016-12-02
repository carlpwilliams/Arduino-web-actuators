#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "lordliams";
const char* password = "Sweetpea123";

// pins do not match the arduino on the wemos. these pins are essentially 4,5,6,7.
const int relay1 = 4;
const int relay2 = 14;
const int relay3 = 12;
const int relay4 = 13;

const int relayPairs[2][2] = {{4, 14}, {12, 13}};
const String relayPairMonikers[2] = {"Bar", "TV"};
const String ActionCommandsOn[] = {"open", "on", "extend"};
const String ActionCommandsOff[] = {"close", "off", "retract"};

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Barduino 2.0!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (int i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

bool identifyCommand() {
  String theURL = server.uri();
  theURL.toLowerCase();
  bool foundCommand = false;
  for (int monikaID = 0; monikaID < 2; monikaID++) {
    for (int i = 0; i < 3; i++) {
      Serial.println("checking on action " + ActionCommandsOn[i]);
      String StringToMatch = "/" + relayPairMonikers[monikaID] + "/" + ActionCommandsOn[i];
      StringToMatch.toLowerCase();
      if (theURL.startsWith(StringToMatch)) {
        toggleRelayPair(monikaID, 0);
        handleStatus();
        foundCommand = true;
        return true;
      }
    }
    String StringToMatch = "/" + relayPairMonikers[monikaID] + "/stop";
    StringToMatch.toLowerCase();
    if (theURL.startsWith(StringToMatch)) {
      toggleRelayPair(monikaID, 3);
      handleStatus();
      foundCommand = true;
      return true;
    }

    for (int i = 0; i < 3; i++) {
      Serial.println("checking off action " + ActionCommandsOff[i]);
      String StringToMatch = "/" + relayPairMonikers[monikaID] + "/" + ActionCommandsOff[i];
      StringToMatch.toLowerCase();
      if (theURL.startsWith(StringToMatch)) {
        Serial.println("found command " + StringToMatch);
        toggleRelayPair(monikaID, 1);
        handleStatus();
        foundCommand = true;
        return true;
      }
    }
  }
  if (foundCommand == false) {
    handleNotFound();
  }
}

void setupRelays() {
  int relayPair, pairPins;
  for (relayPair = 0; relayPair < 2; relayPair++) {
    for (pairPins = 0; pairPins < 2; pairPins++) {
      pinMode(relayPairs[relayPair][pairPins], OUTPUT);
    }
  }
}
void relayPairOff(int pairID) {
  digitalWrite(relayPairs[pairID][0], LOW);
  digitalWrite(relayPairs[pairID][1], LOW);
}

void toggleRelayPair(int pair, int relayID) {
  relayPairOff(pair);
  if (relayID != 3) {
    digitalWrite(relayPairs[pair][relayID], HIGH);
  }
}

void relaysOff() {
  relayPairOff(0);
  relayPairOff(1);
}

void barOpen() {
  toggleRelayPair(0, 0);
}

void barClose() {
  toggleRelayPair(0, 1);
}

void testRelays() {
  for (int testPair = 0; testPair < 2; testPair++) {
    toggleRelayPair(testPair, 0);
    delay(500);
    relaysOff();
    toggleRelayPair(testPair, 1);
    delay(500);
    relaysOff();
  }
}
String getStatusText(int relayPair) {
  String Status = "Stationary";
  if (digitalRead(relayPairs[relayPair][0]) == HIGH) {
    Status = "opening";
  }
  else if (digitalRead(relayPairs[relayPair][1]) == HIGH)
  {
    Status = "closing";
  }
  return "{" + relayPairMonikers[relayPair] + ":" + Status + "}";
}

void handleStatus() {
  String Status = getStatusText(0);
  Status = Status + "," + getStatusText(1);

  server.send(200, "application/json", "{""Status"":" + Status + "}");
}

void safetyCheck() {
  if (digitalRead(relay1) == HIGH && digitalRead(relay1) == HIGH) {
    relaysOff();
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("relay pair1 - " + relayPairMonikers[0]);

  Serial.println("Loading...");

  WiFi.begin(ssid, password);

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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/test", []() {
    testRelays();
    server.send(200, "text/plain", "tested");
  });

  server.onNotFound(identifyCommand);

  server.begin();

  Serial.println("HTTP server started");
}

void loop(void) {
  setupRelays();
  server.handleClient();
}
