#define MOTOR_PIN 6
#define BUTTON_PIN 8
#define BLU_PIN 9
#define RED_PIN 10

#define MS_PER_MIN 60000

#define FALLING_ASLEEP_TIME 10
#define DOZING_INTERVAL 2
#define SNOOZING_INTERVAL 10
#define SLUMBERING_INTERVAL 240

#define DEBOUNCE_DURATION 50
#define VIBRATION_DURATION 1500

enum SleepingType
{
  DOZING,
  SNOOZING,
  SLUMBERING
};

SleepingType currentSleepingType = DOZING;
int interval = DOZING_INTERVAL;

unsigned long previousMillis = 0;
unsigned long vibrationStartedAt = 0;

unsigned long lastTimeButtonStateChanged = 0;

bool asleep = false;
byte lastButtonState = LOW;

void setup()
{
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);
}

void loop()
{
  unsigned long currentMillis = millis();

  switch (currentSleepingType)
  {
  case DOZING:
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(BLU_PIN, LOW);
    break;
  case SNOOZING:
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLU_PIN, HIGH);
    break;
  case SLUMBERING:
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(BLU_PIN, HIGH);
    break;
  }

  if (currentMillis - lastTimeButtonStateChanged > DEBOUNCE_DURATION)
  {
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState)
    {
      lastTimeButtonStateChanged = currentMillis;
      lastButtonState = buttonState;
      if (buttonState == LOW)
      {
        switch (currentSleepingType)
        {
        case DOZING:
          currentSleepingType = SNOOZING;
          interval = SNOOZING_INTERVAL;
          break;
        case SNOOZING:
          currentSleepingType = SLUMBERING;
          interval = SLUMBERING_INTERVAL;
          break;
        case SLUMBERING:
          currentSleepingType = DOZING;
          interval = DOZING_INTERVAL;
          break;
        }
      }
    }
  }

  if (previousMillis == 0)
  {
    previousMillis = currentMillis;
  }

  byte mintuesPassed = (currentMillis - previousMillis) / MS_PER_MIN;

  if (asleep)
  {
    if (mintuesPassed >= interval)
    {

      if (vibrationStartedAt == 0)
      {
        vibrationStartedAt = currentMillis;
      }

      if ((currentMillis - vibrationStartedAt) >= VIBRATION_DURATION)
      {
        previousMillis = currentMillis;
        vibrationStartedAt = 0;
        digitalWrite(MOTOR_PIN, LOW);
      }
      else
      {
        digitalWrite(MOTOR_PIN, HIGH);
      }
    }
  }
  else
  {
    if (mintuesPassed >= FALLING_ASLEEP_TIME)
    {
      asleep = true;
    }
  }
}