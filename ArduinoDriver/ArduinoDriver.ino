#define COARSE_HALL_SENSOR A0
#define VISIBLE_LIGHT_SENSOR A2
#define UV_LIGHT_SENSOR A4

#define FINE_HALL_SENSOR 2
#define MOTOR_COUNTERCLOCKWISE 4
#define MOTOR_CLOCKWISE 5
#define LAMP_WARMUP_PIN 6
#define LAMP_PIN 0

//Leave these here for documentation for now
//#define GEAR_REDUCTION_RATIO 600
//#define TICKS_PER_FAST_ROATION 12
//#define DEGREES_PER_FAST_ROTATION 360
//#define MINUTES_PER_DEGREE 60

//how many hall sensor ticks are required to turn main motor 1 minute (1/60th of a degree)
#define TICKS_PER_DEGREE_TENTH 2
#define TICKS_PER_DEGREE 19 //theoretical 20

#define LAMP_WARMUP_TIME 2000 //20000

#define DUTY_CYCLE 0.

//fast write mode for motor control
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

//TODO use python serial to do computer to arudino I/O

int mode = -1;
volatile long ticks = 0;
int numTurns = 0;

void fineHallChange(){
    ticks++;
    //TODO do these help with motor accuracy?
    //CLR(PORTD, MOTOR_CLOCKWISE);
    //CLR(PORTD, MOTOR_COUNTERCLOCKWISE);
}

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_COUNTERCLOCKWISE,OUTPUT);
  pinMode(MOTOR_CLOCKWISE,OUTPUT); 
  pinMode(LAMP_WARMUP_PIN,OUTPUT); 
  pinMode(LAMP_PIN,OUTPUT); 
  CLR(PORTD, MOTOR_CLOCKWISE);
  CLR(PORTD, MOTOR_COUNTERCLOCKWISE);
  CLR(PORTD, LAMP_WARMUP_PIN);
  CLR(PORTB, LAMP_PIN);

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
      turnMotorTenthDegrees(MOTOR_CLOCKWISE, 1);
    } else {
      mode = 1;
    }
  } else if(mode == 1){
    //turn counter clockwise until in hall sensor range
    int mainHall = analogRead(COARSE_HALL_SENSOR);
    //Serial.println(mainHall);
    if(mainHall > 20){
      turnMotorTenthDegrees(MOTOR_COUNTERCLOCKWISE, 1);
    } else {
      mode = 2;
    }
  } else if(mode == 2){
    //wait for instructions
    if (Serial.available() > 0) {
      int command = Serial.read();
      //Serial.println(command, DEC);
      if(command == 51){
        //TODO let the client send how many degrees to sweep
        //TODO let the client pick uv or visible mode?
        //TODO send the client number of minutes/degrees per sensor reading
        mode = 3;
      }
    }
  } else if(mode == 3){
    //lamp warmup and turn on
    SET(PORTD, LAMP_WARMUP_PIN);
    delay(LAMP_WARMUP_TIME);
    CLR(PORTD, LAMP_WARMUP_PIN);
    SET(PORTB, LAMP_PIN);
    mode = 4;
    numTurns = 0;
  } else if(mode == 4){
    //turn clockwise while taking readings 
    int visible = analogRead(VISIBLE_LIGHT_SENSOR);
    int uv = analogRead(UV_LIGHT_SENSOR);
    Serial.println(visible);
    delay(1);
    turnMotorTenthDegrees(MOTOR_CLOCKWISE, 1); 
    if(numTurns == 900){
      mode = 5;
    }
    numTurns = numTurns + 1;
  } else if(mode == 5){
    //lamp off
    CLR(PORTB, LAMP_PIN);
    delay(5000); //cooldown
    mode = -1;
  }
}

void turnMotorTenthDegrees(int direction, int tenthDegrees){
  ticks = 0;
  long requiredTicks = (long)tenthDegrees * TICKS_PER_DEGREE_TENTH;
  while(ticks < requiredTicks){
    //TODO try analogWrite(direction, motorSpeed) instead?
    SET(PORTD, direction);
  }
  CLR(PORTD, direction);
  //Serial.println(ticks);
}

/*void turnMotorDegrees(int direction, int degrees){
  ticks = 0;
  long requiredTicks = (long)degrees * TICKS_PER_DEGREE;
  //Serial.println(requiredTicks);
  while(ticks < requiredTicks){
    SET(PORTD, direction);
    //Serial.println(ticks);
  }
  CLR(PORTD, direction);
}*/
