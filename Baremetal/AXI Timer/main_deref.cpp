#include "stdbool.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xgpio.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xtmrctr.h"


#include <iostream>
using namespace std;
int main()
{
static XGpio GPIOInstance_Ptr;
u32* Timer_Ptr =(u32*)XPAR_TMRCTR_0_BASEADDR;
// base address = control/status reg
// base address + 4 bytes = load register value (reset value)
// base address + 8 bytes = counter register
int xStatus;
cout << "Application Starts" << endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Step-1: AXI GPIO Initialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
xStatus = XGpio_Initialize(&GPIOInstance_Ptr,XPAR_AXI_GPIO_FOR_OUTPUT_DEVICE_ID);
if(xStatus != XST_SUCCESS)
{
cout << "GPIO A Initialization FAILED" << endl;
return 1;
}
XGpio_SetDataDirection(&GPIOInstance_Ptr, 1, 0);
// set mode to capture mode and specify up-count
// 0x001 = capture
// 0x000 = generate mode
*(Timer_Ptr) = 0x001;
// load 0 as the reset value into the load register of the counter
*(Timer_Ptr + 1) = 0;
// now load this value into the counter register by setting load = 1 in control register
*(Timer_Ptr) = 0x020+0x001;
// put the load bit off to allow the counter to be enabled when the enable bit = 1
// since when load = 1 , the counter is prevented from running.
// start the timer by setting enable = 1 ;
*(Timer_Ptr) = 0x081;
// perform a write to the GPIO output port
XGpio_DiscreteWrite(&GPIOInstance_Ptr, 1, 0xA3);
// stop the timer by setting enable = 0;
*(Timer_Ptr) = 0x001;
unsigned int count;
// read the value of the counter register
count = *(Timer_Ptr + 2);
cout << "Count = " << count << endl;
cout << "Timer reloaded with 0" << endl;
*(Timer_Ptr) = 0x021;
*(Timer_Ptr) = 0x001;
count = *(Timer_Ptr + 2) ;
cout << "Counter after reload = "<< count << endl;
// determine the overhead in starting and stopping the counter
*(Timer_Ptr) = 0x081; // start the timer
*(Timer_Ptr) = 0x001; // stop the timer
count = *(Timer_Ptr + 2);
cout << "Time to access memory two times to start and stop timer Counter = " << count << endl;
return 0;
}
