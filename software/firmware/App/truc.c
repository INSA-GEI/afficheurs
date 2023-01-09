/*
 * truc.c
 *
 *  Created on: 20 mai 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"

uint64_t essai2(uint32_t val1,uint32_t val2,uint32_t val3,uint32_t val4,uint32_t val5,uint32_t val6,uint32_t val7);

uint64_t essai(uint32_t val1,uint32_t val2,uint32_t val3,uint32_t val4,uint32_t val5,uint32_t val6,uint32_t val7) {
	uint64_t ret;

	ret = val1+val2+val3+val4+val5+val6+val7;
	return ret;
}

uint64_t appel(void) {
	uint64_t status;

	status = essai (1, 2, 3, 4, 5, 6, 7);
	return status;
}

uint64_t appel2(void) {
	uint64_t status;

	status = essai2 (1, 2, 3, 4, 5, 6, 7);
	return status;
}
