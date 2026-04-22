//
// Created by Kok on 2/18/26.
//

#include "shared_values.h"

#include <string.h>

SHVAL_HandleTypeDef SHVAL_Init(SHVAL_ConfigTypeDef *Config) {
    SHVAL_HandleTypeDef handle = {
        .Value = Config->InitialValue,
        .EventGroup = xEventGroupCreate(),
        .SubscribersEventBits = Config->SubscribersEventBits,
        .Mutex = xSemaphoreCreateMutex()
    };

    return handle;
}

SHVAL_ErrorTypeDef SHVAL_GetValue(const SHVAL_HandleTypeDef *hshval, uint32_t *Value, uint32_t TimeoutMS) {
    if (xSemaphoreTake(hshval->Mutex, pdMS_TO_TICKS(TimeoutMS))) {
        *Value = hshval->Value;
        xSemaphoreGive(hshval->Mutex);
        return SHVAL_ERROR_OK;
    } else {
        return SHVAL_ERROR_VAL_UNAVAILABLE;
    }
}

SHVAL_ErrorTypeDef SHVAL_SetValue(SHVAL_HandleTypeDef *hshval, uint32_t Value, uint32_t TimeoutMS) {
    if (xSemaphoreTake(hshval->Mutex, pdMS_TO_TICKS(TimeoutMS))) {
        xEventGroupClearBits(hshval->EventGroup, BIT0);

        hshval->Value = Value;

        xEventGroupSetBits(hshval->EventGroup, hshval->SubscribersEventBits);

        xSemaphoreGive(hshval->Mutex);
        return SHVAL_ERROR_OK;
    } else {
        return SHVAL_ERROR_VAL_UNAVAILABLE;
    }
}

SHVAL_ErrorTypeDef SHVAL_WaitForValue(SHVAL_HandleTypeDef *hshval, EventBits_t BitToWait, uint32_t *Value, TickType_t TicksToWait) {
    SHVAL_ErrorTypeDef shval_err = SHVAL_ERROR_OK;
    EventBits_t bits = xEventGroupWaitBits(hshval->EventGroup, BitToWait, pdTRUE, pdFALSE, TicksToWait);

    if ((bits & BitToWait) != 0) {
        if ((shval_err = SHVAL_GetValue(hshval, Value, pdTICKS_TO_MS(TicksToWait))) != SHVAL_ERROR_OK) {
            return shval_err;
        }
        return SHVAL_ERROR_OK;
    }

    return SHVAL_ERROR_TIMEOUT;
}

SHVAL_PointerHandleTypeDef SHVAL_PointerInit(SHVAL_PointerConfigTypeDef *Config) {
    SHVAL_PointerHandleTypeDef handle = {
        .Value = Config->InitialValue,
        .ValueLen = Config->ValueLen,
        .EventGroup = xEventGroupCreate(),
        .SubscribersEventBits = Config->SubscribersEventBits,
        .Mutex = xSemaphoreCreateMutex()
    };

    return handle;
}

SHVAL_ErrorTypeDef SHVAL_PointerGetValue(const SHVAL_PointerHandleTypeDef *hshval, void *Value, uint32_t *ValueLen,
    uint32_t TimeoutMS) {
    if (xSemaphoreTake(hshval->Mutex, pdMS_TO_TICKS(TimeoutMS))) {
        memcpy(Value, hshval->Value, hshval->ValueLen);
        if (ValueLen != NULL) *ValueLen = hshval->ValueLen;
        xSemaphoreGive(hshval->Mutex);
        return SHVAL_ERROR_OK;
    } else {
        return SHVAL_ERROR_VAL_UNAVAILABLE;
    }
}

SHVAL_ErrorTypeDef SHVAL_PointerSetValue(SHVAL_PointerHandleTypeDef *hshval, void *Value,
    uint32_t TimeoutMS) {
    if (xSemaphoreTake(hshval->Mutex, pdMS_TO_TICKS(TimeoutMS))) {
        memcpy(hshval->Value, Value, hshval->ValueLen);

        xEventGroupSetBits(hshval->EventGroup, hshval->SubscribersEventBits);

        xSemaphoreGive(hshval->Mutex);
        return SHVAL_ERROR_OK;
    } else {
        return SHVAL_ERROR_VAL_UNAVAILABLE;
    }
}

SHVAL_ErrorTypeDef SHVAL_PointerWaitForValue(SHVAL_PointerHandleTypeDef *hshval, EventBits_t BitToWait, void *Value, uint32_t *ValueLen, TickType_t TicksToWait) {
    SHVAL_ErrorTypeDef shval_err = SHVAL_ERROR_OK;
    EventBits_t bits = xEventGroupWaitBits(hshval->EventGroup, BitToWait, pdTRUE, pdFALSE, TicksToWait);

    if ((bits & BitToWait) != 0) {
        if ((shval_err = SHVAL_PointerGetValue(hshval, Value, ValueLen, pdTICKS_TO_MS(TicksToWait))) != SHVAL_ERROR_OK) {
            return shval_err;
        }
        return SHVAL_ERROR_OK;
    }

    return SHVAL_ERROR_TIMEOUT;
}