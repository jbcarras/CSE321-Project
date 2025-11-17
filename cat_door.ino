#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include <semphr.h>

#define configUSE_PORT_DELAY 1

//Sensor that is on the inside of the house
#define SENS_IN 9
#define IN_LED 10

//Sensor that is on the outside of the house
#define SENS_OUT 2
#define OUT_LED 3


//Function prototypes
void showNumber(uint8_t num);
void initDisplay();
void testDisplay();

unsigned long lastIn = 0;
unsigned long lastOut = 0;

//The count is how many cats are outside
volatile int count = 0;
//Detects if the indoor sensor was triggered
volatile bool inCatPres = false;
//Detects if the outdoor sensor was triggered
volatile bool outCatPres = false;

//Mutex so that there are no race conditions
SemaphoreHandle_t countAccessSem;

/*
  Marks that the cat is no longer active at 
  the indoor sensor. 
*/
void ditchInCatCallback(TimerHandle_t xTimer) {
  if (digitalRead(SENS_IN) == LOW) {
    Serial.println(":3 Cat is no longer detected indoors");
    inCatPres = false;
  }
}

/*
  Marks that the cat is no longer active at 
  the outdoor sensor. 
*/
void ditchOutCatCallback(TimerHandle_t xTimer) {
  if (digitalRead(SENS_OUT) == LOW) {
    Serial.println(":3 Cat is no longer detected outdoors");
    outCatPres = false;
  }
}

/*
  Indoor sensor task. Handles reading
  from the indoor sensor and determining
  if a cat has entered from outdoors.
*/
void sensorInTask(void *pvParameters) {
  TimerHandle_t inCatTimer = xTimerCreate("Inside Cat Timer", pdMS_TO_TICKS(2000), pdFALSE, NULL, ditchInCatCallback);
  Serial.println(":D Indoor sensor started");
  for (;;) {
    if (digitalRead(SENS_IN) == HIGH && millis() - lastIn > 200) { // TODO: modify to read from motion sensor appropriately
      Serial.println("MOTION IN");
      lastIn = millis();
      digitalWrite(IN_LED, HIGH);
      if (!inCatPres && outCatPres) {
        
        if (xSemaphoreTake(countAccessSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
          Serial.println(":3 A cat has entered!");
          count = --count < 0 ? 0 : count;
          xSemaphoreGive(countAccessSem);
        } else {
          Serial.println(">:( Indoor sensor wants the semaphore!!");
        }
        // outCatPres = false;
      } else {
        digitalWrite(IN_LED, LOW);
      }
      
      
      inCatPres = true;
      xTimerReset(inCatTimer, 0);
    }

    vTaskDelay(1);
  }
} 

/*
  Outdoor sensor task. Handles reading
  from the outdoor sensor and determining
  if a cat has entered from indoors.
*/
void sensorOutTask(void *pvParameters) {
  TimerHandle_t outCatTimer = xTimerCreate("Outside Cat Timer", pdMS_TO_TICKS(2000), pdFALSE, NULL, ditchOutCatCallback);
  Serial.println(":D Outdoor sensor started");
  for (;;) {
    if (digitalRead(SENS_OUT) == HIGH && millis() - lastOut > 200) { // TODO: modify to read from motion sensor appropriately
      Serial.println("MOTION OUT");
      lastOut = millis();
      digitalWrite(OUT_LED, HIGH);
      if (!outCatPres && inCatPres) {
        
        if (xSemaphoreTake(countAccessSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
          Serial.println(":3 A cat has exited!");
          count = ++count > 9 ? 9 : count;
          xSemaphoreGive(countAccessSem);
        } else {
          Serial.println(">:( Outdoor sensor wants the semaphore!!");
        }
        // inCatPres = false;
      }
      outCatPres = true;
      xTimerReset(outCatTimer, 0);
    } else {
      digitalWrite(OUT_LED, LOW);
    }

    vTaskDelay(1);
  }

}

/*
  Handles updating the display.
*/
void displayTask(void *pvParameters) {
  Serial.println(":D DISPLAYING TEST !!");
  initDisplay();
  testDisplay();
  Serial.println(":D Display started");
  int count_prev = count;
  showNumber(count);
  for (;;) {

    if (count != count_prev) {
      showNumber(count);
      count_prev = count;
    }

    vTaskDelay(50);
  }
}


void setup() {
  Serial.begin(9600);

  pinMode(SENS_IN, INPUT_PULLUP);
  pinMode(SENS_OUT, INPUT_PULLUP);
  pinMode(IN_LED, OUTPUT);
  pinMode(OUT_LED, OUTPUT);

  countAccessSem = xSemaphoreCreateMutex();

  if (countAccessSem == NULL) {
    Serial.println(":O FAILED TO CREATE SEMAPHORE??");
    return;
  }

  //Sensors have a priority of 2, Display has a priority of 1
  xTaskCreate(sensorInTask, "Indoor Sensor", 64, NULL, 2, NULL);
  xTaskCreate(sensorOutTask, "Outdoor Sensor", 64, NULL, 2, NULL);
  xTaskCreate(displayTask, "Display", 64, NULL, 1, NULL);

  Serial.println(":D STARTING SCHEDULER !!");

  vTaskStartScheduler();
  
  Serial.println(":O FAILED TO START SCHEDULER !! (out of memory?)");
}

void loop() {}