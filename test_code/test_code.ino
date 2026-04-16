// --- Pin Definitions ---
const int pinBuzzer      = 18;  
const int pinMotorLeft   = 17;  // Left vibration motor
const int pinMotorCentre = 16;  // Centre vibration motor
const int pinMotorRight  = 15;  // Right vibration motor

// --- Configuration ---
const float MAX_DETECTION_DIST = 200.0; // Max distance in cm to trigger feedback

// --- Enums ---
enum feedbackMode { HAPTIC, AUDIO, BOTH, NUM_FEEDBACK_MODES };

// --- State Variables ---
unsigned long prevFeedbackMillis = 0;
bool pulseState = false;
bool wasInRange = true; // Added to prevent Serial Monitor spam

void setup() {
  Serial.begin(115200);
  
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinMotorLeft, OUTPUT);
  pinMode(pinMotorCentre, OUTPUT);
  pinMode(pinMotorRight, OUTPUT);

  Serial.println("System Ready. Starting Sensor Simulation...");
}

// --- Zone-Based Feedback Function ---
void applyZoneFeedback(float distL, float distC, float distR, float distU, feedbackMode currentMode) {
  
  // 1. Find the absolute closest distance
  float closestToF = min(distL, min(distC, distR));
  float absoluteClosest = min(closestToF, distU);

  // 2. Check if the closest object is within our warning threshold
  if (absoluteClosest > 0 && absoluteClosest <= MAX_DETECTION_DIST) {
    wasInRange = true;
    
    // Map the closest distance to a pulse speed
    int interval = map(constrain(absoluteClosest, 10, MAX_DETECTION_DIST), 10, MAX_DETECTION_DIST, 50, 500);

    // 3. Non-blocking pulse timer
    if (millis() - prevFeedbackMillis >= interval) {
      prevFeedbackMillis = millis();
      pulseState = !pulseState; 

      if (pulseState) { // --- PULSE ON PHASE ---
        
        Serial.print("PULSE ON  [Speed: "); 
        Serial.print(interval); 
        Serial.print("ms] -> Active Zones: ");

        // Audio Feedback
        if (currentMode == AUDIO || currentMode == BOTH) {
          digitalWrite(pinBuzzer, HIGH);
          Serial.print("BUZZER | ");
        }

        // Haptic Feedback 
        if (currentMode == HAPTIC || currentMode == BOTH) {
          if (distL <= MAX_DETECTION_DIST) {
            digitalWrite(pinMotorLeft, HIGH);
            Serial.print("LEFT MOTOR | ");
          }
          if (distC <= MAX_DETECTION_DIST || distU <= MAX_DETECTION_DIST) {
            digitalWrite(pinMotorCentre, HIGH);
            Serial.print("CENTRE MOTOR | ");
          }
          if (distR <= MAX_DETECTION_DIST) {
            digitalWrite(pinMotorRight, HIGH);
            Serial.print("RIGHT MOTOR | ");
          }
        }
        Serial.println(); // End the line
        
      } else { // --- PULSE OFF PHASE ---
        digitalWrite(pinBuzzer, LOW);
        digitalWrite(pinMotorLeft, LOW);
        digitalWrite(pinMotorCentre, LOW);
        digitalWrite(pinMotorRight, LOW);
        Serial.println("PULSE OFF");
      }
    }
  } else {
    // No objects in range
    digitalWrite(pinBuzzer, LOW);
    digitalWrite(pinMotorLeft, LOW);
    digitalWrite(pinMotorCentre, LOW);
    digitalWrite(pinMotorRight, LOW);
    pulseState = false; 

    // Print this only once when the object leaves the range, so we don't spam the console
    if (wasInRange) {
      Serial.println("--- CLEAR: No objects within 200cm ---");
      wasInRange = false;
    }
  }
}

void loop() {
  // --- SCENARIO TESTING ZONE ---
  // Change these numbers, hit Upload, and watch the Serial Monitor!
  
  float distanceLeft = 250.0;       // Left is Clear (over 200)
  float distanceCenter = 120.0;     // Centre ToF sees object at 120cm
  float distanceRight = 115.0;      // Right ToF sees object at 115cm
  float distanceUltrasonic = 130.0; // Ultrasonic confirms object ahead
  
  feedbackMode currentMode = BOTH; 
  
  applyZoneFeedback(distanceLeft, distanceCenter, distanceRight, distanceUltrasonic, currentMode);
}
