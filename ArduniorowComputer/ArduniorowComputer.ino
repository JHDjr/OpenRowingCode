#include <LiquidCrystal.h>
// http://www.atm.ox.ac.uk/rowing/physics/ergometer.html#section7
// read reed switch from rowing machine.
// based on Lady Ada's "debounced blinking bicycle light" code
// when we use esp8266... https://www.bountysource.com/issues/27619679-request-event-driven-non-blocking-wifi-api
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int switchPin = 6;                          // switch is connected to pin 6
int val;                                    // variable for reading the pin status
int val2;                                   // variable for reading the delayed/debounced status
int buttonState;                            // variable to hold the button state
int count=0;
unsigned long laststatechange;            //time of last switch.
unsigned long timetakenms;
unsigned long instantaneousrpm;
unsigned long nextinstantaneousrpm;
unsigned long startTime = 0;
unsigned long rotations = 0;
unsigned long laststrokerotations = 0;
unsigned long laststroketime = 0;
unsigned long spm = 0; //strokes per minute.
unsigned long difft;
long diffrotations;
float split;
int screenstep=0;

//unsigned long angularmomentum_gm2 = 100;//0.1001 kgm2 == 100.1g/m2 moment of intertia
float I = 100.1/*moment of  interia*/;
float k = 170;/*drag factor*/
float c = 2.8; //The figure used for c is somewhat arbitrary - selected to indicate a 'realistic' boat speed for a given output power.
         //Concept used to quote a figure c=2.8, which, for a 2:00 per 500m split (equivalent to u=500/120=4.17m/s) gives 203 Watts. 
bool Accelerating;


void setup() 
{
 pinMode(switchPin, INPUT_PULLUP);                // Set the switch pin as input
 //Serial.begin(115200);                      // Set up serial communication at 115200bps
 buttonState = digitalRead(switchPin);  // read the initial state
   // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, Lisa!");
}

void loop()
{
  val = digitalRead(switchPin);            // read input value and store it in val                       
       if (val != buttonState)            // the button state has changed!
          {     
             if (val == LOW)                    // check if the button is pressed
                {  //switch passing.
                  //initialise the start time
                  if(startTime == 0) startTime = millis();
                  count++;
//                  if (count==2)//twp subsequent reads in this state.
//                      {
                          timetakenms = millis() - laststatechange;
                          rotations++;
                          //Serial.print("TimeTaken(ms):");
                          //Serial.println(timetakenms);
                          nextinstantaneousrpm = 60000/timetakenms;
                          if(nextinstantaneousrpm >= (instantaneousrpm))
                          {
                              //lcd.print("Acc");        
                              if(!Accelerating)
                              {
                                //beginning drive.

                              }

                              Accelerating = true;    
                          }
                          else
                          {

                              //lcd.print("Dec");
                              if(Accelerating)//previously accelerating
                              { //finished drive
                                diffrotations = rotations - laststrokerotations;
                                difft = millis() - laststroketime;
                                spm = 60000 /(laststroketime - millis());
                                laststrokerotations = rotations;
                                laststroketime = millis();
                                split =  ((float)difft/1000)/ diffrotations*50 ;
                              }
                              if(nextinstantaneousrpm < 300)
                              {//going slow enough that we have time to print.
                                writeNextScreen();
                              }
                              Accelerating = false;
                          }                          
                          instantaneousrpm = nextinstantaneousrpm;
                          //watch out for integer math problems here
                          //Serial.println((nextinstantaneousrpm - instantaneousrpm)/timetakenms);
                          count=0;
                          laststatechange = millis();
                          //delay(5);//wait 5ms
//                      }                     
                } 
                else
                {
                  //lcd.setCursor(0,0);
                  //lcd.print("HIGH"); 
                  count = 0;
                }
                laststatechange=millis();
          }
          buttonState = val;                       // save the new state in our variable
}

void writeNextScreen()
{
  int timemins;
  screenstep++;
  switch(screenstep)
  {//only write a little bit to the screen to save time.
    case 0:
    lcd.clear();
    break;
    case 1:
      lcd.print("S:");
      int splitmin;
      splitmin = (int)split;
      lcd.print(splitmin);//minutes in split.
      lcd.print(":");
      lcd.print((int)(((split-splitmin)*60)));//seconds
    break;
    case 2:
      lcd.print("SPM:");
      lcd.print(spm);
    break;
    case 3:
//      lcd.print("d");
//      lcd.print(rotations);
    break;
    case 4:
//      lcd.print("s");
//      lcd.print(diffrotations);
      break;
    case 5://next lime
      lcd.setCursor(0,1);
      lcd.print("D:");
      lcd.print(rotations);
      break;
    case 6:
//      lcd.print("d");
//      lcd.print(rotations);
break;
    case 7:
//      lcd.print("r");
//      lcd.print(instantaneousrpm);
break;
    case 8:
//      lcd.print("n");
//      lcd.print(nextinstantaneousrpm);
      lcd.print(" AvS:");
      lcd.print(rotations/(millis()-startTime));
      break;
    case 9:
      //lcd.print(" t:");
      timemins = (millis()-startTime)/60000;
      lcd.print(timemins);//total mins
      lcd.print(":");
      lcd.print((int)(millis()-startTime/1000 - timemins*60));//total seconds.
      break;
    default:
      screenstep = -1;//will clear next time, as 1 is added and if 0 will be cleared.
  }


  //                                lcd.print("RPM:");
  //                                lcd.print(instantaneousrpm);
  //                                lcd.print("distance:");
  //                                lcd.print(rotations);
  //Serial.print("RPM:");
  //Serial.println(instantaneousrpm);
    
    
    
    
}

//void resetT()
//{
//  t1 = t2;
//  t2 = millis();
//}

////return the power supplied in Watts - given angular velocity change, and time change.
//float PowerSupplied()
//{
//  float result = I * dw()/(dt()/(float)1000) * dtheta() + k *  dw()*dw() * dtheta();
//  //reset previous angular velocity;
//  w1 = dtheta()/dt();
//  return result;
//}
//
////change in angular velocity
//float dw()
//{
//  return dtheta()/dt() - w1;
//}
//
////change in angular velocity rad/s
//float dtheta()
//{
//  return 2 * 3.1415926535;
//}
//
////time difference in milliseconds
//float dt()
//{
//  return t2-t1;
//}

//k can be displayed on the Concept monitor (units 10-6 N m s2)
//k = - I ( dω / dt ) (1 / ω2 ) = I d(1/ω) / dt
float calculateDragFactor(float deceleration, float angularvelocity)
{
  return -I*deceleration*(1/pow(angularvelocity,2));
}

//(9.2)	u = ( k / c )1/3 ω
float calculateSplit(float angularVelocity)
{
  return pow((k/c),(1/3)) * angularVelocity;
}


