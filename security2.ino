#define BLYNK_TEMPLATE_ID "TMPL3PDUXUxGr"
#define BLYNK_TEMPLATE_NAME "Smart Security and Energy Efficiency System"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NewPing.h>

// Pin assignments
#define TRIG_PIN 14
#define ECHO_PIN 12
#define IR_SENSOR_PIN 13
#define RELAY_PIN 5
#define LED_PIN 4

// Constants for the ultrasonic sensor
#define MAX_DISTANCE 200

// Initialize Blynk
char auth[] = "Std3QftZ_xKPGHGumKzMyxjV27NLhJQM";  // Blynk Auth Token
char ssid[] = "Redmi Note 13 5G";  // Wi-Fi SSID
char pass[] = "Akhi4@#2";  // Wi-Fi Password

// Ultrasonic sensor
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Variables
bool irDetected = false;
bool relayState = false;

void setup() {
  Serial.begin(115200);

  // Setup pins
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Initialize Blynk connection
  Blynk.begin(auth, ssid, pass);

  // Set initial states
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);  // Ensure LED starts off
}

void loop() {
  Blynk.run();

  // Check IR Sensor for motion detection
  irDetected = digitalRead(IR_SENSOR_PIN);
  if (irDetected) {
    Serial.println("IR Sensor: Motion Detected!");
    digitalWrite(LED_PIN, LOW);  // Turn on LED
    digitalWrite(RELAY_PIN, HIGH);  // Turn on relay (if used for appliance control)
    Blynk.virtualWrite(V1, 1);  // Send signal to Blynk app
  } else {
    digitalWrite(LED_PIN, HIGH);  // Turn off LED when no motion
    digitalWrite(RELAY_PIN, LOW);  // Ensure relay is off
    Blynk.virtualWrite(V1, 0);  // Reset Blynk pin
  }

  // Check Ultrasound Sensor for object detection
  int distance = sonar.ping_cm();
  if (distance > 0 && distance < 50) {
    Serial.println("Ultrasound Sensor: Object Detected!");
    digitalWrite(LED_PIN, HIGH);  // Turn on LED if object is detected within range
    Blynk.virtualWrite(V2, 1);  // Send signal to Blynk app
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off LED when no object is detected
    Blynk.virtualWrite(V2, 0);  // Reset Blynk pin
  }

  // Add a short delay to prevent LED flickering and improve stability
  delay(500);
}
