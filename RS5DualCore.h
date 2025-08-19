#include <FreeRTOS.h>
#include <semphr.h>

//
// Locking code
//

//*********************************************************************************
// Lock for system state changes, 
SemaphoreHandle_t systemStateLock = NULL;

void system_state_lock() {
  while ((systemStateLock == NULL) || (xSemaphoreTake(systemStateLock, (TickType_t)0) == pdFALSE)) {
    delay(1);
  }
}

void system_state_unlock() {
  xSemaphoreGive(systemStateLock);
}


//*********************************************************************************
// Lock for RGB Eyes changes, 
SemaphoreHandle_t neoPixelEyesLock = NULL;

void neopixel_eye_lock() {
  while ((neoPixelEyesLock == NULL) || (xSemaphoreTake(neoPixelEyesLock, (TickType_t)0) == pdFALSE)) {
    delay(1);
  }
}

void neopixel_eye_unlock() {
  xSemaphoreGive(neoPixelEyesLock);
}


//*********************************************************************************
// Lock for RGB Eyes changes, 
SemaphoreHandle_t neoPixelDMXLock = NULL;

void neopixel_DMX_lock() {
  while ((neoPixelDMXLock == NULL) || (xSemaphoreTake(neoPixelDMXLock, (TickType_t)0) == pdFALSE)) {
    delay(1);
  }
}

void neopixel_DMX_unlock() {
  xSemaphoreGive(neoPixelDMXLock);
}


//*********************************************************************************
// Lock for servo Movement, 
SemaphoreHandle_t runDataLock = NULL;

void run_Data_lock() {
  while ((runDataLock == NULL) || (xSemaphoreTake(runDataLock, (TickType_t)0) == pdFALSE)) {
    delay(1);
  }
}

void run_Data_unlock() {
  xSemaphoreGive(runDataLock);
}

