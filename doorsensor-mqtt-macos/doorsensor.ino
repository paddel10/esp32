//Define DEBUG to get the Output from DEBUG_PRINTLN
#define DEBUG 1

//Include Basecamp in this sketch
#include <Basecamp.hpp>
#include <Configuration.hpp>

// Create a new Basecamp instance called iot that will start the ap in secure mode and the webserver ui only in setup mode
Basecamp iot{Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::accessPoint};
// Uncomment the following line and comment to one above to start the ESP with open wifi and always running config ui
// Basecamp iot;

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

void setup() {
  //configuration of the battery and sensor pins
  pinMode(ResetPin, INPUT_PULLDOWN);
  pinMode(SensorPin, INPUT_PULLDOWN);
  pinMode(BatteryPin, INPUT);

  //read the status of the doorsensor as soon as possible to determine the state that triggered it
  sensorValue = digitalRead(SensorPin);

  bool resetPressed = (digitalRead(ResetPin) == HIGH);
  if (resetPressed)
  {
    resetToFactoryDefaults();
  }

  // Initialize Basecamp
  iot.begin();
  // Alternate example: optional initialization with a fixed ap password for setup-mode:
  // iot.begin("yoursecurepassword");

  if (resetPressed) {
    DEBUG_PRINTLN("**** CONFIG HAS BEEN MANUALLY RESET ****");
  }

  //Configure the MQTT topics
  delaySleepTopic = "cmd/" + iot.hostname + "/delaysleep";
  statusTopic = "stat/" + iot.hostname + "/status";
  batteryTopic = "stat/" + iot.hostname + "/battery";
  batteryValueTopic = "stat/" + iot.hostname + "/batteryvalue";

  //Set up the Callbacks for the MQTT instance. Refer to the Async MQTT Client documentation
  // TODO: We should do this actually _before_ connecting the mqtt client...
  iot.mqtt.onConnect(onMqttConnect);
  iot.mqtt.onPublish(suspendESP);
  iot.mqtt.onMessage(onMqttMessage);
}


//This function is called when the MQTT-Server is connected
void onMqttConnect(bool sessionPresent) {
  DEBUG_PRINTLN(__func__);

  //Subscribe to the delay topic
  iot.mqtt.subscribe(delaySleepTopic.c_str(), 0);
  //Trigger the transmission of the current state.
  transmitStatus();
}


//This function transfers the state of the sensor. That includes the door status, battery status and level
void transmitStatus() {
  DEBUG_PRINTLN(__func__);

  if (sensorValue == 0) {
    DEBUG_PRINTLN("Door open");
    //Transfer the current state of the sensor to the MQTT broker
    statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "open" );
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 1);
  } else {
    DEBUG_PRINTLN("Door closed");
    //Transfer the current state of the sensor to the MQTT broker
    statusPacketIdSub = iot.mqtt.publish(statusTopic.c_str(), 1, true, "closed" );
    //Configure the wakeup pin to wake if the door is closed
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SensorPin, 0);
  }

  //Read the current analog battery value
  sensorValue = analogRead(BatteryPin);
  //sensorC stores the battery value as a char
  char sensorC[6];
  //convert the sensor value to a string
  sprintf(sensorC, "%04i", sensorValue);
  //Send the sensor value to the MQTT broker
  iot.mqtt.publish(batteryValueTopic.c_str(), 1, true, sensorC);
  //Check the battery level and publish the state
  if (sensorValue < batteryLimit) {
    DEBUG_PRINTLN("Battery empty");
    iot.mqtt.publish(batteryTopic.c_str(), 1, true, "empty" );
  } else {
    DEBUG_PRINTLN("Battery full");
    iot.mqtt.publish(batteryTopic.c_str(), 1, true, "full" );
  }
  DEBUG_PRINTLN("Data published");
}

//This topic is called if an MQTT message is received
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DEBUG_PRINTLN(__func__);

  //Check if the payload eqals "true" and set delaySleep correspondigly
  //Since we only subscribed to one topic, we only have to compare the payload
  if (strcmp(payload, "true") == 0)  {
    delaySleep = true;
  } else  {
    delaySleep = false;
  }
}

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
    iot.mqtt.disconnect();
    //send the ESP into deep sleep
    esp_deep_sleep_start();
  }
}

void loop()
{
}
