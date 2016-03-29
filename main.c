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
	
	int stop = 0;
	while(!stop){
		//perform fft
		int retval = fft(input, output, num_pts, direction, scale);
	
		if(!retval){
			printf("fft completed successfully");
		}
		else{
			printf("fft returned value %d", retval);
		}
	
		FILE fp2 = fopen("/home/alarm/Workspace/src/results.txt", "w+");
		//fwrite((float*)dest_vaddr, sizeof(float), NUM_PTS * 2, fp2);
		fclose(fp2);
		
		printf("Enter 0 to continue or 1 to close: ");
		scanf("%d",&stop);
	}
	
	return 0;
}

