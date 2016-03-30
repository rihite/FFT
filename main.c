/*
 * main.c
 *
 *  Created on: Mar 21, 2016
 *      Author: hiterd
 */
 
 //portions of code used from http://lauri.lsxn--vsandi-pxa.com/hdl/zynq/xilinx-dma.html

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "fft.h"
#include "stim.h"

extern float sig_two_sine_waves[2*MAX_NUM_PTS];

int main(){

	//magic numbers
	//these should be set by whoever is calling the fft function and not hardcoded
	int num_pts = 8192;
	int direction = 1; // 1 = forward, 0 = inverse
	int scale = 0x2AB;
	
	//allocate data buffers
	//this will be done in a program trying to use fft.h
	//these buffers should be the maximum size alway even if not used
	float *input = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);
	float *output = (float *)malloc(MAX_NUM_PTS*BYTES_PER_PT);
	
	//make sure buffers are clear...probably inefficient
	memset((void *)input, 0, MAX_NUM_PTS*BYTES_PER_PT);
	memset((void *)output, 0, MAX_NUM_PTS*BYTES_PER_PT);
	
	//TODO:  need to change this to read from an input file
	memcpy(input, sig_two_sine_waves, sizeof(float)*2*MAX_NUM_PTS);
	
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
	
	//free malloc'ed memory	
	free(input);
	free(output);
	
	return 0;
}

