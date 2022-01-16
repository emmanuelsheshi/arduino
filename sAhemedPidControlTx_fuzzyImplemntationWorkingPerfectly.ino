// definition of variables
const int genFreqSensePin   = 3;
const int mainFreqSensePin  = 2;
int counterMains, counterGen;

float genFreq, mainsFreq;
float genFreqAve =0, mainsFreqAve;

unsigned long currentTime, previousTime, interval = 2000, prevMill2, interval2=7000;


// load shedding pins
  

// interrupts
void frequencycountingMains();
void frequencycountingGen();


//LCD display part

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//pid delay times send 
int maximum_firing_delay = 7400;
int maxLim = 7400;    //longest delay slowest speed
int minLim = 10;    //shortest delay fastest speed

// fuzzy definitions


// this is the beginning of the fuzzy definition
#include <Fuzzy.h>
Fuzzy *fuzzy = new Fuzzy();


// FuzzyInput // for the wind
FuzzySet *veryLowFreq = new FuzzySet(0, 47.5, 48.5, 49.0);
FuzzySet *optimalFreq = new FuzzySet(46, 47.5, 49.5,50.05);
FuzzySet *highFreq =    new FuzzySet(49.5, 49.8, 51.5, 51.5);




// FuzzyOutput pump speed
FuzzySet *fast = new FuzzySet(10, 12.5, 17.5,20);
FuzzySet *slow = new FuzzySet(20, 3000, 5000, 7400);
FuzzySet *stall = new FuzzySet(7100, 7300, 7300, 7400);




// power source
bool powerSource;


struct STRUCT {
  byte  hb;
  byte  lb;
  byte  ps;
} testStruct;


// function prototypes
void display();
void fuzzyRuleDefinations();



void setup() {
   

  
// uart
Serial.begin(9600);

// lcd setup

  lcd.init();
  lcd.backlight();
  lcd.clear();
  display();

// uart setup
 
  Serial.begin(9600);

//pinSetup
  
  pinMode(2,INPUT);
  pinMode(3,INPUT);

// interupts for counting pulses
  
  attachInterrupt(digitalPinToInterrupt(3), frequencycountingGen,   RISING);
  attachInterrupt(digitalPinToInterrupt(2), frequencycountingMains, RISING);
  

//fuzzy setup

  // FuzzyInput for the frequency
    FuzzyInput *frequency = new FuzzyInput(1); 
    frequency->addFuzzySet(veryLowFreq);
    frequency->addFuzzySet(optimalFreq);
    frequency->addFuzzySet(highFreq);
  
  fuzzy->addFuzzyInput(frequency);


  // FuzzyOutput // for pulseDelay
  FuzzyOutput *pulseDelay = new FuzzyOutput(1);
  pulseDelay->addFuzzySet(fast);
  pulseDelay->addFuzzySet(slow);
  pulseDelay->addFuzzySet(stall);

  
  fuzzy->addFuzzyOutput(pulseDelay);
  
  fuzzyRuleDefinations();


  
  } // End of the setup function





void loop() {
      
    
   // Serial.println();
    
    currentTime = millis();
     
     if(currentTime-previousTime >= interval){    
        genFreq   = constrain((counterGen/4.07),0,50.30);  //-0.07;
        mainsFreq = constrain((counterMains/4.07),0,50.30);   //-0.07;
        counterGen = 0;
        counterMains = 0;
        previousTime = currentTime;

       
        }// end of if

     
    for(int i = 0 ; i <=10; i++){
       genFreqAve += genFreq;
       mainsFreqAve += mainsFreq;
     
    
         }

     genFreqAve/=12;
     mainsFreqAve/=12;

     lcd.setCursor(4,1);
     lcd.print(genFreqAve);
     lcd.setCursor(4,2);
     lcd.print(mainsFreqAve);

       
 // fuzzy calculation here
   int measuredFreq = genFreqAve;
    
  //Serial.println(measuredFreq);
      

 // this is where you set the inputs
  fuzzy->setInput(1, measuredFreq);
  fuzzy->fuzzify();// this function helps you fuzzify your inputs...

  
  // this is the output of the fuzzy and you de-fuzify here!!!

     float delayTimeFuzzy = fuzzy->defuzzify(1);
     float OutputFuzzy = delayTimeFuzzy;
     
     //float limitOutputFuzzy = constrain(OutputFuzzy, minLim, maxLim);
    
    // Serial.println(OutputFuzzy);
     lcd.setCursor(14,3);
     lcd.print(OutputFuzzy,3);
     lcd.setCursor(0,0);
     lcd.print("   ");
     
     // change over code 
     if (genFreqAve > mainsFreqAve-1.5){
      
      powerSource = 1;
      lcd.setCursor(17,1);
      lcd.print("GEN");
      
      
      }

      
     else {
      powerSource  = 0;
      lcd.setCursor(17,1);
      lcd.print("MNS");
      
     
     }
     
   
    
  testStruct.hb = highByte(int(limitOutputFuzzy));
  testStruct.lb = lowByte(int(limitOutputFuzzy));
  testStruct.ps = powerSource;
  Serial.write((const uint8_t *)&testStruct, sizeof(testStruct));   //
  
     
   delay(1000);
    
    
    }// end of the loop




// custum fucntions //

void frequencycountingGen(){


 counterGen++;
} //end of the fucntion


void frequencycountingMains(){
  

 counterMains++;

}// end of the fucnction



// lcd setup
void display(){
lcd.setCursor(4,0);  //  1ST ROW 
lcd.printstr("FREQ");
lcd.setCursor(10, 0);
lcd.printstr("VOLTS");
lcd.setCursor(17, 0);
lcd.printstr("ACT");   

lcd.setCursor(0, 1);  // 2ND COL
lcd.printstr("GEN");
lcd.setCursor(0, 2);
lcd.printstr("MNS");
lcd.setCursor(0, 3);
lcd.printstr("POWER(KW)");


lcd.setCursor(15,2);
lcd.printstr("FUZZY"); //OR FUZZY


}// end of the display function




void fuzzyRuleDefinations(){


  //////////////// Rules and regulations of the fuzzy system //////////////////

  
  // rule 1 if frequency is very low pulse is fast

  // the antecedent
  FuzzyRuleAntecedent *ifFreqIsVeryLow = new FuzzyRuleAntecedent();
  
  // the consequence
  FuzzyRuleConsequent *thenPulseIsFast = new FuzzyRuleConsequent();
  thenPulseIsFast->addOutput(fast);
  
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1,ifFreqIsVeryLow, thenPulseIsFast);
  
  // add the rule
  fuzzy->addFuzzyRule(fuzzyRule1);
  

   // rule 2 if frequency is optimal low pulse is slow

  // the antecedent
  FuzzyRuleAntecedent *ifFreqIsVeryOptimal = new FuzzyRuleAntecedent();
  
  // the consequence
  FuzzyRuleConsequent *thenPulseIsSlow = new FuzzyRuleConsequent();
  thenPulseIsSlow->addOutput(slow);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2,ifFreqIsVeryOptimal, thenPulseIsSlow);
  
  // add the rule
  fuzzy->addFuzzyRule(fuzzyRule2);
  
  

   
   // rule 3 if frequency is optimal low pulse is slow

  // the antecedent
  FuzzyRuleAntecedent *ifFreqIsVeryHigh = new FuzzyRuleAntecedent();
  
  // the consequence
  FuzzyRuleConsequent *stallPulse = new FuzzyRuleConsequent();
  stallPulse->addOutput(stall);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3,ifFreqIsVeryHigh, stallPulse);
  
  // add the rule
  fuzzy->addFuzzyRule(fuzzyRule1);
  

  
  
  
  ////////////////////////////  this is the  end of the rules and regulations of the fuzzy system ///////////////////////////
  
}// end of the fuzzy rule definations
