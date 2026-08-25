#ifndef CONFIG_H
#define	CONFIG_H

// How long to wait for pre-charging to finish before timing out
#define PRECHARGE_TIMEOUT_MS 8000
// Delay between checking pre-charging state

#define TMR1_PERIOD_MS 20

// discrepancy timer
#define MAX_DISCREPANCY_MS 100

#define PRECHARGE_THRESHOLD 4976 // 77V = 90% of nominal accumulator voltage, scaled from range of 0-12800(0V-200V)

//minimum calibration ranges
#define APPS1_MIN_RANGE 500   //will not enter HV until pedal is calibrated
#define APPS2_MIN_RANGE 500
#define BRAKE_MIN_RANGE 50

//in percent:
#define APPS1_BSPD_THRESHOLD 25
#define APPS1_BSPD_RESET_THRESHOLD 5
#define APPS_DEADZONE_PERCENTAGE 5
#define BRAKE_DEADZONE_PERCENTAGE 10

//in raw ADC:
#define APPS_SHORT_THRESH 3900   //~4.75V
#define APPS_OPEN_THRESH 125     //~0.19V

#define BRAKE_LIGHT_THRESHOLD 400
#define RTD_BRAKE_THRESHOLD 50  //brake threshold to enter drive mode
#define BRAKE_BSPD_THRESHOLD 75

#define MAX_TORQUE_NM 220  //220 Nm

// See EV.3.3.1 of FSAE Rules 2025
// Max power draw from accumulator must not exceed 80kW
// max power draw from accumulator, p_acc = 80kW
// drivetrain efficiency, n_drv = 0.9 (90%)
// max power draw from motor, p_motor = p_acc * n_drv = 80kW * 0.9 = 72kW
// leave some gap, use a number lower than 72kW
#define MAX_POWER_MOTOR_W 70000 // TODO: tune this maybe

// used for power limiter
// if AC power hits this value, "dip" the motor power down
#define MAX_POWER_ACCUMULATOR_W 70000 // 75000 max for performance // TODO: tune this

// used when power smoothing is disabled, cut to 0 torque if hard limit exceeded
// this is the old method of power limit enforcement (FE11 and prev)
#define ACCUMULATOR_POWER_HARD_LIMIT 75000

// 0 to disable power smoothing (cut to 0 torque only when hard limit exceeded),
// 1 to use power smoothing (smooth power down if getting close to limit)
#define POWER_SMOOTHING_ENABLED 1

// smoothing parameters
#define SMOOTHING_DOWN_TIME_MS 1000 // TODO: tune this
#define SMOOTHING_UP_TIME_MS 1000 // TODO: tune this
#define SMOOTHING_POWER_DELTA_W 7000 // TODO: tune this


// 0 to only send buttons and knobs when they change, 1 to constantly send them on every main loop
#define ALWAYS_SEND_DRIVER_INPUTS 1

// 0 to disable regen braking, 1 to enable
#define REGEN_BRAKING_ENABLED 1

// if APPS is pressed less than this threshold, attempt to regen brake
// only used if REGEN_BRAKING_ENABLE is 1
#define APPS_REGEN_THRESHOLD 5

// 0 to disable using KNOB1 (launch control knob) to tune strength of regen braking, 1 to enable
#define ALLOW_REGEN_KNOB_TUNING 1

#endif
