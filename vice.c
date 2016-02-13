#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "vice.h"

void vice_deinit(struct vice *vice) {
	munmap(vice->regs, 0x10000);
	close(vice->vice_fd);
}

unsigned vice_get_revision(struct vice *vice) {
	uint32_t rev;

	memcpy(&rev, vice->regs + 0x8, sizeof(rev));
	return rev & 0xF;
}

int vice_init(struct vice *vice) {
	int vice_fd;
	void *regs;

	if ((vice_fd = open("/dev/vice", O_RDWR, 0)) < 0) {
		perror("open");
		return 1;
	}

	if ((regs = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE,
			MAP_PRIVATE, vice_fd, 0)) == MAP_FAILED) {
		perror("mmap");
		close(vice_fd);
		return 1;
	}

	vice->vice_fd = vice_fd;
	vice->regs = (uint8_t *) regs;
	return 0;
}

uint32_t vice_msp_read_pc(struct vice *vice) {
	uint32_t pc;

	memcpy(&pc, vice->regs + 0x50, sizeof(pc));
	return pc;
}

uint32_t vice_msp_read_status(struct vice *vice) {
	uint32_t status;

	memcpy(&status, vice->regs + 0x40, sizeof(status));
	return status;
}

void vice_msp_reset(struct vice *vice) {
	vice_msp_write_control(vice, VICE_MSP_CONTROL_RESET);
	while (vice_msp_read_status(vice) != VICE_MSP_CONTROL_RESET);
	vice_msp_write_control(vice, VICE_MSP_CONTROL_OPERATIONAL);
	while (vice_msp_read_status(vice) != VICE_MSP_CONTROL_OPERATIONAL);
}

void vice_msp_start(struct vice *vice, uint32_t pc) {
	memcpy(vice->regs + 0x60, &pc, sizeof(pc));
	vice_msp_write_control(vice, VICE_MSP_CONTROL_GO);
}

void vice_msp_write_control(struct vice *vice, uint32_t control) {
	memcpy(vice->regs + 0x40, &control, sizeof(control));
}

int vice_msp_iram_read(struct vice *vice, void *ptr, size_t offset, size_t len) {
	if ((offset + len) > 0x1000)
		return -1;

	memcpy(ptr, (vice->regs + 0x2000) + offset, len);
	return 0;
}

int vice_msp_iram_write(struct vice *vice, void *ptr, size_t offset, size_t len) {
	if ((offset + len) > 0x1000)
		return -1;

	memcpy((vice->regs + 0x2000) + offset, ptr, len);
	return 0;
}
