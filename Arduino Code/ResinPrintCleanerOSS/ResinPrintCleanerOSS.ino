#include <AccelStepper.h>

bool RAMPS14 = false;
uint16_t StepperSpeeds[3] = {1900,1900,50};



AccelStepper stepperWashOne(AccelStepper::DRIVER, A0, A1);
AccelStepper stepperWashTwo(AccelStepper::DRIVER, 60, 61);
AccelStepper stepperTurntable(AccelStepper::DRIVER, 46, 48);
byte stepperEnablePins[3] = {38, 56, 62};
byte buttonPins[4] = {2, 3, 14, 15};
byte lightFanPins[4] = {10, 7, 9, 8};


AccelStepper steppers[3];

uint32_t cycleTimes[4] = {120000, 60000, 240000, 240000};
uint32_t cycleStartTimes[4] = {0, 0, 0, 0};
uint32_t cycleStatus[4] = {0, 0, 0, 0};

uint32_t currentTime = 0;
byte buttonStateA[4] = {1, 1, 1, 1};
byte buttonStateB[4] = {1, 1, 1, 1};
byte buttonStateC[4] = {1, 1, 1, 1};






void setup() {
  // put your setup code here, to run once:
  for (int x = 0; x < 4; x++)
  {
    pinMode(lightFanPins[x], OUTPUT);
    pinMode(buttonPins[x], INPUT_PULLUP);
    if (x < 3)
    {
      pinMode(stepperEnablePins[x], OUTPUT);
      digitalWrite(stepperEnablePins[x], HIGH);
    }
  }
  steppers[0] = stepperWashOne;
  steppers[1] = stepperWashTwo;
  steppers[2] = stepperTurntable;

}

void checkRunTime()
{
  for (int x = 0; x++; x < 4)
  {
    if (currentTime - cycleStartTimes[x] > cycleTimes[x] && cycleStatus[x]) stopCycle(x);
  }
}

void stopCycle(byte cycle)
{
  byte x = cycle;
  if (x == 3)x = 2;
  if (cycle == 2 && !cycleStatus[3] || cycle == 3 && !cycleStatus[2] || cycle < 2) digitalWrite(stepperEnablePins[x], HIGH);
  digitalWrite(lightFanPins[cycle], LOW);
  cycleStatus[cycle] = 0;
}

void startCycle(byte cycle)
{
  byte x = cycle;
  if (x == 3)x = 2;
  digitalWrite(stepperEnablePins[x], LOW);
  if (RAMPS14 && x == 1) digitalWrite(lightFanPins[0], HIGH);
  else digitalWrite(lightFanPins[cycle], HIGH);
  
  
  if (cycle == 2 && !cycleStatus[3] || cycle == 3 && !cycleStatus[2] || cycle < 2)
  {
    steppers[x].setCurrentPosition (0);
//    if(cycle < 2)
//    {
    steppers[x].setMaxSpeed(StepperSpeeds[x]);
//    }
//    else steppers[x].setMaxSpeed(50);
    steppers[x].setAcceleration(800.0);
    steppers[x].moveTo(3000000);
  }
  cycleStartTimes[cycle] = currentTime;
  cycleStatus[cycle] = 1;

}

void readButtons()
{
  static uint32_t lastCheck = 0;
  if (currentTime - lastCheck > 10)
  {
    lastCheck = currentTime;
    for (int x = 0; x < 4; x++)
    {
      buttonStateC[x] = buttonStateB[x];
      buttonStateB[x] = buttonStateA[x];
      buttonStateA[x] = digitalRead(buttonPins[x]);
      if (buttonStateA[x] && buttonStateB[x] && !buttonStateC[x])
      {
        if (cycleStatus[x]) stopCycle(x);
        else startCycle(x);
      }
    }
  }

}



void loop() {
  currentTime = millis();
  readButtons();
  for (int x = 0; x < 4; x++)
  {
    if (x < 3)
    {
      if (cycleStatus[x]) steppers[x].run();
    }
    else if (cycleStatus[x]) steppers[x - 1].run();

  }
  // put your main code here, to run repeatedly:

}
