#ifndef VICE_H_
#define VICE_H_
#include <stdint.h>

struct vice {
	int vice_fd;
	uint8_t *regs;
};

/* VICE */
int vice_init(struct vice *vice);
void vice_deinit(struct vice *vice);

unsigned vice_get_revision(struct vice *vice);

/* VICE MSP */
#define VICE_MSP_CONTROL_RESET		0x0
#define VICE_MSP_CONTROL_GO		0x1
#define VICE_MSP_CONTROL_OPERATIONAL	0x2

uint32_t vice_msp_read_pc(struct vice *vice);
uint32_t vice_msp_read_status(struct vice *vice);
void vice_msp_reset(struct vice *vice);
void vice_msp_start(struct vice *vice, uint32_t pc);
void vice_msp_write_control(struct vice *vice, uint32_t control);
int vice_msp_iram_read(struct vice *vice, void *ptr, uint32_t offset, uint32_t len);
int vice_msp_iram_write(struct vice *vice, void *ptr, uint32_t offset, uint32_t len);

#endif
