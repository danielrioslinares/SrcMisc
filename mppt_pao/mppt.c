/**
 * Authors:										Daniel Ríos Linares
 * ------------------------------------------------------------------ *
 * 
 * Description:									MPPT code for Arduino
 * 												with simulation support
 * ------------------------------------------------------------------ *
 * 
*/

#define			switchPin	3					// Pin used for switch (MOS gate)
#define			voltagePin	A5					// Pin used for voltage sense
#define			currentPin	A5					// Pin used for current sense

int				D_MAX = 255;					// Maximum duty cycle
int				D_MIN = 0;						// Minimum duty cycle
int				D_STEP = 1;						// Duty cycle step size
int 			D = D_MIN;						// Duty cycle (0-255)

double			voltage = 0;					// Voltage of the panel
double			current = 0;					// Current of the panel

double			this_power = 0;					// Current power
double			last_power = 0;					// Last power
int				last_D = D;						// Last duty cycle
int				WAIT = 450;						// Time to wait until next observation (ms)
int				SAMPLES = 128;					// Number of samples in the sampling
int				this_step = 0;					// Step modified x1 with successStreak
int				successStreak = 1;				// Faster iterations with a counter
bool			wasIncrease = false;			// Last perturbation was increase duty?
bool			wasSuccessful = true;			// Last increase/decrease has been successful?

/**
 *  Setup
*/

void setup() {
	
	TCCR2B = TCCR2B & B11111000 | B00000001;	// Set analogWrite command to 32kHz on pin 3 (Arduino UNO)
	Serial.begin(9600);							// Initialize the serial COM
	pinMode(voltagePin,INPUT);					// Voltage sense pin
	pinMode(currentPin,INPUT);					// Current sense pin
	get_power(SAMPLES);							// Compute all variables (voltage,current and power) once
	
}

/**
 *  Loop
*/

void loop() {
	
	last_power = this_power;					// Remember last measurement
	this_power = get_powerSIM(SAMPLES);			// Get the new measurement
	/*
	 * this_power = get_powerSIM(SAMPLES);		// Overwrite last line with this to simulate
	 */ 
	last_D = D;									// Remember the last duty cycle
	
	perturbAndObserve();						// Algorithm function
	
	analogWrite(switchPin,D);					// Refresh duty cycle
	
	if(successStreak > 3) {
		delay(WAIT/successStreak);				// Wait some time (modified)
	} else {
		delay(WAIT);							// Default wait time
	}
	printVIPD();								// Serial print V,I,P,D
	
}

/**
 *  Algorithm
*/

void perturbAndObserve() {
	// Main algorithm
		
	wasSuccessful = this_power-last_power > 0;	// Test the Success
	
	this_step = D_STEP*successStreak;			// Shrink/magnify the step
		
	if(wasIncrease && wasSuccessful) {			// Duty increase decisions
		increaseDutyCycle(this_step);
	}
	else if(!wasIncrease && wasSuccessful) {
		increaseDutyCycle(-this_step);
	}
	else if(wasIncrease && !wasSuccessful) {
		increaseDutyCycle(-this_step);
		wasIncrease = false;
	} else {
		increaseDutyCycle(this_step);
		wasIncrease = true;
	}
	
	wasSuccessful ? successStreak += 1 : successStreak = 1;

}

void increaseDutyCycle(int d) {
	// Duty cycle modifier function
	D = (D + d >= D_MAX) ? D_MAX :
		(D + d <= D_MIN) ? D_MIN :
		(D + d);
}

double get_power(int n) {
	// Returns the current power with n samples
	voltage = 0;
	current = 0;
	
	for(int i = 0; i < n; i += 1) {				// Compute the n samples
		voltage += analogRead(voltagePin)/n;
		current += analogRead(currentPin)/n;
	}
	return voltage*current;
	
}

double get_powerSIM(int n) {
	// Returns the current power (SIMULATED)
	voltage = (double) D;
	if(D < 100) {
		current = 1;
	} else {
		current = 1-(voltage-100)/155;
	}
	return voltage*current;
}

/**
 *  Auxiliar functions
*/

void printVIPD() {
	// Print format
	Serial.print(voltage);
	Serial.print(" ");
	Serial.print(current);
	Serial.print(" ");
	Serial.print(voltage*current);
	Serial.print(" ");
	Serial.println(last_D);
	
}












