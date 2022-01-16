#include <UTFT.h> 
#include <URTouch.h>
#include <AsyncDelay.h>

// gas sensor definitions

int SensorPort =    A0; // Set MAP sensor input on Analog port 5
double SensorValue = 0; //Save Sensor input Voltage
float ResultkPa = 0; // Save Result in kPa
float ResultBar = 0; // Save Result in Bar
float ResultPsi = 0; // Save Result in Psi


// async delyas
AsyncDelay delay_3;
AsyncDelay delay_2;
AsyncDelay delay_1;


//==== Creating Objects
UTFT    myGLCD(CTE32_R2,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);



//frequency counting variables....
int count = 0;
unsigned long prevMillis , interval=1000;

// period counting variables
int ontime,offtime,duty;
float freq,period,freqave;


// buzzer pin
#define buzzerPin 11

// inflator

#define inflatorPin 12



// led indicators
const int led1 = 8;
const int led2 = 9;
const int led3 = 10;



//==== Defining Variables // fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t Dingbats1_XL[];
extern uint8_t arial_bold[];
extern uint8_t nadianne[];
extern uint8_t Ubuntu[];
extern uint8_t SevenSegmentFull[];
extern uint8_t SevenSegNumFontPlusPlus[];

extern unsigned int bird01[0x41A];
int x, y;

char currentPage, selectedUnit;
float inflatorPressure;


bool inflatorState = 0;
char inflatorAction = '0';

// unit state change
bool unitState =0;


// tire sizes
float tireDiameters[5] = {22.58, 24.98, 26.33, 27.40,30.00};
String carName[4] = {"206", "406", "508", "PK0"};
int tireCount = 0;
float tireDiameter = tireDiameters[0];

// encoder 
const float  encoderDiameterInches = 4.326; //4.186 was the initial however, i considered the thickness of the magnet which is 0.142" measure the actual value;  // measure the actual value
float encoderCircumference;
long rps;// same as frequency
float tireCircumference;
float encoderLinearDistanceRPM;
float tireLinerDistanceIncesRPM;
float truespeedInMilesPerHour;
float trueSpeedInKillometersPerHour;


float revolusionsPerKilloMeter;
float revolusionsPerMile;
float revolusionsPerMilePerMinuit;



float tireRPM;
double tireRPH;
double TireRevPerMile;
double tireLinerDistanceMilesPerHr;


// /////// ////////// ////////// ////////////////  ///////
const byte PulsesPerRevolution = 1;  // Set how many pulses there are on each revolution. Default: 2.
const unsigned long ZeroTimeout = 300000;  // For high response time, a good value would be 100000.
                                           // For reading very low RPM, a good value would be 300000.


// Calibration for smoothing RPM:
const byte numReadings = 2;  // Number of samples for smoothing. The higher, the more smoothing, but it's going to
                             // react slower to changes. 1 = no smoothing. Default: 2.





/////////////
// Variables:
/////////////

volatile unsigned long LastTimeWeMeasured;  
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  
                       
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  
                      
unsigned long FrequencyRaw;  
unsigned long FrequencyReal;  
unsigned long RPM;  
unsigned int PulseCounter = 1;  

unsigned long PeriodSum; 
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured; 
                                    
unsigned long CurrentMicros = micros(); 

unsigned int AmountOfReadings = 1;

unsigned int ZeroDebouncingExtra;  
// Variables for smoothing tachometer:
unsigned long readings[numReadings];  // The input.
unsigned long readIndex;  // The index of the current reading.
unsigned long total;  // The running total.
unsigned long average;  // The RPM value after applying the smoothing.


float correctionFactor = 2.58644859813;
float speedoooo;


void setup() {
  // put your setup code here, to run once:
  // Initial setup
//////////
pinMode(19,INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(19), Pulse_Event,RISING);
delay(1000);


// others pin DDDR for lights, sound and inflator
for(int i = 8; i <=12;i++){
  pinMode(i, OUTPUT);
  
  }


  //////////////
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  
  drawHomeScreen();  // Draws the Home Screen
  currentPage = '0'; // Indicates that we are at Home Screen
  selectedUnit = '0'; // Indicates the selected unit for the first example, cms or inches




Serial.begin(9600);
 
 
 
 
 delay_3.start(1500, AsyncDelay::MILLIS); 
 delay_2.start(500, AsyncDelay::MILLIS); 
 delay_1.start(1000, AsyncDelay::MILLIS); 




       if(inflatorAction == '0'){

              digitalWrite(led1,1);
              digitalWrite(led3,0);
            
            }// end of if inflator action 

   
   
   
   
   }// end of the setup fucntion



   



void loop() {
  // put your main code here, to run repeatedly:
  
  if(currentPage == '0'){
    if(myTouch.dataAvailable()){
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
       //ledIntro();

      // if the dynamometer button is pressed.
      
      if((x>=35) && (x<=285) && (y>=90) && (y<=130)){
        touchSoundI();
        drawFrame(35,90,285,130);
        currentPage = '1'; // Indicates that we are the first example
        myGLCD.clrScr(); // Clears the screen
        dynamometerPage(); //
        
        
        }// end of if

       // If the sensor check button is pressed 
          
      if ((x>=35) && (x<=285) && (y>=140) && (y<=180)) {
        touchSoundI();
        drawFrame(35, 140, 285, 180);
        currentPage = '2';
        myGLCD.clrScr();
        sensorCheckPage();
        
      }  

      // If the about button is pressed 
          
      if ((x>=35) && (x<=285) && (y>=190) && (y<=300)) {
        touchSoundI();
        drawFrame(35, 140, 285, 180);
        currentPage = '3';
        myGLCD.clrScr();
        aboutPage();
        
      }  
      

      }// end of inner if
    
    }// end of the if





if (currentPage == '1') {    
      getFrequency(); // 
      if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
      
       

        // If we press the Back Button
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          //touchSound();
          drawFrame(10, 10, 60, 36);
          currentPage = '0'; // Indicates we are at home screen
          selectedUnit = '0';
          myGLCD.clrScr();
          drawHomeScreen(); // Draws the home screen
          
            
            }
      
      
      // if the speed pane is presssed
      if((x>=85) && (x<=310) && (y>=110) && (y<=176)){
          
        touchSound();
        unitState = !unitState;
            
              }// end of is the speed pane is pressed
         
      
            if(unitState == 0){selectedUnit = '0';}
            if(unitState == 1){selectedUnit = '1';}
      
      
       
       // if the inflator is pressed

            if((x>=10) && (x<=73) && (y>=93) && (y<=176)){
          
        touchSoundI();
        inflatorState = !inflatorState;
            
              }// end of is the speed pane is pressed
                     if(inflatorState == 1){inflatorAction = '1';}
                
                     if(inflatorState == 0){inflatorAction = '0';}
      
       


           
    
         // if the - is presses
           if((x>=135)&&(x<=170) && (y>=60) && (y<=80)){
            tireCount--;
            if(tireCount<1)tireCount = 0;
            touchSoundM();
              }
           
             // if the + is presses
              if((x>=175)&&(x<=210) && (y>=60) && (y<=80)){
            tireCount++;
             if(tireCount>=sizeof(tireDiameters)/sizeof(float))tireCount = 0;
              touchSoundP();
               }

          tireDiameter = tireDiameters[tireCount];

           
           
           }// end of if touch data is avaiilable for page 1
  
  
     
     //Serial.println(tireDiameter);
     
     }// end of if the current page is 1


//  // when the current page is 2
  if(currentPage == '2'){
       //getPressure();
       //getFrequency();

          myGLCD.printNumI(digitalRead(19),150,80);
           myGLCD.printNumI(getPressure(),150,50); 
       
       if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
      
       
       

           
        // If we press the Back Button
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          //touchSound();
          drawFrame(10, 10, 60, 36);
          currentPage = '0'; // Indicates we are at home screen
          selectedUnit = '0';
          myGLCD.clrScr();
          drawHomeScreen(); // Draws the home screen
          
            
            }


         
       }// end of if touched

    
        
    
    }// end of if the current page is 2 
        

  // when the current page is 3
  if(currentPage == '3'){
       if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
      
       

        // If we press the Back Button
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          //touchSound();
          drawFrame(10, 10, 60, 36);
          currentPage = '0'; // Indicates we are at home screen
          selectedUnit = '0';
          myGLCD.clrScr();
          drawHomeScreen(); // Draws the home screen
          
            
            }
       }


    
    }// end of if the current page is 3

    
  

    
    }// end of the loop fucntion


void drawHomeScreen() {
  // Title
  myGLCD.fillScr(6,0,32);
  myGLCD.setBackColor(6,0,32); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("SMART DYNAMOMETER", CENTER,10); 
    //myGLCD.setFont(BigFont);
    //myGLCD.setBackColor(6,0,32);
  //myGLCD.print("(PAN)", CENTER, 41);
  // Prints the string on the screen
  
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SmallFont); // Sets the font to small
  myGLCD.setBackColor(6,0,32);
  myGLCD.print("Designed by Syncronia-systems", CENTER,50); // Prints the string

  
  // Button - dynamometer
  
  myGLCD.setColor(100, 155, 203); // Sets green color
  myGLCD.fillRoundRect (35, 90, 285, 130); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (35, 90, 285, 130); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(nadianne); // Sets the font to big
  myGLCD.setBackColor(100, 155, 203); // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("Measure Speed", CENTER, 102); // Prints the string



  
  
  // Button - sensor check
  myGLCD.setColor(100, 155, 203); 
  myGLCD.fillRoundRect (35, 140, 285, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 140, 285, 180);
  myGLCD.setFont(nadianne);
 myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("Check Sensors", CENTER, 152);



  // Button - about
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (35, 190, 285, 230);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 190, 285, 230);
  myGLCD.setFont(nadianne);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("About Device", CENTER, 202);


 
}


// Highlights the button when pressed
void drawFrame(int x1, int y1, int x2, int y2) {
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (x1, y1, x2, y2);
}




//===== getFrequency - Custom Function and isr for frequency count



  
float getPressure(){
  
     SensorValue = analogRead(SensorPort);
     ResultkPa = (SensorValue*(.00488)/(.022)+20);
     ResultBar = (ResultkPa * 0.01) - 1.0172; //multiply (1 kPa x 0.01 bar) and deduct atmospheric pressure
     ResultPsi = ResultBar*14.5038;
     //Serial.println(ResultBar);
     //Serial.println(ResultBar*14.5038);
  
ResultkPa = 0;
ResultBar = 0;
SensorValue = 0;

  return ResultPsi;
  
  }// end of fucntion
float getHall(){
  return digitalRead(19);
  }// end of get hall
void isr(){
  
   count++;
   
  }// end of function






// sound

void touchSound(){
 tone(buzzerPin,3000,80); 
  }// end of sound fucnction

void touchSoundP(){
  tone(buzzerPin,3000,50); 
  }


void touchSoundM(){
  tone(buzzerPin,3000,50); 
  }

void touchSoundI(){
  tone(buzzerPin,3000,60); 
  }



void getFrequency() {

  
   
// get the pressue ever 800 miilis seconds

  if(delay_2.isExpired()){
  inflatorPressure = getPressure();
   delay_2.repeat();
   }
 



float npps;float freq;



getRPM();

freq = average;
encoderCircumference = PI*encoderDiameterInches;
tireCircumference = PI*tireDiameter;

//tireRPM = freq/(tireCircumference/encoderCircumference);

tireRPM = freq/(tireDiameter/encoderDiameterInches);
tireRPH  = tireRPM*60;
TireRevPerMile = (63360/tireCircumference);
tireLinerDistanceMilesPerHr = tireRPH/TireRevPerMile;



//Serial.println( String(TireRevPerMile) + " " + String(tireRPH) + " " + String(tireRPH/TireRevPerMile));

truespeedInMilesPerHour  = (tireLinerDistanceMilesPerHr)*correctionFactor; 

truespeedInMilesPerHour  = truespeedInMilesPerHour;
trueSpeedInKillometersPerHour =  truespeedInMilesPerHour*1.60934;



       // corrected version ** you have to consider the wheel hub and the toothed ring..... not the tire hence
//truespeedInMilesPerHour       =  (encoderLinearDistanceRPM *0.00094697)*2.6;
//trueSpeedInKillometersPerHour =  (encoderLinearDistanceRPM *0.001524)*2.6;
 

 
 //char speedKph[3];
 //dtostrf(trueSpeedInKillometersPerHour,3,1,speedKph);
 



// kph unit
  
  if (selectedUnit == '0') {
    myGLCD.setFont(SevenSegNumFontPlusPlus);
    myGLCD.setColor(255,215,0);
    
    
    myGLCD.setBackColor(0, 0, 0);
    // calculation here of kpH
   
    
    //myGLCD.printNumI(trueSpeedInKillometersPerHour ,90, 120, 3,'0');
   myGLCD.printNumF(trueSpeedInKillometersPerHour,1,90,120);//1.125
   if(trueSpeedInKillometersPerHour<=0){
     //myGLCD.fillRect(218,120,250,170);
    myGLCD.print("0.000",90,120);
   
  
    }
   
    //myGLCD.print(speedKph,90,120);
    myGLCD.setFont(BigFont);
    myGLCD.print("KPH", 260, 160);
  
  }// end of if unit is selected

  


  

// mhp unit
 
  if (selectedUnit == '1') {
      
    myGLCD.setFont(SevenSegNumFontPlusPlus);
    myGLCD.setColor(0, 255, 0);
    myGLCD.setBackColor(0, 0, 0);
    // calculation here of mph
    
    myGLCD.printNumF(truespeedInMilesPerHour,1,90, 120);
       if(truespeedInMilesPerHour<=0){
    myGLCD.print("0.000",90,120);
  
 
  
    }
    myGLCD.setFont(BigFont);
    myGLCD.print("MPH", 260, 160);
  } 



// inflator action for real

   if(inflatorAction == '1'){
    analogWrite(inflatorPin,255);
    digitalWrite(led1,1);
    digitalWrite(led3,0);   
    myGLCD.setFont(BigFont);  
    myGLCD.print("I",35,130);
    
    }// end of if

   if(inflatorAction == '0'){
    analogWrite(inflatorPin,0);
    digitalWrite(led1,0);
    digitalWrite(led3,1);
    myGLCD.setFont(BigFont);
    myGLCD.print("0",35,130);
    
    }// end of if




// units display

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.printNumF(freq,1,87, 183);
  myGLCD.print("RPM",175,183);

  if(freq<=0){
      myGLCD.printNumF(0.000,4,87, 183);
    }// end of the if
  
  myGLCD.printNumF(ResultPsi,2,87,208);
  myGLCD.print("psi",175,208);

   
    //tire size placement
   
   myGLCD.printNumF(tireDiameter,2, 90, 64);
   
    
     
     if(delay_1.isExpired()){
          // inflator color change //
    if(inflatorPressure <=-2){
        // warning lights
     myGLCD.setColor(3,220,100);
   myGLCD.fillRect (250, 205, 315, 225);
   myGLCD.setColor(255, 255, 255);
    
     myGLCD.setBackColor(3,220,15);
     myGLCD.setFont(SmallFont);
     myGLCD.print("SAFE",254,210);
    
    }// end of if

   if(inflatorPressure>=-1){
        myGLCD.setColor(255,3,15);
   myGLCD.fillRect (250, 205, 315, 225);
   myGLCD.setColor(255, 255, 255);
   myGLCD.setBackColor(255,3,15);
   myGLCD.setFont(SmallFont);
   myGLCD.print("WARNING",254,210);
     }

     delay_1.repeat();
      
      }






  
    delay(10);

  
  
    // refeshed

  
   
   
   }// end of the get frequaneyc fuction







// the pages 

void dynamometerPage(){
  // the background color
   myGLCD.setBackColor(255, 255, 255);
   myGLCD.fillScr(6,0,32);
 
   
   
  // this is the back button
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<-", 18, 15);
  
  //title
  myGLCD.setBackColor(6,0,36);
  myGLCD.setFont(nadianne);
  myGLCD.print("Dynamometer",95,10); 
  
  
  
  
  // select unit buttons km/hr
  myGLCD.setFont(SmallFont);



     // tire selector
 // title
   myGLCD.setFont(SmallFont);
   myGLCD.print("Tire Diameter\"",85,40);  

   


   

   

    //transperanet pane FOR SPEED
   myGLCD.setColor(66,66,66);
   myGLCD.fillRect (85, 93, 310, 176);
   myGLCD.setColor(255, 255, 255);




    //transperanet pane FOR freqenecy
   myGLCD.setColor(66,66,66);
   myGLCD.fillRect (85, 180, 200, 200);
   myGLCD.setColor(255, 255, 255);

 



    //transperanet pane FOR pressure
   myGLCD.setColor(66,66,66);
   myGLCD.fillRect (85, 204, 200, 224);
   myGLCD.setColor(255, 255, 255);



  // tire selector
    //transperanet pane FOR tire
   myGLCD.setColor(66,66,66);
   myGLCD.fillRect (85, 60, 130, 80);
   myGLCD.setColor(255, 255, 255);


    // buttons for tire
 //-   
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRect (135, 60, 170, 80);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (135, 60, 170, 80);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("-", 145, 62);

//+
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRect (175, 60, 210, 80);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (175, 60, 210, 80);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("+", 185, 62);
  

 // button for inflator
   myGLCD.setColor(100, 155, 215);;
   myGLCD.fillRoundRect (10, 93, 74, 176);
   myGLCD.setColor(255,255 , 255);
   myGLCD.drawRoundRect (10, 93, 74, 176);
   myGLCD.setBackColor(100, 155, 215);
   myGLCD.setFont(BigFont);
   
   myGLCD.print("0",35,130);
  
   
  

    
  }// end of the fucntion


void sensorCheckPage(){
  // the background color
   myGLCD.setBackColor(255, 255, 255);
   myGLCD.fillScr(6,0,32);
 
   
   
  // this is the back button
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<-", 18, 15);
  
  //title
  myGLCD.setBackColor(6,0,36);
  myGLCD.setFont(nadianne);
  myGLCD.print("Sensor Check",95,10); 
  


  // diplayPressure and rpm
   myGLCD.setFont(SmallFont);
  myGLCD.print("Pressure sensor : ",10,50);
  myGLCD.print("Hall-ef  sensor :",10,80);
  
  
  
  

    
  }// end of the fucntion
 



void aboutPage(){
  // the background color
   myGLCD.setBackColor(255, 255, 255);
   myGLCD.fillScr(6,0,32);
 
   
   
  // this is the back button
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<-", 18, 15);
  
  //title
  myGLCD.setBackColor(6,0,36);
  myGLCD.setFont(nadianne);
  myGLCD.print("About Device",95,10); 
  


     myGLCD.setFont(SmallFont);
  myGLCD.print("This device measures vehicles' speed",CENTER,50);
  myGLCD.print("designed by ",CENTER,70);
  myGLCD.print("syncronia-systems",CENTER,90);
  myGLCD.print("contact us @:",CENTER,120);
  myGLCD.print("email:syncroniasystems@gmail.com",CENTER,140);
  myGLCD.print("phoneNo: 09063699411",CENTER,160);
 
  

    
  }// end of the fucntion






void ledIntro(){
  digitalWrite(led2,HIGH);
  delay(500);
  digitalWrite(led2,LOW);
  delay(500);
  }//


void getRPM(){

  // The following is going to store the two values that might change in the middle of the cycle.
  // We are going to do math and functions with those values and they can create glitches if they change in the
  // middle of the cycle.
  LastTimeCycleMeasure = LastTimeWeMeasured;  // Store the LastTimeWeMeasured in a variable.
  CurrentMicros = micros();  // Store the micros() in a variable.





  // CurrentMicros should always be higher than LastTimeWeMeasured, but in rare occasions that's not true.
  // I'm not sure why this happens, but my solution is to compare both and if CurrentMicros is lower than
  // LastTimeCycleMeasure I set it as the CurrentMicros.
  // The need of fixing this is that we later use this information to see if pulses stopped.
  if(CurrentMicros < LastTimeCycleMeasure)
  {
    LastTimeCycleMeasure = CurrentMicros;
  }





  // Calculate the frequency:
  FrequencyRaw = 10000000000 / PeriodAverage;  // Calculate the frequency using the period between pulses.


  

  
  // Detect if pulses stopped or frequency is too low, so we can show 0 Frequency:
  if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
  {  // If the pulses are too far apart that we reached the timeout for zero:
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;  // Change the threshold a little so it doesn't bounce.
  }
  else
  {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }





  FrequencyReal = FrequencyRaw / 10000;  // Get frequency without decimals.
                                          // This is not used to calculate RPM but we remove the decimals just in case
                                          // you want to print it.





  // Calculate the RPM:
  RPM = FrequencyRaw / PulsesPerRevolution * 60;  // Frequency divided by amount of pulses per revolution multiply by
                                                  // 60 seconds to get minutes.
  RPM = RPM / 10000;  // Remove the decimals.





  // Smoothing RPM:
  total = total - readings[readIndex];  // Advance to the next position in the array.
  readings[readIndex] = RPM;  // Takes the value that we are going to smooth.
  total = total + readings[readIndex];  // Add the reading to the total.
  readIndex = readIndex + 1;  // Advance to the next position in the array.

  if (readIndex >= numReadings)  // If we're at the end of the array:
  {
    readIndex = 0;  // Reset array index.
  }
  
  // Calculate the average:
  average = total / numReadings;  // The average value it's the smoothed result.


  Serial.print("Period: ");
  Serial.print(PeriodBetweenPulses);
  Serial.print("\tReadings: ");
  Serial.print(AmountOfReadings);
  Serial.print("\tFrequency: ");
  Serial.print(FrequencyReal);
  Serial.print("\tRPM: ");
  Serial.print(RPM);
  Serial.print("\tTachometer: ");
  Serial.println(average);
  
   }// end of the get rpm
  
void Pulse_Event(){

  PeriodBetweenPulses = micros() - LastTimeWeMeasured;  

  LastTimeWeMeasured = micros();  





  if(PulseCounter >= AmountOfReadings)  
  {
    PeriodAverage = PeriodSum / AmountOfReadings;  
                                                   
    PulseCounter = 1;  
    PeriodSum = PeriodBetweenPulses;
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);  
    AmountOfReadings = RemapedAmountOfReadings;  
  }
  else
  {
    PulseCounter++;  // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses;  // Add the periods so later we can average.
  }

}  // End of Pulse_Event.
