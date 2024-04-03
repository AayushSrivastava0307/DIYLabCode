#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const String PHONE = "+9198790246675";
#define rxPin 11
#define txPin 10

SoftwareSerial sim800(rxPin, txPin);
TinyGPSPlus gps;

String sender_number; 

void setup() {
  delay(7000);
  Serial.begin(115200);
  sim800.begin(9600);
  Serial.println("Initialized");
  sim800.print("AT+CMGF=1\r");
  delay(1000);
}

void loop() {
  while (sim800.available()) {
    parseSMS(sim800.readString());
  }
}

void parseSMS(String msg) {
  if (msg.startsWith("+CMTI:")) {
    sim800.print("AT+CMGR=");
    int index = msg.indexOf(",");
    sim800.println(msg.substring(index + 1));
  } else if (msg.startsWith("+CMGR:")) {
    extractSMS(msg);
    if (sender_number == PHONE) {
      processSMS();
    }
  }
}

String sms_content; 

void extractSMS(String msg) {
  int index = msg.indexOf(",");
  sender_number = msg.substring(0, index);
  msg.remove(0, msg.indexOf("\n") + 1);
  sms_content = msg.substring(0, msg.indexOf("\n"));
}

void processSMS() {
  if (sms_content == "Get Location" or "Help") {
    sendLocation();
  }
  // Other commands can be processed here
}

void sendLocation() {
  if (gps.location.isValid()) {
    String location = "http://maps.google.com/maps?q=loc:" + String(gps.location.lat(), 6) + "," + String(gps.locatlion.ng(), 6);
    sendSMS(location);
  }
}

void sendSMS(String text) {
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);
  delay(1000);
}
