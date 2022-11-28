/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include <xaes_encrypt.h>
#include "xparameters.h"
#include "xil_io.h"
#include <xil_types.h>
#include "xsdps.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"
#define SIZE 16

static const char *cardPath = "0:/";
static FATFS fs;
static FIL read_file, write_file;
FRESULT result;

unsigned char nBytes;
void rand_str(char *dest, size_t length) {

    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}


int main()
{
    init_platform();
    int status;
    XAes_encrypt doencrypt;
    XAes_encrypt *doencrypt_cfg;
    result = f_mount(&fs, cardPath, 1);
    printf("Card mounting: %d\n", result);

    doencrypt_cfg = XAes_encrypt_LookupConfig(XPAR_AES_ENCRYPT_0_DEVICE_ID);

     if(!doencrypt_cfg)
     {
     	xil_printf("Error loading config\n");
     }

     status = XAes_encrypt_CfgInitialize(&doencrypt, doencrypt_cfg);

     if(status != XST_SUCCESS)
     {
     	xil_printf("Error initializing\n");
     }
unsigned char plaintext[16] = "ABCDEFGHIJKLMNOP";
     int addr = XAes_encrypt_Get_plaintext_BaseAddress(&doencrypt);
	XAes_encrypt_Set_Nr(&doencrypt, 10);
	XAes_encrypt_Write_plaintext_Bytes(&doencrypt, XPAR_XAES_ENCRYPT_0_S_AXI_CRTLS_BASEADDR, plaintext, 16);
	XAes_encrypt_Read_plaintext_Bytes(&doencrypt, XPAR_XAES_ENCRYPT_0_S_AXI_CRTLS_BASEADDR, plaintext, 16);

	//XAes_encrypt_WriteReg(XPAR_XAES_ENCRYPT_0_S_AXI_CRTLS_BASEADDR, 0);
	xil_printf("Plaintext : %s\n", plaintext);
	XAes_encrypt_Start(&doencrypt);
	while(!XAes_encrypt_IsDone(&doencrypt));
	int flag = 45;
	flag = XAes_encrypt_Get_return(&doencrypt);
	unsigned char ciphertext[16];
	XAes_encrypt_Read_ciphertext_Bytes(&doencrypt,XPAR_XAES_ENCRYPT_0_S_AXI_CRTLS_BASEADDR + 16, ciphertext, 16);
	if(!flag)
	{
		xil_printf("Success, Encrypted: %s\n", ciphertext);
		//xil_printf("Success, Encrypted: \n");
	}
	else
		xil_printf("Fail\n");

/*	result = f_open(&write_file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
			printf("File opening for write: %d\n", result);

			result = f_write(&write_file, ciphertext, SIZE*sizeof(char), &nBytes);
							//result = f_write(&write_file, "\n", 2*sizeof(char), &nBytes);
							xil_printf("File writing: %s\n", ciphertext);*/
     				/*

    int create = 1;
    int records = 10;
    if(create)
    {
    	unsigned char str[16];
		result = f_open(&write_file, "inputs.txt", FA_WRITE | FA_CREATE_ALWAYS);
		printf("File opening for write: %d\n", result);

		if(result == FR_OK){

			for(int i = 0; i < records; ++i)
			{
				rand_str(str, sizeof str);
				result = f_write(&write_file, str, SIZE*sizeof(char), &nBytes);
				//result = f_write(&write_file, "\n", 2*sizeof(char), &nBytes);
				xil_printf("File writing: %s\n", str);
			}
		}

		result = f_close(&write_file);
		xil_printf("Closing file: %d\n", result);



		result = f_open(&read_file, "inputs.txt", FA_READ);
		printf("File opening: %d\n", result);

		if(result == FR_OK){
			for (int i = 0; i < records; ++i){
			result = f_read(&read_file, str, SIZE*sizeof(char), &nBytes);
			printf("File reading: %d\n", result);
			printf("str: %s\n\r", str);



			}
		}
		result = f_close(&read_file);
		printf("Closing file: %d\n", result);
    }

/*
    doencrypt_cfg = XAes_encrypt_LookupConfig(XPAR_AES_ENCRYPT_0_DEVICE_ID);

    if(!doencrypt_cfg)
    {
    	xil_printf("Error loading config\n");
    }

    status = XAes_encrypt_CfgInitialize(&doencrypt, doencrypt_cfg);

    if(status != XST_SUCCESS)
    {
    	xil_printf("Error initializing\n");
    }


    //print("Hello World\n\r");
    //print("Successfully ran Hello World application");
    // cleanup_platform();
    result = f_open(&read_file, "inputs.txt", FA_READ);
	printf("File opening: %d\n", result);
	FRESULT result2 = f_open(&write_file, "encrypted.txt", FA_WRITE | FA_CREATE_ALWAYS);
	printf("File opening for write: %d\n", result2);

	unsigned char plaintext[16];
    for(int i = 0; i < records; ++i)
    {


		if(result == FR_OK)
		{
				result2 = f_read(&read_file, plaintext, SIZE*sizeof(char), &nBytes);
				printf("File reading: %d\n", result2);
				printf("str: %s\n\r", plaintext);
				int addr = XAes_encrypt_Get_plaintext_BaseAddress(&doencrypt);
				XAes_encrypt_Set_Nr(&doencrypt, 14);
				XAes_encrypt_Write_plaintext_Bytes(&doencrypt, addr, plaintext, 16);
				XAes_encrypt_Start(&doencrypt);
				while(!XAes_encrypt_IsDone(&doencrypt));
				int flag = 45;
				flag = XAes_encrypt_Get_return(&doencrypt);
				unsigned char ciphertext[16];
				XAes_encrypt_Read_ciphertext_Bytes(&doencrypt, XAes_encrypt_Get_ciphertext_BaseAddress(&doencrypt), ciphertext, 16);
				if(!flag)
				{
					xil_printf("Success, Encrypted: %s\n", ciphertext);
				}
				else
					xil_printf("Fail\n");



				if(result2 == FR_OK)
				{
					result2 = f_write(&write_file, ciphertext, SIZE*sizeof(char), &nBytes);
					//result = f_write(&write_file, "\n", 2*sizeof(char), &nBytes);
					xil_printf("File writing: %s\n", ciphertext);

					}
				}


			}


	result = f_close(&read_file);
	printf("Closing file: %d\n", result);
	result = f_close(&write_file);
	printf("Closing file: %d\n", result);
	result = f_unmount("0:/");
	printf("Card unmounting: %d\n\n", result);*/
    return 0;
}
