#define COARSE_HALL_SENSOR A0
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define MOTOR_COUNTERCLOCKWISE 4
#define MOTOR_CLOCKWISE 5
#define FINE_HALL_SENSOR 2

//#define GEAR_REDUCTION_RATIO 600
//#define TICKS_PER_FAST_ROATION 12
//#define DEGREES_PER_FAST_ROTATION 360
//#define MINUTES_PER_DEGREE 60

//how many hall sensor ticks are required to turn main motor 1 minute (1/60th of a degree)
#define TICKS_PER_MINUTE 120
#define TICKS_PER_DEGREE 19 //theoretical 20

//fast write mode for motor control
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

//TODO use python serial to do computer to arudino I/O

int mode = -1;
volatile long ticks = 0;
int numTurns = 0;

void fineHallChange(){
    ticks = ticks + 1;
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
      turnMotor(MOTOR_CLOCKWISE, 1);
    } else {
      mode = 1;
    }
  } else if(mode == 1){
    //turn counter clockwise until in hall sensor range
    int mainHall = analogRead(COARSE_HALL_SENSOR);
    //Serial.println(mainHall);
    if(mainHall > 20){
      turnMotor(MOTOR_COUNTERCLOCKWISE, 1);
    } else {
      mode = 2;
      numTurns = 0;
    }
  } else if(mode == 2){
    //turn clockwise while taking readings 
    int visible = analogRead(VISIBLE_LIGHT_SENSOR);
    int uv = analogRead(UV_LIGHT_SENSOR);
    Serial.println(String(numTurns) + ": " + String(visible));
    turnMotor(MOTOR_CLOCKWISE, 90); 
    delay(1000);
    numTurns = numTurns + 1;
  }
}

void turnMotor(int direction, int degrees){
  ticks = 0;
  long requiredTicks = (long)degrees * TICKS_PER_DEGREE;
  //Serial.println(requiredTicks);
  while(ticks < requiredTicks){
    SET(PORTD, direction);
    //Serial.println(ticks);
  }
  CLR(PORTD, direction);
}
