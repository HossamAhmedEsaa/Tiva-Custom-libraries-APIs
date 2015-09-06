/*
 * drv8833.h
 *
 *  Created on: 09/06/2015
 *      Author: Luís Afonso
 */

#ifndef DRV8833_H_
#define DRV8833_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

void DRV8833_InitMotorA();
void DRV8833_InitMotorB();
void DRV8833_InitMotorsAB();
void DRV8833_MotorA(int32_t _speed);
void DRV8833_MotorB(int32_t _speed);
void DRV8833_MotorsDuty(int32_t _speedA, int32_t speedB);

#endif /* DRV8833_H_ */
