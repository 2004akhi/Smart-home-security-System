#define BLYNK_TEMPLATE_ID "TMPL3V0CKaReE"
#define BLYNK_TEMPLATE_NAME "Integrated Smart Home"


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

#define TRIGGER_PIN 15 // Ultrasonic sensor trigger pin
#define ECHO_PIN 13    // Ultrasonic sensor echo pin
#define IR_PIN 14      // IR sensor pin
#define RELAY_PIN 12   // Relay control pin
#define BUZZER_PIN 2  // Buzzer pin
#define LDR_PIN A0     // LDR pin

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD address and size

char auth[] = "dAFr64NVCPA6fe6coNTzaQjIiQkuBdDl";
char ssid[] = "Redmi Note 13 5G";            // WiFi SSID
char pass[] = "Akhi4@#2";        // WiFi password

NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200);  // Ultrasonic sensor object, max distance 200 cm

int ldrThreshold = 500; // LDR threshold for night detection

BlynkTimer timer;

// Function to control the outdoor bulb based on LDR reading
void checkLDR() {
  int ldrValue = analogRead(LDR_PIN);
  if (ldrValue < ldrThreshold) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn on the bulb
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Turn off the bulb
  }
}

// Function to check for security intrusion
void checkSecurity() {
  int irState = digitalRead(IR_PIN);
  int distance = sonar.ping_cm();

  if (distance > 0 && distance < 50 && irState == LOW) { // If person detected within 50 cm
    Blynk.logEvent("intruder_alert", "Intruder detected at the gate!");
    digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer for intrusion
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);  // Turn off the buzzer
  }
}

// Function to receive messages from Blynk app and display on LCD
BLYNK_WRITE(V1) {
  String message = param.asStr();
  Serial.println("Message received: " + message);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Message: ");
  lcd.setCursor(0, 1);
  lcd.print(message);
  
  digitalWrite(BUZZER_PIN, HIGH);  // Sound a beep for new message
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

// Function to control the bulb through the app
BLYNK_WRITE(V2) {
  int bulbState = param.asInt();
  digitalWrite(RELAY_PIN, bulbState);
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  timer.setInterval(1000L, checkLDR);      // Check LDR every 1 second
  timer.setInterval(2000L, checkSecurity); // Check security every 2 seconds
}

void loop() {
  Blynk.run();
  timer.run();
}
