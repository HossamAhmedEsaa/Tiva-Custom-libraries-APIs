/*
 * drv8833.c
 *
 *  Created on: 09/06/2015
 *      Author: Luís Afonso
 */


#include "drv8833.h"


/*================================
 * PB6 - AIN1
 * PB7 - AIN2
 *
 * PB4 - BIN1
 * PB5 - BIN2
 * This is to keep always!!
 *
 *
 *
 *================================*/


uint32_t freq = (80000000 / 25000)-1; // 3200 values.


void DRV8833_InitMotorA(){
	if(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
		SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	if(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlDelay(3);
	GPIOPinConfigure(GPIO_PB6_M0PWM0);
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
	PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_1);
	PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
			PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);


	uint32_t duty = 0;
	PWMOutputUpdateMode(PWM0_BASE,PWM_OUT_0_BIT|PWM_OUT_1_BIT,PWM_OUTPUT_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, freq);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, duty);
	PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);


	GPIOPinConfigure(GPIO_PB7_M0PWM1);
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, duty);
	PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);

	PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void DRV8833_InitMotorB(){
	if(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
		SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	if(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlDelay(3);

	GPIOPinConfigure(GPIO_PB4_M0PWM2);
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);
	PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_1);
	PWMGenConfigure(PWM0_BASE, PWM_GEN_1,
			PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	uint32_t duty = 0;
	PWMOutputUpdateMode(PWM0_BASE,PWM_OUT_2_BIT|PWM_OUT_3_BIT,PWM_OUTPUT_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, freq);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, duty);
	PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);


	GPIOPinConfigure(GPIO_PB5_M0PWM3);
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, duty);
	PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

	PWMGenEnable(PWM0_BASE, PWM_GEN_1);
}

void DRV8833_InitMotorsAB(){
	DRV8833_InitMotorA();
	DRV8833_InitMotorB();
}



/*
 * Controls Motor1 speed and direction.
 *
 * Duty goes from 0 to 1024
 * The bigger the Inverseduty value, the faster the speed.
 * Remember, bigger match value, lower duty. Since in fast decay the digital pin
 * is at 1, the lower the duty, the faster the speed. Hence why Inverseduty is used.
 * (no need to make freq-Inverseduty since less is faster).
 *
 * Possibily I have this wrong as I don't remember if the pulse starts high or low.
 *
 */
void DRV8833_MotorA(int32_t _speed){

	if(_speed ==0){
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x40;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6,0);
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x80;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7,0);
		return;
	}

	uint8_t backwards=0;
	if(_speed < 0){
		_speed = _speed*(-1);
		backwards=1;
	}
	if(_speed > 1024)
		_speed = 1024;




	uint32_t Inverseduty = _speed*freq/1024;

	if(!backwards){
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x40;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6,1);
		if(_speed == 1024){
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x80;
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7,0);
		}
		else{
			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Inverseduty-1);
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= 0x80;
		}
	}
	else{
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x80;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7,1);
		if(_speed == 1024){
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x40;
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6,0);
		}
		else{
			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Inverseduty-1);
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= 0x40;
		}
	}

}

void DRV8833_MotorB(int32_t _speed){

	if(_speed ==0){
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x10;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4,0);
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x20;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5,0);
		return;
	}

	uint8_t backwards=0;
	if(_speed < 0){
		_speed = _speed*(-1);
		backwards=1;
	}
	if(_speed > 1024)
		_speed = 1024;

	uint32_t Inverseduty = _speed*freq/1024;

	if(!backwards){
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x10;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4,1);
		if(_speed == 1024){
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x20;
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5,0);
		}
		else{
			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, Inverseduty);
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= 0x20;
		}
	}
	else{
		HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x20;
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5,1);
		if(_speed == 1024){
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) &= ~0x10;
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4,0);
		}
		else{
			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, Inverseduty);
			HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= 0x10;
		}
	}

}


void DRV8833_MotorsDuty(int32_t _speedA, int32_t _speedB){
	DRV8833_MotorA(_speedA);
	DRV8833_MotorB(_speedB);
}
