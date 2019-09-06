#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <wiringPi.h>
#include <errno.h>
#include <libusb.h>

int main (int argc, char* argv[]){ 
	libusb_device_handle* psoc; // Pointer to data structure representing USB device

	char rx_data[64]; // Receive data block
	int rx; // Bytes received
	int return_val;
	
	//Check if wiringPi failed or not
	if(wiringPiSetup() == -1)
	{
		perror("Wiring Setup Failure");
		return	-1;
	}

	libusb_init(NULL);

	// Open the USB device (the Cypress device has
	// Vendor ID = 0x04B4 and Product ID = 0x8051)
	psoc = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);

	if (psoc == NULL)
	{
		perror("device not found\n");
	}

	//Get rid of residual data
	if (libusb_reset_device(psoc) != 0)
	{
		perror("Device reset failed\n");
	}	

	// Set configuration of USB device
	if (libusb_set_configuration(psoc, 1) != 0)
	{
		perror("Set configuration failed\n");
	}	


	// Claim the PSoC
	if (libusb_claim_interface(psoc, 0)!=0)
	{
		perror("Cannot claim interface");
	}

	
	// Clear receive buffer
	for (int i=0; i<64; i++) rx_data[i] = 0;

	//Set the pin 12 of the	Pi to the PWM_OUTPUT 
	pinMode(1, PWM_OUTPUT);
	while(1)
	{
		//Receive data from the USB and store the number of bytes received in a return value
		return_val = libusb_bulk_transfer(psoc,	(0x01 |	0x80), rx_data,	64, &rx, 0);
		if(return_val < 0) perror("read error!");
		//For the sake of hardware error if any data from the received exceeds 1000 then roof that up to 1024
		if(((int)rx_data[0] * 4) > 1000)
		{
			pwmWrite(1, 1024);
		}	else	
		{
			//Otherwise simply write the data to the PWM
			pwmWrite(1, (int)rx_data[0] * 4);
		}
	}
	libusb_close(psoc);
}