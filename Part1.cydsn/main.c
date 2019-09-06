#include <project.h>
uint8 buffer[8];
int main()
{
    uint16 length = 8;
    CyGlobalIntEnable;
    ADC_DelSig_1_Start();
    /* Start USBFS operation with 5V operation. */
    USBFS_Start(0, USBFS_5V_OPERATION);

    /* Wait while the USB Starts*/
    while (0u == USBFS_GetConfiguration())
    {
    }
	
	//Create an ADC variable that keeps track of adc value and is an int to ensure no overflow/underflow
    int adc = 0;
    for(;;)
    {
		//Read from adc and check for hardware error
        adc = ADC_DelSig_1_Read16();
        if(adc > 250) adc = 255;
        if(adc < 5) adc = 0;
        
		//set the first byte as the adc value and send it via USB
        buffer[0] = adc;
        USBFS_LoadInEP(1, buffer, length);
        CyDelay(1);
    }
}


/* [] END OF FILE */
