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
	float count=0; //counter w/o cdma
	float count1=0; //counter w/ cdma
	int copy_fail = 0; //data transfer correctly done or not
	int trace; //capture the index where dest[i] != source[i]
	//array of number of integer values to transfer
	//Since each integer is 32-bit, every value in the program takes 4-bytes of space. Hence the maximum number of integer values transferrable is floor[8,388,607/4] = 2,097,151 values. 
	int array[] = {1024,4096,8192,16384,32767,1048576,2097151,2097152,2097153,2097154,2097164,2100000};
	for(int i=0;i<12;i++)
	{
		int num_of_values = array[i];
		int num_of_bytes = num_of_values*4; //each int is 4bytes

		for(int j = 0; j < num_of_values; j++)
		{
			source_address[j] = j;
			dest_address[j] = -1;
		}

		// -----------WITHOUT CDMA--------------

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

		for(int k = 0; k < num_of_values; k++)
		{
			if(dest_address[k] != source_address[k])
			{
				copy_fail = 1;
				trace = k;
				break;
			}
		}
		count = *(Timer_Ptr + 2);
		cout << "For " << num_of_values << " values : " << endl;

		cout << "Without CDMA : " <<  count << " counts.   ";
		if(copy_fail == 0)
		{
			cout << "Copy success" << endl;
		}
		else
		{
			cout << "Copy fail. Error at index: "<< trace << endl;
		}

		copy_fail = 0;
		trace = 0;
		
		// -----------WITH CDMA--------------

		for(int l = 0; l < num_of_values; l++)
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
		//flushes the data in the L2 and L1 cache. Since, the cache memory uses the write-back with write allocation writing policy, data of dirty blocks are written back to the external memory. 			//This is done so that the CDMA reads current data and not ‘garbage’ data from previous runs/state. 
		Xil_DCacheFlush();

		//timer settings:
		*(Timer_Ptr) = 0x001;
		*(Timer_Ptr + 1) = 0;
		*(Timer_Ptr) = 0x021;
		//timer start
		*(Timer_Ptr) = 0x081;

		//bytes to transfer => btt reg
		*(cdma_Ptr +10) = num_of_bytes;
		//if the status register value is 0x2, then the CDMA is idling
		while( (*(cdma_Ptr + 1) & mask) == 0)
		{
		}

		//stop timer
		*(Timer_Ptr) = 0x001;

		count1 = *(Timer_Ptr + 2);

		for(int p=0; p<num_of_values;p++)
		{
			if(source_address[p] != dest_address[p])
			{
				copy_fail = 1;
				trace = p;
				break;
			}
		}

		cout << "With CDMA : " <<  count1 << " counts.      ";

		if(copy_fail == 0)
		{
			cout << "Copy success." << endl;
			cout << "Speed up: " << count/count1 << endl <<endl;
		}
		else
		{
			cout << "Copy fail. Error at index: "<< trace << endl << endl;
		}
		copy_fail = 0;
	}

	cout << "End of Application" << endl ;

	return 0;
}
