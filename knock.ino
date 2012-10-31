#include <Servo.h>

// globals to manage knock sensing
int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
long debounce_delay = 50;
long last;

// globals to manage opening and open state
Servo s1;
int closed_val = 0;
int open_val = 180;
int open_state = 0;
long opened_at = 0; // when last opened
long open_for = 3000; // open for 5 seconds
long closed_at = 0; // when last closed
long close_delay = 2000; // wait 2 seconds after closing

// configure the device and servo
void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  s1.attach(servo_pin);
  s1.write(closed_val);
}

// helper to set open state(s) and write open value to the servo
void open_door() {
  open_state = 1;
  opened_at = millis();
  s1.write(open_val);
}

// helper to set close state(s) and write close value to the servo
void close_door() {
  open_state = 0;
  closed_at = millis();
  s1.write(closed_val);
}

// loop to poll for knocks and handle related activity
void loop () {
  byte val = analogRead(piezo_pin);
  
  if (!open_state && closed_at + close_delay < millis() && // make sure it's not open or recently closed
      val >= knock_threshold && last + debounce_delay < millis()) { // if value is beyond threshold and we're outside debounce
    Serial.println("whoooop");
    digitalWrite(led_pin, HIGH);
    open_door(); // open after one knock (for now)
    last = millis();
  } else if (last + debounce_delay * 3   < millis()) { // if no knock sensed and beyond last by 3*debounce, turn led off
    digitalWrite(led_pin, LOW);
  } 
  
  if (open_state && opened_at + open_for < millis()) { // if it's been open more than open_for millis
    close_door();
  }
}
