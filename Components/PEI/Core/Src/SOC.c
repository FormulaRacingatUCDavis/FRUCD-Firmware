/*
 * SOC.c
 *
 *  Created on: Sep 15, 2024
 *      Author: Abhineet
 */


// https://www.mathworks.com/help/simscape-battery/ref/socestimatorkalmanfilter.html

#include <stdint.h>
#include <math.h>

#include "SOC.h"
#include "matrix_mult.h"
#include "cell_interface.h"

extern BAT_PACK_t bat_pack;

// TODO: Update for new pack
// SOC Constants
static const float dt = 0.1f; // Sampling Period
static const float R0 = 0.0145f;
static const float Rc = 0.009f;
static const float Cbat = 4.1f * 60.0f * 60.0f;
static const float Cc = 1666.0f;

// Initializing probability matrices
static Matrix Aprime;
static Matrix Aprime_transpose;

// Matrix Eprime;
// Matrix Eprime_transpose;

static Matrix Cprime; // VOC(SOC)
static Matrix Cprime_transpose;

// Coefficients for probability
static float Rk;

// --------------------------------------------------------------------------
// At time k
// --------------------------------------------------------------------------

// Variables Qualitatively
// (Constant) VOC : Open circuit voltage
// (Time variant) Vc : Voltage across capacitor in battery circuit model
// (Time variant) V : Voltage we measure(output from battery circuit model)
// (Constant)Cc : Capacitor's capacitance
// (Constant)Rc : Resistor in parallel with capacitor
// (Constant)R0 : Ohmic resistance
// (Constant)Cbat : Capacity of battery in AmpHours ?

//// Initializing the matrixes we'll need to use
static Matrix xhat; // xhat is a 2-by-1 matrix, top value is: SOC_estimated, bottom is
             // Vc_estimated
static Matrix Qk1;
static Matrix xhatk_1;
static Matrix fk_;
static Matrix xhatCorrected;
static Matrix PCorrected;
static Matrix Lk;
static Matrix P;
static Matrix Pk_1;
static Matrix sub_mat_1;
static Matrix sub_mat_2;
static Matrix sub_mat_3;

// Variables subject to constant change
static float I;
static float V;

static uint8_t initialized = 0;

void init_SOC_vars() {
  // Aprime
  mat_init(2, 2, &Aprime);
  mat_set(1, 1, 1.0, &Aprime);
  mat_set(2, 2, exp(-dt / (Cc * Rc)), &Aprime);
  // Aprime_transpose
  mat_transpose(&Aprime, &Aprime_transpose);

  // Eprime
  // mat_init(2, 2, &Eprime);
  // mat_set(1, 1, 1.0, &Eprime);
  // mat_set(2, 2, 1.0, &Eprime);
  // Eprime_transpose
  // mat_transpose(&Eprime, &Eprime_transpose);

  // Cprime
  mat_init(1, 2, &Cprime);
  mat_set(1, 2, -1, &Cprime);
  // Cprime_transpose
  mat_init(2, 1, &Cprime_transpose);
  mat_set(2, 1, -1, &Cprime_transpose);

  // Rk
  Rk = pow(10, -4);

  // xhat
  mat_init(2, 1, &xhat);

  // Qk1
  mat_init(2, 2, &Qk1);
  mat_set(1, 1, 2.5 * pow(10, -7), &Qk1);

  // xhatk_1
  mat_init(2, 1, &xhatk_1);
  mat_set(1, 1, 1, &xhatk_1);

  // fk_
  mat_init(2, 1, &fk_);
  mat_init(2, 1, &xhatCorrected);
  mat_init(2, 2, &PCorrected);
  mat_init(2, 1, &Lk);

  mat_init(2, 2, &P);
  mat_set(1, 1, Rk, &P);
  mat_set(2, 2, Rk, &P);

  mat_init(2, 2, &Pk_1);
  mat_set(1, 1, Rk, &Pk_1);
  mat_set(2, 2, Rk, &Pk_1);

  mat_init(2, 2, &sub_mat_1);
  mat_init(2, 2, &sub_mat_2);
  mat_init(2, 2, &sub_mat_3);

  I = 0;
  V = 3.6; // nominal voltage per cell
  initialized = 1;
}

void update_SOC_input() {
  V = bat_pack.filt_pack_voltage / 120; // 120 cells series
  I = bat_pack.current / 3;         // 3 cells parallel
}

//------------------------------- SMALLER CALCULATION FUNCTIONS
//--------------------------------

// Functions we need to calculate
static float VOC(float soc) {
  uint8_t soc_int = (uint8_t)(100 * soc);

  // TODO: Update for new packs
  const uint16_t voc_lt[101] = {
      28930, 29662, 30239, 30689, 31101, 31510, 31915, 32304, 32661, 32960,
      33245, 33466, 33703, 33931, 34100, 34159, 34222, 34313, 34372, 34451,
      34539, 34650, 34781, 34921, 35038, 35172, 35276, 35374, 35472, 35560,
      35646, 35744, 35838, 35940, 36028, 36094, 36150, 36248, 36320, 36395,
      36467, 36539, 36624, 36726, 36788, 36889, 36987, 37043, 37145, 37249,
      37331, 37426, 37511, 37615, 37678, 37782, 37854, 37962, 38053, 38155,
      38259, 38364, 38475, 38573, 38667, 38769, 38854, 38923, 39030, 39103,
      39188, 39289, 39390, 39501, 39609, 39710, 39824, 39971, 40078, 40209,
      40320, 40405, 40408, 40496, 40539, 40582, 40600, 40639, 40657, 40694,
      40700, 40714, 40757, 40794, 40837, 40906, 40968, 41070, 41213, 41427,
      41668};

  if (soc_int < 0) {
    return 0;
  } else if (soc_int > 100) {
    return 4.2;
  } else {
    return ((float)voc_lt[soc_int]) / 10000;
  }
}

static float dVOC_dSOC(float soc) {
  if (soc < 0)
    soc = 0;
  if (soc > 0.99)
    soc = 0.99;

  return (VOC(soc + 0.01) - VOC(soc)) / 0.01;
}

// hk is observation function
// since measured voltage is our observed value, hk returns estimated voltage
// based on current and SOC
static float hk(float SOC_val, float I_val) {
  float vc_now = mat_get(2, 1, &xhat);
  float Vt = VOC(SOC_val) - (R0 * I_val) - vc_now;
  // printf("Vt estimated: %f\n", Vt);
  return Vt;
}

static void fk(Matrix *xhatk_1_ptr, float I_val, Matrix *xhat_ptr) {
  float Vc = mat_get(2, 1, xhatk_1_ptr);

  Matrix f;
  mat_init(2, 1, &f);
  mat_set(1, 1, (-1 * I_val / Cbat), &f);
  mat_set(2, 1, ((I_val / Cc) - (Vc / (Cc * Rc))), &f);

  // f is the state transition matrix
  // xhat = xhatk_1 + (dt * f)
  mat_scale(&f, dt, &sub_mat_1);
  mat_add(xhatk_1_ptr, &sub_mat_1, xhat_ptr);

  // xhat = previous xhat + change in xhat in time (dt)
  // xhat is a derivative
}

//------------------------------------ EKF CALCULATOR FUNCTION
//-------------------------------------

// EKF Calculator function (to compute values of xhatCorrected and PCorrected
// continuously)
void EKF() {
  if (!initialized) return;

  //----------------- CALCULATIONS FOR xhat -----------------
  // advance the state estimation based on measured current
  fk(&xhatk_1, I, &xhat);

  //----------------- CALCULATIONS FOR P -----------------
  // Multiplying Aprime * Pk_1 * Aprime_transpose
  // Aprime is the state transition matrix for the estimated state covarience?
  // essentially this step is the equivalent of fk(), exept for the state
  // covarience instead of the state
  mat_multiply(&Aprime, &Pk_1, &sub_mat_1);
  mat_multiply(&sub_mat_1, &Aprime, &sub_mat_2);

  // Multiplying Eprime * Qk1 * Eprime_transpose
  // this does literally nothing because Eprime is the identity matrix
  // Q is the process noise, i.e. the estimated noise involved with predicting
  // next state based on I
  // mat_multiply(Eprime_ptr, Qk1_ptr, sub_mat_1_ptr);
  // mat_multiply(sub_mat_1_ptr, Eprime_transpose_ptr, sub_mat_3_ptr);

  // P = (Aprime * Pk_1 * Aprime_transpose) + (Eprime * Qk1 * Eprime_transpose)
  // replacing Eprime Qk1 Eprime_transpose with just Qk1 since Eprime is I
  // adding process noise to the new predicted covarience
  // because of the process noise, our estimated state becomes more uncertain
  // each time we predict a new state
  mat_add(&sub_mat_2, &Qk1, &P);

  //----------------- CALCULATIONS FOR Lk -----------------
  // error_estimate = (Cprime * P * Cprime_tranpose)
  // update dVOC/dSOC in Cprime and Cprime_transpose
  float soc_now = mat_get(1, 1, &xhat);
  float c11 = dVOC_dSOC(soc_now);
  mat_set(1, 1, c11, &Cprime);
  mat_set(1, 1, c11, &Cprime_transpose);

  mat_multiply(&Cprime, &P, &sub_mat_1);
  mat_multiply(&sub_mat_1, &Cprime_transpose, &sub_mat_2);

  // result is scalar
  float error_estimate = mat_get(1, 1, &sub_mat_2);

  // combined_error = [(Cprime * P * Cprime_tranpose) + Rk]^(-1)
  float combined_error = error_estimate + Rk;
  combined_error = 1 / combined_error;

  // Lk = combined_error * (P * Cprime_transpose)
  // Lk is the ideal Kalman gain
  //
  mat_multiply(&P, &Cprime_transpose, &sub_mat_1);
  mat_scale(&sub_mat_1, combined_error, &Lk);

  //--------------- CALCULATIONS FOR xhatCorrected ---------------
  // Vt_error = (SOC + Vc) - hk(100*SOC, I, Voc0, R0)
  soc_now = mat_get(1, 1, &xhat);
  float vt_now = hk(soc_now, I);
  float Vt_error = V - vt_now;
  // printf("Vt_error: %f\n", Vt_error);

  // xhatCorrected = (Lk * Vt_error) + xhat
  mat_scale(&Lk, Vt_error, &sub_mat_1);
  mat_add(&xhat, &sub_mat_1, &xhatCorrected);

  //----------------- CALCULATIONS FOR PCorrected -----------------
  // FINDING: (Lk * Cprime) * P
  mat_multiply(&Lk, &Cprime, &sub_mat_1);
  mat_multiply(&sub_mat_1, &P, &sub_mat_2);

  // PCorrected = P - [(Lk * Cprime) * P]
  mat_scale(&sub_mat_2, -1, &sub_mat_1);
  mat_add(&P, &sub_mat_1, &PCorrected);

  mat_set(1, 1, mat_get(1, 1, &xhatCorrected), &xhatk_1);
  mat_set(2, 1, mat_get(2, 1, &xhatCorrected), &xhatk_1);

  mat_set(1, 1, mat_get(1, 1, &PCorrected), &Pk_1);
  mat_set(2, 1, mat_get(2, 1, &PCorrected), &Pk_1);
  mat_set(1, 2, mat_get(1, 2, &PCorrected), &Pk_1);
  mat_set(2, 2, mat_get(2, 2, &PCorrected), &Pk_1);

  bat_pack.SOC_percent = (uint8_t)(mat_get(1, 1, &xhatCorrected) * 100);

  // float soc = mat_get(1, 1, xhatk_1_ptr);
  // if (soc < 0) {
  //   mat_set(1, 1, 0, xhatk_1_ptr);
  // }
}
