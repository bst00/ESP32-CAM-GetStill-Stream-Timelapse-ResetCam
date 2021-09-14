# ESP32-CAM-TimeLapse

/*****************************************************************************************************
 * ESP32-CAM OV2640 Time Lapse
 *
 *  Customized for my environment by Bob Trevan 02 September 2021
 *
 *   based on code found at:
 *
 *   https://olimex.wordpress.com/2020/06/05/how-to-configure-and-use-esp32-cam-with-arduino-ide-and-linux/
 *   https://randomnerdtutorials.com/
 *   
 *   https://github.com/bitluni/ESP32CamTimeLapse/pull/9/files
 * 
 *   https://www.youtube.com/watch?v=0_pewS4IPN4
 * 
 ******************************************************************************************************/
/*
 *    last mod to lapse.cpp   sprintf(path, "/esp32-cam_%02d", lapseIndex);
 *                            sprintf(path, "/esp32-cam_%02d/tmLapse_%04d.jpg", lapseIndex, fileIndex);
 *                            
 *         mod to camera.cpp  framesize, vflip, hmirror                   
 ******************************************************************************************************/
/*   ESP32-CAM

     Experimenting with ESP in AP and STA modes
     Get Chip ID and display on serial console
     Get and display date and time from NTP Server
     Display connection status RSSI
     Added Camera Reset Button
     TBD ... Add controls for onboard FLASHLIGHT LED
*/
/********************************************************************************************************
   Board Settings:
   Board: "AI Thinker ESP32-CAM"
   Upload Speed: "921600"
   CPU Frequency: "240MHz (WiFi/BT)"
   Flash Frequency: "80MHz"
   Flash Mode: "QIO"
   Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
   Core Debug Level: "None"
   COM Port: Depends *On Your System
**********************************************************************************************************/
