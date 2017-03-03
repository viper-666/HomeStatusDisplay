#include "FhemStatusDisplay.h"

// function declarations
void handleMqttMessage(String topic, String msg);

#define WINDOW_STRING (F("window"))
#define DOOR_STRING (F("door"))
#define LIGHT_STRING (F("light"))
#define ALARM_STRING (F("alarm"))

int getFreeRamSize();

FhemStatusDisplay::FhemStatusDisplay()
:
m_webServer(m_config),
m_wifi(m_config),
m_mqttHandler(m_config, std::bind(&FhemStatusDisplay::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_leds(m_config),
m_lastWifiConnectionState(false),
m_lastMqttConnectionState(false)
{
}

void FhemStatusDisplay::begin(const char* version, const char* identifier)
{
  // initialize serial
  Serial.begin(115200);
  Serial.println(F(""));

  m_config.begin(version, identifier);
  m_webServer.begin();
  m_leds.begin();
  m_wifi.begin();
  m_mqttHandler.begin(); 

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

void FhemStatusDisplay::work()
{
  checkConnections();

  m_webServer.handleClient();

  if(m_wifi.connected())
  {
    m_mqttHandler.handle();
  }
  
  m_leds.update();

  delay(50);
}

void FhemStatusDisplay::mqttCallback(char* topic, byte* payload, unsigned int length)
{
  int i = 0;

  for(i = 0; (i < length) && (i < MQTT_MSG_MAX_LEN); i++) 
  {
    mqttMsgBuffer[i] = payload[i];
  }
  mqttMsgBuffer[i] = '\0';

  String mqttTopicString(topic);
  String mqttMsgString = String(mqttMsgBuffer);
  
  Serial.print(F("Received an MQTT message for topic ")); Serial.println(mqttTopicString + ": " + mqttMsgString);

  if(mqttTopicString.equals(m_config.getMqttTestTopic()))
  {
    handleTest(mqttMsgString);
  }
  else
  {
    if(mqttTopicString.substring(12, 17) == LIGHT_STRING)
    {
      handleStatus(mqttTopicString.substring(18), TYPE_LIGHT, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 18) == WINDOW_STRING)
    {
      handleStatus(mqttTopicString.substring(19), TYPE_WINDOW, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 16) == DOOR_STRING)
    {
      handleStatus(mqttTopicString.substring(17), TYPE_DOOR, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 17) == ALARM_STRING)
    {
      handleStatus(mqttTopicString.substring(18), TYPE_ALARM, mqttMsgString);
    }
    else
    {
      Serial.println(F("Unknown topic, ignoring"));
    }
  }
}

void FhemStatusDisplay::handleTest(String msg)
{
  int type = msg.toInt();
  if(type > 0)
  {
    Serial.print(F("Showing testpattern ")); Serial.println(type);
    m_leds.test(type);
  }
  else if(type == 0)
  {
    m_leds.clear();
    m_mqttHandler.reconnect();  // back to normal
  }
}

void FhemStatusDisplay::handleStatus(String device, deviceType type, String msg)
{
  int ledNumber = m_config.getLedNumber(device, type);
  int colorMapIndex = m_config.getColorMapIndex(type, msg);

  if( (ledNumber != -1) && (colorMapIndex != -1) )
  {
    Led::Behavior behavior = m_config.getLedBehavior(colorMapIndex);
    Led::Color color = m_config.getLedColor(colorMapIndex);

    Serial.println("Set led number " + String(ledNumber) + " to behavior " + String(behavior) + " with color " + String(color, HEX));
    m_leds.set(ledNumber, behavior, color);
  }
  else
  {
    Serial.println("No LED defined for device " + device + " of type " + String(type) + ", ignoring it");
  }
}

void FhemStatusDisplay::checkConnections()
{
  if(!m_lastMqttConnectionState && m_mqttHandler.connected())
  {
    m_leds.clear();
    m_lastMqttConnectionState = true;
  }
  else if(m_lastMqttConnectionState && !m_mqttHandler.connected())
  {
    m_leds.clear();
    m_lastMqttConnectionState = false;
  }

  if(!m_mqttHandler.connected() && m_wifi.connected())
  {
    m_leds.setAll(Led::ON, Led::YELLOW);
  }
  
  if(!m_lastWifiConnectionState && m_wifi.connected())
  {
    m_leds.clear();

    if(!m_mqttHandler.connected())
    {
      m_leds.setAll(Led::ON, Led::YELLOW);
    }
    
    m_lastWifiConnectionState = true;
  }
  else if(m_lastWifiConnectionState && !m_wifi.connected())
  {
    m_leds.clear();
    m_lastWifiConnectionState = false;
  }

  if(!m_wifi.connected())
  {
    m_leds.setAll(Led::ON, Led::RED);
  }
}


