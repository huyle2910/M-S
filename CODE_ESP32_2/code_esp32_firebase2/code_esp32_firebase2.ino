#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "DHT.h"
/* 1. Define the WiFi credentials */
#define WIFI_SSID "Huy"
#define WIFI_PASSWORD "123456789"

#define API_KEY "AIzaSyDZMgWA8s9gZmoWi_F2-qBRU1jvf2du7cQ"
#define DATABASE_URL "https://iot-app-ltuddd-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "huynh7a123@gmail.com"
#define USER_PASSWORD "24032001"


#define DHTPIN 15
#define DHTTYPE DHT11
#define DEVICE2_PIN 5
#define DEVICE3_PIN 18
#define DEVICE4_PIN 19
#define DEVICE5_PIN 21

DHT dht(DHTPIN, DHTTYPE);
// Define Firebase Data object
FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

int count = 0;

void create_node_lamp(String device_num, String name, int serial, int state, String type);
void create_node_dht11(String device_num, String name, int serial, int humid, int temp , String type);
void update_node_dht11(String device_num, int humid, int temp);
//void handle_device(String device_path, );

void streamCallback(StreamData data)
{
  String device_path = data.dataPath();
  Serial.println(device_path);
  Serial.println(data.to<int>());
  if (device_path == "/Device2/state")
    digitalWrite(DEVICE2_PIN, data.to<int>());
  else if (device_path == "/Device3/state")
    digitalWrite(DEVICE3_PIN, data.to<int>());
  else if (device_path == "/Device4/state")
    digitalWrite(DEVICE4_PIN, data.to<int>());
  else if (device_path == "/Device5/state")
    digitalWrite(DEVICE5_PIN, data.to<int>());
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(stream, "/Device"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);

  pinMode(DEVICE5_PIN, OUTPUT);
  pinMode(DEVICE2_PIN, OUTPUT);
  pinMode(DEVICE3_PIN, OUTPUT);
  pinMode(DEVICE4_PIN, OUTPUT);
  digitalWrite(DEVICE5_PIN, 0);
  digitalWrite(DEVICE2_PIN, 0);
  digitalWrite(DEVICE3_PIN, 0);
  digitalWrite(DEVICE4_PIN, 0);
  create_node_dht11("Device1", "Sensor1", 20192910, 0, 0 , "DHT11");
  create_node_lamp("Device2", "Lamp 1", 20192911, 0, "LAMP");
  create_node_lamp("Device3", "Lamp 2", 20192912, 0, "LAMP");
  create_node_lamp("Device4", "Lamp 3", 20192913, 0, "LAMP");
  create_node_lamp("Device5", "Lamp 4", 20192914, 0, "LAMP");
  dht.begin();
}

/*--------------------------------------main----------------------------------*/
void loop()
{
  int  h = dht.readHumidity();
  int  t = dht.readTemperature();
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    update_node_dht11("Device1", h, t);
  }
}


void create_node_lamp(String device_num, String name, int serial, int state, String type)
{
  FirebaseJson json;
  json.add("name", name);
  json.add("serial", serial);
  json.add("state", state);
  json.add("type", type);
  Firebase.setJSON(fbdo, "/Device/" + device_num , json);
}

void create_node_dht11(String device_num, String name, int serial, int humid, int temp , String type)
{
  FirebaseJson json;
  json.add("name", name);
  json.add("serial", serial);
  json.add("humid", humid);
  json.add("temp", temp);
  json.add("type", type);
  Firebase.setJSON(fbdo, "/Device/" + device_num , json);
}

void update_node_dht11(String device_num, int humid, int temp)
{
  Firebase.setInt(fbdo, "/Device/" + device_num + "/humid", humid);
  Firebase.setInt(fbdo, "/Device/" + device_num + "/temp", temp);
}
