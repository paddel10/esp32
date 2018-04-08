//Define DEBUG to get the Output from DEBUG_PRINTLN
#define DEBUG 1

#define BOTtoken "BotToken" //token of TestBOT
#define BOTRoomId "BotRoomId"
#define BOTAuthToken "BotAuthToken"
#define HIPCHATFingerprint "df 91 f2 8c c1 dc 54 a9 50 97 7b 5b 84 b7 a5 c3 c7 c1 53 05"
#define HIPCHAT_BASE_URL "https://iweb.hipchat.com"
// change topic
// curl -d '{"topic":"LOCKED"}' -H 'Content-Type: application/json' -X PUT https://iweb.hipchat.com/v2/room/4494135/topic?auth_token=NAVff...IoP


//Include Basecamp in this sketch
#include <Basecamp.hpp>
#include <Configuration.hpp>
#include <HTTPClient.h>


#define BASECAMP_NOMQTT  // disable MQTT *after* including Basecamp.hpp
const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----\n";

// create telegram bot - http://t.me/iLooZHBot, https://github.com/Gianbacchio/ESP8266-TelegramBot
//TelegramBOT bot(BOTname, BOTusername);
String botRoomId;
String botAuthToken;

// Create a new Basecamp instance called iot that will start the ap in secure mode and the webserver ui only in setup mode
Basecamp iot{Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::accessPoint};
// Uncomment the following line and comment to one above to start the ESP with open wifi and always running config ui
// Basecamp iot;
HTTPClient http;

//Variables for the sensor and the battery
static const int ResetPin = 35;
static const int SensorPin = 32;
static const int BatteryPin = 34;
int sensorValue = 0;
//The batteryLimit defines the point at which the battery is considered empty.
int batteryLimit = 3300;

//This is used to control if the ESP should enter sleep mode or not
bool delaySleep = false;

//Variables for the mqtt packages and topics
uint16_t statusPacketIdSub = 0;
String delaySleepTopic;
String statusTopic;
String batteryTopic;
String batteryValueTopic;

// Reset the configuration to factory defaults (all empty)
void resetToFactoryDefaults()
{
    DEBUG_PRINTLN("Resetting to factory defaults");
    Configuration config(String{"/basecamp.json"});
    config.load();
    config.resetExcept({ConfigurationKey::accessPointSecret, });
    config.save();
}
void putMessage(String status){
	String url = String(HIPCHAT_BASE_URL) + String("/v2/room/") + botRoomId + String("/topic?auth_token=") + botAuthToken;
	Serial.println((String)"URL: " + url);
	if (!http.begin(url, root_ca)) {
		Serial.println("http.begin() failed");
	}
	http.addHeader("Content-Type", "application/json");
	String putMessage = String("{\"topic\":\"") + status + String("\"}");
	Serial.println(String("http put: ") + putMessage);
	int httpCode = http.PUT(putMessage);
	Serial.print("http result:");
	Serial.println(httpCode);
	http.writeToStream(&Serial);
	String payload = http.getString();
	Serial.print("response: ");
	Serial.println(payload);
	http.end();
}

void sleepEnable() {
  DEBUG_PRINTLN(__func__);

  if (sensorValue == 0) {
    DEBUG_PRINTLN("Door LOCKED");
    //Transfer the current state of the sensor to the MQTT broker
    // statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "open" );
    // bot.sendMessage(botRoomId, "Door LOCKED", "");
	putMessage("Door LOCKED");
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 1);
  } else {
    DEBUG_PRINTLN("Door unlocked");
    //Transfer the current state of the sensor to the MQTT broker
    // statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "closed" );
    // bot.sendMessage(botRoomId, "Door unlocked", "");
	putMessage("Door unlocked");
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 0);
  }
}

void setChatBotInterface() {
  iot.web.addInterfaceElement(
    BOTAuthToken,
    "input",
    "Auth Token",
    "#configform",
    BOTAuthToken
  );
  iot.web.addInterfaceElement(
    BOTRoomId,
    "input",
    "Room ID",
    "#configform",
    BOTRoomId
  );
  
}
void setup() {
  //configuration of the battery and sensor pins
  pinMode(ResetPin, INPUT_PULLDOWN);
  pinMode(SensorPin, INPUT_PULLDOWN);
  pinMode(BatteryPin, INPUT);

  //read the status of the doorsensor as soon as possible to determine the state that triggered it
  sensorValue = digitalRead(SensorPin);

  bool resetPressed = (digitalRead(ResetPin) == HIGH);
  if (resetPressed) {
    resetToFactoryDefaults();
  }

  // Initialize Basecamp
  DEBUG_PRINTLN("initialize basecamp");
  iot.begin();
  setChatBotInterface();
  // Alternate example: optional initialization with a fixed ap password for setup-mode:
  // iot.begin("yoursecurepassword");

  if (resetPressed) {
    DEBUG_PRINTLN("**** CONFIG HAS BEEN MANUALLY RESET ****");
  }

  //Configure the MQTT topics
  /*delaySleepTopic = "cmd/" + iot.hostname + "/delaysleep";
  statusTopic = "stat/" + iot.hostname + "/status";
  batteryTopic = "stat/" + iot.hostname + "/battery";
  batteryValueTopic = "stat/" + iot.hostname + "/batteryvalue";*/

  //Set up the Callbacks for the MQTT instance. Refer to the Async MQTT Client documentation
  // TODO: We should do this actually _before_ connecting the mqtt client...
  if (iot.wifi.getOperationMode() == WifiControl::Mode::client) {
    DEBUG_PRINTLN("in client mode");
    while (iot.wifi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = iot.wifi.getIP();
    Serial.println(ip);
    botAuthToken = iot.configuration.get(BOTAuthToken);
    botRoomId = iot.configuration.get(BOTRoomId);
	
    // bot.begin(token);
    sleepEnable();
    esp_deep_sleep_start();
  } else {
    DEBUG_PRINTLN("access point mode");
  }
  /*iot.mqtt.onConnect(onMqttConnect);
  iot.mqtt.onPublish(suspendESP);
  iot.mqtt.onMessage(onMqttMessage);*/
}


//This function is called when the MQTT-Server is connected
void onMqttConnect(bool sessionPresent) {
  DEBUG_PRINTLN(__func__);

  //Subscribe to the delay topic
  // iot.mqtt.subscribe(delaySleepTopic.c_str(), 0);
  //Trigger the transmission of the current state.
  transmitStatus();
}


//This function transfers the state of the sensor. That includes the door status, battery status and level
void transmitStatus() {
  DEBUG_PRINTLN(__func__);

  if (sensorValue == 0) {
    DEBUG_PRINTLN("Door open");
    //Transfer the current state of the sensor to the MQTT broker
    // statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "open" );
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 1);
  } else {
    DEBUG_PRINTLN("Door closed");
    //Transfer the current state of the sensor to the MQTT broker
    // statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "closed" );
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 0);
  }

  //Read the current analog battery value
  // sensorValue = analogRead(BatteryPin);
  //sensorC stores the battery value as a char
  // char sensorC[6];
  //convert the sensor value to a string
  // sprintf(sensorC, "%04i", sensorValue);
  //Send the sensor value to the MQTT broker
  // iot.mqtt.publish(batteryValueTopic.c_str(), 1, true, sensorC);
  //Check the battery level and publish the state
  // if (sensorValue < batteryLimit) {
    // DEBUG_PRINTLN("Battery empty");
    // iot.mqtt.publish(batteryTopic.c_str(), 1, true, "empty" );
  // } else {
    // DEBUG_PRINTLN("Battery full");
    // iot.mqtt.publish(batteryTopic.c_str(), 1, true, "full" );
  // }
  DEBUG_PRINTLN("Data published");
}

//This topic is called if an MQTT message is received
// void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
//   DEBUG_PRINTLN(__func__);

  //Check if the payload eqals "true" and set delaySleep correspondigly
  //Since we only subscribed to one topic, we only have to compare the payload
//   if (strcmp(payload, "true") == 0)  {
//     delaySleep = true;
//   } else  {
//     delaySleep = false;
//   }
// }

void suspendESP(uint16_t packetId) {
  DEBUG_PRINTLN(__func__);

  //Check if the published package is the one of the door sensor
  if (packetId == statusPacketIdSub) {

    if (delaySleep == true) {
      DEBUG_PRINTLN("Delaying Sleep");
      return;
    }
    DEBUG_PRINTLN("Entering deep sleep");
    //properly disconnect from the MQTT broker
    //iot.mqtt.disconnect();
    //send the ESP into deep sleep
    esp_deep_sleep_start();
  }
}

void loop()
{
}
