#include <Joystick.h>

#define CH1_PIN 1
#define CH2_PIN 0
#define CH3_PIN 2
#define CH4_PIN 3

// Nominal range is 1000-2000. Some radios allow setting up to 150% travel
// Reverse the range per Sanwa convention (Futaba will need to REV the channels)
#define AXIS_MIN 2250
#define AXIS_MAX 750
// 50% of a nominal "high" value
#define SWITCH_THRESHOLD 1750

// Update once every 100ms even if there are no changes
#define MIN_UPDATE_INTERVAL 100000

volatile uint32_t start_timing[4];
volatile uint16_t isr_values[4];
uint16_t values[4];
uint32_t last_updated;

uint8_t CH1_MASK;
uint8_t CH2_MASK;
uint8_t CH3_MASK;
uint8_t CH4_MASK;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
    2, 0, true, true, false, // Buttons, Hat Switches, X, Y, Z
    false, false, false, false, false, false, false, false);

void setup() {
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
  pinMode(CH3_PIN, INPUT);
  pinMode(CH4_PIN, INPUT);

  CH1_MASK = digitalPinToBitMask(CH1_PIN);
  CH2_MASK = digitalPinToBitMask(CH2_PIN);
  CH3_MASK = digitalPinToBitMask(CH3_PIN);
  CH4_MASK = digitalPinToBitMask(CH4_PIN);

  // Don't auto-send, otherwise we're writing to the bus multiple times during the update loop
  Joystick.begin(false);

  Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);

  attachInterrupt(digitalPinToInterrupt(CH1_PIN), &ch1_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH2_PIN), &ch2_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH3_PIN), &ch3_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH4_PIN), &ch4_isr, CHANGE);
}

void loop() {
  bool update_required = false;

  // Detect if the values have changed and send an update if any has
  for (int i = 0; i < 4; ++i) {
    // Atomic read since it's a multi-byte value
    noInterrupts();
    uint16_t value = isr_values[i];
    interrupts();
    
    if (values[i] != value) {
      values[i] = value;
      update_required = true;
    }
  }

  if (!update_required && micros() - last_updated > MIN_UPDATE_INTERVAL) {
    update_required = true;
  }

  if (update_required) {
    Joystick.setXAxis(values[0]);
    Joystick.setYAxis(values[1]);
    Joystick.setButton(0, values[2] > SWITCH_THRESHOLD);
    Joystick.setButton(1, values[3] > SWITCH_THRESHOLD);
  
    Joystick.sendState();
    last_updated = micros();
  }
}

void ch1_isr() {
  update_channel(CH1_MASK, 0);
}

void ch2_isr() {
  update_channel(CH2_MASK, 1);
}

void ch3_isr() {
  update_channel(CH3_MASK, 2);
}

void ch4_isr() {
  update_channel(CH4_MASK, 3);
}

void update_channel(uint8_t mask, int channel){
  if (PIND & mask != 0) {
    start_timing[channel] = micros();
  } else {
    isr_values[channel] = micros() - start_timing[channel];
  }
}
