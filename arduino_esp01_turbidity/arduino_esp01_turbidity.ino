#include <SoftwareSerial.h>
#include <limits.h>
  
#include <Wire.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>    

String ssid = "network";                 // We write the name of our network here.
String password = "network password";           // We write the password of our network here.

String api_key ="9D9YPGYTPOO0QKMR";

int rxPin = 10;                                               //ESP8266 RX pin
int txPin = 11;                                               //ESP8266 TX pin
int turb_sens_pin = A0;

int buzzer_pin = 9; //buzzer_pin to arduino pin 9
int red_led = 8;
int green_led = 7;

String ip = "184.106.153.149";                                //Thingspeak ip adress
float volt, ntu;

LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial esp(rxPin, txPin);                             // We make serial communication pin settings.

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

void setup() {

  Serial.begin(9600);  // We are starting our communication with the serial port.
  Serial.println("Started");
  esp.begin(115200);                                          // We are starting serial communication with ESP8266.
  esp.println("AT");                                         // We do the module control with the AT command.
  Serial.println("AT  sent ");
  while (!esp.find("OK")) {                                   // We wait until the module is ready.
    esp.println("AT");
    Serial.println("ESP8266 Not Find.");
  }
  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");                                 // We set the ESP8266 module as a client.
  while (!esp.find("OK")) {                                   // We wait until the setting is done.
    esp.println("AT+CWMODE=1");
    Serial.println("Setting is ....");
  }
  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\""); // We are connecting to our network.
  while (!esp.find("OK"));                                    // We wait until it is connected to the network.
  Serial.println("connected to the network.");
  delay(1000);

  lcd.begin();
  lcd.backlight();

  pinMode(buzzer_pin, OUTPUT); // Set buzzer_pin - pin 9 as an output
  pinMode(red_led, OUTPUT); // Set red_ led - pin 8 as an output
  pinMode(green_led, OUTPUT); // Set green_led - pin 7 as an output
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\"" + ip + "\",80");       // We connect to Thingspeak.
  if (esp.find("Error")) {                                    // We check the connection error.
    Serial.println("AT+CIPSTART Error");
  }
  volt = 0;
  for (int i = 0; i < 800; i++) {
    volt += ((float)analogRead(turb_sens_pin ) / 1023) * 5;
  }
  volt = volt / 800;
  volt = round_to_dp(volt, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  if (volt < 2.5) {
    ntu = 3000;
    
    lcd.print(ntu);
    lcd.print(" NTU");
    
    digitalWrite(green_led, HIGH);
    delay(1000);
    digitalWrite(green_led, LOW);
    
  } else {
    ntu = -1120.4 * sq(volt) + 5742.3 * volt - 4353.8;

    lcd.print(ntu);
    lcd.print(" NTU");
    
    tone(buzzer_pin, 1000); 
    digitalWrite(red_led, HIGH);
    delay(1000);
    noTone(buzzer_pin); 
    digitalWrite(red_led, LOW);
  }

  
  String veri = "GET https://api.thingspeak.com/update?api_key=";   // Thingspeak command.
  veri += api_key;
  veri += "&field1=";
  veri += String(ntu);                                         // The turbidity variable we will send
  veri += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri.length() + 2);
  delay(2000);
  if (esp.find(">")) {                                        // The commands in it are running when ESP8266 is ready..
    esp.print(veri);                                          // We send the data.
    Serial.println(veri);
    Serial.println("Data sent.");
    delay(1000);
  }
  Serial.println("Connection Closed.");
  esp.println("AT+CIPCLOSE");                                // we close the link
  delay(1000);                                               // We wait 1 second for sending new data.
}
