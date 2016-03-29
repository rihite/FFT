/*
 * main.c
 *
 *  Created on: Mar 21, 2016
 *      Author: hiterd
 */
 
 //portions of code used from http://lauri.lsxn--vsandi-pxa.com/hdl/zynq/xilinx-dma.html

#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "fft.h"

int main(){
	
	//magic numbers
	//these should be set by whoever is calling the fft function and not hardcoded
	int num_pts = 8192;
	int direction = 1; // 1 = forward, 0 = inverse
	int scale = 0x2AB;
	
	//allocate input and output buffers
	float *input = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);
	float *output = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);	
	
	FILE *fp1 = fopen("/home/alarm/Workspace/src/inputs.txt", "w+");
	//fwrite(source_vaddr, sizeof(float), NUM_PTS * 2, fp1);
	fclose(fp1);
	
	//perform fft
	int retval = fft(input, output, num_pts, direction, scale);
	
	
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
	
	//copy input data to physical data registers
	//memcpy(source_vaddr, temp_source_vaddr, TOT_BYTES);
	
	
	FILE *fp2;
	fp2 = fopen("/home/alarm/Workspace/src/results.txt", "w+");
	//fwrite((float*)dest_vaddr, sizeof(float), NUM_PTS * 2, fp2);
	fclose(fp2);
	
	return 0;
}

