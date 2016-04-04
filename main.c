/*
 * main.c
 *
 *  Created on: Mar 21, 2016
 *      Author: hiterd
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fft.h"

int main(){
	//declarations
	FILE *fp;
	int i, retval = 0; //loop counter
	
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
	
	//read from input file
	fp = fopen("./inputs.txt", "r");
        for(i = 0; i < MAX_NUM_PTS * 2; i++){
        	retval = fscanf(fp, "%f", (input+i));
		if(retval == EOF){
			*(input+i) = 0;
			break;
		}
        }
        fclose(fp);


	int stop = 0;
	while(stop == 0){
		//perform fft
		int retval = fft(input, output, num_pts, direction, scale);
	
		if(!retval){
			printf("FFT completed successfully\n");
		}
		else{
			printf("FFT returned value %d\n", retval);
		}
	
		fp = fopen("./outputs.txt", "w+");
		for(i =0 ; i < MAX_NUM_PTS * 2; i++){
			fprintf(fp, "%f ", *((volatile float*)(output + i)));
		}
		fclose(fp);
		
		fp = fopen("./cplx_outputs.txt", "w+");
		for(i =0 ; i < MAX_NUM_PTS * 2; i+=2){
			fprintf(fp, "%f + j*%f\n", *((volatile float*)(output + i)), *((volatile float*)(output + i + 1)));
		}
		fclose(fp);
		
		printf("Enter 0 to continue or 1 to close: ");
		scanf("%d",&stop);
	}
	
	//free allocated memory	
	free(input);
	free(output);
	
	return 0;
}

