#ifndef HSD_SWITCH_H_
#define HSD_SWITCH_H_
#ifndef HSD_SWITCH_H
#define HSD_SWITCH_H

#include <Bounce2.h>   
#define Resetpin D7
#define Pluspin D5
#define Relaispin D6
unsigned int plus = 1800000;
unsigned int sperrzeit = 800;
unsigned int LEDstate = 0;
unsigned int ASTATE = 0;
unsigned long millisstart = 0;
unsigned long millisaktuell = 0;
unsigned long millisbutton = 0;
unsigned long millisrefresh = 0;

String getlink = "INIT1";


// Define "bounce" objects for input buttons.

// http://playground.arduino.cc/Code/Bounce
Bounce Button1 = Bounce(); 
Bounce Button2 = Bounce(); 

void callback(char* topic, byte* payload, unsigned int length) {
  
}

void button () {
pinMode(Pluspin, INPUT_PULLUP);
Button2.attach( Pluspin );
Button2.interval(800);
pinMode(Resetpin, INPUT_PULLUP);
Button1.attach( Resetpin );
Button1.interval(5);
pinMode(Relaispin, OUTPUT);

if (millisrefresh == 0)
{ 
millisrefresh = millis();
};

if (millisrefresh + 1000 < millis())
{
  ASTATE = 1;
  millisrefresh =0;
};

  if (Button1.read() == LOW) {
    millisstart = 0;
    millisaktuell = 0;
    digitalWrite(Relaispin, LOW);
    mqtt1.publish ("ledrahmen/light/mute","0");
    Serial.println("Reset");
  }

   if ((Button2.read() == LOW) & (millisstart == 0)) {
//  if ((Button2.read() == LOW) & (millisstart == 0) & ((millis() - millisbutton) > sperrzeit)) {
    millisstart = (millis() + plus);
    millisbutton = millis();
    Serial.println("Start+");
    ASTATE = 1;
  }

 if ((Button2.read() == LOW) & (millisstart > 0) & ((millis() - millisbutton) > sperrzeit) & (millisaktuell < (4 * plus))) {

  millisstart = (millisstart + plus);
  millisbutton = millis();
    Serial.println("+");
    ASTATE = 1;
  }

   if (millisstart != 0)  {
   millisaktuell = (millisstart - millis());
   }


//    Serial.println("kleiner 1");
    LEDstate = 0;

    if (millisaktuell > 1){
//    Serial.println("über 0h");
    LEDstate = 1;
    }  

    if (millisaktuell > (plus)){
//    Serial.println("über 0,5h");
    LEDstate = 2;
    }

    if (millisaktuell > (2 * plus)){
//    Serial.println("über 1h");
    LEDstate = 3;
    }

    if (millisaktuell > (3 * plus)){
//    Serial.println("über 1,5h");
    LEDstate = 4;
    }

    if (millisaktuell > (5 * plus)){
//    Serial.println("zu Hoch");
    LEDstate = 0;
    }

    switch(LEDstate)
                  {
   case 0: 
    millisstart = 0;
    millisaktuell = 0;
    digitalWrite(Relaispin, LOW);
    
    
    if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","0"); 
   ASTATE = 0;
   Serial.println("Case 0");
    }
   break;

   case 1:
    digitalWrite(Relaispin, HIGH);
    //getmqtt(topic2);
     if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","1"); 
   ASTATE = 0;
   
    Serial.println("Case 1");
     }
    
   break;

   case 2:
    digitalWrite(Relaispin, HIGH);
    if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","2"); 
   ASTATE = 0;
   
    Serial.println("Case 2");
     }
   break;

    case 3:
    digitalWrite(Relaispin, HIGH);
    //getmqtt(topic2);
    if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","3"); 
   ASTATE = 0;
   
    Serial.println("Case 3");
     }
   break;

   case 4:
     digitalWrite(Relaispin, HIGH);
    // getmqtt(topic2);
     if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","4"); 
   ASTATE = 0;
   
    Serial.println("Case 4");
     }
   break;

   case 5:
    digitalWrite(Relaispin, HIGH);
    //getmqtt(topic2);
    if (ASTATE == 1) {
   mqtt1.publish ("ledrahmen/light/mute","5"); 
   ASTATE = 0;
   
    Serial.println("Case 5");
     }
   break;

    }
  

}

#endif // HSD_SWITCH_H
#endif // HSD_SWITCH_H_