//#include <SoftwareSerial.h>
//#include <limits.h>
//  
//#include <Wire.h> 
#include <SPI.h>
#include <LoRa.h>

//#include <LiquidCrystal_I2C.h>    

//String ssid = "network";                 // We write the name of our network here.
//String password = "network password";           // We write the password of our network here.
//
//String api_key ="9D9YPGYTPOO0QKMR";

//int rxPin = 10;                                               //ESP8266 RX pin
//int txPin = 11;                                               //ESP8266 TX pin
int turb_sens_pin = A0;
int ph_sens_pin =A1;

//int buzzer_pin = 9; //buzzer_pin to arduino pin 9
//int red_led = 8;
//int green_led = 7;

//String ip = "184.106.153.149";                               //Thingspeak ip adress
float calibration_value = 21.34;
float volt_turb,volt_ph, ntu, ph_val;

//LiquidCrystal_I2C lcd(0x27, 16, 2);

//SoftwareSerial esp(rxPin, txPin);                             // We make serial communication pin settings.

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

void setup() {

  Serial.begin(9600);  // We are starting our communication with the serial port.
  Serial.println("Started");
  pinMode(turb_sens_pin, INPUT);
  pinMode(ph_sens_pin, INPUT);
//  pinMode(buzzer_pin, OUTPUT); // Set buzzer_pin - pin 9 as an output
//  pinMode(red_led, OUTPUT); // Set red_ led - pin 8 as an output
//  pinMode(green_led, OUTPUT); // Set green_led - pin 7 as an output
  if(!LoRa.begin(915E6)){
    Serial.println("Failed to start LoRa");
    delay(2000);
    while(1);
  }

}
void loop() {
  volt_turb = 0;
  for (int i = 0; i < 800; i++) {
    volt_turb += ((float)analogRead(turb_sens_pin ) / 1023) * 5;
  }
  volt_turb = volt_turb / 800;
  volt_turb = round_to_dp(volt_turb, 2);

  if (volt_turb < 2.5) {
    ntu = 3000;
  } else {
    ntu = -1120.4 * sq(volt_turb) + 5742.3 * volt_turb - 4353.8;
  }                                            // We wait 1 second for sending new data.
  volt_ph = 0;
  for(int i=0;i<10;i++){
    volt_ph+= ((float)analogRead(ph_sens_pin) *5.0/1024/6);
  }

  volt_ph = volt_ph/10;
  volt_ph =round_to_dp(volt_ph,2);

  ph_val = -5.70 * volt_ph +calibration_value;
  
  LoRa.beginPacket();
  LoRa.print("NTU");
  LoRa.print(ntu);
  LoRa.print(" PH");
  LoRa.print(ph_val);
  LoRa.endPacket();
  delay(2000);
}
