
#include <SPI.h>
#include <Metro.h>
#include <EEPROM.h>
#include <math.h>



#define CS 10
#define SW1 8
#define LED 9

#define INT1 A0

long timeCounter;
long switchCounter1;
byte mode;
byte switchStatus1;
long lowByteTime;
long highByteTime;
int addr;
Metro metro10 = Metro(10);          //10mS second time counter
int sensorLowByte;
int sensorHighByte;
int value;
byte ledStatus;
float force;
float VARIABLE_COMPUTED_PITCHANGLE;
float radian;
float force_abs;
float pitchError;
float acceleration;
void setup()
{
  Serial.begin(9600);
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  
  pinMode(CS, OUTPUT);      //chip select 
  pinMode(SW1, INPUT_PULLUP);      //Tact Switch
  pinMode(LED, OUTPUT);      //LED
  pinMode(A0, INPUT);          //interrupt 1
  
  
  
  digitalWrite(CS,HIGH);
  delay(1000);
 
      digitalWrite(CS,LOW);
      SPI.transfer(0x27);                  
      SPI.transfer(0x44);                //only x axis is enabled for activity
      digitalWrite(CS,HIGH);
      delayMicroseconds(10);
      
      digitalWrite(CS,LOW);              //enable measure 
      SPI.transfer(0x2D);
      SPI.transfer(0x08);
      digitalWrite(CS,HIGH);
      delayMicroseconds(10);
      
      digitalWrite(CS,LOW);              //data rate = 1600 hz
      SPI.transfer(0x0F);
      SPI.transfer(0x08);
      digitalWrite(CS,HIGH);
      delayMicroseconds(10);
      
      digitalWrite(CS,LOW);              //fifo off
      SPI.transfer(0x38);
      SPI.transfer(0x00);
      digitalWrite(CS,HIGH);
      delayMicroseconds(10);
      
      digitalWrite(CS,LOW);              //full resolution 4mg/LSB +/-8 g scale
      SPI.transfer(0x31);
      SPI.transfer(0x0A);
      digitalWrite(CS,HIGH);         


}

void loop()
{

  //***************************** 10mS time base *************************
  if (metro10.check() == 1)
     {
       timeCounter++;
       switchCounter1 ++;
      
       if (digitalRead(SW1) == 0 && switchStatus1 == 0)
       {
         switchStatus1 = 1;
         if (ledStatus == 1)
           {
             digitalWrite(LED,LOW);
             ledStatus = 0;
           }
          else
           {
             digitalWrite(LED,HIGH);
             ledStatus = 1;
           } 
       }
       
       if (digitalRead(SW1) == 0 && switchStatus1 == 0 && mode != 0)
       {
         
         switchStatus1 = 1;
       }
       

      metro10.reset();
     }
  //******************** read accelerometer ************************************   
  digitalWrite(CS,LOW);
  SPI.transfer(0xF2);
  sensorLowByte=SPI.transfer(0x0);
  sensorHighByte=SPI.transfer(0x0);
  digitalWrite(CS,HIGH);
  delayMicroseconds(5);
  //******************************** calculate force *********************
//This loop frequency needs to be determined by VARIABLE_SAMPLE_RATE
  value=(sensorHighByte * 256)+sensorLowByte;   //combine registers to make value
  force = value * 0.004;        // each bit is 0.004g
//force then needs to be pushed into the VARIABLE_QUEUE_DECELERATION
//initialize a counter and increment (forloop?).  Counter should loop around every N samples; N determined by VARIABLE_STRAIGHTANDLEVEL_DECISION_RATE
//when counter = N, loop:
// If Range(VARIABLE_QUEUE_DECELERATION) <= CONSTANT_STRAIGHTANDLEVELRANGE
//Then:
//VARIABLE_COMPUTED_PITCHANGLE = arcsine(mean(VARIABLE_QUEUE_DECELERATION)/CONSTANT_GRAVITY)
//ACTUATOR_SL(PITCHCALIBRATED_SIGNAL)











  force_abs = abs(force);        //take absolute value of force to calculate theta      
  radian = asin(force_abs);      //math function generates value in radians
  VARIABLE_COMPUTED_PITCHANGLE = radian * 57.29578;      //convert radians to angle for viewing
  Serial.print("Measured Force: ");Serial.print(force,4); Serial.print(" Angle Tilted: ");Serial.println(VARIABLE_COMPUTED_PITCHANGLE,4); 
  pitchError = force - (sin(radian));
  acceleration = pitchError * cos(radian);
  Serial.print("Pitch Error");Serial.print(pitchError,4); Serial.print(" Acceleration ");Serial.println(acceleration,4); 
  Serial.println();
  delay(250);
 //******************************************* switch monitoring ********************    
     if (digitalRead(SW1) == 1)
     {
       switchStatus1 = 0;
       switchCounter1 = 0;
     }

}



