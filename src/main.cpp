#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SimpleDHT.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <SPI.h>

// TFT Display
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#define TFT_CS        13
#define TFT_RST        5  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         6
#define TFT_MOSI 4        // Data out
#define TFT_SCLK 3        // Clock out

#define CONNECTION_BAUD 57600
#define PIN_DHT 2

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
SimpleDHT11   dht11(PIN_DHT);
RtcDS3231<TwoWire> Rtc(Wire);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  char date[11];
  char time[9];

  snprintf_P(date,
          countof(date),
          PSTR("%02u/%02u/%04u"),
          dt.Day(),
          dt.Month(),
          dt.Year());
  snprintf_P(time,
          countof(time),
          PSTR("%02u:%02u:%02u"),
          dt.Hour(),
          dt.Minute(),
          dt.Second());

  Serial.print(date); Serial.print(" "); Serial.println(time);
  lcd.setCursor(8,0); lcd.print(time);
  lcd.setCursor(6,1); lcd.print(date);
}

void setup() {
  Serial.begin(CONNECTION_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(16, 2);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);

  if (!Rtc.IsDateTimeValid())
  {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  // TFT Display
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  lcd.setCursor(0, 0); lcd.print((int) temperature); lcd.print("C");
  lcd.setCursor(0, 1); lcd.print((int) humidity); lcd.print("%");

  if (!Rtc.IsDateTimeValid())
  {
    // Common Cuases:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
  }

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(5, 30);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(4);
  tft.print(temperature); tft.println("C");

  delay(1500);
}
