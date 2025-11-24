/*
 * Example: ESP32 using SdFat (SdFat2) + WiFi
 * Based on ESP32_SD_WiFi example but using SdFat library
 *
 * Place in MultiFTPServer/examples/ESP32_SD_SdFat2/
 */

#include "Arduino.h"
#include <WiFi.h>
#include "FS.h"
#include <SPI.h>
#include <SdFat.h>
#include <sdios.h>

// Create the SdFat instance expected by the library
SdFat sd;

#include <SimpleFTPServer.h>

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

FtpServer ftpSrv;

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace){
  switch (ftpOperation) {
    case FTP_CONNECT:
      Serial.println(F("FTP: Connected!"));
      break;
    case FTP_DISCONNECT:
      Serial.println(F("FTP: Disconnected!"));
      break;
    case FTP_FREE_SPACE_CHANGE:
      Serial.printf("FTP: Free space change, free %u of %u!\n", freeSpace, totalSpace);
      break;
    default:
      break;
  }
}

void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize){
  switch (ftpOperation) {
    case FTP_UPLOAD_START:
      Serial.println(F("FTP: Upload start!"));
      break;
    case FTP_UPLOAD:
      Serial.printf("FTP: Upload of file %s byte %u\n", name, transferredSize);
      break;
    case FTP_TRANSFER_STOP:
      Serial.println(F("FTP: Finish transfer!"));
      break;
    case FTP_TRANSFER_ERROR:
      Serial.println(F("FTP: Transfer error!"));
      break;
    default:
      break;
  }
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print(F("Initializing SdFat (SdFat2)..."));
  SPI.begin();
  if (sd.begin(SS)){
    Serial.println(F("done."));
  } else {
    Serial.println(F("fail."));
    Serial.println(F("SdFat initialization failed. Check CS pin and wiring."));
    while (true) { delay(1000); }
  }

  ftpSrv.setCallback(_callback);
  ftpSrv.setTransferCallback(_transferCallback);

  Serial.println("Start FTP with user: user and passwd: password!");
  ftpSrv.begin("user","password");
}

void loop(){
  ftpSrv.handleFTP();
}

