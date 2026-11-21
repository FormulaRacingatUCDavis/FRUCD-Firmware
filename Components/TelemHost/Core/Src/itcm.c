#include "itcm.h"
#include <stdint.h>

// Gets the addresses from the linker script defined previously.
// Defining as arrays makes them return the address when called.
extern uint32_t _itcm_start_[], _itcm_end_[], _itcm_load_[];

void itcm_init(void) {
    uint32_t *src = _itcm_load_;
    uint32_t *dst = _itcm_start_;

    while (dst < _itcm_end_) {
        uint32_t word = *src; // Read four bytes at src (from flash)
        *dst = word; // Copy the read word to itcmram at dst
        dst++; // Increment both pointers
        src++;
    }
}
