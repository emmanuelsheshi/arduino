int count = 0;
int states[] = {1,2,3};
int state = 0;

void setup() {
  // put your setup code here, to run once:



    for(int i = 9; i<=13; i++){
    pinMode(i, 1);    
    }
      pinMode(2, INPUT_PULLUP);  

    attachInterrupt(digitalPinToInterrupt(2),isr,RISING);


    Serial.begin(9600);
Serial.println("finite Machine Control ");

}

void loop() {
  // put your main code here, to run repeatedly:


  

 
  switch(count){
    
    case 0: mode1();
    break;

    case 1: mode2();
    break;

    case 2: mode3();
    break;
    
    default:
     break;
    //Serial.println("end of switch");

    
    
    
    
    
    }



}



int mode1(){
  for(int i = 9; i<=13; i++){
    digitalWrite(i, 1);
    
    }

    
  Serial.println("mode1");
    for(int i = 9; i<=13; i++){
    digitalWrite(i, 0);
    
    }
       return 1;
       delay(50);
  
  }


int mode2(){

   Serial.println("mode2");
   return 2;
   delay(50);
  
  }



int mode3(){
 Serial.println("Mode 3");
 return 3;
  delay(50);
  }

void isr(){
  
  count++;
    if (count >= sizeof(states)/sizeof(int) ){count = 0;}
  }
