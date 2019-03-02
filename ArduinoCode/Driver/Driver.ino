#define COURSE_HALL_SENSOR A0
#define FINE_HALL_SENSOR A1
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define MOTOR_COUNTERCLOCKWISE 2
#define MOTOR_CLOCKWISE 3

/*int time = 0;
int lastTime = 0;
int state = 0;
int ups = 0;
int downs = 0;*/

//mode 0 = seek to home position
//mode 1 = sweep
int mode = -1;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_COUNTERCLOCKWISE,OUTPUT);
  pinMode(MOTOR_CLOCKWISE,OUTPUT); 
  digitalWrite(MOTOR_CLOCKWISE, LOW);
  digitalWrite(MOTOR_COUNTERCLOCKWISE, LOW);
}

void loop() {
  //delay(200); 
  if(mode == -1){
    //started at magnetic sensor, find clockwise-most edge
    int mainHall = analogRead(COURSE_HALL_SENSOR);
    if(mainHall > 20){
      mode = 1;
    } else {
      mode = 0;
    }
  } else if(mode == 0){
    //move right until out of hall sensor range
    int mainHall = analogRead(COURSE_HALL_SENSOR);
    if(mainHall < 20){
      digitalWrite(MOTOR_CLOCKWISE, HIGH);
    } else {
      digitalWrite(MOTOR_CLOCKWISE, LOW);
      mode = 1;
    }
  } else if(mode == 1){
    //turn counter clockwise until in hall sensor range
    int mainHall = analogRead(COURSE_HALL_SENSOR);
    //Serial.println(mainHall);
    if(mainHall > 20){
      // turn the motor left
      digitalWrite(MOTOR_COUNTERCLOCKWISE, HIGH);
    } else {
      digitalWrite(MOTOR_COUNTERCLOCKWISE, LOW);
      mode = 2;
    }
  } else if(mode == 2){
    //turn clockwise until 
    int secondaryHall = analogRead(FINE_HALL_SENSOR);
    int visible = analogRead(VISIBLE_LIGHT_SENSOR);
    int uv = analogRead(UV_LIGHT_SENSOR);
    turnMotor(MOTOR_CLOCKWISE, 1000);
    delay(500); 
    //Serial.println(uv);
    //digitalWrite(MOTOR_CLOCKWISE, HIGH);
    /*if(secondaryHall > 500 && state == 0){
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
    }*/
  }
}

void turnMotor(int direction, int clicks){
  int secondaryHall = analogRead(FINE_HALL_SENSOR);
  int onClick = 0;
  int state = 0;
  if(secondaryHall > 500){
    state = 1;
  }
  while(onClick < clicks){
    digitalWrite(direction, HIGH);
    int secondaryHall = analogRead(FINE_HALL_SENSOR);
    if(secondaryHall > 500 && state == 0){
      state = 1;
      onClick++;
    } else {
      state = 0;
    }
  }
  digitalWrite(direction, LOW);
}
