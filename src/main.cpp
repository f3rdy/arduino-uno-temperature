#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SimpleDHT.h>
#include <Wire.h>
#include <RtcDS3231.h>


#define CONNECTION_BAUD 57600
#define PIN_DHT 2

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
SimpleDHT11   dht11(PIN_DHT);
RtcDS3231<TwoWire> Rtc(Wire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
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
  Rtc.SetDateTime(compiled);
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

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);

  delay(1500);
}
