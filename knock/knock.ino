#include <Servo.h>

int piezo_pin = 0;
int led_pin = 13;
int servo_pin = 12;
int knock_threshold = 3;
long debounce_delay = 50;
long last;
Servo s1;

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
}

void loop () {
  byte val = analogRead(piezo_pin);
  
  if (val >= knock_threshold && last + debounce_delay < millis()) {
    Serial.println("whoooop");
    digitalWrite(led_pin, HIGH);
    last = millis();
  } else if (last + debounce_delay * 3   < millis()) {
    digitalWrite(led_pin, LOW);
  }
}

double find_min(double[] array, int length){
	
	int i = 0;
	double cur_min = TIMEOUT;
	for (i = 0; i < length; i++)
	{
		if(array[i]<cur_min)
			cur_min = array[i];
	}
	return cur_min;

}

void ratio_input_array(double[] array, int length){
	
	double min = find_min (array, length);

	int i = 0;
	for (i = 0; i < length; i++)
	{
		array[i] = array[i] / min;
	}
}

int compare_array(double[] match_array, double[] input_array, length)
{
	int i = 0;
	for (int i = 0; i < length; i++){
		double lower = match_array[i] - THRESHOLD;
		double higher = match_array[i] + THRESHOLD;
		if (input_array[i] > higher || input_array[i] < lower)
			return 0;
	}	
	return 1;
}





