#include "DHTesp.h"
DHTesp dht;
 
void setup() {
    Serial.begin(115200);
    dht.setup(D3, DHTesp::DHT22);
}
 
void loop() {
    float h = dht.getHumidity();
    float t = dht.getTemperature();
 
    Serial.print("{\"humidity\": ");
    Serial.print(h);
    Serial.print(", \"temp\": ");
    Serial.print(t);
    Serial.print("}\n");
 
    delay(2000);
}
