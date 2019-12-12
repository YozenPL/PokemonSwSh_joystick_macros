/*
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/

// make && sudo dfu-programmer atmega16u2 erase && sudo dfu-programmer atmega16u2 flash Joystick.hex

// make && ./teensy_loader_cli -mmcu=atmega32u4 -w Joystick.hex

#include "Joystick.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	X,
	Y,
	A,
	B,
	L,
	R,
	THROW,
	NOTHING,
	PLUS,
	MINUS,
	TRIGGERS,
	HOME
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command; 

static const command step[] = {
	// Setup controller
	{ NOTHING,   10 },
	{ TRIGGERS,   5 },
	{ NOTHING,   10 },
	
	// RAID RESET	
	{ A,          5 },
	{ NOTHING,   90 },
	{ HOME,       5 }, 
	{ NOTHING,   20 },
	{ RIGHT,      5 }, 
	{ NOTHING,    1 },
	{ RIGHT,      5 },
	{ NOTHING,    1 },
	{ DOWN,       5 },
	{ NOTHING,    1 },
	{ RIGHT,      5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,   15 },
	{ DOWN,      70 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ DOWN,      20 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ DOWN,      15 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,   10 },
	{ UP,         5 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ A,          5 },
	{ NOTHING,    1 },
	{ HOME,       5 },
	{ NOTHING,   20 },
	{ HOME,       5 },
	{ NOTHING,   20 },
	{ B,          5 },
	{ NOTHING,   40 },
	{ A,          5 },
	{ NOTHING,  170 },
	{ A,          5 },
	{ NOTHING,   20 },
	{ A,          5 },
	{ NOTHING,   20 },
	{ A,          5 },
	{ NOTHING,   40 },

	// Finish with nothing
	//{ NOTHING,   -1 },
};

// Main entry point.
int main(void) {
	// We'll start by performing hardware and peripheral setup.
	SetupHardware();
	// We'll then enable global interrupts for our use.
	GlobalInterruptEnable();
	// Once that's done, we'll enter an infinite loop.
	for (;;)
	{
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);
	// We can then initialize our hardware and peripherals, including the USB stack.

	#ifdef ALERT_WHEN_DONE
	// Both PORTD and PORTB will be used for the optional LED flashing and buzzer.
	#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
	PORTD will toggle when printing is done.
	DDRD  = 0xFF; //Teensy uses PORTD
	PORTD =  0x0;
                  //We'll just flash all pins on both ports since the UNO R3
	DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
	PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
	#endif
	// The USB stack should be initialized last.
	USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
		GetNextReport(&JoystickInputData);
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

typedef enum {
	SYNC_CONTROLLER,
	SYNC_POSITION,
	BREATHE,
	PROCESS,
	CLEANUP,
	DONE
} State_t;
State_t state = SYNC_CONTROLLER;

#define LED_ON (PORTD |= (1<<6))
#define LED_OFF (PORTD &= ~(1<<6))
#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int duration_count = 0;
int portsval = 0;
int next = 0;

// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {
	
	uint8_t PushButton_GetStatus(void) {
		//return 1; !(uint8_t)(PINB & _BV(PB6));
		//static uint8_t btn = _BV(PB6);
		//uint8_t t = PINB;  // read port status
		DDRB &= ~(1<<PB7);
		PORTB |= (1<<PB7);
		if (! (PINB & (1<<PB7))){
			return 0;
		} else {
			return 1;
		}
	}
	uint8_t PushButton_GetStatus2(void) {
		//return 1; !(uint8_t)(PINB & _BV(PB6));
		//static uint8_t btn = _BV(PB6);
		//uint8_t t = PINB;  // read port status
		DDRD &= (1<<PD2);
		PORTD |= (1<<PD2);
		if (! (PIND & (1<<PD2))){
			return 0;
		} else {
			return 1;
		}
	}

	uint8_t JoyStatus_Skip = PushButton_GetStatus();
	uint8_t JoyStatus_Y = PushButton_GetStatus2();

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;

	// Repeat ECHOES times the last report
	if (echoes > 0)
	{
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}

	// States and moves management
	switch (state)
	{

		case SYNC_CONTROLLER:
			state = BREATHE;
			break;

		case SYNC_POSITION:
			bufindex = 0;
			ReportData->Button = 0;
			ReportData->LX = STICK_CENTER;
			ReportData->LY = STICK_CENTER;
			ReportData->RX = STICK_CENTER;
			ReportData->RY = STICK_CENTER;
			ReportData->HAT = HAT_CENTER;
			state = BREATHE;
			break;

		case BREATHE:
			state = DONE;
			break;

		case PROCESS:
			LED_ON;
			switch (step[bufindex].button)
			{
				case UP:
					ReportData->LY = STICK_MIN;				
					break;

				case LEFT:
					ReportData->LX = STICK_MIN;				
					break;

				case DOWN:
					ReportData->LY = STICK_MAX;				
					break;

				case RIGHT:
					ReportData->LX = STICK_MAX;				
					break;

				case PLUS:
					ReportData->Button |= SWITCH_PLUS;
					break;

				case MINUS:
					ReportData->Button |= SWITCH_MINUS;
					break;

				case A:
					ReportData->Button |= SWITCH_A;
					break;

				case B:
					ReportData->Button |= SWITCH_B;
					break;

				case X:
					ReportData->Button |= SWITCH_X;
					break;

				case Y:
					ReportData->Button |= SWITCH_Y;
					break;

				case R:
					ReportData->Button |= SWITCH_R;
					break;

				case L:
					ReportData->Button |= SWITCH_L;
					break;

				case THROW:
					ReportData->LY = STICK_MIN;				
					ReportData->Button |= SWITCH_R;
					break;

				case TRIGGERS:
					ReportData->Button |= SWITCH_L | SWITCH_R;
					break;

				case HOME:
					ReportData->Button |= SWITCH_HOME;
					break;

				default:
					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
					break;
			}

			duration_count++;

			if (duration_count > step[bufindex].duration){
				bufindex++;
				duration_count = 0;				
			}
			if (next >= 0 && next <= 30){
				if (bufindex > (int)( sizeof(step) / sizeof(step[0])) - 1){
					state = CLEANUP;
					bufindex = 3;
					duration_count = 0;

					//state = BREATHE;

					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
				}
			} else {
				if (bufindex > (int)( sizeof(step) / sizeof(step[0])) - 5){
					state = CLEANUP;
					bufindex = 3;
					duration_count = 0;

					//state = BREATHE;

					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
					next = 0;
				}
			}
			break;

		case CLEANUP:
			state = DONE;
			break;

		case DONE:
			LED_OFF;
			state = PROCESS;
			_delay_ms(100);
			JoyStatus_Skip = 0;
			next++;
			break;
			if (JoyStatus_Y == 1){
				ReportData->Button |= SWITCH_A;
			}
		return;
	}

	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;

}