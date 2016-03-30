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
//#include "stim.h"
#include "fft.h"

//extern float sig_two_sine_waves[2*MAX_NUM_PTS];

int main(){

	//magic numbers
	//these should be set by whoever is calling the fft function and not hardcoded
	int num_pts = 8192;
	int direction = 1; // 1 = forward, 0 = inverse
	int scale = 0x2AB;
	
	float *input = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);
	float *output = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);
	
	int stop = 0;
	while(stop == 0){
		//perform fft
		int retval = fft(input, output, num_pts, direction, scale);
	
		if(!retval){
			printf("fft completed successfully");
		}
		else{
			printf("fft returned value %d", retval);
		}
	
		//~ FILE fp2 = fopen("/home/alarm/Workspace/src/results.txt", "w+");
		//~ fwrite((float*)dest_vaddr, sizeof(float), NUM_PTS * 2, fp2);
		//~ fclose(fp2);
		
		printf("Enter 0 to continue or 1 to close: ");
		scanf("%d",&stop);
	}
	
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

	

	
	
	free(input);
	free(output);
	
	return 0;
}

