#include <Servo.h>

Servo fireServo; // Servo for aiming the nozzle

// Pin Definitions
const int motorPin1 = 9;
const int motorPin2 = 10;
const int servoPin = 11;

String fireSignal = "";

void setup() {
    Serial.begin(9600);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    fireServo.attach(servoPin);
    fireServo.write(90); // Neutral position
    Serial.println("System Ready. Waiting for fire data...");
}

void loop() {
    if (Serial.available() > 0) {
        fireSignal = Serial.readStringUntil('\n'); // Read incoming data
        fireSignal.trim(); // Remove leading/trailing spaces

        if (fireSignal.startsWith("FIRE")) {
            String fireXStr = fireSignal.substring(5); // Extract number
            int fireX = fireXStr.toInt(); // Convert to integer

            if (fireX >= 40 && fireX <= 560) { // Ensure valid range
                int servoAngle = map(fireX, 40, 560, 180, 0); // Reversed mapping
                
                fireServo.detach(); // Detach to prevent lag
                fireServo.attach(servoPin);
                fireServo.write(servoAngle); // Move instantly

                // Activate pump
                digitalWrite(motorPin1, HIGH);
                digitalWrite(motorPin2, LOW);
                
                Serial.print("Fire detected! Pump ON & Servo set to ");
                Serial.println(servoAngle);
            } else {
                Serial.println("Invalid fire position received.");
            }
        } 
        else if (fireSignal == "NO_FIRE") {
            // Stop pump
            digitalWrite(motorPin1, LOW);
            digitalWrite(motorPin2, LOW);
            
            // Instantly reset servo to center
            fireServo.detach();
            fireServo.attach(servoPin);
            fireServo.write(90);

            Serial.println("No fire detected. Pump OFF & Servo Reset.");
        }

        Serial.flush(); // Clear serial buffer
    }
}
