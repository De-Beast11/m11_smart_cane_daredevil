// --- Additional Pin Definitions ---
const int pinBuzzer      = 25;  
const int pinMotorLeft   = 26;  // Left vibration motor
const int pinMotorCentre = 27;  // Centre vibration motor
const int pinMotorRight  = 14;  // Right vibration motor

// --- Enums ---
enum feedbackMode { HAPTIC, AUDIO, BOTH, NUM_FEEDBACK_MODES };

enum ObjectDirection { 
  NONE, 
  LEFT, 
  CENTRE_LEFT, 
  CENTRE, 
  CENTRE_RIGHT, 
  RIGHT 
};

// --- State Variables ---
unsigned long prevFeedbackMillis = 0;
bool pulseState = false;

// (Assume ToggleButton class and modeButton object are defined here as before)

void setup() {
  Serial.begin(115200);
  
  // Initialize output pins
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinMotorLeft, OUTPUT);
  pinMode(pinMotorCentre, OUTPUT);
  pinMode(pinMotorRight, OUTPUT);
  
  // modeButton.begin(); // Setup your button
  // Initialize sensors...
}

// --- Directional Feedback Function ---
void applyDirectionalFeedback(float distance, ObjectDirection dir, feedbackMode currentMode) {
  // 1. Check if the object is within the warning threshold (e.g., 200cm)
  if (distance > 0 && distance <= 200.0 && dir != NONE) {
    
    // 2. Map distance to pulse speed: Closer = Faster (50ms to 500ms)
    // constrain() ensures distance doesn't drop below 10 or exceed 200 before mapping
    int interval = map(constrain(distance, 10, 200), 10, 200, 50, 500);

    // 3. Non-blocking pulse timer
    if (millis() - prevFeedbackMillis >= interval) {
      prevFeedbackMillis = millis();
      pulseState = !pulseState; // Toggle the pulse ON or OFF

      // If the pulse is in the "ON" phase
      if (pulseState) {
        
        // --- AUDIO FEEDBACK ---
        if (currentMode == AUDIO || currentMode == BOTH) {
          digitalWrite(pinBuzzer, HIGH);
        }

        // --- HAPTIC FEEDBACK ---
        if (currentMode == HAPTIC || currentMode == BOTH) {
          // Logic mapping: Turns HIGH only if the direction matches the motor's zone
          digitalWrite(pinMotorLeft,   (dir == LEFT || dir == CENTRE_LEFT) ? HIGH : LOW);
          digitalWrite(pinMotorCentre, (dir == CENTRE_LEFT || dir == CENTRE || dir == CENTRE_RIGHT) ? HIGH : LOW);
          digitalWrite(pinMotorRight,  (dir == CENTRE_RIGHT || dir == RIGHT) ? HIGH : LOW);
        }
        
      } else {
        // If the pulse is in the "OFF" phase, turn everything off
        digitalWrite(pinBuzzer, LOW);
        digitalWrite(pinMotorLeft, LOW);
        digitalWrite(pinMotorCentre, LOW);
        digitalWrite(pinMotorRight, LOW);
      }
    }
  } else {
    // If no object is detected, ensure all outputs stay off
    digitalWrite(pinBuzzer, LOW);
    digitalWrite(pinMotorLeft, LOW);
    digitalWrite(pinMotorCentre, LOW);
    digitalWrite(pinMotorRight, LOW);
    pulseState = false; // Reset pulse state
  }
}

void loop() {
  // modeButton.update();
  // feedbackMode currentMode = modeButton.getMode();
  
  // --- PLACEHOLDER SENSOR LOGIC ---
  // In your actual code, you will use the 16 zones (4x4 matrix) of your VL53L7CX 
  // ToF sensors and the ultrasonic data to calculate these two variables:
  float detectedDistance = 85.5; // Example: Object is 85.5cm away
  ObjectDirection detectedDirection = CENTRE_RIGHT; // Example direction
  feedbackMode currentMode = BOTH; // Example mode
  
  // Apply the dynamic feedback
  applyDirectionalFeedback(detectedDistance, detectedDirection, currentMode);
}
