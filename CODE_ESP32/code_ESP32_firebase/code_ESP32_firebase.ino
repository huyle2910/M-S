#include "DHT.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Huy"
#define WIFI_PASSWORD "123456789"
#define API_KEY "QpPSmfQRiLIcV2BuOn6k39OxwvWDZvQn0BrDwS84"
#define DATABASE_URL "https://iot-app-ltuddd-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define DHTPIN 15
#define DHTTYPE DHT11
#define DEVICE1_PIN 5
#define DEVICE2_PIN 18
#define DEVICE3_PIN 19
#define DEVICE4_PIN 21

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

StreamLibFirebase firebase;
String main = "";

int a, b, c, d;

void setup() {
  Serial.begin(115200);
  delay(1000);
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

  config.database_url = DATABASE_URL;

  Firebase.begin(DATABASE_URL, API_KEY);
  Firebase.setDoubleDigits(5);

  pinMode(DEVICE1_PIN, OUTPUT);
  pinMode(DEVICE2_PIN, OUTPUT);
  pinMode(DEVICE3_PIN, OUTPUT);
  pinMode(DEVICE4_PIN, OUTPUT);
  digitalWrite(DEVICE1_PIN, 0);
  digitalWrite(DEVICE2_PIN, 0);
  digitalWrite(DEVICE3_PIN, 0);
  digitalWrite(DEVICE4_PIN, 0);
  dht.begin();
}

void loop() {
  float  x = dht.readHumidity();
  float  y = dht.readTemperature();
  //  x = random(0, 9);
  //  y = random(10, 19);

  if (Firebase.ready())
  {

    //Firebase.setInt(fbdo, main, 5);
    Firebase.setFloat(fbdo, "/Device/DHT11/Sensor1/humi", x);
    Firebase.setFloat(fbdo, "/Device/DHT11/Sensor1/temp", y);
    Firebase.setInt(fbdo, "/Device/LAMP/Device1", 1);
    Firebase.setInt(fbdo, "/Device/LAMP/Device2", 1);
    Firebase.setInt(fbdo, "/Device/LAMP/Device3", 0);
    Firebase.setInt(fbdo, "/Device/LAMP/Device4", 1);
    delay(200);


    Serial.printf("Get int a--  %s\n", Firebase.getInt(fbdo, "/Device/LAMP/Device1") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    a = fbdo.to<int>();
    Serial.printf("Get int b--  %s\n", Firebase.getInt(fbdo, "/Device/LAMP/Device2") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    b = fbdo.to<int>();
    Serial.printf("Get int c--  %s\n", Firebase.getInt(fbdo, "/Device/LAMP/Device3") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    c = fbdo.to<int>();
    Serial.printf("Get int d--  %s\n", Firebase.getInt(fbdo, "/Device/LAMP/Device4") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    d = fbdo.to<int>();

    Serial.println();
    Serial.print("a:");
    Serial.print(a);
    Serial.print("  b: ");
    Serial.print(b);

    Serial.println();
    Serial.println("------------------");
    Serial.println();
  }
  Serial.print(F("Humidity: "));
  Serial.print(x);
  Serial.print(F("%  Temperature: "));
  Serial.print(y);
  Serial.print(F("°C "));
  delay(1000);


}
