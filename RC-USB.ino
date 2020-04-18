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

volatile unsigned long start_timing[4];
volatile int values[4];

int CH1_INT;
int CH2_INT;
int CH3_INT;
int CH4_INT;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
    2, 0, true, true, false, // Buttons, Hat Switches, X, Y, Z
    false, false, false, false, false, false, false, false);

void setup() {
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
  pinMode(CH3_PIN, INPUT);
  pinMode(CH4_PIN, INPUT);

  CH1_INT = digitalPinToInterrupt(CH1_PIN);
  CH2_INT = digitalPinToInterrupt(CH2_PIN);
  CH3_INT = digitalPinToInterrupt(CH3_PIN);
  CH4_INT = digitalPinToInterrupt(CH4_PIN);

  Joystick.begin();

  Joystick.setXAxisRange(AXIS_MIN, AXIS_MAX);
  Joystick.setYAxisRange(AXIS_MIN, AXIS_MAX);
  
  attachInterrupt(CH1_INT,&ch1_isr,CHANGE);
  attachInterrupt(CH2_INT,&ch2_isr,CHANGE);
  attachInterrupt(CH3_INT,&ch3_isr,CHANGE);
  attachInterrupt(CH4_INT,&ch4_isr,CHANGE);
}

void loop() {
  Joystick.setXAxis(values[0]);
  Joystick.setYAxis(values[1]);
  Joystick.setButton(0, values[2] > SWITCH_THRESHOLD);
  Joystick.setButton(1, values[3] > SWITCH_THRESHOLD);

  delay(1);
}

void ch1_isr() {
  update_channel(CH1_PIN, 0);
}

void ch2_isr() {
  update_channel(CH2_PIN, 1);
}

void ch3_isr() {
  update_channel(CH3_PIN, 2);
}

void ch4_isr() {
  update_channel(CH4_PIN, 3);
}

void update_channel(int pin, int channel){
  if (digitalRead(pin) == HIGH) { // TODO read direct from port registers for faster reads
    start_timing[channel] = micros();
  } else {
    values[channel] = micros() - start_timing[channel];
  }
}