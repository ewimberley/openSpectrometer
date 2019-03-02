#define MAIN_HALL_SENSOR A0
#define SECONDARY_HALL_SENSOR A1
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define MOTOR_LEFT 2
#define MOTOR_RIGHT 3

int time = 0;
int lastTime = 0;
int state = 0;
int ups = 0;
int downs = 0;

//mode 0 = seek to home position
//mode 1 = sweep
int mode = -1;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_LEFT,OUTPUT);
  pinMode(MOTOR_RIGHT,OUTPUT); 
  digitalWrite(MOTOR_RIGHT, LOW);
  digitalWrite(MOTOR_LEFT, LOW);
}

void loop() {
  delay(200); 
  if(mode == -1){
    //started at magnetic sensor, find rightmost edge
    int mainHall = analogRead(MAIN_HALL_SENSOR);
    if(mainHall > 20){
      mode = 1;
    } else {
      mode = 0;
    }
  } else if(mode == 0){
    //move right until out of hall sensor range
    int mainHall = analogRead(MAIN_HALL_SENSOR);
    if(mainHall < 20){
      digitalWrite(MOTOR_RIGHT, HIGH);
    } else {
      digitalWrite(MOTOR_RIGHT, LOW);
      mode = 1;
    }
  } else if(mode == 1){
    int mainHall = analogRead(MAIN_HALL_SENSOR);
    //Serial.println(mainHall);
    if(mainHall > 20){
      // turn the motor left
      digitalWrite(MOTOR_LEFT, HIGH);
    } else {
      digitalWrite(MOTOR_LEFT, LOW);
      mode = 2;
    }
  } else if(mode == 2){
    int secondaryHall = analogRead(SECONDARY_HALL_SENSOR);
    int visible = analogRead(VISIBLE_LIGHT_SENSOR);
    int uv = analogRead(UV_LIGHT_SENSOR);
    Serial.println(uv);
    digitalWrite(MOTOR_RIGHT, HIGH);
    if(secondaryHall > 500 && state == 0){
      ups = ups + 1;
      state = 1;
    }
    if(secondaryHall <= 500 && state == 1){
      downs = downs + 1;
      state = 0;
    }

    time = millis();
    if((time - lastTime) > 1000){
      lastTime = time;
      int freq = (ups + downs) / 2;
      //Serial.println(freq);
      ups = 0;
      downs = 0;
    }
  }
}
