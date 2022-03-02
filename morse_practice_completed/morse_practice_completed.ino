#define buzz 5
#define space 6
#define trigger 7 
#define dot 12 
#define dash 13 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {
  // Set pinModes for all LEDS and Switches
  pinMode(dot, OUTPUT);
  pinMode(dash, OUTPUT);
  pinMode(trigger, INPUT);
  pinMode(space, INPUT);
  pinMode(buzz, OUTPUT);

  // Turn on LCD Display
  lcd.begin();

  // Turn on Serial Monitor
  Serial.begin(9600);
  Serial.println("Whats the code lookin like?");
}

char decrypt(int code[], int len){ // Translates morse to ASCII
  /* Decrypt takes the array that stores the inputs and
   * determines what the array equals to in morse, with
   * 1 being dots and 2 being dash and 0 being nothing.
   */


  /*  Alphabet meaning
   *  A = {1,2,0,0,0}; B = {2,1,1,1,0}; C = {2,1,2,1,0};
   *  D = {2,1,1,0,0}; E = {1,0,0,0,0}; F = {1,1,2,1,0};
   *  G = {2,2,1,0,0}; H = {1,1,1,1,0}; I = {1,1,0,0,0};
   *  J = {1,2,2,2,0}; K = {2,1,2,0,0}; L = {1,2,1,1,0};
   *  M = {2,2,0,0,0}; N = {2,1,0,0,0}; O = {2,2,2,0,0};
   *  P = {1,2,2,1,0}; Q = {2,2,1,2,0}; R = {1,2,1,0,0};
   *  S = {1,1,1,0,0}; T = {2,0,0,0,0}; U = {1,1,2,0,0};
   *  V = {1,1,1,2,0}; W = {1,2,2,0,0}; X = {2,1,1,2,0};
   *  Y = {2,1,2,2,0}; Z = {2,2,1,1,0};
   */
  
  // The entire alphabet into an array
  char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  // The morse alphabet in alphabetical order
  int alphabet_morse[26][5] =   { 
                            {1,2,0,0,0},{2,1,1,1,0},{2,1,2,1,0},
                            {2,1,1,0,0},{1,0,0,0,0},{1,1,2,1,0},
                            {2,2,1,0,0},{1,1,1,1,0},{1,1,0,0,0},
                            {1,2,2,2,0},{2,1,2,0,0},{1,2,1,1,0},
                            {2,2,0,0,0},{2,1,0,0,0},{2,2,2,0,0},
                            {1,2,2,1,0},{2,2,1,2,0},{1,2,1,0,0},
                            {1,1,1,0,0},{2,0,0,0,0},{1,1,2,0,0},
                            {1,1,1,2,0},{1,2,2,0,0},{2,1,1,2,0},
                            {2,1,2,2,0},{2,2,1,1,0}
                            };

  // Checking every letter in the alphabet
  for (int i=0 ; i < 26; i++){
    
    // Initilising correct == 0, if all 5 arrays 
    // in the letter are correct, correct == 5
    int correct = 0;
    
    // Checking the morse of each alphabet
    for (int j=0 ; j < 5; j++){
      
      // If the array matches with 
      if (code[j] == alphabet_morse[i][j]){
        correct++;
      }
      
      // If all 5 array slots match, its a valid letter
      if (correct == 5){ 
        // The index of alphabet_morse[i] 
        // matches its letter in alphabet[i]
        char result = alphabet[i];
        
        Serial.println();
        Serial.println("result = CORRECT");
        Serial.println("Result");
        Serial.println(result);
        return result;
      }
    }
  }

  // If the input morse does not match, 
  // its invalid and returns spacebar
  Serial.println();
  Serial.println("INVALID");
  return ' ';
}


void bop(int state){
  /* Displays the LED for dots */
  if (state == 1){
      // dot LED
  digitalWrite(dot, HIGH);
  digitalWrite(dash, LOW);
  }
}

void beep(int state){
  /* Displays the LED for dashes */
  if (state == 1){
    // dash LED
  digitalWrite(dot, HIGH);
  digitalWrite(dash, HIGH);
  
  }
  
}

void loop() {
  // Frequency is the amount of time whilst 
  // switch is on before the dot becomes a dash.
  int frequency = 200; // miliseconds
  int interval = frequency/10;

  // morse[] is where the input morse will be stored.
  static int morse[5] = {0,0,0,0,0};
  
  // sample[] is a dummy array which will compared with morse[].
  int sample[5] = {0,0,0,0,0};

  // reset is used to track at what point the loop is in.
  int reset = 0;

  // create_word[] is a where the word is stored before confirming.
  static char create_word[32];
  static int word_init = 0;

  // Stores the word without the 'x' filler.
  char new_word[32];
  
  // Uses create_word[] to store the letters produces,
  // having 'x' as filler in the 32 char.
  if (word_init == 0){
    for (int i = 0; i < 32; i++){
      create_word[i] = 'x';
    }
  Serial.println("ERASED, NEW WORD!");
  Serial.println(create_word);
    word_init++; // changes word_init so cannot run again.
  }

  
  // Display (MORSE)
      lcd.setCursor(1,0);
      lcd.print("Morse: ");
  for (int i=0; i < 5; i++){
    /* Reads the morse in its '1' and '2' state 
     * and translate it to dots and dashes
     */

    // Dots
    if (morse[i] == 1){
      lcd.setCursor(8+i,0);
      lcd.print(".");
    }
    
    // Dashes
    if (morse[i] == 2){
      lcd.setCursor(8+i,0);
      lcd.print("_");
    }
  }

  // DECRYPTS MORSE
  // fullstop is used to track how many times 'space' has been pressed
  static int fullstop = 0;
  if (digitalRead(space) == 1 and fullstop ==0){ 
    /*  The 'space' button means the morse code is finished
     *  and its ready to decrypt. By pressing it, it stores
     *  the translated morse code into an array and resets
     *  the morse code storage.
     *  
     *  If 'space' is pressed twice, it does another function below.
     */

    // To track how many times 'space' has been pressed, fullstop++.  
    fullstop = 1;

    // Decrypts the stored morse code into a english letter.
    char letter = decrypt(morse, 5); // DECRYPTS MORSE

    // Places decrypted letter into temporary word, create_word[].
    for (int i=0; i< 32; i++){ // PLACES LETTER INTO WORD
      // Replaces earliest 'x' with the decrypted letter.
      if (create_word[i] == 'x'){
        create_word[i] = letter;
        Serial.println("REPLACING x");
        Serial.println(create_word);
        break;
      }
    }

    // Resets current morse to 00000
    for (int n = 0; n<5; n++){ // RESETS MORSE TO 00000
      morse[n] = 0;
    }

    // DISPLAY (MORSE TRANSLATION)
    // Erases the morse code after every 'space' input
    for (int i = 8; i<13; i++){
      lcd.setCursor(i, 0);
      lcd.print(" "); // Erases them by replacing with ' '.
    }
    
    // Places letter into word and displays underneath.
    for (int i=0; i< 32; i++){
      if (create_word[i] == 'x'){
        lcd.setCursor(1,1);
        lcd.print("-> ");

        for (int j=0; j<i; j++){
          lcd.setCursor(4+j,1);
          lcd.print(create_word[j]);
        }
        break;
      }
    }

    // Come back to it
    reset = 0;
    delay(200);
  }

  // Completes the word
  if (digitalRead(space) == 1 and fullstop !=0){
    /*  Functions:  By double pressing 'space', it 
     *              stores the previous word without 'x',
     *              and resets the sample word xxxxxxxxx...
     */
     
    lcd.clear();
    Serial.println("COMPLETED THE WORD");

    // Scans create_word for earliest 'x'.
    for (int i=0; i< 32; i++){
      if (create_word[i] == 'x'){
        Serial.println("Taking out 'x'");
        
        // Takes the 'x' out of create_word 
        // and turns it into new_word
        for (int j=0; j < i; j++){
          new_word[j] = create_word[j];
        }
        Serial.println(create_word);
        Serial.println(new_word);
        break;
      }
    }
    
    // Resets create_world by setting word_init = 0,
    // making create_word to be filled with 'x' again.
    word_init = 0;
    delay(200); // Added so it doesnt spam from a single press
  }



  // DOT
  /*  Adds the dots into the morse array
   *  Sets Dot LEDS to HIGH
   *  Sets Tone for Buzzer
   */
  if (digitalRead(trigger) == 1){
    fullstop = 0; // resets fullstop back to 0
    tone(buzz, 3000);  

    // Runtime before it becomes a dash is the frequency.
    for (int i = 0; i<interval; i++){
      bop(digitalRead(trigger)); // LEDS
      if (digitalRead(trigger) == 0){
        Serial.print(". ");

        // Replaces next available spot with '1' in morse[].
        for (int a = 0; a < 5; a++){
          if (morse[a] == sample[a]){
            morse[a] = 1; // int for dots
            break;
          }
        }
        break;
      } else{
        
        // If 200ms (freq) has passed whilst 
        // trigger is pressed, it turns into a dash.
        delay(10);
      }
    }
    reset++;
  }



  // DASH
  /*   Functions the same as dots except its a dash instead
   *   so different LEDs ON
   *   and different display symbol.
   */
  if (digitalRead(trigger) == 1 and reset !=0){
    fullstop = 0; // resets fullstop back to 0
    Serial.print("- ");

    // Replaces next available spot with '2' in morse[].
    for (int a = 0; a < 5; a++){
          if (morse[a] == sample[a]){
            morse[a] = 2; // int for dash
            break;
          }
    }
    while (digitalRead(trigger) == 1){ // dash
      beep(digitalRead(trigger)); // LEDS for dash
    }
    reset = 0;
  }

  // If the trigger is not pressed, no sound from buzzer.  
  if (digitalRead(trigger) == 0){
    noTone(buzz);
  }
}
    
