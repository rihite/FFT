/*
 * main.c
 *
 *  Created on: Mar 21, 2016
 *      Author: hiterd
 */
 
 //portions of code used from http://lauri.lsxn--vsandi-pxa.com/hdl/zynq/xilinx-dma.html

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stim.h"
#include "fft.h"

extern float sig_two_sine_waves[2*MAX_NUM_PTS];

//write value to address + offset
void write_vaddr(void* vaddr, int offset, int value) {
    *((volatile uint32_t *)(vaddr + offset)) = value;
}

//read from value at address + offset
uint32_t read_vaddr(void* vaddr, int offset) {
    return *((volatile uint32_t *)(vaddr + offset)) ;
}

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

int main(){

	//declarations
	int fd;
	void *dma_vaddr, *gpio_vaddr;
	float *source_vaddr, *dest_vaddr;	
	uint32_t fft_config;
	
	//begin area that will be allocated by another program or module
	//allocte memory for stiumuls buffer
	//~ float *temp_source_vaddr, *temp_dest_vaddr;
	
	//~ temp_source_vaddr = (float*)malloc(sizeof(float)*2*MAX_NUM_PTS);
	//~ if (temp_source_vaddr == NULL){
		//~ printf("ERROR! Failed to allocate memory for the stimulus buffer.\n\r");
		//~ return -1;
	//~ }

	//allocate memory for result buffer
	//temp_dest_vaddr = (float*)malloc(sizeof(float)*2*MAX_NUM_PTS);
	
	//~ if (temp_dest_vaddr == NULL){
		//~ printf("ERROR! Failed to allocate memory for the result buffer.\n\r");
		//~ return -1;
	//~ }	
	
	//end area that will be done by another program or module

	fd = open("/dev/mem", O_RDWR);

	if(fd == -1){
		printf("cant open /dev/mem.\n");
		return 1;
	}

	//set dma virtual base address
	dma_vaddr = mmap(0, DMA_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_CTRL_AXI_DMA_0_BASEADDR);
	
	if (dma_vaddr == (void *) -1) {
		printf("Can't map the dma to user space.\n");
		return 1;
	}
	
	//set gpio virtual base addres
	gpio_vaddr = mmap(0, GPIO_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_CTRL_AXI_GPIO_0_BASEADDR);
	
	if (gpio_vaddr == (void *) -1) {
		printf("Can't map the gpio to user space.\n");
		return 1;
	}
	
	//set source virtual base addres
	source_vaddr = (float*)mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SOURCE_PADDR);
	
	if (source_vaddr == (void *) -1) {
		printf("Can't map source address to user space.\n");
		return 1;
	}
	
	//set  destination virtual base addres
	dest_vaddr = (float*)mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, DEST_PADDR);
	
	if (dest_vaddr == (void *) -1) {
		printf("Can't map destination address to user space.\n");
		return 1;
	}
	
	//copy input data to physical data registers
	//memcpy(source_vaddr, temp_source_vaddr, TOT_BYTES);
	//copy from stim.h
	memset((void *)source_vaddr,0,sizeof(float)*2*MAX_NUM_PTS);
	printf("the first val is %f\n", *((volatile float*)(source_vaddr+1)));
	memcpy(source_vaddr, sig_two_sine_waves, sizeof(float)*2*MAX_NUM_PTS);
	printf("the first val is %f\n", *((volatile float*)(source_vaddr+1)));
	
	memset((void *)dest_vaddr,0,sizeof(float)*2*MAX_NUM_PTS);
	printf("the first out val is %f\n", *((volatile float*)(dest_vaddr)));
	
	
	//~ FILE *fp1 = fopen("./inputs.txt", "
	//~ int i;
	//~ for(i=0; i<MAX_NUM_PTS * 2; i++){
		//~ fprintf(fp1, "%f ", *(source_vaddr + i));
	//~ }
	//~ fclose(fp1);
	
	printf("DMA mapped to address %p.\n", dma_vaddr);
	printf("GPIO mapped to address %p.\n", gpio_vaddr);	
	
	//create fft config
	fft_config = (LOG2_NUM_PTS << PTS_SHIFT);
	fft_config |= (DIRECTION << DIRECTION_SHIFT);
	fft_config |= (SCALE << SCALE_SHIFT);
	
	//write fft control to fft control register via gpio
	write_vaddr(gpio_vaddr, 0, fft_config);	
	printf("%x written to gpio\n", read_vaddr(gpio_vaddr, 0));
	
	//reset dma. writing it once resets both tx and rx channels
	printf("Resetting DMA\n");
    write_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER, DMA_RESET);
    //wait for dma to reset itself
    while((read_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER) & DMA_RESET) != 0);
    while((read_vaddr(dma_vaddr, S2MM_CONTROL_REGISTER) & DMA_RESET) != 0);
    printf("DMA is reset\n");      
    dma_s2mm_status(dma_vaddr);
    dma_mm2s_status(dma_vaddr);		
	
	//flush cache??
	
	//check if transfer in progress	
    
    printf("Writing source address...\n");
    write_vaddr(dma_vaddr, MM2S_START_ADDRESS, SOURCE_PADDR); // Write source address
    dma_mm2s_status(dma_vaddr);    
    
    printf("Starting MM2S channel with all interrupts disabled...\n");
    write_vaddr(dma_vaddr, MM2S_CONTROL_REGISTER, START);
    dma_mm2s_status(dma_vaddr);
    
    printf("Writing MM2S transfer length...\n");
    write_vaddr(dma_vaddr, MM2S_LENGTH, TOT_BYTES);
    dma_mm2s_status(dma_vaddr);
    
    
    printf("Writing destination address\n");
    write_vaddr(dma_vaddr, S2MM_DESTINATION_ADDRESS, DEST_PADDR); // Write destination address
    dma_s2mm_status(dma_vaddr);
    
    printf("Starting S2MM channel with all interrupts disabled...\n");
    write_vaddr(dma_vaddr, S2MM_CONTROL_REGISTER, START);
    dma_s2mm_status(dma_vaddr);     
    
    printf("Writing S2MM transfer length...\n");
    write_vaddr(dma_vaddr, S2MM_LENGTH, TOT_BYTES);
    dma_s2mm_status(dma_vaddr); 

    printf("Waiting for MM2S synchronization...\n");
    while((read_vaddr(dma_vaddr, MM2S_STATUS_REGISTER) & IDLE) == 0);
    printf("finished MM2S synch\n");
    dma_mm2s_status(dma_vaddr);	
    
    dma_s2mm_status(dma_vaddr);
    printf("Waiting for S2MM sychronization...\n");
    while((read_vaddr(dma_vaddr, S2MM_STATUS_REGISTER) & IDLE) == 0); 

    dma_s2mm_status(dma_vaddr);
    dma_mm2s_status(dma_vaddr);	

	
	
	//delete from here to----
	printf("the first output val is %f\n", *((volatile float*)(dest_vaddr)));
	
	FILE *fp2 = fopen("./outputs.txt", "w+");
	int j;
	for(j=0; j<MAX_NUM_PTS * 2; j++){
		fprintf(fp2, "%f ", *((volatile float*)(dest_vaddr + j)));
	}
	fclose(fp2);
	
	return 0;
}

