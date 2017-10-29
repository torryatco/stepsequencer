#include <EEPROM.h>

int buttonPin[4] = { 13, 14, 15, 16 };
int ledPin[4] = { 35, 36, 37, 38 };
boolean lastButtonState[4] = {LOW, LOW, LOW, LOW};
boolean buttonState[4] = {LOW, LOW, LOW, LOW};
boolean stepState[3][4] = {
  {true, false, true, false},
  {false, false, false, true},
  {true, true, true, true}
};
int midiNote[3][4] = {
  {60, 60, 60, 60 },
  {62, 62, 62, 62 },
  {63, 63, 63, 63 }
};
int channelPin[3] = {7, 8, 9};
int tempo = 500;

int currentStep = 0;
unsigned long lastStepTime = 0;

int buttonUp = 33;
int buttonDown = 34;
boolean buttonState1 = LOW;
boolean lastButtonState1 = LOW;
boolean buttonState2 = LOW;
boolean lastButtonState2 = LOW;

int channel = 0;


void setup() {
  Serial.begin(9600);
  loadFromEEPROM();
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT);
    pinMode(ledPin[i], OUTPUT);
  }
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(channelPin[i], OUTPUT);
  }
  
}

void loop() {
 
  checkButtons();
  setLeds();
  sequence();
  switchChannel();
  for (int i = 0; i < 3; i++) {
    digitalWrite(channelPin[i], LOW);
  }
  digitalWrite(channelPin[channel], HIGH);
}

void checkButtons() {
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      if (stepState[channel][i] == false) {
        stepState[channel][i] = true;
      } else if (stepState[channel][i] == true) {
        stepState[channel][i] = false;
      }
      saveToEEPROM();
    }
  }
}

void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (stepState[channel][i] == true) {
      analogWrite(ledPin[i], 20);
    } else {
      analogWrite(ledPin[i], 0);
    }
    if (i == currentStep ) {
      if (stepState[channel][i] == true) {
        analogWrite(ledPin[i], 0);
      }
      digitalWrite(ledPin[i], HIGH);
    } else {
      digitalWrite(ledPin[i], LOW);
    }
  }
}

void sequence() {
  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...
    //digitalWrite(ledPin[currentStep], LOW);  //turn off the current led

    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }
    midiSeq();
    //digitalWrite(ledPin[currentStep], HIGH); //turn on the new led

    lastStepTime = millis();               //set lastStepTime to the current time
  }
}

void midiSeq() {
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 4; i++) {
      if (stepState[j][i] == true && i == currentStep) {
        usbMIDI.sendNoteOff(midiNote[j][i], 0, 1);
        usbMIDI.sendNoteOn(midiNote[j][i], 127, 1);

      }
    }
  }
}

void switchChannel() {
  lastButtonState1 = buttonState1;
  buttonState1 = digitalRead(buttonUp);
  lastButtonState2 = buttonState2;
  buttonState2 = digitalRead(buttonDown);
  if (buttonState1 == HIGH && lastButtonState1 == LOW) {
    Serial.print("hi");
    channel += 1;
    if (channel == 3) {
      channel = 0;
    }
  }
  if (buttonState2 == HIGH && lastButtonState2 == LOW) {
    channel -= 1;
    if (channel == -1) {
      channel = 2;
    }
  }


}

void saveToEEPROM() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      EEPROM.write((j + (4 * i)), stepState[i][j]);
    }
  }
}

void loadFromEEPROM() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      stepState[i][j] = EEPROM.read(j + (4 * i));
    }
  }
}

