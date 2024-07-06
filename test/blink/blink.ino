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
 Serial.println("Unit Testing started -- changed inouts and chip change");
}
 
void loop ()
{
  //both_forward_changing();
 // delay(5000);
  Serial.println("START SLOW PWM");
  for ( int u=0; u<5;u++)
  {
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  delay(10);
   digitalWrite(ENA,LOW);
   digitalWrite(ENB,LOW);
    delay(10);
    Serial.print("Run =");
        Serial.println(u);
  }
    Serial.println("END SLOW PWM");
    
    Serial.println("START END IN1");
  for ( int u=0; u<5;u++)
  {
  digitalWrite(IN1,HIGH);
  delay(100);
  digitalWrite(IN1,LOW);
  delay(10);
      Serial.print("Run =");
        Serial.println(u);
  }
    Serial.println("END IN1");
    delay(5000);
  /*
 both_backward();
 delay(5000);

 stopRobot_LOW_EN();
 delay(5000);
  

 both_forward();
 delay(5000);
 
  stopRobot_HIGH_EN();
  delay(5000);



 
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
  analogWrite(ENA , 254);
   analogWrite(ENB, 254);
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
  Serial.print(" speed ");
  Serial.println(y);
  }
  for ( int y=255;y > 0;y=y-10)
  {
  analogWrite(ENA, y);
  analogWrite(ENB, y);
  delay(delay_space);
  Serial.print(" speed ");
  Serial.println(y);
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
  Serial.println("STOP ENA nad ENB LOW");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void stopRobot_HIGH_EN() {
  Serial.println("STOP ENA nad ENB HIGH");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, HIGH);
  analogWrite(ENB, HIGH);
}