#define COARSE_HALL_SENSOR A0
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define MOTOR_COUNTERCLOCKWISE 4
#define MOTOR_CLOCKWISE 5
#define FINE_HALL_SENSOR 2

#define GEAR_REDUCTION_RATIO 600

//fast write mode for motor control
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

//TODO use python serial to do computer to arudino I/O

int mode = -1;
volatile int clicks = 0;
int numTurns = 0;

void fineHallChange(){
    clicks = clicks + 1;
}

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_COUNTERCLOCKWISE,OUTPUT);
  pinMode(MOTOR_CLOCKWISE,OUTPUT); 
  CLR(PORTD, MOTOR_CLOCKWISE);
  CLR(PORTD, MOTOR_COUNTERCLOCKWISE);

  pinMode(FINE_HALL_SENSOR,INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(FINE_HALL_SENSOR), fineHallChange, RISING);
}

void loop() {
  //delay(200); 
  if(mode == -1){
    //started at magnetic sensor, find clockwise-most edge
    int mainHall = analogRead(COARSE_HALL_SENSOR);
    if(mainHall > 20){
      mode = 1;
    } else {
      mode = 0;
    }
  } else if(mode == 0){
    //turn clockwise until out of hall sensor range
    int mainHall = analogRead(COARSE_HALL_SENSOR);
    if(mainHall < 20){
      turnMotor(MOTOR_CLOCKWISE, 10);
    } else {
      mode = 1;
    }
  } else if(mode == 1){
    //turn counter clockwise until in hall sensor range
    int mainHall = analogRead(COARSE_HALL_SENSOR);
    //Serial.println(mainHall);
    if(mainHall > 20){
      turnMotor(MOTOR_COUNTERCLOCKWISE, 10);
    } else {
      mode = 2;
      numTurns = 0;
    }
  } else if(mode == 2){
    //turn clockwise while taking readings 
    int visible = analogRead(VISIBLE_LIGHT_SENSOR);
    int uv = analogRead(UV_LIGHT_SENSOR);
    Serial.println(String(numTurns) + ": " + String(visible));
    turnMotor(MOTOR_CLOCKWISE, 1);
    delay(100); 
    numTurns = numTurns + 1;
  }
}

void turnMotor(int direction, int turnClicks){
  clicks = 0;
  while(clicks < turnClicks){
    SET(PORTD, direction);
  }
  CLR(PORTD, direction);
}
