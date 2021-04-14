/*
 * PWM  C++ Application
 * Author: Noor Alali
 */
#include "stdbool.h"
#include "xil_types.h"
#include "xgpio.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xtmrctr.h"
#include <iostream>
#include "xparameters.h"

using namespace std;

//ps0
int main()
{
	static XGpio out, in;
	
	int xStatus = XGpio_Initialize(&in, XPAR_AXI_GPIO_1_DEVICE_ID);
	int xStatus1 = XGpio_Initialize(&out, XPAR_AXI_GPIO_0_DEVICE_ID);

	if( (xStatus != XST_SUCCESS)  || (xStatus1 != XST_SUCCESS)){
		cout << "GPIO INIT FAILED" << endl;
		return 1;
	}

	XGpio_SetDataDirection(&out,1,0x0);
	XGpio_SetDataDirection(&in,1,0xF);

	u32* Timer_Ptr = (u32*)XPAR_TMRCTR_0_BASEADDR;

	*(Timer_Ptr) = 0x206;
	*(Timer_Ptr+4) = 0x206;

	*(Timer_Ptr+1) = 0;
	*(Timer_Ptr+5) = 0;

	*(Timer_Ptr) = 0x226;
	*(Timer_Ptr+4) = 0x226;

	*(Timer_Ptr) = 0x206;
	*(Timer_Ptr+4) = 0x206;

	u32 current=0xf;
	u32 old=0xf;
	float PWM_PERIOD = 1;
	float DUTY_CYCLE = 50;
	XGpio_DiscreteWrite(&out, 1, 0x0);
	while (true) {

		current = XGpio_DiscreteRead(&in, 1);
		if(current != old){

			old = current;

			if(XGpio_DiscreteRead(&in, 1) == 0x8){
				//go up
				XGpio_DiscreteWrite(&out, 1, 0x2);
				*(Timer_Ptr + 1) = (PWM_PERIOD * 50000000) - 2;
				*(Timer_Ptr + 5) = (((DUTY_CYCLE / 100) * PWM_PERIOD) * 50000000) - 2;
				*(Timer_Ptr) = 0x286;
				*(Timer_Ptr + 4) = 0x286;
			}

			else if(XGpio_DiscreteRead(&in, 1) == 0x9){
				//go down
				XGpio_DiscreteWrite(&out, 1, 0x4);
				*(Timer_Ptr + 1) = (PWM_PERIOD * 50000000) - 2;
				*(Timer_Ptr + 5) = (((DUTY_CYCLE / 100) * PWM_PERIOD) * 50000000) - 2;
				*(Timer_Ptr) = 0x286;
				*(Timer_Ptr + 4) = 0x286;
			}
			else{

				*(Timer_Ptr) = 0x0;
				*(Timer_Ptr + 4) = 0x0;
			}
		}

	}
	
	return 0;
}
