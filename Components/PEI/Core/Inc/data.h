/*
 * data.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_

#define N_OF_SUBPACK 5u // number of subpacks

#define IC_PER_SUBPACK 2u // ADBMS6830s per subpack
#define CELLS_PER_ADBMS 12u   // number of cells per IC
#define CELL_TEMPS_PER_ADBMS 8u

#define CELL_TEMPS_PER_SUBPACK (CELL_TEMPS_PER_ADBMS * IC_PER_SUBPACK)

#define N_OF_ADBMS (IC_PER_SUBPACK * N_OF_SUBPACK)          // total number of ADBMS6830s
#define N_OF_CELL (N_OF_ADBMS * CELLS_PER_ADBMS)            // total number of cells
#define CELLS_PER_SUBPACK (N_OF_CELL / N_OF_SUBPACK)        // cells per subpack
#define N_OF_TEMP_CELL (CELL_TEMPS_PER_SUBPACK * N_OF_SUBPACK) // total number of temps

#endif /* INC_DATA_H_ */
