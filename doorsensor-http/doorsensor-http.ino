//Define DEBUG to get the Output from DEBUG_PRINTLN
//#define DEBUG 1

#define RASPIURL "RaspiUrl"
// change topic
// curl -d '{"topic":"LOCKED"}' -H 'Content-Type: application/json' -X PUT https://iweb.hipchat.com/v2/room/4494135/topic?auth_token=NAVff...IoP

//Include Basecamp in this sketch
#include <Basecamp.hpp>
#include <Configuration.hpp>
#include <HTTPClient.h>

#define BASECAMP_NOMQTT  // disable MQTT *after* including Basecamp.hpp

// configuration variables
String raspiUrl;

// Create a new Basecamp instance called iot that will start the ap in secure mode and the webserver ui only in setup mode
Basecamp iot{Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::accessPoint};
HTTPClient http;

//Variables for the sensor and the battery
static const int ResetPin = 35;
static const int SensorPin = 32;
static const int BatteryPin = 34;
int sensorValue = 0;
int batteryValue = 0;
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
void resetToFactoryDefaults() {
    DEBUG_PRINTLN("Resetting to factory defaults");
    Configuration config(String{"/basecamp.json"});
    config.load();
    config.resetExcept({ConfigurationKey::accessPointSecret, });
    config.save();
}

void putMessage(String status) {
	String url = raspiUrl + String(sensorValue) + String(batterValue); //String(ILOO_BASE_URL) + String("/v2/room/") + raspiUrl + String("/topic?auth_token=") + botAuthToken;
	Serial.println((String)"URL: " + url);
	if (!http.begin(url)) {
		Serial.println("http.begin() failed");
	}
	// http.addHeader("Content-Type", "application/json");
	//String putMessage = String("{\"topic\":\"") + status + String("\"}");
	//Serial.println(String("http put: ") + putMessage);
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
	putMessage();
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 1);
  } else {
    DEBUG_PRINTLN("Door unlocked");
	putMessage();
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 0);
  }
}

void setChatBotInterface() {
  iot.web.addInterfaceElement(
    RASPIURL,
    "input",
    "Raspi URL",
    "#configform",
    RASPIURL
  );
  
}

void setup() {
  //configuration of the battery and sensor pins
  pinMode(ResetPin, INPUT_PULLDOWN);
  pinMode(SensorPin, INPUT_PULLDOWN);
  pinMode(BatteryPin, INPUT);

  //read the status of the doorsensor as soon as possible to determine the state that triggered it
  sensorValue = digitalRead(SensorPin);
  //Read the current analog battery value
  batteryValue = analogRead(BatteryPin);

  bool resetPressed = (digitalRead(ResetPin) == HIGH);
  if (resetPressed) {
    resetToFactoryDefaults();
  }

  // Initialize Basecamp
  DEBUG_PRINTLN("initialize basecamp");
  iot.begin();
  setChatBotInterface();

  if (resetPressed) {
    DEBUG_PRINTLN("**** CONFIG HAS BEEN MANUALLY RESET ****");
  }

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
    raspiUrl = iot.configuration.get(RASPIURL);
	
    // bot.begin(token);
    printBatteryStatus();
    sleepEnable();
    esp_deep_sleep_start();
  } else {
    DEBUG_PRINTLN("access point mode");
  }
}

String batteryStatusAsString() {
  if (batteryValue > 0 && batteryValue < batteryLimit) {
    //sensorC stores the battery value as a char
    char sensorC[6];
    //convert the sensor value to a string
    sprintf(sensorC, "%04i", batteryValue);
    return String(" - battery weak (") + String(sensorC) + String(")");
  }
  return String("");
}

void printBatteryStatus() {
  //sensorC stores the battery value as a char
  char sensorC[6];
  //convert the sensor value to a string
  sprintf(sensorC, "%04i", batteryValue);
  Serial.println(String("Battery value: ") + String(sensorC));
}
void loop()
{
}
