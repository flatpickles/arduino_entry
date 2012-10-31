#include <Servo.h>

#define TIMEOUT 5000
#define THRESHOLD .5

int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
long debounce_delay = 100;
long last;
Servo s1;

double to_match[4] = {1, 2, 1, 1};
double input[4];
int counter;
double x;
double temp;

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  s1.attach(servo_pin);
  
  temp = -1;
  counter = 0;
}

void loop () {
  byte val = analogRead(piezo_pin);
  
  int cur_time = millis() - temp;
  
  if (temp != -1 && cur_time > TIMEOUT)
    {
      counter = 0;
      temp = -1;
      Serial.println("timeout");
    }
  
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
      Serial.println(counter);
      temp = millis();
    }
    
    
  } else if (last + debounce_delay * 3   < millis()) {
    digitalWrite(led_pin, LOW);
  }
  
  
  if(counter == 4)
  {
    ratio_input_array(input, 4);
    
    int comp = compare_array(to_match, input, 4);
    
    if(comp == 1)
    {
      s1.write(0);
      delay(1000);
      s1.write(180);
      delay(2000);
    }
    
    temp = -1;
    counter = 0;
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
