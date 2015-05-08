#include <Wire.h>
//#include <SoftwareSerial.h>

//**********************************************************************************************//
//					DEFINES						 	//
//**********************************************************************************************//

#define		SLAVE_ADDR	0x68		//Slave address of Accelerometer//For MPU6050 its 0x68
#define		ACCEL_REG_XL	0x3B		//Accelerometer axes register which data will be stored//For MPU6050 Its 0x3B
#define		ACCEL_REG_XH	0x3C		//Needs to be changed which depends on Accelerometer
#define		ACCEL_REG_YL	0x3D
#define		ACCEL_REG_YH	0x3E
#define		ACCEL_REG_ZL	0x40
#define		ACCEL_REG_ZH	0x41
#define		MAX_ANGLE	180		//Motor Max Rotation Angle
#define		MIN_ANGLE	0		//Motor Min Rotation Angle
#define		ANGLE_INCRE	1		//decides the incrementation between the 2 consecutive angles
#define		OPEN_ANGLE	120		//Close angle for the door lock
#define		CLOSE_ANGLE	0		//Open angle for the door lock

//**********************************************************************************************//
//					GLOBAL VARIABLES				 	//
//**********************************************************************************************//

volatile int angle=0;			//Stores the angle calculated from the received data
//SoftwareSerial BLE(2,3);

//**********************************************************************************************//
//				   FUNCTIONS DECLARATIONS				 	//
//**********************************************************************************************//
void accel_init();			//Accelerometer initialization//Depends on the type of accelerometer
unsigned int getData(int reg_addr);	//returns data from 2 set of consecutive registers// Can be easily made for 1 register
int getAngle();				//Gives the angle rotation of motor
char getPosition();			//Gives the position whether [Closed:Open:Intermediate]
void jamAlert();			//Debug purpose to give the jamming alert//currently in progress

//**********************************************************************************************//
//					FUNCTIONS					 	//
//**********************************************************************************************//

void setup(){
	accel_init();       
}

/*
	Initialialization of TWI [Two wire Interface](I2C) for the Accelerometer so that the I2C pins
	gets activated.
*/
void accel_init(){
	Wire.begin();
	/* Code to set some initial registers of accelerometer
	   before starting the actual transmission */
	/* e.g setting the power management setting in MPU 6050
	   and also selecting the desired range of accelerometer 
	   which is very essestial part as it determines the
	   resolution of our angle */ 
}

/* 	Function:	The main aim of this function is to give the data stored in particular registers
	Parameter:	Take the register address from which data has to extracted
   	Retuns:		It returns the data stored in 2 consecutive addresses of accelerometer
*/
unsigned int getData(int reg_addr){
	unsigned int data=0;
	Wire.beginTransmission(SLAVE_ADDR); 	//starting the communication again
  	Wire.write(reg_addr);  			//start with this register address (its the first data register
  	Wire.endTransmission(false);  		//continue to read data
  	Wire.requestFrom(SLAVE_ADDR,2,true);  	//request the slave to send the 2 byte data
  	data=Wire.read()<<8|Wire.read();  	//data is 16 bit data .the data is automatically read sequently

//Need to add extra loop and code for error codes returned by  Wire.endTransmission//
	Wire.endTransmission(); 
return data;
}

/*
	Function: 	map(value, fromLow, fromHigh, toLow, toHigh) 
	Description:	Re-maps a number from one range to another.That is,a value of fromLow would get mapped 
			to toLow, a value of fromHigh to toHigh, values in-between to values in-between.
*/
int getAngle(){
	return map(getData(ACCEL_REG_XL),0,32767,0,180);
}

/*
	Returns the the position of the (open, closed, unknown) of the knob
*/
char getPosition(){ 
  char pos = 'j';
  int angle=getAngle();
  if (angle <= CLOSE_ANGLE) {pos = 'c';}
  else if (angle >= OPEN_ANGLE) {pos = 'o';}
  //if (angle < closedPot()) {pos = 'c';}
  //else if (angle > openPot()) {pos = 'o';}
  else {pos = 'm';}
  return pos;
}

//I have commented out these functions as I have declared the <OPEN_ANGLE> and <CLOSE_ANGLE> in <#define>
//int closedPot(){return (closedAngle+potBuffer*turnDirection);}// returns the angle of the lock for it to be considered "closed"
//int openPot(){return (openAngle-potBuffer*turnDirection);}	// ditto

/*
	Prints the Postion of the Motor. Tells whether the lock is [closed:open:intermediate] position based on the angle readings.
*/

void givePosition(){
	BLE.println(getPosition());				// sends the position (of the (open, closed, unknown) to bluetooth
} 		

/*	
	Prints the angle of the motor rotated. It uses the function <getAngle()>
*/ 
void giveAngle(){
	BLE.println(getAngle());
}

/*
	Its currently in development phase. Basically it will be used to determine whether the lock is jamed or not. There is some 
	thinking required to design the function. It requires the tracking of the previous readings as well as change of angle with
	respect to time.
*/
void jamAlert(){
	char text='j'; BLE.println(text);			// sends a "j" to BLE.  right now it's just for debugging purposes.
} 		

//**********************************************************************************************//
//					    LOOP FOR TESTING				 	//
//**********************************************************************************************//

void loop(){
	char position;
	position=getPosition();
	switch(position){
		case 'm':{
				BLE.println("Unknown Position");
				break;
			}
		case 'c':{
				BLE.println("Door Closed");
				break;
			}
		case 'o':{
				BLE.println("Door Open");
				break;
			}
		default:	BLE.println("Error");
	}
	BLE.print("ACCEL DATA: ");
	BLE.println(angle);	
	
	delay(100);	
}































// =========== accelerometer ===============

/*
          int getAngle(){// returns angle relative to positive x-axis, where z axis is pointing out of the door and positive y-axis is pointing up (away from gravity)
            int angle = 42;
            return angle;}
*/

//out dated

//===========Pot stuff====================

/*
          
          int potScaling = 360/1000;
          int potOffset = 0;// in degrees
          // for calibrating the analog values we get from the potentiometer to the actual angle of the lock switcher
          
          int potPin = A0; // the potentiometer is plugged into the analog 0 pin
          
          int getAngle(){
            int sensorValue = analogRead(potPin);
            return (sensorValue)*potScaling+potOffset;
          }

*/
