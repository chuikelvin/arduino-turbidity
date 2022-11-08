#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;


#include <LiquidCrystal_I2C.h>

const char* ssid = "network";                 // We write the name of our network here.
const char* password = "network password";           // We write the password of our network here.
//
const char* api_key = "9D9YPGYTPOO0QKMR";

const char* serverName = "https://api.thingspeak.com/update?api_key=";

int buzzer_pin = 9; //buzzer_pin to arduino pin 9
int red_led = 8;
int green_led = 7;

unsigned long previousMillis = 0;
const long interval = 5000;

const char* ip = "184.106.153.149";                               //Thingspeak ip adress
float calibration_value = 21.34;
float volt_turb, volt_ph, ntu, ph_val;

LiquidCrystal_I2C lcd(0x27, 16, 2);


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void setup() {

  Serial.begin(115200);  // We are starting our communication with the serial port.

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  pinMode(buzzer_pin, OUTPUT); // Set buzzer_pin - pin 9 as an output
  pinMode(red_led, OUTPUT); // Set red_ led - pin 8 as an output
  pinMode(green_led, OUTPUT); // Set green_led - pin 7 as an output

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LoRa Receiver");
  delay(2500);
  lcd.clear();

  if (!LoRa.begin(915E6)) {
    Serial.println("Failed to start LoRa");
    delay(2000);
    while (1);
  }

}
void loop() {
  //  volt_turb = 0;
  //  for (int i = 0; i < 800; i++) {
  //    volt_turb += ((float)analogRead(turb_sens_pin ) / 1023) * 5;
  //  }
  //  volt_turb = volt_turb / 800;
  //  volt_turb = round_to_dp(volt_turb, 2);
  //
  //  if (volt_turb < 2.5) {
  //    ntu = 3000;
  //  } else {
  //    ntu = -1120.4 * sq(volt_turb) + 5742.3 * volt_turb - 4353.8;
  //  }                                            // We wait 1 second for sending new data.
  //  volt_ph = 0;
  //  for(int i=0;i<10;i++){
  //    volt_ph+= ((float)analogRead(ph_sens_pin) *5.0/1024/6);
  //  }
  //
  //  volt_ph = volt_ph/10;
  //  volt_ph =round_to_dp(volt_ph,2);
  //
  //  ph_val = -5.70 * volt_ph +calibration_value;

  lcd.clear();
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    Serial.print("Received packet '");

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    lcd.print(packetSize);

    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    delay(2000);
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      httpGETRequest(serverName);

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}
