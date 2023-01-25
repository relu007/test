// ==========================================================================
// motor_test.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Test motors software
// =========================================================================


#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include "stm32h7xx_hal.h"

extern void MotorTest_Start(void);
extern void MotorTest_Server(void);
extern void MotorTest_Health(void);
extern void MotorTest_PheromoneMix(void);

#endif
