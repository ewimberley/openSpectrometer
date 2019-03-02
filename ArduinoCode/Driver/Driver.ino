#define COARSE_HALL_SENSOR A0
#define FINE_HALL_SENSOR A1
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define MOTOR_COUNTERCLOCKWISE 2
#define MOTOR_CLOCKWISE 3

#define GEAR_REDUCTION_RATIO 600

//XXX use fast write mode for motor control
//#define CLR(x,y) (x&=(~(1<<y)))
//#define SET(x,y) (x|=(1<<y))
//SET(PORTB, 0);

//XXX use external interrupts for hall sensor changes/counting

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
