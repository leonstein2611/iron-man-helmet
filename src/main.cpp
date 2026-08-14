#include <Arduino.h>
#include <ESP32Servo.h>

// --- Pins ---
const int servoLeftPin  = 18;
const int servoRightPin = 19;
const int ledPin        = 23; 
const int buttonPin     = 4;

// --- Instanzen ---
Servo servoLeft;
Servo servoRight;

const int ledChannel    = 0;
const int ledFreq       = 5000;
const int ledResolution = 8;

bool isOpen = false; 

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);

  // --- NEUE AUSRICHTUNG (Z-Achse Spiegelung) ---
  // Start jetzt dort, wo vorher aufgehört 
  // Left: war vorher 180, jetzt 0
  // Right: war vorher 0, jetzt 180
  servoLeft.write(0); 
  servoRight.write(180);
  
  servoLeft.setPeriodHertz(50);
  servoRight.setPeriodHertz(50);

  servoLeft.attach(servoLeftPin, 500, 2400); 
  servoRight.attach(servoRightPin, 500, 2400);

  ledcSetup(ledChannel, ledFreq, ledResolution);
  ledcAttachPin(ledPin, ledChannel);

  pinMode(buttonPin, INPUT_PULLUP);
  ledcWrite(ledChannel, 0); 
  
  Serial.println("Stein Technologies: Z-Axis Mirroring Active.");
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    
    if (!isOpen) {
      // --- SEQUENZ: OEFFNEN ---
      Serial.println("Opening (Mirrored)...");
      
      for (int pos = 0; pos <= 110; pos++) {
        // Wir drehen die Bewegung beider Servos um:
        // Left: Geht jetzt von 0 HOCH auf 110
        servoLeft.write(pos);
        // Right: Geht jetzt von 180 RUNTER auf 70
        servoRight.write(180 - pos);
        
        // Helligkeit fadet ein
        int brightness = map(pos, 0, 110, 0, 255); 
        ledcWrite(ledChannel, brightness);
        
        delay(15); 
      }
      
      delay(200);
      ledcWrite(ledChannel, 0); // Augen oben AN
      isOpen = true;
    } 
    else {
      // --- SEQUENZ: SCHLIESSEN ---
      Serial.println("Closing (Mirrored)...");
      
      for (int pos = 110; pos >= 0; pos--) {
        // Rückwärtsbewegung
        servoLeft.write(pos);
        servoRight.write(180 - pos);
        delay(15);
      }
      
      ledcWrite(ledChannel, 0); // Augen AUS
      isOpen = false;
    }

    while(digitalRead(buttonPin) == LOW);
    delay(300);
  }
}