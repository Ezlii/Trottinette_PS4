/**
 * @file   vl53l1_platform.c
 * @brief  Code function definitions for EwokPlus25 Platform Layer
 *         RANGING SENSOR VERSION
 *
 */

//#include <windows.h>

#include <stdio.h>      // sprintf(), vsnprintf(), printf()
#include <stdint.h>
#include <string.h>     // strncpy(), strnlen()

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include "vl53l1_platform.h"
#include "vl53l1_platform_log.h"
#include "vl53l1_platform_user_data.h"

#include "main.h"
extern I2C_HandleTypeDef hi2c3;  // Dein I2C-Handle

#ifdef PAL_EXTENDED
	#include "vl53l1_register_strings.h"
#else
	#define VL53L1_get_register_name(a,b)
#endif

//#include "ranging_sensor_comms.h"
//#include "power_board_defs.h"

// flag to indicate if power board has been accessed
const uint32_t _power_board_in_use = 0;

// flag to indicate if we can use the extended voltage ranges (not laser safe!)
uint32_t _power_board_extended = 0;

// cache of the comms type flag
uint8_t global_comms_type = 0;

#define  VL53L1_COMMS_CHUNK_SIZE  56
#define  VL53L1_COMMS_BUFFER_SIZE 64

#define GPIO_INTERRUPT          RS_GPIO62
#define GPIO_POWER_ENABLE       RS_GPIO60
#define GPIO_XSHUTDOWN          RS_GPIO61
#define GPIO_SPI_CHIP_SELECT    RS_GPIO51

/*!
 *  The intent of this Abstraction layer is to provide the same API
 *  to the underlying SystemVerilog tasks as the C driver will have
 *  to ST Comms DLL's for the talking to Ewok via the USB + STM32
 *  or if the C-driver is implemented directly on the STM32
 */

#define trace_print(level, ...) \
	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_PLATFORM, \
	level, VL53L1_TRACE_FUNCTION_NONE, ##__VA_ARGS__)

#define trace_i2c(...) \
	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_NONE, \
	VL53L1_TRACE_LEVEL_NONE, VL53L1_TRACE_FUNCTION_I2C, ##__VA_ARGS__)




/*
 * ----------------- COMMS FUNCTIONS -----------------
 */


VL53L1_Error VL53L1_WriteMulti(VL53L1_Dev_t *pdev, uint16_t reg, uint8_t *pdata, uint32_t count) {
	VL53L1_Error retvalue = VL53L1_ERROR_NONE;
	  if (HAL_I2C_Mem_Write(&hi2c3, pdev->i2c_slave_address << 1, reg, I2C_MEMADD_SIZE_16BIT, pdata, count, 1000) == HAL_OK) {
        retvalue = VL53L1_ERROR_NONE;
    } else {
    	retvalue =  VL53L1_ERROR_CONTROL_INTERFACE;
    }
    return retvalue;
}

VL53L1_Error VL53L1_ReadMulti(VL53L1_Dev_t *pdev, uint16_t reg, uint8_t *pdata, uint32_t count) {
	VL53L1_Error retvalue = VL53L1_ERROR_NONE;
	if (HAL_I2C_Mem_Read(&hi2c3, pdev->i2c_slave_address << 1, reg, I2C_MEMADD_SIZE_16BIT, pdata, count, 1000) == HAL_OK) {
    	retvalue =  VL53L1_ERROR_NONE;
    } else {
    	retvalue =  VL53L1_ERROR_CONTROL_INTERFACE;
    }
    return  retvalue;
}


VL53L1_Error VL53L1_WrByte(VL53L1_Dev_t *pdev, uint16_t reg, uint8_t data) {
    return VL53L1_WriteMulti(pdev, reg, &data, 1);
}

VL53L1_Error VL53L3CX_RdByte(VL53L1_Dev_t *pdev, uint16_t reg, uint8_t *data) {
    return VL53L1_ReadMulti(pdev, reg, data, 1);
}

VL53L1_Error VL53L3CX_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms) {
    HAL_Delay(wait_ms);
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdWord(VL53L1_Dev_t *pdev, uint16_t reg, uint16_t *pdata) {
    VL53L1_Error retvalue = VL53L1_ERROR_NONE;
    uint8_t buffer[2];

    if (HAL_I2C_Mem_Read(&hi2c3, pdev->i2c_slave_address << 1, reg,
                         I2C_MEMADD_SIZE_16BIT, buffer, 2, 1000) == HAL_OK) {
        *pdata = ((uint16_t)buffer[0] << 8) | buffer[1];
        retvalue = VL53L1_ERROR_NONE;
    } else {
        retvalue = VL53L1_ERROR_CONTROL_INTERFACE;
    }

    return retvalue;
}



/*
 * ----------------- HOST TIMING FUNCTIONS -----------------
 */


/*
 * ----------------- DEVICE TIMING FUNCTIONS -----------------
 */

VL53L1_Error VL53L1_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
	*ptimer_freq_hz = 0;

	trace_print(VL53L1_TRACE_LEVEL_INFO, "VL53L1_GetTimerFrequency: Freq : %dHz\n", *ptimer_freq_hz);
	return VL53L1_ERROR_NONE;
}


VL53L1_Error VL53L1_GetTimerValue(int32_t *ptimer_count)
{
	*ptimer_count = 0;

	trace_print(VL53L1_TRACE_LEVEL_INFO, "VL53L1_GetTimerValue: Freq : %dHz\n", *ptimer_count);
	return VL53L1_ERROR_NONE;
}






VL53L1_Error  VL53L1_GpioInterruptEnable(void (*function)(void), uint8_t edge_type)
{
	VL53L1_Error status         = VL53L1_ERROR_NONE;

	SUPPRESS_UNUSED_WARNING(function);
	SUPPRESS_UNUSED_WARNING(edge_type);

	return status;
}


VL53L1_Error  VL53L1_GpioInterruptDisable(void)
{
	VL53L1_Error status         = VL53L1_ERROR_NONE;

	return status;
}



