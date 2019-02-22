int sensorPin1 = A0;    // select the input pin for the potentiometer
int sensorPin2 = A1;    // select the input pin for the potentiometer

int time = 0;
int lastTime = 0;
int state = 0;
int ups = 0;
int downs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  time = millis();
  if((time - lastTime) > 1000){
    lastTime = time;
    int freq = (ups + downs) / 2;
    Serial.println(freq);
    ups = 0;
    downs = 0;
  }
  int sensorValue1 = analogRead(sensorPin1);
  if(sensorValue1 > 500 && state == 0){
    ups = ups + 1;
    state = 1;
  }
  if(sensorValue1 <= 500 && state == 1){
    downs = downs + 1;
    state = 0;
  }
}
