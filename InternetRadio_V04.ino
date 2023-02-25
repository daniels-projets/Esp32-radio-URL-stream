/*
      Connexion TFT         TOUCH          Connexion VS1053             
              Vcc 3,3v                                           
              Gnd                                                
     Tft CS   25            Miso 12                              
     Tft Rst  -1, Vcc     T CLK  14       Vcc 5v         --      
     Tft DC   26          T CS   13       Gnd       Miso 19      
     Tft Mosi 27          T Din  27       Mosi 23   Sck  18      
     Tft SCK  14          T D0   12       Dreq  4   Xrst en      
     Tft LED  Vcc         T Irq  --       Xcs   5   Xdcs 16                                 
                                                                
     User_Setup.h
     #define USER_SETUP_INFO     #define SPI_FREQUENCY  40000000
     #define ILI9341_DRIVER      #define SPI_READ_FREQUENCY  20000000 
     #define TFT_MOSI 27         #define SPI_TOUCH_FREQUENCY  2500000
     #define TFT_SCLK 14         #define USE_HSPI_PORT
     #define TFT_CS   25         
     #define TFT_DC   26 
     #define TFT_RST  -1
     #define TFT_MISO 12 
     #define TOUCH_CS 13            
     
     SOURCES D'INSPIRATION
     http://electroniqueamateur.blogspot.com/2021/03/esp32-et-vs1053-ecouter-la-radio-sur.html
     http://electroniqueamateur.blogspot.com/2021/05/utilisation-dun-ecran-tactile-tft-spi.html
     https://github.com/maditnerd/st7789_bitmap
     https://github.com/Bodmer/TFT_eSPI   
     https://github.com/baldram/ESP_VS1053_Library
     PARTIE GRAPHIQUE
     https://www.deviantart.com/red888guns/art/Olympus-2207-Interfaces-6-666155877     
     GIMP-2.10
     http://www.rinkydinkelectronics.com/t_imageconverter565.php
     
*/

#include <SPI.h>
#include <TFT_eSPI.h>
          TFT_eSPI tft = TFT_eSPI(); // 
#include "Free_Fonts.h"
//        tft.setFreeFont(TT1);
//        tft.setFreeFont(FSS9);// 9,12,18,24
#include "fondradio.h"

#define   NOMBRE_BOUTONS 2
          TFT_eSPI_Button bouton[NOMBRE_BOUTONS];
          uint16_t calData[6] = { 504, 3290, 459, 3092, 7 };
          uint8_t calDataOK = 0;
          int numero;          

#include <VS1053.h> 
         #define VS1053_CS     5
         #define VS1053_DCS    16
         #define VS1053_DREQ   4
         
#include <ESP32_VS1053_Stream.h>
          ESP32_VS1053_Stream stream;
   String stationOrg,station, morceau, interpreteOrg, interprete, titreOrg, titre, streamFormat;
      int longStation, pixStation, posStation, longMorceau, tiret, cesure;
      int longInterprete, pixInterprete, posInterprete;
      int longTitre, pixTitre, posTitre;
      int chaine = 0;
      int max_chaines = 15;
      bool newChaine = true;

const char* SSID = "@@@@@@@@@";
const char* PSK = "@@@@@@@@@@";

//-----------------------------------------------------------------------------------------------

void setup() {

     Serial.begin(115200); Serial.println();

     tft.init();
     tft.setRotation(3);
     tft.setSwapBytes(true);
//     touch_calibrate();  // procédure de calibration de l'écran tactile

     tft.fillScreen(TFT_BLACK);
     tft.setTextColor(TFT_ORANGE);
     tft.setFreeFont(FSS9);// 9,12,18,24     
     tft.pushImage(0, 0,  320, 240, fondradio);
     tft.pushImage(10, 179, 75, 50, down);
     tft.pushImage(230, 179, 75, 50, up);

     tft.drawString("Esp32  &  VS1053", 80, 40, 1); delay(500);          
     tft.drawString("Internet  Radio", 92, 80, 1); delay(500); 
     tft.setTextColor(TFT_CYAN);
     tft.drawString("Connextion ...", 95, 120, 1);     
     tft.setTextColor(TFT_ORANGE);
     WiFi.begin(SSID, PSK);
     while (!WiFi.isConnected())
             delay(10);
     tft.pushImage(20, 111,  250, 35, zonetitre); delay(500);
     tft.drawString("Connecte", 95, 120, 1); delay(500);

     tft.setTextColor(TFT_CYAN);
     tft.drawString("Stream ...", 115, 200, 1);
     tft.setTextColor(TFT_ORANGE);
     SPI.begin();  /* start SPI before starting decoder */    
     stream.startDecoder(VS1053_CS, VS1053_DCS, VS1053_DREQ);
     stream.setVolume(100);

     bouton[1].initButton(&tft, 80, 200, 120, 50, TFT_TRANSPARENT, TFT_TRANSPARENT, TFT_TRANSPARENT, "", 2);
     bouton[0].initButton(&tft, 250, 200, 120, 50, TFT_TRANSPARENT, TFT_TRANSPARENT, TFT_TRANSPARENT, "", 2);

}

//-----------------------------------------------------------------------------------------------

void loop(void) {
    
     connexion_chaine();
     
     uint16_t t_x = 0, t_y = 0;
     boolean pressed = tft.getTouch(&t_y, &t_x); // vrai si contact avec l'écran !!! INVERSE !!!
     for (numero = 0; numero < NOMBRE_BOUTONS; numero++) {
          if (pressed && bouton[numero].contains(t_y, t_x)) {
              bouton[numero].press(true);              
             } else {
              bouton[numero].press(false);  
             }
         }

     for (numero = 0; numero < NOMBRE_BOUTONS; numero++) {
          if (bouton[numero].justPressed()) {
              switch (numero) {
                      case 0:
                             tft.pushImage(10, 179, 75, 50, upPress);
                             chaine --;
                             if (chaine < 0) { chaine = max_chaines - 1; } 
                             tft.pushImage(50, 40,  215, 35, zonestation);
                             tft.pushImage(30, 76,  250, 35, zoneartiste);
                             tft.pushImage(30, 115,  250, 35, zonetitre);
                             tft.pushImage(98, 199,  110, 20, zonestream);
                             tft.setTextColor(TFT_CYAN);
                             tft.drawString("Stream ...", 120, 200, 1);
                             stream.stopSong();
                             tft.setTextColor(TFT_ORANGE);
                       break;
                      case 1:
                             tft.pushImage(230, 179, 75, 50, downPress);
                             chaine ++;                           
                             if (chaine > max_chaines - 1) { chaine = 0; } 
                             tft.pushImage(50, 40,  215, 35, zonestation);
                             tft.pushImage(30, 76,  250, 35, zoneartiste);
                             tft.pushImage(30, 115,  250, 35, zonetitre);
                             tft.pushImage(98, 199,  110, 20, zonestream);
                             tft.setTextColor(TFT_CYAN);
                             tft.drawString("Stream ...", 120, 200, 1);
                             tft.setTextColor(TFT_ORANGE);
                             stream.stopSong();
                       break;
                      }/* switch (numero) */

              delay(100); // anti-rebond

             }/* if (bouton[numero].justPressed()) */

          }/* for (uint8_t numero = 0; numero < NOMBRE_BOUTONS; numero++) */
          
}


//   ACCES CHAINES RADIO
//   -------------------   
void connexion_chaine() {
  
     switch (chaine) {
       case 0:
              stream.connecttohost("https://audiopanel.com.ar:8110/radio.mp3");//Vinilo - The 80's $$$
              stream.loop();
        break;      
       case 1:
              stream.connecttohost("http://orion.shoutca.st:8998/stream");//Digital Impulse - Dance from '80s $$$
              stream.loop();
        break;
       case 2:
              stream.connecttohost("https://stream1.themediasite.co.uk:7018/;");//Love 80's Manchester $$$
              stream.loop();
        break;            
       case 3:
              stream.connecttohost("https://audiopanel.com.ar:8000/radio.mp3");//La Rocka 80's $$$
              stream.loop();
        break;            
       case 4:
              stream.connecttohost("http://hemnos.cdnstream.com/1467_128");//Cleansing 80's $$$
              stream.loop();
        break;            
       case 5:
              stream.connecttohost("https://carina.streamerr.co:8600/stream");//Vintage Radio - 80's Hits $$$
              stream.loop();
        break;            
       case 6:
              stream.connecttohost("http://67.249.184.45:8000/listen.pls");//Totally 80's FM $$$
              stream.loop();
        break;            
       case 7:
              stream.connecttohost("http://192.111.140.11:8524/stream");//HM-80's Radio $$$   
              stream.loop();
        break;            
       case 8:
              stream.connecttohost("https://streamer.radio.co/s81fc850fd/listen");//Auckland 80's $$$
              stream.loop();
        break;            
       case 9:
              stream.connecttohost("http://pureplay.cdnstream1.com/6056_128.mp3");//100hitz - 90's Hitz $$$
              stream.loop();
        break;            
       case 10:
              stream.connecttohost("http://hemnos.cdnstream.com/1468_128");//Cleansing 90's $$$
              stream.loop();
        break;            
       case 11:
              stream.connecttohost("http://pureplay.cdnstream1.com/6052_128.mp3");//GotRadio The 90's $$$
              stream.loop();
        break;            
       case 12:
              stream.connecttohost("https://audiopanel.com.ar:8030/radio.mp3");//La Rocka 90's $$$
              stream.loop();
        break;            
       case 13:
              stream.connecttohost("https://stream.btsstream.com:8042/nonstop90s.mp3");//Non-Stop 90's $$$
              stream.loop();
        break;            
       case 14:
              stream.connecttohost("http://uk1.streamingpulse.com:7020/stream");//The 90s Network $$$ 256
              stream.loop();
        break;            

             }//  switch (chaine)     
               
}


//   STREAM DATAS INFOS
//   ------------------
void audio_showstation(const char* info) { 
   
     stationOrg = ("showstation: %s\n", info);     
     station = stationOrg.substring(0, 20);
     tft.pushImage(50, 40,  215, 35, zonestation);
     tft.setFreeFont(FSS12);// 9,12,18,24
     tft.drawCentreString(station, 155, 50, 1);
     tft.setFreeFont(FSS9);// 9,12,18,24
                      
}


void audio_showstreamtitle(const char* info) { 

     morceau = ("streamtitle: %s\n", info);
Serial.println(morceau);
     longMorceau = morceau.length();
     for (tiret = 0; tiret <= longMorceau; tiret ++) {
          String lettre = String (morceau.charAt(tiret));
          if (lettre == "-") {
              cesure = tiret;
              tiret = longMorceau;      
             } 
         }
     tft.pushImage(10, 179, 75, 50, down);
     tft.pushImage(230, 179, 75, 50, up);

     interpreteOrg = String (morceau.substring(0, cesure));
     interprete = interpreteOrg.substring(0, 20);
     tft.pushImage(30, 76,  250, 35, zoneartiste);
     tft.drawCentreString(interprete, 155, 85, 1);
     
     titreOrg =  String (morceau.substring(cesure + 2, longMorceau)); 
     titre = titreOrg.substring(0, 26);
     tft.pushImage(30, 115,  250, 35, zonetitre);
     tft.drawCentreString(titre, 155, 125, 1);
     
     String txtCodec = stream.currentCodec();
     int txtBitrate = stream.bitrate();
     tft.pushImage(98, 199,  110, 20, zonestream);
     tft.drawString(streamFormat = ((String (txtCodec)) + "/" 
                 + (String (txtBitrate)) + "kbps"), 118, 200, 2);

}


void audio_eof_stream(const char* info) {  

}

//-----------------------------------------------------------------------------------------------
/*
// procédure de calibration de l'écran tactile
void touch_calibrate() {
  
     uint16_t calData[5];
     uint8_t calDataOK = 0;

     tft.fillScreen(TFT_BLACK);
     tft.setCursor(25, 70);
     tft.setTextFont(2);
     tft.setTextSize(2);
     tft.setTextColor(TFT_WHITE, TFT_BLACK);
     tft.println("Touchez l'ecran a ");
     tft.setCursor(15, 110);
     tft.println("chaque coin indique.");
     tft.setTextFont(1);
     tft.println();
     tft.calibrateTouch(calData, TFT_YELLOW, TFT_BLACK, 20);
     tft.setTextColor(TFT_GREEN, TFT_BLACK);
     tft.println("Calibration terminee!");

     Serial.println("calDatas:");
     for (int boucle = 0; boucle <= 5; boucle ++) {    
          Serial.print(calData[boucle]); Serial.print(" , ");      
         } Serial.println();
     
}
*/
