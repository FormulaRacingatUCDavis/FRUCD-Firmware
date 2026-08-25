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
#define DEADZONE_PERCENTAGE 5

//in raw ADC:
#define APPS_SHORT_THRESH 3900   //~4.75V
#define APPS_OPEN_THRESH 200     //~0.25V

#define BRAKE_LIGHT_THRESHOLD 400
#define RTD_BRAKE_THRESHOLD 50  //brake threshold to enter drive mode
#define BRAKE_BSPD_THRESHOLD 30

#define MAX_TORQUE_NM 230  //230 Nm
#define MAX_POWER_W 66000   // rules is 80, leave some gap

#endif
