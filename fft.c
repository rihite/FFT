#include <sys/mman.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "fft.h"

//write value to address + offset
void write_vaddr(void* vaddr, int offset, int value) {
    *((volatile uint32_t *)(vaddr + offset)) = value;
}

//read from value at address + offset
uint32_t read_vaddr(void* vaddr, int offset) {
    return *((volatile uint32_t *)(vaddr + offset)) ;
}

//prints status of DMA S2MM registers
void dma_s2mm_status(unsigned int* dma_virtual_address) {
    unsigned int status = read_vaddr(dma_virtual_address, S2MM_STATUS_REGISTER);
    printf("Stream to memory-mapped status (0x%08x@0x%02x):", status, S2MM_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

//prints status of DMA MM2S registers
void dma_mm2s_status(unsigned int* dma_virtual_address) {
    unsigned int status = read_vaddr(dma_virtual_address, MM2S_STATUS_REGISTER);
    printf("Memory-mapped to stream status (0x%08x@0x%02x):", status, MM2S_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

int fft(float* input, float* output, int num_pts, int direction, int scale){
	
	int fd;
	void *dma_vaddr, *gpio_vaddr;
	float *source_vaddr, *dest_vaddr;	
	uint32_t fft_config;
	
	fd = open("/dev/mem", O_RDWR);

	if(fd == -1){		
		return OPEN_MEM_FAILURE;
	}

	//set dma virtual base address
	dma_vaddr = mmap(0, DMA_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_CTRL_AXI_DMA_0_BASEADDR);
	
	if (dma_vaddr == (void *) -1) {		
		return MMAP_FAILURE;
	}
	
	//set gpio virtual base addres
	gpio_vaddr = mmap(0, GPIO_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_CTRL_AXI_GPIO_0_BASEADDR);
	
	if (gpio_vaddr == (void *) -1) {		
		return MMAP_FAILURE;
	}
	
	//set source virtual base addres
	source_vaddr = (float*)mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SOURCE_PADDR);
	
	if (source_vaddr == (void *) -1) {		
		return MMAP_FAILURE;
	}
	
	//set  destination virtual base addres
	dest_vaddr = (float*)mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, DEST_PADDR);
	
	if (dest_vaddr == (void *) -1) {		
		return MMAP_FAILURE;
	}
		
	//copy input data to physical data registers	
	memcpy(source_vaddr, input, sizeof(float)*2*MAX_NUM_PTS);
	
	//zero out destination memory
	memset((void *)dest_vaddr,0,sizeof(float)*2*MAX_NUM_PTS);		
	
	//create fft config
	//TODO:  neet to change these macros to variables in function call
	fft_config = (LOG2_NUM_PTS << PTS_SHIFT);
	fft_config |= (DIRECTION << DIRECTION_SHIFT);
	fft_config |= (SCALE << SCALE_SHIFT);
	
	//write fft control to fft control register via gpio
	write_vaddr(gpio_vaddr, 0, fft_config);		
	
	//reset dma. writing it once resets both tx and rx channels	
    write_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER, DMA_RESET);
    
    //wait for both dma channels to reset
    while((read_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER) & DMA_RESET) != 0);
    while((read_vaddr(dma_vaddr, S2MM_CONTROL_REGISTER) & DMA_RESET) != 0);    	
	
	//TODO: flush cache here??	   
    
    // Write source address to MM2S start address register
    write_vaddr(dma_vaddr, MM2S_START_ADDRESS, SOURCE_PADDR); 
      
    //start MM2S channel   
    write_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER, START);        
    
    //write MM2S transfer length to register 
    //this actually starts transfer
    write_vaddr(dma_vaddr, MM2S_LENGTH, TOT_BYTES);        
  
    // Write destination address to S2MM destination address register
    write_vaddr(dma_vaddr, S2MM_DESTINATION_ADDRESS, DEST_PADDR);     
    
    //start S2MM channel
    write_vaddr(dma_vaddr, S2MM_CONTROL_REGISTER, START);     
    
    //write S2MM transfer length
    //this actually starts transfer
    write_vaddr(dma_vaddr, S2MM_LENGTH, TOT_BYTES);    

    //wait for both channels to go idle signaling completion
    while((read_vaddr(dma_vaddr, MM2S_STATUS_REGISTER) & IDLE) == 0);    
    while((read_vaddr(dma_vaddr, S2MM_STATUS_REGISTER) & IDLE) == 0);     
	
	//write results to output buffer
	memcpy(output, dest_vaddr, MAX_NUM_PTS * BYTES_PER_PT);
	
	return FFT_SUCCESS;
}










