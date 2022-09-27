/*  Team #6 Project #2
 *  GE1501  Section 14
 *  Stephen Catalina, Alyssa Chiulli, Ephram Cola Jacquin, Olivia Valenti
 *  Water_Sample_Analysis
 */

#include <LiquidCrystal.h>                                                                                            //initializing LCD library
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);                                                                              //initializing LCD pins

const int button_L(2), button_R(3), button_enter(4), button_turb(5), button_ph(6), button_conduct(7);                 //input button pins (2-7)
int test_turb, test_ph, test_conduct;                                                                                 //whether test is activated or not                                        

const int t_sensor = A0, p_sensor = A1, c_sensor = A3;                                                                //sensor pins (A0-A2)

const int numReadings = 10;                                                                                           //number of readings sensor does in one second
float total_t(0), total_p(0), total_c(0);                                                                             //total number of added up readings from 1-10
float avg_t, avg_p, avg_c;                                                                                            //average value calculated by: total / numReadings
  
int val_t, val_p, val_c;                                                                                              //variables to find specific values from analog inputs
float t_voltage, p_voltage, c_voltage;                                                                                //converted analog values into voltages
float pH, conductivity;                                                                                               //final pH value
int turbidity;                                                                                                        //Calcultaed turbidity value before average


void setup() {
  Serial.begin(9600);
  pinMode(t_sensor, INPUT);                                                                                           //set sensors as inputs
  pinMode(p_sensor, INPUT);
  pinMode(c_sensor, INPUT);
  test_ph = 0;                                                                                                        //reset test variables (in setup to allow for "reset" command to function properly)
  test_turb = 0;
  test_conduct = 0;
  lcd.begin(16,2);                                                                                                    //initialize LCD display
  lcd.clear();                                                                                                        //clear previous written information (in setup to allow for "reset" command to function properly)
  lcd.print("Select Tests: ");                                                                                        //asks user to select the tests they would like to run
  for (int i=2; i<=7; i++) {                                                                                          //sets digital pins 2-7 as inputs for pushbuttons
    pinMode(i, INPUT_PULLUP);
  }
}

void loop() {

  while(digitalRead(button_enter) != LOW) {                                                                           //run the "main_menu" function until the "enter" button is pushed
    main_menu();
  }
  
  delay(200);                                                                                                         //once the "enter" button is pushed, delay 200ms and write "running tests"
      lcd.setCursor(0,0);
      lcd.clear();
      delay(100);
      lcd.print("Running Tests");

      if(test_ph == 1) {                                                                                              //run the "ph_func" function if the pH test is activated (will run the test)
        ph_func();
        Serial.println(avg_p);
        lcd.setCursor(5,1);
        lcd.print(".");                                                                                               //the dot indicates that the test is compeleted
      }
      else if (test_ph == 0) {                                                                                        //if the test is not activated, then the dot prints immediately
        delay(100);
        Serial.print("pH N/A");
        lcd.setCursor(5,1);
        lcd.print(".");
      }
      
      if(test_turb == 1) {                                                                                            //run the "turbidity_func" function if the turbidity test is activated (will run the test)
        turbidity_func();
        Serial.println(t_voltage);
        Serial.println(turbidity);
        lcd.setCursor(7,1);
        lcd.print(".");
      }
      else if (test_turb == 0) {
        delay(100);
        Serial.print("Turb N/A");
        lcd.setCursor(7,1);
        lcd.print(".");
      }

      if(test_conduct == 1) {                                                                                        //run the "conductivity_func" function if the conductivity test is activated (will run the test)
        conductivity_func();
        Serial.println(c_voltage);
        Serial.println(conductivity);
        lcd.setCursor(9,1);
        lcd.print(".");
      }
      else if(test_conduct == 0) {
        delay(100);
        Serial.print("Conductivity N/A");
        lcd.setCursor(9,1);
        lcd.print(".");
        delay(500);
      }

      if(test_ph == 1 && test_turb == 1 && test_conduct == 1) {                                                    //if the 3 tests were selected, then run the "results" function
        results();
      }
      else {
        indiv_results();                                                                                           //if less than 3 tests were selected, then run the "indiv_results" function. This is because it is not safe to assume the safety of the drinking water based on one or two tests alone
      }      
    
   
}

void main_menu() {                                                                                                //this is the main_menu function
  
  if (digitalRead(button_ph) == LOW) {                                                                            //if the pH button is pressed, it will toggle the test on, if the test is already on, it will toggle it off
    if (test_ph == 0) {
      test_ph = 1;
      lcd.setCursor(14,0);          
      lcd.print("pH");                                                                                            //the LCD indicates the user that the test is activated by printing "pH"
    }
    else {
      test_ph = 0;
      lcd.setCursor(14,0);
      lcd.print("  ");                                                                                            //the LCD indicates the user that the test is not activated by erasing "pH"
    }
    delay(200);
  }

  if (digitalRead(button_turb) == LOW) {                                                                          //if the turbidity button is pressed, it will toggle the test on, if the test is already on, it will toggle it off
    if (test_turb == 0) {
      test_turb = 1;
      lcd.setCursor(1,1);
      lcd.print("Turb");                                                                                          //the LCD indicates the user that the test is activated by printing "Turb"
    }
    else {
      test_turb = 0;
      lcd.setCursor(1,1);
      lcd.print("    ");                                                                                          //the LCD indicates the user that the test is not activated by erasing "Turb"
    }
    delay(200);
  }

  if(digitalRead(button_conduct) == LOW) {                                                                        //if the conductivity button is pressed, it will toggle the test on, if the test is already on, it will toggle it off
    if (test_conduct == 0) {
      test_conduct = 1;
      lcd.setCursor(8,1);
      lcd.print("Conduct");                                                                                       //the LCD indicates the user that the test is activated by printing "Conduct"
    }
    else {
      test_conduct = 0;
      lcd.setCursor(8,1);
      lcd.print("       ");                                                                                       //the LCD indicates the user that the test is not activated by erasing "Conduct"
    }
    delay(200);
  }
}

float ph_func() {                                                                                                 //this is the function that runs the pH test
  for(int i=0; i<numReadings; i++) {                                                                              //this will read from the sensor 10 times
    val_p = analogRead(p_sensor);                                                                                 //read the analog value from the pH probe
    p_voltage = val_p * (5.0/1024.0);                                                                             //convert the analog value into voltage                                                         
    pH = -5.70 * p_voltage + 21.34;                                                                               //this is the equation to convert the voltage into the pH value
    total_p = total_p + pH;                                                                                       //add up each value until 10 readings have been made
    delay(100);                                                                                                   //100ms delay means it takes 1s to complete the test
  }

  avg_p = total_p / numReadings;                                                                                  //calculate the average by dividing the total of all 10 readings by 10

  total_p = 0;                                                                                                    //reset the total to 0 for the next test
  return avg_p;                                                                                                   //return the calculated pH value
}

float turbidity_func() {                                                                                          //this is the function that runs the turbidity test
  for(int i=0; i<numReadings; i++) {                                                                              //this will read from the sensor 10 times
    val_t = analogRead(t_sensor);                                                                                 //read the analog value from the turbidity probe
    t_voltage = val_t * (5.0/1024.0);                                                                             //convert the analog value into voltage
    total_t = total_t + t_voltage;                                                                                //add up each value until 10 readings have been made
    delay(100);                                                                                                   //100ms delay means it takes 1s to complete the test
  }

  avg_t = total_t / numReadings;                                                                                  //calculate the average by dividing the total of all 10 readings by 10
  
  if(avg_t <= 4.2) {                                                                                              //this is the equation to calculate the turbidity if the voltage is under 4.2v
    turbidity = (-1120.4 * pow(avg_t,2)) + (5742.3 * avg_t) - 4352.9;
  }
  else if(avg_t > 4.2) {                                                                                          //anything over 4.2 will return a negative value for turbidity, which is impossible, the lowest it can go is 0.
    turbidity = 0;
  }

  total_t = 0;                                                                                                    //reset the total to 0 for the next test
  return turbidity;                                                                                               //return the calculated turbidity value
}

float conductivity_func() {                                                                                       //this is the function that runs the conductivity test
  for(int i=0; i<numReadings; i++) {                                                                              //this will read from the sensor 10 times
    val_c = analogRead(c_sensor);                                                                                 //read the analog value from the conductivity probe
    c_voltage = val_c * (5.0/1024.0);                                                                             //convert the analog value into voltage
    total_c = total_c + c_voltage;                                                                                //add up each value until 10 readings have been made
    delay(100);                                                                                                   //100ms delay means it takes 1s to complete the test
  }

  avg_c = total_c / numReadings;                                                                                  //calculate the average by dividing the total of all 10 readings by 10
  conductivity = 5.0 - avg_c;                                                                                     //calculates the difference in voltages from input to output of probe, to determine how much voltage is lost by water resistance

  total_c = 0;                                                                                                    //reset the total to 0 for the next test
  return conductivity;                                                                                            //return the calculated conductivity value
}

void results() {                                                                                                  //this function will let the user know if the water is safe to drink
  lcd.setCursor(0,0); 
  lcd.clear();

  while(digitalRead(button_R) != LOW) {                                                                           //it will show the information until the right arrow button is pressed 
    if(avg_p >= 6.5 && avg_p <= 8.5 && turbidity <= 1 && conductivity >= 0.1) {
      lcd.setCursor(2,0);
      lcd.print("Water is");
      lcd.setCursor(1,1);
      lcd.print("Safe to Drink");
    }
    else if(avg_p <= 6.5 || avg_p >= 8.5 || turbidity >= 1 || conductivity < 0.1) {
      lcd.setCursor(2,0);
      lcd.print("Water is");
      lcd.setCursor(1,1);
      lcd.print("Safe to Drink");
    }
  }

  delay(200);
  indiv_results();                                                                                                //once the right arrow button is pressed, run the "indiv_results" function
  
}

void indiv_results() {                                                                                            //this function shows the individual values for each test

  lcd.setCursor(0,0);
  lcd.clear();

  while(digitalRead(button_R) != LOW) {                                                                           //the LCD will show the information until the right arrow button is pressed
  
    if(test_ph == 1) {                                                                                            //if the pH test was activated, then the LCD will print the pH value of the sample
      lcd.setCursor(0,0);
      lcd.print("pH ");
      lcd.setCursor(3,0);
      lcd.print(avg_p);
    } 
    else if(test_ph == 0) {                                                                                       //if the pH test was not activated, then the LCD will print N/A for that test
      lcd.setCursor(0,0);
      lcd.print("pH N/A");
    }

    if(test_turb == 1) {                                                                                          //if the turbidity test was activated, then the LCD will print the turbidity value of the sample
      lcd.setCursor(0,1);
      lcd.print("Turb ");
      lcd.setCursor(5,1);
      lcd.print(turbidity);
      lcd.setCursor(10,1);
      lcd.print("NTU");
    }
    else if(test_turb == 0) {                                                                                    //if the turbidity test was not activated, then the LCD will print N/A for that test
      lcd.setCursor(0,1);
      lcd.print("Turb N/A");
    }

    if(test_conduct == 1) {                                                                                      //if the conductivity test was activated, then the LCD will print the conductivity value of the sample
      lcd.setCursor(9,0);
      lcd.print("Co ");
      lcd.setCursor(12,0);
      lcd.print(conductivity);
      lcd.setCursor(15,0);
      lcd.print("v");
    }
    else if(test_conduct == 0) {                                                                                 //if the conductivity test was not activated, then the LCD will print N/A for that test
      lcd.setCursor(9,0);
      lcd.print("Co N/A");
    }    
  }

  delay(200);                                                                                                   //clear the display once the right arrow button is pressed
  lcd.setCursor(0,0);
  lcd.clear();

    while(digitalRead(button_enter) != LOW) {                                                                     
      
      lcd.setCursor(0,0);                                                                                       //if the right button is pressed, it will ask the user to reset
      lcd.print("Reset?");

    if(digitalRead(button_L) == LOW) {                                                                          //if the user does not want to reset, pressing the left arrow button will return to the results page
      indiv_results();
    } 
  }

  delay(300);                                                                                                   //if the enter button is pressed, then the reset is confirmed and the setup and loop functions will run
  setup();
  loop();
  
}
