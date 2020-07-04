#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHTesp.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define MIN_DUTY 150     // Minimal speed when fan starts to spin
#define MAX_DUTY 1024    // Limit maximum fan speed
#define DIF_DUTY 1.10    // Difference between in and out PWM signal

#define FAN_IN_PIN D6
#define FAN_OUT_PIN D7
#define DHP_POWER_PIN D0
#define DHT_DATA_PIN D3

#define MIN_TEMP 25
#define MAX_TEMP 35

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dht;
int bad_reads;

int fan_in_duty;
int fan_out_duty;
int fan_in_load;
int fan_out_load;

void setup() {
    Serial.begin(115200);

    pinMode(DHP_POWER_PIN, OUTPUT);
    digitalWrite(DHP_POWER_PIN, HIGH);
    dht.setup(DHT_DATA_PIN, DHTesp::AM2302);
    bad_reads = 0;

    analogWriteFreq(25000); // 25KHz
    pinMode(FAN_IN_PIN, OUTPUT);
    pinMode(FAN_OUT_PIN, OUTPUT);    

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    showSplashScreen();
}

void(* resetFunc) (void) = 0;
 
void loop() {

    float temperature = dht.getTemperature();  
    float humidity = dht.getHumidity();      
    char buffer[50];
    sprintf(buffer, "Temperature: %.2fC, Humidity: %.2f", temperature, humidity);
    Serial.println(buffer);

    //analogWrite(FAN_IN_PIN, MIN_DUTY);

    /*
    for(int dutyCycle = MIN_DUTY; dutyCycle < MAX_DUTY; dutyCycle+=10){
      // changing the LED brightness with PWM
      Serial.print("dutyCycle: ");
      Serial.println(dutyCycle);
      analogWrite(FAN_IN_PIN, dutyCycle);
      delay(1000);
    }

    for(int dutyCycle = MAX_DUTY; dutyCycle > MIN_DUTY; dutyCycle-=10){
      // changing the LED brightness with PWM
      Serial.print("dutyCycle: ");
      Serial.println(dutyCycle);
      analogWrite(FAN_IN_PIN, dutyCycle);      
      delay(1000);
    }
    */

    if (!isnan(temperature)) {
      bad_reads = 0;
      fan_in_duty = map(temperature*100, MIN_TEMP*100, MAX_TEMP*100, MIN_DUTY, MAX_DUTY);
      fan_out_duty = map(temperature*100, MIN_TEMP*100, MAX_TEMP*100, MIN_DUTY, MAX_DUTY);
      fan_in_load = (fan_in_duty  * 100 / MAX_DUTY);
      fan_out_load = (fan_out_duty  * 100 / MAX_DUTY) / DIF_DUTY;
      analogWrite(FAN_IN_PIN, fan_in_duty);
      analogWrite(FAN_OUT_PIN, fan_out_duty);
    } else {
      bad_reads++;
      if (bad_reads > 5) {
        Serial.println("Resetting DHT");
        digitalWrite(DHP_POWER_PIN, LOW);
        delay(2000);
        digitalWrite(DHP_POWER_PIN, HIGH);
        bad_reads = 0;
      }
    }

    testscrolltext(temperature, fan_in_load, fan_out_load, true);

    delay(5000);
}
 
void testscrolltext(float temperature, int fan_in_load, int fan_out_load, bool latest) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    char buffer[10];
    
    display.setCursor(0, 0);
    display.println("Fanitizer");
    
    display.setCursor(0, 16);
    sprintf(buffer, "Tmp %.2fC", temperature);
    display.println(buffer);
  
    display.setCursor(0, 33);
    sprintf(buffer, "F.IN %i", fan_in_load);
    display.println(buffer);
  
    display.setCursor(0, 49);
    sprintf(buffer, "F.OUT %i", fan_out_load);
    display.println(buffer);
      
    display.display();
    //delay(100);
}

void showSplashScreen(){
  char base[10]="";
  base[0]='8';
  for(int stiff=1; stiff<10; ++stiff) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,27);
    display.print(base);
    display.print('>');
    display.display();
    base[stiff]='=';
    delay(100);
  }
}