/*
 * Empty C++ Application
 */
#include "xil_exception.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include <iostream>
#include "xil_types.h"
#include "xil_exception.h"
#include "xil_io.h"
using namespace std;

int main()
{
	//cdma:
	u32* cdma_Ptr = (u32*)XPAR_AXI_CDMA_0_BASEADDR;
	//source:
	u32* source_address = (u32*)XPAR_PS7_DDR_0_S_AXI_HP0_BASEADDR;
	//destination:
	u32* dest_address = (u32*)XPAR_PS7_DDR_0_S_AXI_HP2_BASEADDR;

	//timer:
	u32* Timer_Ptr = (u32*)XPAR_TMRCTR_0_BASEADDR;

	u32 mask = 0x2;
	float count=0;
	float count1=0;
	int array[] = {1024,4096,8192,16384,32767,1048576};
	for(int i=0;i<6;i++)
	{
		int num_of_values = array[i];
		//each integer value takes 4-bytes of space
		int num_of_bytes = num_of_values*4;

		for(int j = 0; j < num_of_values; j++)
		{
			source_address[j] = j;
			dest_address[j] = -1;
		}

		// -----------------------------------------WITHOUT CDMA-----------------------------------------

		//timer settings:
		*(Timer_Ptr) = 0x001;
		*(Timer_Ptr + 1) = 0;
		*(Timer_Ptr) = 0x021;

		//timer start
		*(Timer_Ptr) = 0x081;

		for(int k = 0; k < num_of_values; k++)
		{
			dest_address[k] = source_address[k];
		}

		//stop timer
		*(Timer_Ptr) = 0x001;

		count = *(Timer_Ptr + 2);
		cout << "For " << num_of_values << "values : " << endl;
		cout << "Without CDMA : " <<  count << " counts" << endl;

		// -----------------------------------------WITH CDMA--------------------------------------------

		for(int l = 0; l < 4096; l++)
		{
			source_address[l] = l;
			dest_address[l] = -1;
		}

		//reset
		*(cdma_Ptr) = 0x00000004;
		//simple mode
		*(cdma_Ptr) = 0x00000020;

		//SA +6      DA +8
		*(cdma_Ptr +6) = 0x20000000;
		*(cdma_Ptr +8) = 0x30000000;

		//flush the cache
		Xil_DCacheFlush();

		//timer settings:
		*(Timer_Ptr) = 0x001;
		*(Timer_Ptr + 1) = 0;
		*(Timer_Ptr) = 0x021;
		//timer start
		*(Timer_Ptr) = 0x081;

		//bytes to transfer => btt reg
		*(cdma_Ptr +10) = num_of_bytes;

		while( (*(cdma_Ptr + 1) & mask) == 0)
		{
		}

		//stop timer
		*(Timer_Ptr) = 0x001;

		count1 = *(Timer_Ptr + 2);
		cout << "With CDMA : " <<  count1 << " counts" << endl;
		cout << "Speed up: " << count/count1 << endl<<endl;
	}


	// -----------------------------------------Going over Max bytes allowed--------------------------------

	for(int l = 0; l < 2097152; l++)
	{
		source_address[l] = l;
		dest_address[l] = -1;
	}

	//reset
	*(cdma_Ptr) = 0x00000004;
	//simple mode
	*(cdma_Ptr) = 0x00000020;

	//SA +6      DA +8
	*(cdma_Ptr +6) = 0x20000000;
	*(cdma_Ptr +8) = 0x30000000;

	//flush the cache
	Xil_DCacheFlush();

	//timer settings:
	*(Timer_Ptr) = 0x001;
	*(Timer_Ptr + 1) = 0;
	*(Timer_Ptr) = 0x021;
	//timer start
	*(Timer_Ptr) = 0x081;

	//bytes to transfer => btt reg
	*(cdma_Ptr +10) = 8388608;

	while( (*(cdma_Ptr + 1) & mask) == 0)
	{
	}

	//stop timer
	*(Timer_Ptr) = 0x001;

	count1 = *(Timer_Ptr + 2);
	cout << "With CDMA with over the maximum bytes allowed (@8388608): " <<  count1 << " counts" << endl<<endl;
	cout << "End of Application" << endl;

	return 0;
}

