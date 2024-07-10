#define BLYNK_TEMPLATE_ID "TMPL6aD_JAATR"
#define BLYNK_TEMPLATE_NAME "NodeMCUV2"

#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG

// Define the GPIO connected with Relays
#define RelayPin1 5   // D1 (GPIO5)
#define RelayPin2 4   // D2 (GPIO4)
#define RelayPin3 14  // D5 (GPIO14)
#define RelayPin4 12  // D6 (GPIO12)

// Define the virtual pins for the Blynk buttons
#define VPIN_BUTTON_1 V1
#define VPIN_BUTTON_2 V2
#define VPIN_BUTTON_3 V3 
#define VPIN_BUTTON_4 V4

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "BlynkEdgent.h"

// Replace with your network credentials
const char *ssid = "SilenceOfTheLANs";
const char *password = "NewSudoEnter";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 21600); // GMT+6 for Bangladesh

bool previousRelay1State = HIGH; // To store the previous state of Relay 1

// Function to print current time in human-readable format
void printTime() {
  timeClient.update();
  Serial.print(timeClient.getFormattedTime());
  Serial.print(" - ");
}

// Function to control relays based on time
void controlRelaysBasedOnTime() {
  timeClient.update();
  int currentSecond = timeClient.getSeconds();
  bool relay1State = (currentSecond % 6 < 3) ? LOW : HIGH;

  if (relay1State != previousRelay1State) {
    digitalWrite(RelayPin1, relay1State);
    previousRelay1State = relay1State;
    printTime();
    Serial.println(relay1State == LOW ? "Relay 1 is ON (Scheduled)" : "Relay 1 is OFF (Scheduled)");
    Blynk.virtualWrite(VPIN_BUTTON_1, relay1State == LOW ? 1 : 0); // Sync with virtual pin state
  }
}

// Relay Control via Blynk
BLYNK_WRITE(VPIN_BUTTON_1) {
  if (param.asInt() == 1) {
    digitalWrite(RelayPin1, LOW);  // Turn the relay ON
    previousRelay1State = LOW;
    printTime();
    Serial.println("Relay 1 is ON (Virtual Pin)");
  } else { 
    digitalWrite(RelayPin1, HIGH); // Turn the relay OFF
    previousRelay1State = HIGH;
    printTime();
    Serial.println("Relay 1 is OFF (Virtual Pin)");
  }
}

// Relay Control for other relays via Blynk
BLYNK_WRITE(VPIN_BUTTON_2) {
  if (param.asInt() == 1) {
    digitalWrite(RelayPin2, LOW);  // Turn the relay ON
    printTime();
    Serial.println("Relay 2 is ON");
  } else { 
    digitalWrite(RelayPin2, HIGH); // Turn the relay OFF
    printTime();
    Serial.println("Relay 2 is OFF");
  }
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  if (param.asInt() == 1) {
    digitalWrite(RelayPin3, LOW);  // Turn the relay ON
    printTime();
    Serial.println("Relay 3 is ON");
  } else { 
    digitalWrite(RelayPin3, HIGH); // Turn the relay OFF
    printTime();
    Serial.println("Relay 3 is OFF");
  }
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  if (param.asInt() == 1) {
    digitalWrite(RelayPin4, LOW);  // Turn the relay ON
    printTime();
    Serial.println("Relay 4 is ON");
  } else { 
    digitalWrite(RelayPin4, HIGH); // Turn the relay OFF
    printTime();
    Serial.println("Relay 4 is OFF");
  }
}

// Sync the virtual pin state with the Blynk server upon connection
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_3);
  Blynk.syncVirtual(VPIN_BUTTON_4);
}

void setup() {
  // Initialize the relay pins as output
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  // Initialize serial communication for debugging
  Serial.begin(115200);
  delay(100);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize Blynk connection
  BlynkEdgent.begin();

  // Initialize NTP client
  timeClient.begin();
}

void loop() {
  // Run Blynk tasks
  BlynkEdgent.run();
  
  // Update NTP client
  timeClient.update();

  // Control relays based on time
  controlRelaysBasedOnTime();
}
