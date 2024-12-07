#ifndef HSD_INPUT_H
#define HSD_INPUT_H

#include <PicoMQTT.h>
 #define Inputpin A0

unsigned int warten = 5000;
unsigned long loopTime;
int ANALOG_EINGANG0 = 0;
//String topic2 = "ledrahmen/light/mute/#";
String antwort = "INIT";


  
    PicoMQTT::Client mqtt1(
    "192.168.178.189",    // broker address (or IP)
    1883,                   // broker port (defaults to 1883)
    "HSD-Klingel",           // Client ID
    "mqvipertt",             // MQTT username
    "mqahb36wdtt"              // MQTT password
);
/*
 mqtt1.subscribe("ledrahmen/light/mute", [](const char * topic, const char * payload) {
        // payload might be binary, but PicoMQTT guarantees that it's zero-terminated
        Serial.printf("Received message in topic '%s': %s\n", topic, payload);
    });

    mqtt1.begin(); 
   
*/




  /*
  Serial.println("In Funktion getmqtt!");
  Serial.println("Topic: ");
  Serial.print(topic1);
  */


 
void In(){

ANALOG_EINGANG0 = analogRead(Inputpin);
//Serial.println(ANALOG_EINGANG0);

 if((ANALOG_EINGANG0 > 800) & ((loopTime + warten) < millis())){
   Serial.println("Klingel");
   loopTime = millis();

  mqtt1.publish("ledrahmen/light/blink", "9");
  Serial.println("Published");
}

}



#endif // HSD_INPUT_H