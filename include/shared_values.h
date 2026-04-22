//
// Created by Kok on 2/18/26.
//

#ifndef ESP32S3_BLE_SHARED_VALUES_H
#define ESP32S3_BLE_SHARED_VALUES_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"

typedef enum {
    SHVAL_ERROR_OK,
    SHVAL_ERROR_VAL_UNAVAILABLE,
    SHVAL_ERROR_SUB_QUEUE_FULL,
    SHVAL_ERROR_TIMEOUT
} SHVAL_ErrorTypeDef;

typedef struct {
    uint32_t InitialValue;
    EventBits_t SubscribersEventBits;
} SHVAL_ConfigTypeDef;

typedef struct {
    uint32_t Value;
    SemaphoreHandle_t Mutex;

    EventGroupHandle_t EventGroup;
    EventBits_t SubscribersEventBits;
} SHVAL_HandleTypeDef;

typedef struct {
    void *InitialValue;
    uint32_t ValueLen;
    EventBits_t SubscribersEventBits;
} SHVAL_PointerConfigTypeDef;

typedef struct {
    void *Value;
    uint32_t ValueLen;
    SemaphoreHandle_t Mutex;

    EventGroupHandle_t EventGroup;
    EventBits_t SubscribersEventBits;
} SHVAL_PointerHandleTypeDef;

SHVAL_HandleTypeDef SHVAL_Init(SHVAL_ConfigTypeDef *Config);
SHVAL_ErrorTypeDef SHVAL_GetValue(const SHVAL_HandleTypeDef *hshval, uint32_t *Value, uint32_t TimeoutMS);
SHVAL_ErrorTypeDef SHVAL_SetValue(SHVAL_HandleTypeDef *hshval, uint32_t Value, uint32_t TimeoutMS);
SHVAL_ErrorTypeDef SHVAL_WaitForValue(SHVAL_HandleTypeDef *hshval, EventBits_t BitToWait, uint32_t *Value, TickType_t TicksToWait);

SHVAL_PointerHandleTypeDef SHVAL_PointerInit(SHVAL_PointerConfigTypeDef *Config);
SHVAL_ErrorTypeDef SHVAL_PointerGetValue(const SHVAL_PointerHandleTypeDef *hshval, void *Value, uint32_t *ValueLen, uint32_t TimeoutMS);
SHVAL_ErrorTypeDef SHVAL_PointerSetValue(SHVAL_PointerHandleTypeDef *hshval, void *Value, uint32_t TimeoutMS);
SHVAL_ErrorTypeDef SHVAL_PointerWaitForValue(SHVAL_PointerHandleTypeDef *hshval, EventBits_t BitToWait, void *Value, uint32_t *ValueLen, TickType_t TicksToWait);

#endif //ESP32S3_BLE_SHARED_VALUES_H