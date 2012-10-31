#include <Servo.h>

int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
long debounce_delay = 50;
long last;
Servo s1;

double[5] to_match = {1, 1, 1, 1, 1};
double[5] input;
int counter;
double x;
double temp;

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  s1.attach(servo_pin);
  
  // servo test
  int pos = 0;
  for (pos = 0; pos < 180; pos++) {
    s1.write(pos);
    delay(15);
  }
  
  temp = -1;
  counter = 0;
}

void loop () {
  byte val = analogRead(piezo_pin);
  
  if (val >= knock_threshold && last + debounce_delay < millis()) {
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
    }
    
    
  } else if (last + debounce_delay * 3   < millis()) {
    digitalWrite(led_pin, LOW);
  }
  
  
  if(counter == 5)
  {
    // check for validity of knock
  }
}
