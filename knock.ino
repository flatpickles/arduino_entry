#include <Servo.h>

#define TIMEOUT 5000
#define THRESHOLD .5

// globals to manage knock sensing
int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
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

// knock detection vars
double to_match[4] = {1, 2, 1, 1};
double input[4];
int counter;
double x;
double temp;

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

  int cur_time = millis() - temp;

  if (temp != -1 && cur_time > TIMEOUT)
  {
    counter = 0;
    temp = -1;
    Serial.println("timeout");
  }

  if (!open_state && closed_at + close_delay < millis() && // make sure it's not open or recently closed
      val >= knock_threshold && last + debounce_delay < millis()) { // if value is beyond threshold and we're outside debounce
    Serial.println("whoooop");
    digitalWrite(led_pin, HIGH);
    last = millis();

    if(temp == -1)
    {
      temp = millis();
    }
    else
    {
      x = millis();
      int diff = x - temp;
      input[counter] = diff;
      counter++;
      Serial.println(counter);
      temp = millis();
    }


  } else if (last + debounce_delay * 3   < millis()) { // if no knock sensed and beyond last by 3*debounce, turn led off
    digitalWrite(led_pin, LOW);
  }

  if(counter == 4)
  {
    ratio_input_array(input, 4);

    int comp = compare_array(to_match, input, 4);

    if(comp == 1)
    {
      open_door();
    }

    temp = -1;
    counter = 0;
  }

  if (open_state && opened_at + open_for < millis()) { // if it's been open more than open_for millis
    close_door();
  }
}

double find_min(double array[], int length){

        int i = 0;
        double cur_min = TIMEOUT;
        for (i = 0; i < length; i++)
        {
                if(array[i]<cur_min)
                        cur_min = array[i];
        }
        return cur_min;

}

void ratio_input_array(double array[], int length){

        double min_ = find_min (array, length);
        Serial.println(min_);

        int i = 0;
        for (i = 0; i < length; i++)
        {
                array[i] = array[i] / min_;
                Serial.println(array[i]);
        }
}

int compare_array(double match_array[], double input_array[], int length)
{
        int i = 0;
        for (int i = 0; i < length; i++){
                double lower = match_array[i] - THRESHOLD;
                double higher = match_array[i] + THRESHOLD;
                if (input_array[i] > higher || input_array[i] < lower)
                {
//                    Serial.println(lower);
//                    Serial.println(higher);
//                    Serial.println(input_array[i]);
                    return 0;
                }
        }
        Serial.println("match");
        return 1;
}
