const int pinFeedbackModeButton = 19;

enum feedbackMode {
  HAPTIC,
  AUDIO,
  BOTH,
  NUM_FEEDBACK_MODES
};

class ToggleButton {
private:
  int pin;

  feedbackMode mode = HAPTIC;

  bool lastButtonState = HIGH;
  bool stableState = HIGH;

  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 20;

public:
  ToggleButton(int buttonPin) {
    pin = buttonPin;
  }

  void begin() {
    pinMode(pin, INPUT_PULLUP);
  }

  void update() {
    bool buttonState = digitalRead(pin);

    if (buttonState != lastButtonState) {
      lastDebounceTime = millis();
    }

    // If the reading stayed stable long enough
    if ((millis() - lastDebounceTime) > debounceDelay) {

      if (buttonState != stableState) {
        stableState = buttonState;

        // Detect button press (HIGH -> LOW)
        if (stableState == LOW) {
          mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
          Serial.print("BUTTON PRESSED, Feedback Mode: ");
          Serial.println(mode);
        }
      }
    }

    lastButtonState = buttonState;
  }

  feedbackMode getFeedbackMode() {
    return mode;
  }
};

ToggleButton feedbackButton(pinFeedbackModeButton);

void setup() {
  Serial.begin(9600);
  feedbackButton.begin();
}

void loop() {
  feedbackButton.update();
}