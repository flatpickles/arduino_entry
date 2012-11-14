#include <Servo.h>

#define TIMEOUT 5000
#define THRESHOLD .5

// globals to manage knock sensing
int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
int led_pin_2 = 11;
int button_pin = 2;
long debounce_delay = 100;
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

// pattern detection vars
double to_match[4] = {1, 1, 1, 1};
double input[4];
int curr_knock;
long pattern_last;

boolean recording = false;
long last_record;

// configure the device and servo
void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  s1.attach(servo_pin);
  s1.write(closed_val);
  pinMode(led_pin_2, OUTPUT);
  pinMode(button_pin, INPUT); 
}
// loop to poll for knocks and handle related activity
void loop () {
  
  if(digitalRead(button_pin) == HIGH) {
    recording = true;
    last_record = millis();
    digitalWrite(led_pin_2, HIGH);
  }
   
  
  // get current piezo value
  byte val = analogRead(piezo_pin);
  //if (val > 0)
  //Serial.println(val);

  // check for timeout within lock detection
  int cur_time = millis() - pattern_last;
  if (pattern_last != -1 && cur_time > TIMEOUT) {
    curr_knock = 0;
    pattern_last = -1;
  }

  // handle a detected knock
  if (!open_state && closed_at + close_delay < millis() && // if it's not open or recently closed
      val >= knock_threshold && last + debounce_delay < millis()) { // and if value is beyond threshold and we're outside debounce
    Serial.println("knock");
    // power LED
    digitalWrite(led_pin, HIGH);
    // register in pattern detectionin
    if (pattern_last >= 0) {
      int diff = millis() - pattern_last;
            Serial.println(diff);
      input[curr_knock] = diff;
      curr_knock++;
    }
    // set debounce and pattern timing values
    last = millis();
    pattern_last = millis();
    // if recording
    last_record = millis();
  }
  
  // record timeout
  if (millis() - last_record > TIMEOUT) {
    digitalWrite(led_pin_2, LOW);
    recording = false;
  }

  // handle completed pattern
  if (curr_knock == 4) {
    // get ratios
    ratio_input_array(input, 4);
    // handle matchkno
    if (compare_array(to_match, input, 4)) open_door();
    // update to blank slate
    pattern_last = -1;
    curr_knock = 0;
  }

  // if it's been open more than open_for millis, close
  if (open_state && opened_at + open_for < millis()) {
    close_door();
  }

  // if no knock sensed and beyond last by 3*debounce, turn led off
  if (last + debounce_delay * 3 < millis()) {
    digitalWrite(led_pin, LOW);
  }
}

/****** Helpers for pattern recognition ******/

// find minimum value within array of length len
double find_min(double array[], int len) {
  int i = 0;
  double cur_min = TIMEOUT;
  for (i = 0; i < len; i++) {
    if(array[i]<cur_min) {
      cur_min = array[i];
    }
  }
  return cur_min;
}

// in place, modify array such that each element is its ratio to the minimum
void ratio_input_array(double array[], int length) {
  double array_min = find_min(array, length);

  int i = 0;
  for (i = 0; i < length; i++) {
    array[i] = array[i] / array_min;
  }
}

// compare timing values in two arrays, within error range
bool compare_array(double match_array[], double input_array[], int length) {
  int i = 0;
  for (int i = 0; i < length; i++) {
    double lower = match_array[i] - THRESHOLD;
    double higher = match_array[i] + THRESHOLD;
    Serial.println(input_array[i]);
    if (input_array[i] > higher || input_array[i] < lower) {
        return false;
    }
  }
  return true;
}

/****** Helpers for lock manipulation ******/

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

/**** Set up recording ****/


