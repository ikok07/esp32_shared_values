//
// Created by Kok on 2/18/26.
//

#include "shared_values.h"

#include <string.h>

/**
 * @brief Initializes shared value using the provided config structure
 * @param Config Config structure
 */
SHVAL_HandleTypeDef SHVAL_Init(SHVAL_ConfigTypeDef *Config) {
    SHVAL_HandleTypeDef handle = {
        .Value = Config->InitialValue,
        .EventGroup = xEventGroupCreate(),
        .SubscribersEventBits = Config->SubscribersEventBits,
        .Mutex = xSemaphoreCreateMutex()
    };

    return handle;
}

/**
 * @brief Checks whether the shared value is initialized
 * @param hshval Shared Value Handle
 */
uint8_t SHVAL_Exists(SHVAL_HandleTypeDef *hshval) {
    return hshval->Mutex != NULL;
}

/**
 * @brief Gets shared value WITHOUT copying it
 * @param hshval Shared Value Handle
 * @param Value Pointer, which will be assigned to the value's location
 * @param TimeoutMS Timeout in milliseconds
 */
SHVAL_ErrorTypeDef SHVAL_GetValue(const SHVAL_HandleTypeDef *hshval, uint32_t *Value, uint32_t TimeoutMS) {
    if (xSemaphoreTake(hshval->Mutex, pdMS_TO_TICKS(TimeoutMS))) {
        *Value = hshval->Value;
        xSemaphoreGive(hshval->Mutex);
        return SHVAL_ERROR_OK;
    } else {
        return SHVAL_ERROR_VAL_UNAVAILABLE;
    }
}

/**
 * @brief Sets shared value
 * @param hshval Shared Value Handle
 * @param Value Value to set
 * @param TimeoutMS Timeout in milliseconds
 */
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

/**
 * @brief Waits for shared value for 'TicksToWait' period and gets it WITHOUT copying it
 * @param hshval Shared Value Handle
 * @param BitToWait Event bit to wait for
 * @param Value Pointer, which will be assigned to the value's location
 * @param TicksToWait FreeRTOS ticks to wait
 */
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

/**
 * @brief Initializes shared value using the provided config structure
 * @note The `*Pointer*` methods are used for non uint32_t values
 * @param Config Config structure
 */
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

/**
 * @brief Checks whether the shared value is initialized
 * @note The `*Pointer*` methods are used for non uint32_t values
 * @param hshval
 */
uint8_t SHVAL_PointerExists(SHVAL_PointerHandleTypeDef *hshval) {
    return hshval->Mutex != NULL;
}

/**
 *
 * @param hshval Shared Value Handle
 * @param Value Pointer, which will be assigned to the value's location
 * @param ValueLen Length of data
 * @param TimeoutMS Timeout in milliseconds
 * @note The `*Pointer*` methods are used for non uint32_t values
 * @return
 */
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

/**
 * @brief Sets shared value by COPYING the provided 'Value'
 * @param hshval Shared Value Handle
 * @param Value Value to set (length will be inferred from 'hshval')
 * @param TimeoutMS Timeout in milliseconds
 * @note The `*Pointer*` methods are used for non uint32_t values
 */
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

/**
 * @brief Waits for shared value for 'TicksToWait' period and gets it WITHOUT copying it
 * @param hshval Shared Value Handle
 * @param BitToWait Event bit to wait for
 * @param Value Pointer, which will be assigned to the value's location
 * @param ValueLen (optional) Length of data
 * @param TicksToWait FreeRTOS ticks to wait
 * @note The `*Pointer*` methods are used for non uint32_t values
 */
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