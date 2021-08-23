/* 
 * This code uses SEOS to execute 4 Tasks, we see them as
 * we move forward. The Arduino circuit is set up as presented 
 * in my screenshot of TinkerCAD.
*/


int pot_pin = A1; 
int pot_val;

int blueled = 8;
int greenled = 7;
int yellowled = 6;
int orangeled = 5;
int redled = 4;

int whiteled = 3;

int pushbutton = 11;       
int state;          //Current state of the push button
int oldstate = 0;   //Previous state of the pushbutton, assuming as OFF/0
int ledflag = 0;    //Flag integer to link the pushbutton state to the white LED 

const int maxtasks = 4; //defining the maximum number of tasks used

/*Defining two structures 'TaskType' and 'TaskList' with the counter, period i.e,
frequency and number of tasks variables needed to carry out the functions*/

typedef struct  
{
  int counter;
  int period;
  void (*function)(void);
} TaskType;

typedef struct 
{
  TaskType tasks[maxtasks];
  int number_of_tasks;
} TaskList;

TaskList tlist; //assigning a variable 'tlist' to the struct


/*Defining the add_task() function with the required parameters. When the 
number of tasks reaches the maximum tasks specified, '0' is returned, or else
a task is added and it returns the number of tasks in the tlist*/

int add_task(int period, int initial, void(*fun)(void))
{
  if (tlist.number_of_tasks == maxtasks)return 0;         
  tlist.tasks[tlist.number_of_tasks].period = period;     
  tlist.tasks[tlist.number_of_tasks].counter = initial;   
  tlist.tasks[tlist.number_of_tasks].function = fun;      
  tlist.number_of_tasks++;                                
  
  return tlist.number_of_tasks;
}


void setup()
{   
pinMode(blueled, OUTPUT);   
pinMode(greenled, OUTPUT); 
pinMode(yellowled, OUTPUT); 
pinMode(orangeled, OUTPUT); 
pinMode(redled, OUTPUT);    
   
pinMode(pushbutton, INPUT);         
pinMode(whiteled, OUTPUT);

Serial.begin(9600); 


/*Defining the 'period' and the 'initial' value and the function name 
of tasks, with staggering implemented*/
tlist.number_of_tasks = 0;

if (add_task(10, 0, potentiometer))     //Frequency of 10 milliseconds
       {
            Serial.println("Task 1 to check potentiometer added successfully");
       }
if (add_task(10, 5, updateleds))        //'Frequency of 10 milliseconds
        {
            Serial.println("Task 2 to update the 5 LEDs added successfully");
        }
if (add_task(100, 50, checkpushbutton)) //Frequency of 100 milliseconds
        {
            Serial.println("Task 3 to check the pushbutton added successfully");
        }
if (add_task(100, 60, blinkwhiteled))  //Frequency of 100 milliseconds
        {
            Serial.println("Task 4 to blink the white LED added successfully");
        }


noInterrupts();              //Disable interrupts while setting up the registers          
TCCR1A = 0;                  //Clearing TCCR1A control register to zero
TCCR1B = 0;                  //Clearing TCCR1B control register to zero   
TCNT1 = 0;                   //Setting timer counter to zero
OCR1A = 16000 - 1;           //Setting output compare register to '16000-1', which produces a Tick every millisecond
TCCR1B |= (1 << WGM12);      //Turns on the CTC mode
TCCR1B |= (1 << CS10);       //No prescaler divider, i.e, normal counter
TIMSK1 |= (1 << OCIE1A);     //Interrupt is generated
interrupts();                //Enable the interrupts 
 
}


void loop()
{
  //Empty loop. Instead, using SEOS
}  


/*Defining the Timer Compare Interrupt Service Routine. Incrementing the task counter 
by 1 and when the counter equals the task period, it resets the counter and calls the function*/
ISR(TIMER1_COMPA_vect)         
{
  for (int i = 0; i < tlist.number_of_tasks; i++)
  {
    tlist.tasks[i].counter++;
    if (tlist.tasks[i].counter == tlist.tasks[i].period)
    {
      tlist.tasks[i].function();
      tlist.tasks[i].counter = 0;
    }
  }
}


/*Calling the task function 'potentiometer' by a frequency of 10 milliseconds, 
to read the value of the potentiometer.*/
void potentiometer(void)         
{
    pot_val = analogRead(pot_pin);
}


/*Calling the task 'updateleds' by a frequency of 10 milliseconds, which updates
the appropriate LEDs based upon the pot's value at that time i.e, between 0 and 1023*/
void updateleds(void)         
{ 
  if ( pot_val <= 204.6) //when potentiometer is at extreme left at <20%, only blue LED glows
  {
  digitalWrite(blueled, HIGH);    
  digitalWrite(greenled, LOW);
  digitalWrite(yellowled, LOW);
  digitalWrite(orangeled, LOW);
  digitalWrite(redled, LOW);
  }
  else if ( pot_val <= 409.2) //when potentiometer is at 40%, blue and green LED glows
  {
  digitalWrite(blueled, HIGH);    
  digitalWrite(greenled, HIGH);   
  digitalWrite(yellowled, LOW);
  digitalWrite(orangeled, LOW);
  digitalWrite(redled, LOW);
  }
  else if ( pot_val <= 613.8) //when potentiometer is at 60%, blue,green and yellow LED glows
  {
  digitalWrite(blueled, HIGH);    
  digitalWrite(greenled, HIGH);   
  digitalWrite(yellowled, HIGH);  
  digitalWrite(orangeled, LOW);
  digitalWrite(redled, LOW);
  }
  else if ( pot_val <= 818.4) //when potentiometer is at 80%, blue,green,yellow, and orange LED glows
  {
  digitalWrite(blueled, HIGH);    
  digitalWrite(greenled, HIGH);   
  digitalWrite(yellowled, HIGH);  
  digitalWrite(orangeled, HIGH);  
  digitalWrite(redled, LOW);
  }
  else if ( pot_val <= 1023) //when potentiometer is at maximum value, 100%, all 5 LEDs glows
  {
  digitalWrite(blueled, HIGH);    
  digitalWrite(greenled, HIGH);   
  digitalWrite(yellowled, HIGH);  
  digitalWrite(orangeled, HIGH); 
  digitalWrite(redled, HIGH);     
  }
}


/* Calling the task 'checkpushbutton' with a frequency of 100 milliseconds to read and capture 
 * the value of the push button at that time. The pushbutton should be held for at least 100ms which 
 * is equal to the frequency of this task. Because, as the task is called every 100ms and suppose the
 * pushbutton is pressed and released within 10ms then its value wouldn't get captured.*/

/* The Pushbutton has two states LOW, and HIGH. Here, to keep the status of Pushbutton as HIGH forever 
 * just by clicking it once even when it's not being kept pressed, we record and store its values in the 
 * variables 'state' and 'oldstate'. The 'ledflag' becomes 1 or 0 when the button is pressed.*/
void checkpushbutton(void)         
{
  state = digitalRead(pushbutton); 
  if( ( state==HIGH) && (oldstate==LOW))
  {
  ledflag = !ledflag;   
  }
  oldstate = state; //Updating the old button state with the current button state
}


/* Calling the task 'blinkwhiteled' with a frequency of 100 milliseconds
 * Based upon the value of the pushbutton captured in the third task, this task will 
 * turn ON or OFF the blinking action of the white LED.*/
void blinkwhiteled(void)         
{  
if (ledflag==1) 
  { 
    //White LED has blinking frequency of 0.1s
    digitalWrite(whiteled, HIGH);
    delay(100); 
    digitalWrite(whiteled, LOW);
    delay(100); 
  }
else 
  {
    digitalWrite (whiteled,LOW);
  } 
}
