#ifndef FFT_H_
#define FFT_H_

#include <stdint.h>

//physical addresses
#define XPAR_CTRL_AXI_DMA_0_BASEADDR 	0x40400000
#define XPAR_CTRL_AXI_GPIO_0_BASEADDR 	0x41200000
#define XPAR_PS7_SCUGIC_0_BASEADDR 		0xF8F00100
#define SOURCE_PADDR									0x1FF00000
#define DEST_PADDR										0x1FF80000

//DMA register offsets
#define MM2S_CONTROL_REGISTER 					0x00
#define MM2S_STATUS_REGISTER 					0x04
#define MM2S_START_ADDRESS 						0x18
#define MM2S_LENGTH 									0x28
#define S2MM_CONTROL_REGISTER 					0x30
#define S2MM_STATUS_REGISTER 					0x34
#define S2MM_DESTINATION_ADDRESS 			0x48
#define S2MM_LENGTH 									0x58

#define MAX_NUM_PTS 									8192
#define BYTES_PER_PT									8
#define FLOATS_PER_PT									2
#define LOG2_NUM_PTS									13
#define DMA_MAP_SIZE 									1024
#define GPIO_MAP_SIZE									1024
#define BUFFER_SIZE										(MAX_NUM_PTS * BYTES_PER_PT)
#define DMA_RESET 										0x04 
#define DIRECTION											1 //forward
#define SCALE													0x2AB
#define PTS_SHIFT											0
#define DIRECTION_SHIFT								8		
#define SCALE_SHIFT										9
#define START													0x01
#define IDLE													0x02

//return macros
#define FFT_SUCCESS										0
#define MMAP_FAILURE									1
#define OPEN_MEM_FAILURE							2

//function prototypes
void write_vaddr(void* vaddr, int offset, int value);
uint32_t read_vaddr(void* vaddr, int offset);
void dma_s2mm_status(unsigned int* dma_virtual_address);
void dma_mm2s_status(unsigned int* dma_virtual_address);
int fft(float* input, float* output, int num_pts, int direction, int scale);
int log2ofX(int x);

#endif /* FFT_H_ */
