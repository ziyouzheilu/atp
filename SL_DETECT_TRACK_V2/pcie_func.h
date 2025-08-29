
#ifndef PCIE_FUNC_H
#define PCIE_FUNC_H
#ifdef __cplusplus
 extern "C" {
 #endif
int pcie_init();
void pcie_deinit();
void disp_start();
void disp_reset();

int pcie_read_frame(unsigned char *buffer);
//int pcie_read_frame1(unsigned char *buffer);

int wait_for_s2mm_intr();
int wait_for_s2mm_intr1();
void timespec_sub(struct timespec *t1, const struct timespec *t2);
#ifdef __cplusplus
}
#endif
#endif

