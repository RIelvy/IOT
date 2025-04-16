#include <DHT.h>
#include <DHT_U.h>
#include "secrets.h"
#include <Firebase.h>
#include <ArduinoJson.h>

Firebase fb(REFERENCE_URL);

#define ledPin1     D1    // Pin LED 1
#define ledPin2     D2    // Pin LED 2
#define ledPin3     D3    // Pin LED 3
#define DHTPIN      D5    // Pin Data Sensor DHT11
#define DHTTYPE DHT11     // Tipe sensor DHT11
#define echoPin     D6    // Pin Echo Sensor Ultrasonik
#define triggerPin  D7    // Pin Trigger Sensor Ultrasonik

// Firebase fb(REFERENCE_URL, AUTH_TOKEN);
// Variabel untuk menyimpan data sensor
long duration, jarak;     // Variabel sensor ultrasonik
int led1, led2, led3;     // Status LED
float temperatur, humid;                // Variabel suhu (t) dan kelembaban (h)
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Setup pin mode
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);

  // Konfigurasi WiFi
  #if !defined(ARDUINO_UNOWIFIR4)
    WiFi.mode(WIFI_STA);
  #else
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif
  WiFi.disconnect();
  delay(1000);

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println();

  #if defined(ARDUINO_UNOWIFIR4)
    digitalWrite(LED_BUILTIN, HIGH);
  #endif

  /* ----- */ 

  /* ----- Serialization: Set example data in Firebase ----- */

  // Create a JSON document to hold the output data
   StaticJsonDocument<200> docOutput;

  // Add various data types to the JSON document
  docOutput["led1"] = "0";  //key1
  docOutput["led2"] = "0";  //key2 
  docOutput["led3"] = "0";  //key3

  // Create a string to hold the serialized JSON data
  String output;

  // Optional: Shrink the JSON document to fit its contents exactly
  // docOutput.shrinkToFit();

  // Serialize the JSON document to a string
  serializeJson(docOutput, output);

  // Set the serialized JSON data in Firebase
  fb.setJson("dataLED", output);

  /* ----- Deserialization: Retrieve example data from Firebase ----- */

  // Mengambil data LED dari Firebase
  String input = fb.getJson("dataLED");

  // Check if the retrieved data is "NULL", indicating a retrieval error
  if (input == "NULL") {
    Serial.println("Could not retrieve data from Firebase");
  } else {
    StaticJsonDocument<200> docInput;
    DeserializationError error = deserializeJson(docInput, input);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Mengambil status LED
    led1 = docInput["led1"];               // 0 atau 1
    led2 = docInput["led2"];               // 0 atau 1
    led3 = docInput["led3"];               // 0 atau 1

    /* Print the deserialized input */
    Serial.print("Retrieved String LED1:\t");
    Serial.println(led1);
    Serial.print("Retrieved String LED2:\t");
    Serial.println(led2);
    Serial.print("Retrieved String LED3:\t");
    Serial.println(led3);

  }

  // Remove the example data from Firebase
  // fb.remove("Example");
}

void loop() {
  // Baca dan kirim data sensor jarak (Ultrasonik)
  dataUltrasonic();
  StaticJsonDocument<200> docOutput;
  docOutput["jarak"] = jarak;
  String output;
  serializeJson(docOutput, output);
  fb.setJson("dataUltrasonic", output);

  // Baca dan kirim data sensor suhu & kelembaban (DHT11)
  dataDHT();
  StaticJsonDocument<200> docDHT;
  docDHT["temperatur"] = String(temperatur, 2);    // Suhu
  docDHT["humid"] = humid;               //Kelembapan
  String dhtData;
  serializeJson(docDHT, dhtData);
  fb.setJson("dataDHT", dhtData);

  // Ambil data kontrol LED dari Firebase
  String input = fb.getJson("dataLED");

  // Check if the retrieved data is "NULL", indicating a retrieval error
  if (input == "NULL") {
    Serial.println("Could not retrieve data from Firebase");
  } else {
    // Create a JSON document to hold the deserialized data
    // JsonDocument docInput;
    StaticJsonDocument<200> docInput;

    // Deserialize the JSON string into the JSON document
    DeserializationError error = deserializeJson(docInput, input);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    led1 = docInput["led1"];               // 0 atau 1
    led2 = docInput["led2"];               // 0 atau 1
    led3 = docInput["led3"];               // 0 atau 1

    // Kontrol LED
    
    //kendali LED1
    if(led1 == 1) digitalWrite(ledPin1, HIGH);
    else digitalWrite(ledPin1, LOW);
  
    //kendali LED2
    if(led2 == 1) digitalWrite(ledPin2, HIGH);
    else digitalWrite(ledPin2, LOW);
  
    //kendali LED3
    if(led3 == 1) digitalWrite(ledPin3, HIGH);
    else digitalWrite(ledPin3, LOW);
}
  // Print semua data sensor dan status
    Serial.print("Retrieved String LED1:\t");
    Serial.println(led1);
    Serial.print("Retrieved String LED2:\t");
    Serial.println(led2);
    Serial.print("Retrieved String LED3:\t");
    Serial.println(led3);

    Serial.print(F("Humidity: "));
    Serial.print(humid);
    Serial.print("\n");
    Serial.print(F("Temperature: "));
    Serial.print(temperatur);
    Serial.print(F("°C "));

    Serial.print("Jarak:\t");
    Serial.println(jarak);
}

// Fungsi membaca data Sensor Ultrasonik HC-SR04
void dataUltrasonic() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration/2) / 29.1;
}

// Fungsi membaca data Sensor DHT11
void dataDHT() {
  humid = dht.readHumidity();     // Membaca kelembaban
  temperatur = dht.readTemperature();  // Membaca suhu dalam Celsius
}
