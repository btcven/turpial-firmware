/**
 * @file WAP.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */
#include <WiFi.h>
#include "WAP.h"
#include "NVS/NVStorage.h"
#include "hal/hardware.h"

esp_err_t WAP_INIT()
{
  NVStorage nvs;

  // Declare and define default values, before reading NVS
  char *apSSID = WAP_SSID;
  char *apPassword = WAP_PASS;
  int32_t apChannel = WAP_CHANNEL;
  int32_t apMaxConn = WAP_MAXCONN;

  // Open nvs
  bool isOpen = nvs.open(NVS_WIFI_NAMESPACE, false);
  // Verifying if opened
  if (isOpen)
  {
      // This is just for testing purposes
      //=========================================================
      // Change the Wi-Fi mode permanently here...
      //nvs.setString(NVS_WAP_SSID_KEY, "Turpial");
      //nvs.setString(NVS_WAP_PASS_KEY, "123456789");
      //nvs.setInt(NVS_WAP_CHANNEL_KEY, 6);
      //nvs.setInt(NVS_WAP_MAXCONN_KEY, 5);
      //=========================================================

      // get values from nvs or set default values from hal/hardware.h
      apSSID = nvs.getString(NVS_WAP_SSID_KEY, WAP_SSID);
      apPassword = nvs.getString(NVS_WAP_PASS_KEY, WAP_PASS);
      apChannel = nvs.getInt(NVS_WAP_CHANNEL_KEY, WAP_CHANNEL);
      apMaxConn = nvs.getInt(NVS_WAP_MAXCONN_KEY, WAP_MAXCONN);
  }
  else
  {
      ESP_LOGE(__func__, "Error opening the NVS");
  }
  
  bool initAP = WiFi.softAP(apSSID, apPassword, apChannel, 0, apMaxConn);

  if (initAP)
  {
    ESP_LOGD(__func__, "AP init OK");
    return ESP_OK;
  }
  else
  {
    ESP_LOGE(__func__, "AP init ERROR");
    return ESP_FAIL;
  }
}

/*
void responses_WAP(WiFiClient client)
{
  if (client)
  { // if you get a client,
    ESP_LOGD(__func__, "New Client");
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        // if the byte is a newline character
        if (c == '\n')
        {
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no, user-scalable=no'><title>Locha.mesh.io</title></head><body><H1>Hi! from Turpial</H1></body></html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
  }
}

*/
