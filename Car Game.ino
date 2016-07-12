//Car game
//To be used on the 2 x 16 LCD keypad shield
//By: Shela Qiu and Vicky Xu
//INSTRUCTIONS
//Whenever ▼ appears, press down to continue
//Use left and right to choose difficulty, and press Select to start
//Because of the delay in the game, the user needs to hold down up/down/left/right
//instead of just pressing them to move

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

//LCD controls
int adc_key_val[5] = {
  50, 200, 400, 600, 800
};
int NUM_KEYS = 5;
int adc_key_in;
int key = -1;

//CARS
//Note: the LCD is split into 4 rows for the game
int gameSpace[4][16]; //Array storing the position of the cars (0 - nothing, 1 - car)
int sent = 0; //Counter to delay adding new cars
int randRow; //Randomly generate which rows cars are added

//USER
int yPos = 3; //y-coordinate of user
int xPos = 15; //x-coordinate of user
int score = 0; //Incremented each loop
boolean gameEnded = false; //If this is true, the game will reset

//DIFFICULTY
int difficulty = 0; //0 - easy, 1 - medium, 2 - hard
int carFreq = 2; //How often cars come
int delaySpeed = 1000; //How fast the game moves (the higher this number, the slower)
double multiplier = 0.7; //Multiplier for scoring - easy is 0.7, medium is 1.0, hard is 1.0

//GRAPHICS
//Car in top half of row
byte car1[8] = {
  0b00000,
  0b01110,
  0b11111,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};
//Car in bottom half of row
byte car2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b11111,
  0b01010,

};
//Person in top half of row
byte person1[8] = {
  0b00100,
  0b01110,
  0b00100,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b00000,

};
//Person in bottom half of row
byte person2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b00100,
  0b01010,

};
//Person in top half, car in bottom half
byte perCar [8] {
  0b00100,
  0b01110,
  0b00100,
  0b01010,
  0b00000,
  0b01110,
  0b11111,
  0b01010,

};
//Person in bottom half, car in top half
byte carPer [8] {
  0b00000,
  0b01110,
  0b11111,
  0b01010,
  0b00100,
  0b01110,
  0b00100,
  0b01010,

};
//Crash scene (X)
byte crash [8] {
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000

};
//Down arrow symbol
byte downArrow[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100
};

void setup()
{
  // Initiate the down arrow character
  lcd.createChar(7, downArrow);

  //Clear screen and display instructions
  lcd.clear();
  lcd.begin(16, 2); // dimensions of 16 columns x 2 rows
  lcd.setCursor(0, 0); //row 1
  lcd.print("Avoid the Cars!");
  lcd.setCursor(0, 1); //row 2
  lcd.print("HOLD up/down/ ");
  lcd.write((uint8_t)7);
  waitForKey(2); //Wait for the user to press down

  //Next instruction screen
  lcd.clear();
  lcd.setCursor(0, 0); //row 1
  lcd.print("left/right to");
  lcd.setCursor(0, 1);
  lcd.print("move. "); //row 2
  lcd.write((uint8_t)7);
  delay(500);
  waitForKey(2); //Wait for the user to press down

  //Let the user choose their difficulty
  selectDifficulty();

  //Initiate the graphics for the rest of the game
  lcd.begin(16, 2);
  lcd.createChar(0, car1); //Car at top half of row
  lcd.createChar(1, car2); //Car at bottom half of row
  lcd.createChar(2, person1); //Person at top half of row
  lcd.createChar(3, person2); //Person at bottom half of row
  lcd.createChar(4, perCar); //Person at top half, car at bottom half
  lcd.createChar(5, carPer); //Car at bottom half, person at top half
  lcd.createChar(6, crash); //Crash scene
}

//Game
void loop()
{
  //Get input
  readInput();
  //Change the array (cars and user position)
  changeArray(key);
  //Draw array and user
  draw();

  //If the user died, reset the game
  if (gameEnded)
  {
    lcd.clear();
    reset();
    gameEnded = false;
  }

  //Delay of cars moving
  delay(delaySpeed);
}

//Get input from user
int get_key(unsigned int input) // Convert ADC value to key number
{
  int k;

  for (k = 0; k < NUM_KEYS; k++) { // right=0, up=1, down=2, left=3, select=4
    if (input < adc_key_val[k])
      return k;
  }

  if (k >= NUM_KEYS)             // No valid key pressed
    k = -1;

  return k;

}

//Method to stall program until user presses a certain button
int waitForKey(unsigned int input)
{
  readInput();
  while (key != input)
  {
    readInput();
  }
  key = -1;
}

//Get input from user
void readInput()
{
  adc_key_in = analogRead(0);      // read the value from the sensor
  key = get_key(adc_key_in);       // convert into key press
}

//Prompt for user to select difficulty
void selectDifficulty()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Difficulty: ");
  lcd.setCursor(0, 1);
  lcd.print("<"); //Prompt
  lcd.setCursor(7, 1);
  lcd.print(">  Select");

  lcd.setCursor(1, 1);
  key = -1;

  readInput();
  while (key != 4) //While the user hasn't press select, allow them to choose difficulty
  {
    if (key == 0) //Cycle up through difficulties
      difficulty++;
    else if (key == 3) //Cucle down through difficulties
    {
      if (difficulty == 0)
        difficulty = 2;
      else
        difficulty--;
    }

    difficulty = difficulty % 3; //Only 3 levels of difficulty

    //Display current choice
    lcd.setCursor(1, 1);
    if (difficulty == 0)
      lcd.print("Easy  ");
    else if (difficulty == 1)
      lcd.print("Medium");
    else if (difficulty == 2)
      lcd.print("Hard  ");
    delay(250);
    
    readInput();
  }

  //Set the appropriate game constraints for the difficulty selected
  if (difficulty == 0)
  {
    carFreq = 2;
    delaySpeed = 1000;
    multiplier = 0.7;
  }
  else if (difficulty == 1)
  {
    carFreq = 2;
    delaySpeed = 500;
    multiplier = 1;
  }
  else if (difficulty == 2)
  {
    carFreq = 1;
    delaySpeed = 400;
    multiplier = 1;
  }
}

//Go throug the array to change the car and user position
void changeArray(int input)
{
  //Check in case the user tries to run into the car - game ends
  if (gameSpace[yPos][xPos - 1] == 1 && input == 3)
    gameEnded = true;

  //Move the user's position based on input
  if (input == 0 && xPos < 15) //Move right
    xPos++;
  else if (input == 1 && yPos > 0) //Move up
    yPos--;
  else if (input == 2 && yPos < 3) //Move down
    yPos++;
  else if (input == 3 && xPos > 1 && gameEnded == false) //Move left - don't let person go into last column
    xPos--;

  //Check if the user dies (if it's beside or on a car space)
  if (gameSpace[yPos][xPos - 1] == 1 || gameSpace[yPos][xPos] == 1)
    gameEnded = true;

  //Move cars to the right
  for (int i = 0; i < 4; i++)
  {
    for (int j = 15; j >= 0; j--)
    {
      if (gameSpace[i][j] == 1)
      {
        gameSpace[i][j] = 0;
        if (j < 15)
          gameSpace[i][j + 1] = 1;
      }
    }
  }

  //Checking to make sure the car won't be printed if it had switched spots with the user
  //(Collision should be printed instead)
  if (gameEnded == true && gameSpace[yPos][xPos + 1] == 1)
    gameSpace[yPos][xPos + 1] = 0;

  //Add new cars coming in from the left
  //Frequency they're added depends on difficulty level
  if (sent == 0)
  {
    //Generate car in a random row
    randRow = random(0, 4);
    gameSpace[randRow][0] = 1;
  }
  sent++; //Cycle through counter
  sent = sent % carFreq;

  //Increment score
  score++;
}

//Draw where the car and user are
void draw()
{
  lcd.clear();

  //Cycle through the array - if it's a 1, print a car
  for (int y = 0; y < 4; y++)
  {
    for (int x = 0; x < 16; x++)
    {
      //Top half of rows
      if (gameSpace[y][x] == 1)
      {
        if (y == 0 || y == 2)
        {
          if (y == 0)
            lcd.setCursor(x, 0);
          else if (y == 2)
            lcd.setCursor(x, 1);
          lcd.write((uint8_t)0);
        }
        //Bottom half of rows
        else
        {
          if (y == 1)
            lcd.setCursor(x, 0);
          else if (y == 3)
            lcd.setCursor(x, 1);
          lcd.write((uint8_t)1);
        }
      }
    }
  }

  //Draw person depending on their current x and y coordinate
  //Top half of rows
  if (yPos == 0 || yPos == 2)
  {
    //Set cursor either to row 1 or 2
    if (yPos == 0)
      lcd.setCursor(xPos, 0);
    else if (yPos == 2)
      lcd.setCursor(xPos, 1);
    //Watch for cases where the person is above a car - need to print appropriate graphic
    if (gameSpace[yPos + 1][xPos] == 1)
      lcd.write((uint8_t)4); //Draw person and car
    else 
      lcd.write((uint8_t)2); //Draw normal person
  }
  //Bottom half of rows
  else
  {
    //Set cursor either to row 1 or 2
    if (yPos == 1)
      lcd.setCursor(xPos, 0);
    else if (yPos == 3)
      lcd.setCursor(xPos, 1);
    //Watch for cases where the person is below a car - need to print appropriate graphic
    if (gameSpace[yPos - 1][xPos] == 1) //ADD
      lcd.write((uint8_t)5); //Draw person and car
    else //ADD
      lcd.write((uint8_t)3);//Draw normal person
  }

  //Crash scene - if the user died
  if (gameEnded == true)
  {
    //Print crash in either the top or bottom row, depending on where the person died
    if (yPos == 0 || yPos == 1)
    {
      lcd.setCursor(xPos, 0);
      lcd.write((uint8_t)6); //crash
    }
    if (yPos == 2 || yPos == 3)
    {
      lcd.setCursor(xPos, 1);
      lcd.write((uint8_t)6); //crash
    }
    delay(1000);
  }
}

//If game ended, reset
//Display score and allow user to re-select difficulty level
void reset()
{
  //Multiplier depending on the level
  score = multiplier * score;

  //Display
  lcd.setCursor(0, 0);
  lcd.print("You died");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.print(" ");
  lcd.write((uint8_t)7);
  //Wait for user to press down
  waitForKey(2);

  //Choose difficulty
  selectDifficulty();

  //Reset all variables and arrays
  score = 0;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      gameSpace[i][j] = 0;
    }
  }

  xPos = 15;
  yPos = 3;
}
