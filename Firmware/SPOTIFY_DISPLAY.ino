#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJson.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

#define TFT_CS   5
#define TFT_RST  6
#define TFT_DC   4
#define TFT_SCLK 7
#define TFT_MOSI 10

#define BTN_PLAY 1
#define BTN_NEXT 2
#define BTN_PREV 3

char* SSID = "***";
const char* PASSWORD = "***";
const char* CLIENT_ID = "***";
const char* CLIENT_SECRET = "***";

String lastArtist = "";
String lastTrack = "";

Spotify sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

unsigned long lastUpdate = 0;
const int refreshTime = 5000;

void drawProgressBar(int progress) {

  int barWidth = map(progress, 0, 100, 0, 200);

  tft.drawRect(20, 200, 200, 10, ST77XX_WHITE);
  tft.fillRect(20, 200, barWidth, 10, ST77XX_GREEN);
}

void setup() {

  Serial.begin(115200);

  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(240,240);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  WiFi.begin(SSID, PASSWORD);

  tft.setCursor(20,20);
  tft.println("Connecting WiFi");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(20,20);
  tft.println("WiFi Connected");

  sp.begin();

  while(!sp.is_auth()){
      sp.handle_client();
  }

  tft.fillScreen(ST77XX_BLACK);
}

void loop() {

  if (millis() - lastUpdate > refreshTime) {

    lastUpdate = millis();

    String artist = sp.current_artist_names();
    String track = sp.current_track_name();

    if (artist != lastArtist || track != lastTrack) {

      tft.fillScreen(ST77XX_BLACK);

      tft.setCursor(10,30);
      tft.setTextSize(2);
      tft.println("Artist:");

      tft.setCursor(10,50);
      tft.setTextSize(2);
      tft.println(artist);

      tft.setCursor(10,100);
      tft.setTextSize(2);
      tft.println("Track:");

      tft.setCursor(10,120);
      tft.setTextSize(2);
      tft.println(track);

      lastArtist = artist;
      lastTrack = track;
    }

    int progress = sp.current_progress_ms();
    int duration = sp.current_duration_ms();

    if(duration > 0) {

      int percent = (progress * 100) / duration;

      drawProgressBar(percent);
    }
  }

  // Buttons

  if(digitalRead(BTN_PLAY) == LOW){
      sp.play_pause();
      delay(300);
  }

  if(digitalRead(BTN_NEXT) == LOW){
      sp.next_track();
      delay(300);
  }

  if(digitalRead(BTN_PREV) == LOW){
      sp.previous_track();
      delay(300);
  }

}