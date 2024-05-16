/*
  LoRa - Controller
  Central Controller for
  Uses ESP32
  Requires LoRa Library by Sandeep Mistry - https://github.com/sandeepmistry/arduino-LoRa
*/

// Include required libraries
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Logger.h>

// Define the pins used by the LoRa module
const int csPin = 5;      // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 2;     // Must be a hardware interrupt pin

// Start LoRa module at local frequency
// 433E6 for Asia
// 433E6 for Europe
// 866E6 for Europe
// 915E6 for North America
const long loraFrequency = 433E6;

const Logger::Level logLevel = Logger::VERBOSE;

int msgCount = 0;

// Source and sensorAddress1 addresses
byte localAddress = 0x01;    // Address of this device (Controller = 0x01)
byte clientAddress1 = 0xAA;  // Address of Sensor 1
byte clientAddress2 = 0xBB;  // Address of Sensor 2

String clientData1 = "";
String clientData2 = "";


// Remote sensor time variables
unsigned long clientLastActive1 = millis();
unsigned long clientLastActive2 = millis();
const long checkInterval = 60000;  // 60 second client check interval


// FUNCTION getValue() - Extract value from delimited string
String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// FUNCTION onReceive() - Receive call-back function
void onReceive(int packetSize) {
    if (packetSize == 0) return;  // if there's no packet, return

    // read packet header bytes:
    int recipient = LoRa.read();        // recipient address
    byte sender = LoRa.read();          // sender address
    byte incomingMsgId = LoRa.read();   // incoming msg ID
    byte incomingLength = LoRa.read();  // incoming msg length

    String incoming = "";  // payload of packet

    while (LoRa.available()) {        // can't use readString() in callback, so
        incoming += (char) LoRa.read();  // add bytes one by one
    }

    if (incomingLength != incoming.length()) {  // check length for error
        Serial.println("error: message length does not match length");
        return;  // skip rest of function
    }

    // if the recipient isn't this device
    if (recipient != localAddress) {
        Serial.println("This message is not for me.");
        return;  // skip rest of function
    }

    // Determine sender, then update data variables and time stamps
    if (sender == clientAddress1) {
        //Remote Sensor 1
        clientData1 = incoming;
        clientLastActive2 = millis();
    } else if (sender == clientAddress2) {
        //Remote Sensor 2
        clientData2 = incoming;
        clientLastActive2 = millis();
    }

    Logger::log(logLevel, "Received message:");
    Logger::log(logLevel, "  Sender:" + String(sender));
    Logger::log(logLevel, "  Recipient:" + String(recipient));
    Logger::log(logLevel, "  Message ID:" + String(incomingMsgId));
    Logger::log(logLevel, "  Message:" + String(incoming));
}



// FUNCTION sendMessage() - Send LoRa Packet
void sendMessage(String outgoing, byte target) {
    LoRa.beginPacket();             // start packet
    LoRa.write(target);             // add sensorAddress1 address
    LoRa.write(localAddress);       // add sender address
    LoRa.write(msgCount);           // add message ID
    LoRa.write(outgoing.length());  // add payload length
    LoRa.print(outgoing);           // add payload
    LoRa.endPacket();               // finish packet and send it
    msgCount++;                     // increment message ID

    Logger::log(logLevel, "Sent message:");
    Logger::log(logLevel, "  Target:" + String (target));
    Logger::log(logLevel, "  Sender:" + String (localAddress));
    Logger::log(logLevel, "  Message ID:" + String (msgCount));
    Logger::log(logLevel, "  Message:" + String (outgoing));

}

// FUNCTION  getValues() - get the temperature and humidity values from the data variables
void getValues() {
    // Check to see if sensors have reported in recently
    // Get current timestamp value
    unsigned long currentMillis = millis();

}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Setup LoRa module
    LoRa.setPins(csPin, resetPin, irqPin);

    Serial.println("LoRa Receiver Test");


    if (!LoRa.begin(loraFrequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    LoRa.onReceive(onReceive);
    LoRa.receive();
    Serial.println("LoRa init succeeded.");
}

void loop() {
    // Send message to remote 1
    String outMsg1 = "OLDS_PING#1";
    outMsg1 = outMsg1 + msgCount;
    sendMessage(outMsg1, clientAddress1);

    // Delay 30 seconds for receive
    delay(30000);

    // Send message to remote 2
    String outMsg2 = "OLDS_PING#2";
    outMsg2 = outMsg2 + msgCount;
    sendMessage(outMsg2, clientAddress2);

    // Place LoRa back into Receive Mode
    LoRa.receive();

    // Delay 30 seconds for receive
    delay(30000);
}