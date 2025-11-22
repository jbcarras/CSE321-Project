#include <Arduino_FreeRTOS.h>

#define SENS_IN 9
#define IN_LED 10

#define SENS_OUT 2
#define OUT_LED 3

#define BL 13
#define B 12
#define BR 11
#define MD 7
#define TL 6
#define T 5
#define TR 4


uint8_t inStatus = 0;
uint8_t outStatus = 0;

void inSensorTask(void *pvParameters) {
  for (;;) {
    uint8_t st = digitalRead(SENS_IN);
    if (st != inStatus) {
      Serial.print("IN SENSOR NOW ");
      Serial.println(st == 0 ? "LOW" : "HIGH");
      digitalWrite(BL, 1 - st);
      digitalWrite(TL, st);
      inStatus = st;
    } 
    vTaskDelay(1);
  }
}

void outSensorTask(void *pvParameters) {
  for (;;) {
    uint8_t st = digitalRead(SENS_OUT);
    if (st != outStatus) {
      Serial.print("OUT SENSOR NOW ");
      Serial.println(st == 0 ? "LOW" : "HIGH");
      digitalWrite(BR, 1 - st);
      digitalWrite(TR, st);
      outStatus = st;
    } 
    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(SENS_IN, INPUT_PULLUP);
  pinMode(SENS_OUT, INPUT_PULLUP);

  pinMode(BL, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(BR, OUTPUT);
  pinMode(MD, OUTPUT);
  pinMode(TL, OUTPUT);
  pinMode(T, OUTPUT);
  pinMode(TR, OUTPUT);

  digitalWrite(BL, 0);
  digitalWrite(B, 0);
  digitalWrite(BR, 0);
  digitalWrite(MD, 0);
  digitalWrite(TL, 0);
  digitalWrite(T, 0);
  digitalWrite(TR, 0);

  xTaskCreate(inSensorTask, "Indoor Sensor", 64, NULL, 1, NULL);
  xTaskCreate(outSensorTask, "Outdoor Sensor", 64, NULL, 1, NULL);
  vTaskStartScheduler();

  Serial.println("Failed to start scheduler");
}

void loop() {

}
