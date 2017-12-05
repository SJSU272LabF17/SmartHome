#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

/************ WIFI and MQTT INFORMATION (CHANGE THESE FOR YOUR SETUP) ******************/
#define wifi_ssid "abc" //type your WIFI information inside the quotes
#define wifi_password "abc"
#define mqtt_server "yourcloudmqtt.com"
#define mqtt_user "ESP"
#define mqtt_password "esp"
#define mqtt_port XXXXX

 int ledPinD0 = 4;
 int ledPinD1 = 5;
 int ledPinD2 = 16;
 int ledPinD3 = 0;

String pinOut;

/************* MQTT TOPICS (change these topics as you wish)  **************************/
#define light_state_topic "home/sensornode1"
#define light_set_topic "home/sensornode1/set"

const char* on_cmd = "ON";
const char* off_cmd = "OFF";



/**************************** FOR OTA **************************************************/
#define SENSORNAME "sensornode1"
#define OTApassword "YouPassword" // change this to whatever password you want to use when you upload OTA
int OTAport = 8266;



/**************************** PIN DEFINITIONS ********************************************/
#define PIRPIN    D5
#define DHTPIN    D7
#define DHTTYPE   DHT22
#define LDRPIN    A0



/**************************** SENSOR DEFINITIONS *******************************************/
float ldrValue;
int LDR;
float calcLDR;
float diffLDR = 25;

float diffTEMP = 0.2;
float tempValue;

float diffHUM = 1;
float humValue;

int pirValue;
int pirStatus;
String motionStatus;

char message_buff[100];

int calibrationTime = 0;

const int BUFFER_SIZE = 300;

#define MQTT_MAX_PACKET_SIZE 512


/******************************** GLOBALS for fade/flash *******************************/

bool stateOn = false;
bool startFade = false;
bool inFade = false;



WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String strTopic = String((char*)topic);

  if(strTopic == "home/bedroom2/D0_tubelight")
    {
    pinOut = String((char*)payload);
    if(pinOut == "ON")
      {
        Serial.println("ON");
        digitalWrite(ledPinD0, LOW);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(ledPinD0, HIGH);
      }
    }

      if(strTopic == "home/bedroom2/D1_bulb")
    {
    pinOut = String((char*)payload);
    if(pinOut == "ON")
      {
        Serial.println("ON");
        digitalWrite(ledPinD1, LOW);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(ledPinD1, HIGH);
      }
    }

    if(strTopic == "home/bedroom2/D2_fan")
    {
    pinOut = String((char*)payload);
    if(pinOut == "ON")
      {
        Serial.println("ON");
        digitalWrite(ledPinD2, LOW);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(ledPinD2, HIGH);
      }
    }

   if(strTopic == "home/bedroom2/D3_TV")
    {
    pinOut = String((char*)payload);
    if(pinOut == "ON")
      {
        Serial.println("ON");
        digitalWrite(ledPinD3, LOW);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(ledPinD3, HIGH);
      }
    }

    Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (!processJson(message)) {
    return;
  }

  inFade = false; // Kill the current fade

  sendState();


}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("home/bedroom2/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(PIRPIN, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(LDRPIN, INPUT);

  Serial.begin(115200);
  delay(10);

  ArduinoOTA.setPort(OTAport);

  ArduinoOTA.setHostname(SENSORNAME);

  ArduinoOTA.setPassword((const char *)OTApassword);

  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Starting Node named " + String(SENSORNAME));




  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(ledPinD0, OUTPUT);
  pinMode(ledPinD1, OUTPUT);
  pinMode(ledPinD2, OUTPUT);
  pinMode(ledPinD3, OUTPUT);

  digitalWrite(ledPinD0, LOW);                        //setting pin as output
  digitalWrite(ledPinD1, LOW);
  digitalWrite(ledPinD2, LOW);
  digitalWrite(ledPinD3, LOW);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);


  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IPess: ");
  Serial.println(WiFi.localIP());
  reconnect();

}

void loop()
{

    ArduinoOTA.handle();

  if (!client.connected()) {
    reconnect();
     software_Reset();
  }
  client.loop();



  if (!inFade) {

    float newTempValue = dht.readTemperature(true); //to use celsius remove the true text inside the parentheses
    float newHumValue = dht.readHumidity();

    //PIR CODE
    pirValue = digitalRead(PIRPIN); //read state of the

    if (pirValue == LOW && pirStatus != 1) {
      motionStatus = "standby";
      sendState();
      pirStatus = 1;
    }

    else if (pirValue == HIGH && pirStatus != 2) {
      motionStatus = "motion detected";
      sendState();
      pirStatus = 2;
    }

    delay(100);

    if (checkBoundSensor(newTempValue, tempValue, diffTEMP)) {
      tempValue = newTempValue;
      sendState();
    }

    if (checkBoundSensor(newHumValue, humValue, diffHUM)) {
      humValue = newHumValue;
      sendState();
    }


    int newLDR = analogRead(LDRPIN);

    if (checkBoundSensor(newLDR, LDR, diffLDR)) {
      LDR = newLDR;
      sendState();
    }

  }
}

/*
 * Calculate Heat Index value AKA "Real Feel"
 * NOAA heat index calculations taken from
 * http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
 */
float calculateHeatIndex(float humidity, float temp) {
  float heatIndex= 0;
  if (temp >= 80) {
    heatIndex = -42.379 + 2.04901523*temp + 10.14333127*humidity;
    heatIndex = heatIndex - .22475541*temp*humidity - .00683783*temp*temp;
    heatIndex = heatIndex - .05481717*humidity*humidity + .00122874*temp*temp*humidity;
    heatIndex = heatIndex + .00085282*temp*humidity*humidity - .00000199*temp*temp*humidity*humidity;
  } else {
     heatIndex = 0.5 * (temp + 61.0 + ((temp - 68.0)*1.2) + (humidity * 0.094));
  }

  if (humidity < 13 && 80 <= temp <= 112) {
     float adjustment = ((13-humidity)/4) * sqrt((17-abs(temp-95.))/17);
     heatIndex = heatIndex - adjustment;
  }

  return heatIndex;
}




/********************************** START PROCESS JSON*****************************************/
bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }

  if (root.containsKey("state")) {
    if (strcmp(root["state"], on_cmd) == 0) {
      stateOn = true;
    }
    else if (strcmp(root["state"], off_cmd) == 0) {
      stateOn = false;
    }
  }

  // If "flash" is included, treat RGB and brightness differently
  return true;
}




/********************************** START CHECK SENSOR **********************************/
bool checkBoundSensor(float newValue, float prevValue, float maxDiff) {
  return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}


/********************************** START SEND STATE*****************************************/
void sendState() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? on_cmd : off_cmd;
  root["humidity"] = (String)humValue;
  root["motion"] = (String)motionStatus;
  root["ldr"] = (String)LDR;
  root["temperature"] = (String)tempValue;
  root["heatIndex"] = (String)calculateHeatIndex(humValue, tempValue);


  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  Serial.println(buffer);
  client.publish(light_state_topic, buffer, true);
}
/****reset***/
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  Serial.print("resetting");
  ESP.reset();
}
