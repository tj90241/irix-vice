#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "vice.h"

int main(int argc, char **argv) {
	struct vice vice;

	if (vice_init(&vice)) {
		fprintf(stderr, "Failed to initialize the VICE library\n");
		return EXIT_FAILURE;
	}

	printf("VICE revision: %u\n", vice_get_revision(&vice));

	/* OK, now try resetting the MSP */
	vice_msp_reset(&vice);
	if (!(vice_msp_read_status(&vice) & VICE_MSP_CONTROL_OPERATIONAL)) {
		fprintf(stderr, "Failed to reset the VICE MSP\n");
		vice_deinit(&vice);
		return EXIT_FAILURE;
	}

	/* Upload a simple busy loop to MSP IRAM */
	uint32_t msp_iram[3];
	uint32_t msp_iram_test[3];

	msp_iram[0] = 0x00000000; /* nop */
	msp_iram[1] = 0x08000000; /* j 0x0 */
	msp_iram[2] = 0x00000000; /* nop */

	memset(msp_iram_test, 0x5A, sizeof(msp_iram_test));
	vice_msp_iram_write(&vice, msp_iram, 0, sizeof(msp_iram));
	vice_msp_iram_read(&vice, msp_iram_test, 0, sizeof(msp_iram_test));

	if (memcmp(msp_iram, msp_iram_test, sizeof(msp_iram))) {
		fprintf(stderr, "Failed to read/write VICE MSP IRAM\n");
		vice_deinit(&vice);
		return EXIT_FAILURE;
	}

	/* Start executing out of MSP IRAM */
	vice_msp_start(&vice, 0x0);

	/* XXX: Why does the PC bounce all over the place? */
	while (1) {
		printf("VICE MSP status: 0x%.8X, $pc: 0x%.8X\n",
			vice_msp_read_status(&vice),
			vice_msp_read_pc(&vice));

		sleep(1);
	}

	vice_deinit(&vice);
	return EXIT_SUCCESS;
}
