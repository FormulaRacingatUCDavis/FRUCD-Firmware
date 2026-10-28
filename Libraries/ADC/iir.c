#include <stdint.h>

typedef struct {
    float alpha;
    uint32_t prev_val;
} IIR_t;

void IIR_Init(IIR_t *filter, float init_alpha, uint32_t init_val) {
    filter->alpha = init_alpha;
    filter->prev_val = init_val;
}

void IIR_Init(IIR_t *filter, uint16_t f_c, uint16_t f_s, uint32_t init_val) {
    filter->alpha = calculate_alpha(f_c, f_s);
    filter->prev_val = init_val;
}

uint32_t IIR_update(IIR_t *filter, uint16_t curr_input) {
    float y_curr = filter->prev_val + filter->alpha * (curr_input - filter->prev_val);
    //derived from first order IIR filter equation y[n] = y[n-1] + a(x[n] - y[n-1])
    //curr_input corresponds to x[n] 
    //prev_val corresponds to y[n-1]
    //y_curr corresponds to y[n]

    filter->prev_val = y_curr;
    //save current y to previous value

    return y_curr;
}

inline float calculate_alpha(uint16_t f_c, uint16_t f_s) {
    return (1.0 - exp(-2.0 * 3.1415 * (f_c / f_s)));
    //f_c is the cutroff frequency
    //f_s is the sampling frequency
}