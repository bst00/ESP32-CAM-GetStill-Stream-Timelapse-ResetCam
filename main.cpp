#include <Arduino.h>

/*****************************************************************************************************
 * ESP32-CAM Time Lapse
 *   
 * Source - https://github.com/bitluni/ESP32CamTimeLapse/pull/9/files
 * 
 *          https://www.youtube.com/watch?v=0_pewS4IPN4
 * 
 *****************************************************************************************************/
/*
 *    last mod to lapse.cpp   sprintf(path, "/esp32-cam_%02d", lapseIndex);
 *                            sprintf(path, "/esp32-cam_%02d/tmLapse_%04d.jpg", lapseIndex, fileIndex);
 *                            
 *         mod to camera.cpp  framesize, vflip, hmirror                   
 *****************************************************************************************************/
/*   ESP32-CAM

     Experimenting with ESP in AP and STA modes
     Get Chip ID and display on serial console
     Get and display date and time from NTP Server
     Display connection status RSSI
     Add controls for onboard FLASHLIGHT LED


     Customized for my environment by Bob Trevan 02 September 2021

     based on code found at:
     https://olimex.wordpress.com/2020/06/05/how-to-configure-and-use-esp32-cam-with-arduino-ide-and-linux/
     and   https://randomnerdtutorials.com/
*/
/********************************************************************************************************************
   Board Settings:
   Board: "AI Thinker ESP32-CAM"
   Upload Speed: "921600"
   CPU Frequency: "240MHz (WiFi/BT)"
   Flash Frequency: "80MHz"
   Flash Mode: "QIO"
   Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
   Core Debug Level: "None"
   COM Port: Depends *On Your System
*********************************************************************************************************************/
/*
  ///////////////////////////////////////////////
  //   ESP32-CAM Camera Resolutions            //
  ///////////////////////////////////////////////

  UXGA  1600 x 1200
  SXGA  1280 x 1024
   XGA  1024 x  768
  SVGA   800 x  600
   VGA   640 x  480
   CIF   400 x  296
  QVGA   320 x  240
  HQVGA  240 x  176
  QQVGA  160 x  120

 **************************************/
 /*
  ///////////////////////////////////////////////
  //   ESP32-CAM Camera Settings               //
  ///////////////////////////////////////////////
  sensor_t * s = esp_camera_sensor_get()

  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_sharpness(s, 0);      // (-2 to 2)     << extra info found on esp32.com
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable      // Auto White Balance?
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable      // automatic exposure sensor? (0 or 1)
  s->set_ae_level(s, 0);       // -2 to 2                       // auto exposure levels (-2 to 2)
  s->set_aec_value(s, 300);    // 0 to 1200                     // automatic exposure correction? (0-1200)
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_quality(s, 10);       // 0 to 63
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_gainceiling(s, GAINCEILING_32X); // Image gain (GAINCEILING_x2, x4, x8, x16, x32, x64 or x128) << extra info found on esp32.com
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable      // black pixel correction
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable      // white pixel correction
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable      // lens correction? (1 or 0)
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable      // flip horizontally
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable      // flip Vertically
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable      // downsize enable? (1 or 0)?
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable      // testcard
*/
/***********************************************************************
 *    ESP.restart();      // ESP8266
 *    ESP.reset();        // ESP32
 ************************************************************************/
#include <WiFi.h>
#include "file.h"
#include "camera.h"
#include "lapse.h"

#include "time.h"

///////////////////////////////////////////////
// Replace with your network credentials     //
///////////////////////////////////////////////
// For STA mode                              //
///////////////////////////////////////////////
const char* ssid = "Stargate";
const char* password = "bigBang!";
//const char* ssid = "Unknown";
//const char* password = "none";
///////////////////////////////////////////////
//   For AP mode                             //
///////////////////////////////////////////////
//IPAddress apIP = IPAddress(192, 168, 5, 1);
//const char *hostname = "esp32TimeLapseCam2";
const char* soft_ap_ssid = "ESP32-Cam-FishEye";
const char* ap_password = "esp4IoT!"; // password to connect to this esp32 when in AP mode
//const char* ap_password = "12e45678"; // password to connect to this esp32 when in AP mode
//const char* soft_ap_password = "esp4IoT!";
const char* passphrase = "NULL";
int   ch = 7;
int   ssid_hidden=0;

const char* ChipModel = 0;
uint32_t    chipId = 0;
uint32_t    ChipRev = 0;
uint32_t    ChipCores = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//            NTP Server
/////////////////////////////////////////////////////////////////////////////////////////////////////
//const char* ntpServer = "pool.ntp.org";
const char* ntpServer = "uk.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
IPAddress ntp(192, 168, 1, 123);  //   <<< My Time Tools NTP Server

/////////////////////////////////////////////////////////////////////////////////////////////////////
void startCameraServer();
/////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//   Print Local Time                                           //
//////////////////////////////////////////////////////////////////
void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      Serial.println("");
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//                initWiFi                                      // 
//////////////////////////////////////////////////////////////////
void initWifi(int wifiMode)
{
  if (wifiMode == 0)
  {
    // IP address of the ESP32 when in AP mode
    IPAddress apIP = IPAddress(192, 168, 5, 1);
    const char *hostname = "esp32TimeLapseCam2";
    
    Serial.println("Starting softAP");
    WiFi.mode(WIFI_AP);
    delay(100);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    delay(100);
    bool result = WiFi.softAP(hostname, ap_password, 1, 0);
    if (!result)
    {
        Serial.println("AP Config failed.");
        return;
    }
    else
    {
        Serial.println("=====================================================================");
        Serial.println("AP Config Success.");
        Serial.print("AP MAC: ");
        Serial.println(WiFi.softAPmacAddress());

        IPAddress ip = WiFi.softAPIP();
        Serial.print("IP Address: ");
        Serial.println(ip);

        Serial.print("AP Name: ");
        Serial.println(hostname);

        Serial.print("AP Password: ");
        Serial.println(ap_password);
        Serial.println("=====================================================================");
        Serial.print("softMAC address  : ");
        Serial.println(WiFi.softAPmacAddress());
        Serial.print("softAPIP  : ");
        Serial.println(WiFi.softAPIP());
        Serial.print("Stations count  : ");
        Serial.println(WiFi.softAPgetStationNum());
        Serial.print("Hostname  : ");
        Serial.println(WiFi.getHostname());               //  <<< may not work in AP mode
        Serial.print("Channel  : ");
        Serial.println(WiFi.channel());
        Serial.print("Status  : ");
        Serial.println(WiFi.status());
        Serial.print("psk  : ");
        Serial.println(WiFi.psk());
        Serial.print("BSSID  : ");
        Serial.println(WiFi.BSSIDstr());
        Serial.println("====================================================================="); 
    }
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
      Serial.println("");
      Serial.println("======================================================");
      Serial.print("WiFi connected to SSID: \"");         // <<< escape the "
      Serial.print(WiFi.SSID());
      Serial.print("\", Signal Level: ");
      Serial.println(WiFi.RSSI());
      Serial.print("MAC address  : ");
      Serial.println(WiFi.macAddress());
      Serial.print("softMAC address  : ");
      Serial.println(WiFi.softAPmacAddress());
      Serial.print("softAPIP  : ");
      Serial.println(WiFi.softAPIP());
      Serial.print("Stations count  : ");
      Serial.println(WiFi.softAPgetStationNum());
      Serial.print("Hostname  : ");
      Serial.println(WiFi.getHostname());      //  <<< may not work in AP mode
      Serial.print("Channel  : ");
      Serial.println(WiFi.channel());
      Serial.print("Status  : ");
      Serial.println(WiFi.status());
      Serial.print("psk  : ");
      Serial.println(WiFi.psk());
      Serial.print("BSSID  : ");
      Serial.println(WiFi.BSSIDstr());
      Serial.println("======================================================"); 
      Serial.println("");  
  } 
//////////////////////////////////////////////////////////////////
//   init and get the time                                      //
//////////////////////////////////////////////////////////////////
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
      delay(2000);                                          
  printLocalTime();
}
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Determine on boot: 0 = AP Mode, 1 = STA Mode                    //
/////////////////////////////////////////////////////////////////////
int determineWifiMode()
{
    Serial.println("Scan start..");
    /////////////////////////////////////////////////////////////////
    // WiFi.scanNetworks will return the number of networks found  //
    /////////////////////////////////////////////////////////////////
    int n = WiFi.scanNetworks();
        Serial.print("Scan complete :)");
    if (n == 0) {
        Serial.println("No networks found :(");
        return 0;
    } else {
        Serial.print(n);
        Serial.println(" networks found:");
        for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(100);
        if (WiFi.SSID(i).equals(ssid)) {
          return 1;
            }
        }
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////          Setup          //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial.println();

  Serial.println("         |================================================|");
  Serial.println("         | Sketch:  ESP32-CAM2_Time_Lapse_Auto_STA_AP_RST |");
  Serial.println("         |================================================|");

  ////////////////////////////////////////////
  //   Get ESChip Info                      //
  ////////////////////////////////////////////
    Serial.println("Getting Chip Info...");
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  
  ChipModel = ESP.getChipModel();
  ChipRev   = ESP.getChipRevision();
  ChipCores = ESP.getChipCores();

    Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("This chip has %d cores\n", ESP.getChipCores());
    Serial.print("Chip ID: "); Serial.println(chipId); Serial.println();

  
  ////////////////////////////////////////////
  //              initFS                    //
  //////////////////////////////////////////// 
  initFileSystem();

  ////////////////////////////////////////////
  //             initCamera                 //
  ////////////////////////////////////////////
  initCamera();

    int wifiMode = determineWifiMode();
    Serial.printf("Wifi Mode: %u\n", wifiMode);
  
  ////////////////////////////////////////////
  //             initWiFi                   //
  ////////////////////////////////////////////   
  initWifi(wifiMode);

  ////////////////////////////////////////////
  //   Start Camera Server                  //
  ////////////////////////////////////////////
  startCameraServer();

    Serial.println("Camera Ready! ");
    Serial.print("Use 'http://");
  if (wifiMode == 0)
  {
    Serial.print(WiFi.softAPIP());
  }
  else
  {
    Serial.print(WiFi.localIP());
  }
    Serial.println("' to connect");
}

//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////        End of Setup          /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//                    Main Loop                                 //
//////////////////////////////////////////////////////////////////
void loop()
{
  unsigned long t = millis();
  static unsigned long ot = 0;
  unsigned long dt = t - ot;
  ot = t;
  processLapse(dt);       // bool processLapse() in lapse.cpp
}
