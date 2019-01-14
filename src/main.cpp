#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SimpleDHT.h>


#define CONNECTION_BAUD 57600
#define PIN_DHT 2

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
SimpleDHT11   dht11(PIN_DHT);

void setup() {
  Serial.begin(CONNECTION_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(16, 2);
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
  lcd.setCursor(5, 0); lcd.print((int) humidity); lcd.print("%");
  delay(1500);
}
