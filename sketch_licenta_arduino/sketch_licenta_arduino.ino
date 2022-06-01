#include <Servo.h>
#include <Stepper.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#include <AccelStepper.h>

// US - UltraSonic
// MOTOR - Motor
// SERVO - Servomotor

#define USEchoPin 2
#define USTrigPin 3
#define MOTORFirstRelayPin 4
#define MOTORSecondRelayPin 5
#define MOTORENA 6
#define SERVOPin 7
#define SERVOPositionInitial 105
#define SERVOPositionOffset 30

bool turnedOn = true;
bool obstacleAnnounced = false;
bool obstacleFound = false;
bool turning = false;

enum Direction {
  NONE,
  FORWARDLEFT,
  FORWARD,
  FORWARDRIGHT,
  BACKWARDLEFT,
  BACKWARD,
  BACKWARDRIGHT
};

class MyMotor {
  private:
    Direction m_direction;

  public:
    void Turn() {
      switch (m_direction) {
        case NONE:
          digitalWrite(MOTORFirstRelayPin, LOW);
          digitalWrite(MOTORSecondRelayPin, LOW);
          turning = false;
          break;
        case FORWARDLEFT:
        case FORWARD:
        case FORWARDRIGHT:
          digitalWrite(MOTORFirstRelayPin, HIGH);
          digitalWrite(MOTORSecondRelayPin, LOW);
          break;
        case BACKWARDLEFT:
        case BACKWARD:
        case BACKWARDRIGHT:
          digitalWrite(MOTORFirstRelayPin, LOW);
          digitalWrite(MOTORSecondRelayPin, HIGH);
          obstacleFound = false;
          Serial.print("00\n");
          break;
      }
    }

    MyMotor() {
      m_direction = NONE;
    }

    void SetDirection(Direction value) {
      m_direction = value;
    }

    bool IsNotTurning() {
      return m_direction == NONE;
    }
};

class MyServo {
  private:
    Servo m_servo;
    Direction m_direction;
    int m_position;

  public:
    MyServo() {
      m_direction = NONE;
    }

    bool IsNotTurning() {
      return m_direction == NONE;
    }

    void SetDirection(Direction value) {
      m_direction = value;
    }

    void Attach(int pin) {
      m_servo.attach(pin);
      m_position = SERVOPositionInitial;
    }

    void Turn() {
      switch (m_direction) {
        case FORWARDLEFT:
        case BACKWARDLEFT:
          m_servo.write(SERVOPositionInitial - SERVOPositionOffset);
          break;
        case FORWARDRIGHT:
        case BACKWARDRIGHT:
          m_servo.write(SERVOPositionInitial + SERVOPositionOffset);
          break;
        default:
          m_servo.write(SERVOPositionInitial);
          break;
      }
    }
};

class MyCar {
  private:
    MyServo m_myServo;
    MyMotor m_myMotor;
    Direction m_direction;

  public:
    void Initialize() {
      m_myServo.SetDirection(NONE);
      m_myMotor.SetDirection(NONE);
      m_myServo.Attach(SERVOPin);
    }

    void ProcessInput(char input[3]) {
      if (!turning && m_direction == NONE) {
        return;
      }
      if (input[1] == '0') {
        m_direction = NONE;
      }
      else {
        switch (input[0]) {
          case '1':
            if (obstacleFound) {
              m_direction = NONE;
              break;
            }
            m_direction = FORWARDLEFT;
            break;
          case '2':
            if (obstacleFound) {
              m_direction = NONE;
              break;
            }
            m_direction = FORWARD;
            break;
          case '3':
            if (obstacleFound) {
              m_direction = NONE;
              break;
            }
            m_direction = FORWARDRIGHT;
            break;
          case '4':
            m_direction = BACKWARDLEFT;
            break;
          case '5':
            m_direction = BACKWARD;
            break;
          case '6':
            m_direction = BACKWARDRIGHT;
            break;
          default:
            m_direction = NONE;
            break;
        }
      }

      m_myServo.SetDirection(m_direction);
      m_myMotor.SetDirection(m_direction);
      m_myServo.Turn();
      m_myMotor.Turn();
    }
};

MyCar myCar;

const byte numChars = 3;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void setup() {
  myCar.Initialize();
  
  Serial.begin(115200);

  pinMode(USTrigPin, OUTPUT);
  pinMode(USEchoPin, INPUT);
  pinMode(MOTORENA, OUTPUT);
  pinMode(MOTORFirstRelayPin, OUTPUT);
  pinMode(MOTORSecondRelayPin, OUTPUT);
  digitalWrite(MOTORFirstRelayPin, LOW);
  digitalWrite(MOTORSecondRelayPin, LOW);

  analogWrite(MOTORENA, 190);
}

void loop() {
  if (!obstacleFound) {
    if (isObstacleAhead()) {
      obstacleFound = true;
      Serial.print("01\n");
    }
  }

  recvWithEndMarker();
  processNewData();

  myCar.ProcessInput(receivedChars);
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void processNewData() {
  if (newData == true) {
    if (receivedChars[numChars - 2] == '1') {
      turning = true;
    } else if (receivedChars[numChars - 2] == '0') {
      turning = false;
    }

    newData = false;
  }
}

bool isObstacleAhead() {
  // Clears the trigPin condition
  digitalWrite(USTrigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(USTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(USTrigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(USEchoPin, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance < 10;
}
