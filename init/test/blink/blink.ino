// Motor A
int ENA = 10;
int IN1 = 9;
int IN2 = 8;
// Motor B
int ENB = 5;
int IN3 = 7;
int IN4 = 6;
int delay_space=300;
bool pwm = true;
void setup ()
{
 pinMode (ENA, OUTPUT);
 pinMode (IN1, OUTPUT);
 pinMode (IN2, OUTPUT);
 pinMode (ENB, OUTPUT);
 pinMode (IN3, OUTPUT);
 pinMode (IN4, OUTPUT);

 Serial.begin(115200);
 Serial.println("Unit Testing started");
}
 
void loop ()
{
 
left_255_right_0();
 delay(15000);
 both_backward();
 delay(5000);

 left_254_right_10();
  delay(15000);
/*
  stopRobot_LOW_EN();
  delay(5000);
  

 both_forward();
  delay(5000);
 


stopRobot_HIGH_EN();
  delay(5000);


  both_forward_changing();
  delay(1000);
 
  one_forward_changing();
  delay(1000);
*/

}

void both_backward () {
  Serial.print("both backward ");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  if ( pwm)
  {
    int A_speed=random(1,254);
    int B_speed=random(1,254);
    analogWrite(ENA , A_speed);
    analogWrite(ENB, B_speed);
    Serial.print( "Speed A =");
    Serial.print( A_speed);
    Serial.print( " Speed B =");
    Serial.println( B_speed);
  }
  else
  {
   digitalWrite(ENA, HIGH);
   digitalWrite(ENB, HIGH);
   Serial.print( "Speed A =");
   Serial.print( "Full");
   Serial.print( " Speed B =");
   Serial.println( "Full");
  }
delay(5000);
}


void motor1_backward () {
  Serial.println("backward ");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  //analogWrite(ENA , 255);
 // analogWrite(ENB, 255);
}

void motor2_backward () {
  Serial.println("backward ");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  //analogWrite(ENA , 255);
 // analogWrite(ENB, 255);
}

void both_forward() {
  Serial.print("both Forward ");
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  if ( pwm)
  {
    int A_speed=random(1,254);
    int B_speed=random(1,254);
    analogWrite(ENA , A_speed);
    analogWrite(ENB, B_speed);
    Serial.print( "Speed A =");
    Serial.print( A_speed);
    Serial.print( " Speed B =");
    Serial.println( B_speed);
  }
  else
  {
   digitalWrite(ENA, HIGH);
   digitalWrite(ENB, HIGH);
   Serial.print( "Speed A =");
   Serial.print( "Full");
   Serial.print( " Speed B =");
   Serial.println( "Full");
  }
delay(5000);
}

void both_forward_changing() {
   Serial.println("both forward changing speed");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  for ( int y=0;y<255;y=y+10)
  {
  analogWrite(ENA, y);
  analogWrite(ENB, y);
  delay(delay_space);
  }
  for ( int y=255;y > 0;y=y-10)
  {
  analogWrite(ENA, y);
  analogWrite(ENB, y);
  delay(delay_space);
  }
}

void one_forward_changing() {
  Serial.println("one forward changing changing speed");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  for ( int y=0;y<255;y=y+10)
  {
  analogWrite(ENA, y);

  delay(delay_space);
  }
  for ( int y=255;y > 0;y=y-10)
  {
  analogWrite(ENA, y);

  delay(delay_space);
  }
}


void stopRobot_LOW_EN() {
  Serial.println("STOP is   ENA and ENB LOW all other pins LOW");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, LOW);
  analogWrite(ENB, LOW);
}

void stopRobot_HIGH_EN() {
  Serial.println("STOP is   ENA and ENB HIGH all other pins LOW");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, HIGH);
  analogWrite(ENB, HIGH);
}


void left_255_right_0() {
   Serial.println("left 255 right 0  FORWARD");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  

  analogWrite(ENA, 0);
  analogWrite(ENB, 255);
  delay(delay_space);
}

void left_254_right_10() {
   Serial.println("left 250 right 10  FORWARD");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  

  analogWrite(ENA, 10);
  analogWrite(ENB, 250);
  delay(delay_space);
}