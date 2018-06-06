//Define DEBUG to get the Output from DEBUG_PRINTLN
//#define DEBUG 1

// send status updates to BASEURL/?status=<sensorValue>&battery=<batteryValue>
#define BASEURL "BaseUrl"

//Include Basecamp in this sketch
#include <Basecamp.hpp>
#include <Configuration.hpp>
#include <HTTPClient.h>

#define BASECAMP_NOMQTT  // disable MQTT *after* including Basecamp.hpp

// configuration variables - format: http://127.0.0.1/
String baseUrl;

// Create a new Basecamp instance called iot that will start the ap in secure mode and the webserver ui only in setup mode
Basecamp iot{Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::accessPoint};
HTTPClient http;

//Variables for the sensor and the battery
static const int ResetPin = 35;
static const int SensorPin = 32;
static const int BatteryPin = 34;
int sensorValue = 0;
int batteryValue = 0;

// Reset the configuration to factory defaults (all empty)
void resetToFactoryDefaults() {
    DEBUG_PRINTLN("Resetting to factory defaults");
    Configuration config(String{"/basecamp.json"});
    config.load();
    config.resetExcept({ConfigurationKey::accessPointSecret, });
    config.save();
}

void putMessage() {
    String url = baseUrl + String("/?status=") + String(sensorValue) + String("&battery=") + String(batteryValue);
    Serial.println((String)"URL: " + url);
    if (!http.begin(url)) {
        Serial.println("http.begin() failed");
    }
    // http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.GET();
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

void setBaseUrlConfigInterface() {
    iot.web.addInterfaceElement(
        BASEURL,
        "input",
        "Base URL",
        "#configform",
        BASEURL
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
    setBaseUrlConfigInterface();

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
        baseUrl = iot.configuration.get(BASEURL);

        // bot.begin(token);
        printBatteryStatus();
        sleepEnable();
        esp_deep_sleep_start();
    } else {
        DEBUG_PRINTLN("access point mode");
    }
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
