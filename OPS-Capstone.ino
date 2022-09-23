#define LM1 2
#define LM2 4
#define RM1 6
#define RM2 7
#define lSpd 5
#define rSpd 3
#define trigPin 8
#define echoPin 9

float findPID(float error);
float elapsedTime = 0.006, speed_change = 0, error = 0;

const int Kp = 5, Kd = 0.5;
unsigned long currTime = 0, prevTime = 0; 
float prevError = 0, cumError = 0, rateError = 0, cm = 0;

//const float defaultSpeed = 220;
const float dlspd = 200, drspd = 180;

const float target = (400/2)*0.0343;
const float sensitivity = 1;

float leftSpeed = dlspd, rightSpeed = drspd;
float maxSpeed = 250;
float minSpeed = 175;
float maxDis = 26.5;
unsigned long duration;


void setup() {
  Serial.begin(9600);
  pinMode(LM1, INPUT);
  pinMode(LM2, INPUT);
  pinMode(RM1, INPUT);
  pinMode(RM2, INPUT);
  pinMode(lSpd, INPUT);
  pinMode(rSpd, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(LM1, LOW);
  digitalWrite(LM2, HIGH); // Forward is defualt when HIGH
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, HIGH); // Forward is defualt when HIGH

  // Start up car
  leftSpeed = dlspd;
  rightSpeed = drspd;
}

void loop() {
  // Reset Error
  error = 0;
  
  // Get signal
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); //Send pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Recieve pulse
  cm = (duration/2)*0.0343;

  // Distance error
  error = cm - target;

  // Speed change
  speed_change = findPID(error);

  // IF nothing
  if (error >= maxDis){
    leftSpeed = 0;
    rightSpeed = 0;
  }
  // The hand is far away
  if (error >= target + sensitivity && error < maxDis){
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH); // Forward is defualt when HIGH
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH); // Forward is defualt when HIGH
    if (speed_change > maxSpeed - leftSpeed && speed_change > maxSpeed - rightSpeed){
      leftSpeed = maxSpeed;
      rightSpeed = maxSpeed;
    }
    else{
      leftSpeed += speed_change;
      rightSpeed += speed_change;
    }
  }
  // The hand is in the good range
  if (error < target + sensitivity && error > target - sensitivity){
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH); // Forward is defualt when HIGH
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH); // Forward is defualt when HIGH
    leftSpeed = dlspd;
    rightSpeed = drspd;
  }
  // The hand is close
  if (error <= target - sensitivity && error > 3){
    leftSpeed -= speed_change;
    rightSpeed -= speed_change;
  }
  // The hand is TOO close
  if (error <= 3){
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);

    leftSpeed += speed_change;
    rightSpeed += speed_change;
    
  }

  analogWrite(lSpd, leftSpeed);
  analogWrite(rSpd, rightSpeed);

  // Reset Speeds
  leftSpeed = dlspd;
  rightSpeed = drspd;
  
}

// Define findPID
float findPID(float error){
  error = abs(error);
  currTime = millis();
  rateError = (error - prevError)/elapsedTime;
  speed_change = Kp * error + Kd * rateError;
  prevError = error;

  return speed_change;
}
