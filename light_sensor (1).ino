#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6
#define NUMPIXELS      6
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


const int ledPin[2] = {3, 5};       // the pin that the LED is attached to

int numOfMarbles[6] = {3, 3, 0, 0, 0, 0};
int currentLightValues[6];
int photocellReading[6];     // the analog reading from the sensor divider

int currentNumOfMarbles = 0;
int maxIndex = 5;

int triggerValueUnitMin = 10;      //Light sensing trigger value for single unit
int triggerValueUnitMax = 30;      //Light sensing trigger value for single unit

int playerTurn = 1;

bool gameOverState = false;

//Method declarations
int checkIfOneStackPicked(void);
void showGameStatus(void);
void moveMarbles(int, int);
void resetNeopixels(void);
uint32_t Wheel(byte WheelPos);
void gameOver(int playerIndex);
int checkWhichPlayerWon(void);

void setup(void) {
  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);

  digitalWrite(ledPin[0], HIGH);
  digitalWrite(ledPin[1], HIGH);
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  Serial.print("Init values\n");
  for(int i = 0; i < 6; i++) {
    currentLightValues[i] = analogRead(i);
    Serial.println(currentLightValues[i]);
  }
   Serial.println("------------------------");
}
 
void loop(void) {
    if(gameOverState != true) {
      int pickedIndex = checkIfOneStackPicked();
      Serial.print("Picked stack number is: ");
      Serial.println(pickedIndex);
  
      if(pickedIndex > -1) {
        resetNeopixels();
        moveMarbles(pickedIndex, numOfMarbles[pickedIndex]);
        showGameStatus();
        if(playerTurn == 1)
        {
          playerTurn = 2;
        }
        else {
          playerTurn = 1;
        }
      }
      else {
        resetNeopixels();
        if(numOfMarbles[0] == 0 && numOfMarbles[1] == 0) {
          numOfMarbles[5] = numOfMarbles[5] + numOfMarbles[4] + numOfMarbles[3];
          gameOverState = true;
        }
        else if(numOfMarbles[3] == 0 && numOfMarbles[4] == 0) {
          numOfMarbles[2] = numOfMarbles[2] + numOfMarbles[1] + numOfMarbles[0];
          gameOverState = true;
        }
        else {
          if(playerTurn == 1) 
        {
          if(numOfMarbles[0] > 0)
          {
              pixels.setPixelColor(0,pixels.Color(0,50,0));   
          }
          if(numOfMarbles[1] > 0)
          {
              pixels.setPixelColor(1,pixels.Color(0,50,0));   
          }
        }
        if(playerTurn == 2) 
        {
          if(numOfMarbles[3] > 0)
          {
              pixels.setPixelColor(3,pixels.Color(0,0,50));   
          }
          if(numOfMarbles[4] > 0)
          {
              pixels.setPixelColor(4,pixels.Color(0,0,50));   
          }
        }
        pixels.show();
        }
      }
      delay(500);
    }
    else {
      gameOver(checkWhichPlayerWon());
      photocellReading[5] = analogRead(5);
      photocellReading[2] = analogRead(2);
      Serial.print(photocellReading[5]);
      Serial.print(":");
      Serial.println(photocellReading[2]);
      
      if(photocellReading[5] < 150 && photocellReading[2] < 150)
      {
        numOfMarbles[0] = 3;
        numOfMarbles[1] = 3;
        numOfMarbles[2] = 0;
        numOfMarbles[3] = 3;
        numOfMarbles[4] = 3;
        numOfMarbles[5] = 0;
        
        playerTurn = 1;
        gameOverState = false;
        Serial.println("Game reset");
        resetNeopixels();
        checkIfOneStackPicked();
      }
    }
}

int checkIfOneStackPicked() {
  Serial.println("Reading photocell values to check which one is checked");
  int indexToReturn = -1;
  for(int i = 0; i < 6; i++) {
    photocellReading[i] = analogRead(i);
     Serial.print(photocellReading[i]);
    Serial.print(":");
    Serial.println(currentLightValues[i]);
     
    if(photocellReading[i] - currentLightValues[i] > triggerValueUnitMin * numOfMarbles[i] 
    && photocellReading[i] - currentLightValues[i] < triggerValueUnitMax * numOfMarbles[i] 
    && i != 2 && i != 5
    && numOfMarbles[i] != 0) {
        if(playerTurn == 1 && (i == 0 || i == 1))
        {
          indexToReturn = i;     
        }
        
        if(playerTurn == 2 && (i == 3 || i == 4)) 
        {
          indexToReturn = i;
        }
        
    }
    
    currentLightValues[i] = photocellReading[i];  
  }
   Serial.println("------------------------");
  return indexToReturn;
}

int checkWhichPlayerWon() {
  if(numOfMarbles[2] > numOfMarbles[5])
  {
    return 2;
  }
  else 
  {
    return 5;
  }
}

void moveMarbles(int fromIndex, int currentNumOfMarbles)
{
      int currentIndex = fromIndex + 1;
      int j = 0;
      while(j < currentNumOfMarbles) 
      {
        if(currentIndex > maxIndex) {
          currentIndex = 0;
        }
        Serial.print("Turning on neo pixel with index: ");
        Serial.println(currentIndex);
        
        pixels.setPixelColor(currentIndex,pixels.Color(50,0,0));
        currentIndex++;
        j++;
      } 
      pixels.show();
      
      numOfMarbles[fromIndex] = 0;

      currentIndex = fromIndex + 1;
      j = 0;

      Serial.println("Moving marbles");
      Serial.println(currentIndex);
      Serial.println(numOfMarbles[currentIndex]);
      while(j < currentNumOfMarbles) {
        //Increase num of marbles in the bowl
        if(currentIndex > maxIndex) {
          currentIndex = 0; 
        } 
        photocellReading[currentIndex] = analogRead(currentIndex);
        //Serial.print("photo cell reading");
        //Serial.println(photocellReading[currentIndex]);
        //Serial.print(" - current light values");
        //Serial.println(currentLightValues[currentIndex]);
        
        if(currentLightValues[currentIndex] - photocellReading[currentIndex] > triggerValueUnitMin 
        && currentLightValues[currentIndex] - photocellReading[currentIndex] < triggerValueUnitMax) {
          //Increase num of marbles in the bowl
          numOfMarbles[currentIndex] = numOfMarbles[currentIndex] + 1;
          currentLightValues[currentIndex] = photocellReading[currentIndex];
          pixels.setPixelColor(currentIndex,pixels.Color(0,0,0));
          Serial.print(j+1);
          Serial.print(" of ");
          Serial.print(currentNumOfMarbles);
          Serial.print("\n");
          currentIndex++;        
          j++; 
          pixels.show();
        }
        delay(500);
     }
}

void resetNeopixels() {
  for(int i=0; i <NUMPIXELS; i++) {
        pixels.setPixelColor(i,pixels.Color(0,0,0)); 
  }
}
void gameOver(int playerIndex) {
  for(int j=0; j<256; j++) {
      pixels.setPixelColor(playerIndex, Wheel(j & 255));
      pixels.show();
      delay(20);
  }
}

void showGameStatus() {
    Serial.println("Show game status");
    if(numOfMarbles[0] > numOfMarbles[3])
    {
      digitalWrite(ledPin[0], LOW);
      digitalWrite(ledPin[1], HIGH); 
      Serial.println("Player 1 is in the lead");
    }
    else if(numOfMarbles[0] == numOfMarbles[3]) {
        digitalWrite(ledPin[0], HIGH);
        digitalWrite(ledPin[1], HIGH); 
        Serial.println("Even");
    }
    else {
       digitalWrite(ledPin[1], LOW);
       digitalWrite(ledPin[0], HIGH);
      Serial.println("Player 2 is in the lead");
    }
    for(int k = 0; k < 6; k++) 
    {
      Serial.println(numOfMarbles[k]);
    }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

