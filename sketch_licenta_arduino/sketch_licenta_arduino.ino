#include <Servo.h>
#include <Stepper.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#include <AccelStepper.h>

// US - UltraSonic
// MOTOR - Motor
// SERVO - Servomotor

#define US_echoPin 2
#define US_trigPin 3
#define MOTOR_relay1 5
#define MOTOR_relay2 6

/* Constants */
const int remoteKey = 8;
const int buzzerPin = 3;
const byte IR_RECEIVE_PIN = 12;
const int joystickHorizontalPin = A5;
const int joystickVerticalPin = A4;
const int joystickButtonPin = 4;
const int engineLedPin = 7;
const int obstacleLedPin = 6;

/* Variables */
int decodedRemoteKey = 0;

bool turnedOn = true;
bool obstacleAnnounced = false;
bool obstacleFound = false;
bool turning = false;

enum Direction {
  NONE,
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD
};

class MyMotor {
  private:
    Direction m_direction;

    void Turn() {
      switch (m_direction) {
        case NONE:
          digitalWrite(MOTOR_relay1, LOW);
          digitalWrite(MOTOR_relay2, LOW);
          turning = false;
          Serial.println("stop");
          break;
        case FORWARD:
          digitalWrite(MOTOR_relay1, HIGH);
          digitalWrite(MOTOR_relay2, LOW);
          Serial.println("forward");
          break;
        case BACKWARD:
          digitalWrite(MOTOR_relay1, LOW);
          digitalWrite(MOTOR_relay2, HIGH);
          obstacleFound = false;
          Serial.println("backward");
          break;
      }
    }

  public:
    MyMotor() {
      m_direction = NONE;
    }

    bool IsNotTurning() {
      return m_direction == NONE;
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
            m_direction = FORWARD;
            break;
          case '3':
            m_direction = BACKWARD;
            break;
          default:
            m_direction = NONE;
            break;
        }
      }

      Turn();
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

    void Attach(int pin) {
      m_servo.attach(pin);
      m_position = 90;
    }

    void Turn(int value) {
      delay(10);
      if (value > -20 && value < 20) {
        if (m_direction != NONE && m_position != 90) {
          ReturnToInitial();
        }
        return;
      }

      if ((m_direction == LEFT || m_direction == NONE) && value < 0) {
        if (m_position > 0) {
          m_servo.write(--m_position);
          m_direction = LEFT;
        }
        return;
      }
      if ((m_direction == RIGHT || m_direction == NONE)  && value > 0) {
        if (m_position < 180) {
          m_servo.write(++m_position);
          m_direction = RIGHT;
        }
        return;
      }

      if (m_direction == LEFT && value > 0) {
        ReturnToInitial();
        m_direction = RIGHT;
      }
      else if (m_direction == RIGHT && value < 0) {
        ReturnToInitial();
        m_direction = LEFT;
      }
    }

    void ReturnToInitial() {
      if (m_direction == LEFT) {
        for (int pos = m_position; pos <= 90; pos++) {
          m_servo.write(pos);
          delay(5);
        }
      }
      else if (m_direction == RIGHT) {
        for (int pos = m_position; pos >= 90; pos--) {
          m_servo.write(pos);
          delay(5);
        }
      }
      m_direction = NONE;
      m_position = 90;
    }
};

MyServo myServo;
MyMotor myMotor;

const byte numChars = 3;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void setup() {
  Serial.begin(115200);
  // myServo.Attach(2);

  pinMode(US_trigPin, OUTPUT);
  pinMode(US_echoPin, INPUT);

  pinMode(MOTOR_relay1, OUTPUT);
  pinMode(MOTOR_relay2, OUTPUT);
  digitalWrite(MOTOR_relay1, LOW);
  digitalWrite(MOTOR_relay2, LOW);

  pinMode(joystickHorizontalPin, INPUT);
  pinMode(joystickVerticalPin, INPUT);
  pinMode(joystickButtonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  pinMode(engineLedPin, OUTPUT);
  pinMode(obstacleLedPin, OUTPUT);

  IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {
  if (!obstacleFound) {
    if (isObstacleAhead()) {
      obstacleFound = true;
    }
  }

  recvWithEndMarker();
  processNewData();

  myMotor.ProcessInput(receivedChars);
  
  digitalWrite(MOTOR_relay1, LOW);
  digitalWrite(MOTOR_relay2, LOW);
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
  digitalWrite(US_trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(US_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(US_echoPin, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance < 7;
}
