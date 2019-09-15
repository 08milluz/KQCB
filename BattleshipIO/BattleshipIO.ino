#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Digital
#define KRONK_LED 2
#define KEY_SWITCH 3
#define HADES_LED 8
#define HADES_HAIR_LED 9
#define ESTOP 22
#define POWERED_LED 23
#define SELECT 53
#define KRONK_OR_HADES 52

// Analog
#define X_MOVE 14
#define Y_MOVE 15
#define FIRE_LEVER 10

// Board States
#define SEA 0
#define MISS 1
#define HIT 2
#define RESULT 3
#define LARGE_SHIP 4
#define MEDIUM_A_SHIP 5
#define MEDIUM_B_SHIP 6
#define SMALL_SHIP 7


// Game States
#define STARTUP 0
#define HADES_PLACE 1
#define KRONK_PLACE 2
#define READY 3
#define HADES_TURN 4
#define KRONK_TURN 5
#define END 6

// Ship LED Index Start
#define smallShip 100 // always up/down
#define mediumShip1 103 // always sideways
#define mediumShip2 107 // always up/down
#define largeShip 111  // always box

#define NUM_LEDS 150

#define BRIGHTNESS 50

struct targetPos{
  int x;
  int y;
};

targetPos large[5];
targetPos mediumA[4];
targetPos mediumB[4];
targetPos small[4];

Adafruit_NeoPixel kronk = Adafruit_NeoPixel(NUM_LEDS, KRONK_LED, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel hades = Adafruit_NeoPixel(NUM_LEDS, HADES_LED, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel hadesHair = Adafruit_NeoPixel(NUM_LEDS, HADES_HAIR_LED, NEO_GRB + NEO_KHZ800);

byte kronkField[100];

byte hadesField[100];

// Colors
const uint32_t red = kronk.Color(255, 0, 0);
const uint32_t green = kronk.Color(0, 255, 0);
const uint32_t blue = kronk.Color(0, 0, 255);
const uint32_t purple = kronk.Color(255, 0, 255);
const uint32_t white = kronk.Color(255, 255, 255);

bool poweredUp;
bool kronkPlaced;
bool hadesPlaced;

byte state;

byte hadesShipHealth[4];
byte kronkShipHealth[4];

const byte ledSnake[] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
  19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
  39, 38, 37, 36, 35, 34, 33, 32, 31, 30,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
  59, 58, 57, 56, 55, 54, 53, 52, 51, 50,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
  79, 78, 77, 76, 75, 74, 73, 72, 71, 70,
  80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
  99, 98, 97, 96, 95, 94, 93, 92, 91, 90
};

const byte winner [] = {
  blue, blue,   blue,   blue,   blue,   blue,   blue,   blue,   blue,   blue,
  blue, purple, blue,   blue,   blue,   blue,   blue,   purple, blue,   blue,
  blue, purple, blue,   blue,   blue,   blue,   blue,   purple, blue,   blue,
  blue, purple, blue,   blue,   blue,   blue,   blue,   purple, blue,   blue,
  blue, blue,   purple, blue,   blue,   blue,   purple, blue,   blue,   blue,
  blue, blue,   purple, blue,   blue,   blue,   purple, blue,   blue,   blue,
  blue, blue,   purple, blue,   purple, blue,   purple, blue,   blue,   blue,
  blue, blue,   blue,   purple, purple, purple, blue,   blue,   blue,   blue,
  blue, blue,   blue,   purple, blue,   purple, blue,   blue,   blue,   blue,
  blue, blue,   blue,   blue,   blue,   blue,   blue,   blue,   blue,   blue
};

const byte loser [] = {
  blue, blue, blue, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, blue, blue, blue, blue, blue, blue, blue,
  blue, blue, purple, purple, purple, purple, purple, blue, blue, blue,
  blue, blue, blue, blue, blue, blue, blue, blue, blue, blue
};


byte neopix_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


void setup() {
  Serial.begin(9600);
  pinMode(KRONK_OR_HADES, INPUT);
  pinMode(POWERED_LED, OUTPUT);
  pinMode(SELECT, INPUT);
  digitalWrite(SELECT, HIGH);
  digitalWrite(KRONK_OR_HADES, HIGH);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  kronk.setBrightness(BRIGHTNESS);
  hades.setBrightness(BRIGHTNESS);
  kronk.begin();
  hades.begin();
  
  for(byte i = 0; i<100; i++)
  {
    kronkField[i] = SEA;
    hadesField[i] = SEA;
    kronk.setPixelColor(i, blue);
    hades.setPixelColor(i, blue);
  }

  // Set boat status
  for (byte i = 100; i<116; i++)
  {
    kronk.setPixelColor(i, white);
    hades.setPixelColor(i, white);
  }

  hadesShipHealth[0] = 5;
  hadesShipHealth[1] = 4;
  hadesShipHealth[2] = 4;
  hadesShipHealth[3] = 3;

  kronkShipHealth[0] = 5;
  kronkShipHealth[1] = 4;
  kronkShipHealth[2] = 4;
  kronkShipHealth[3] = 3;
  
  state = STARTUP;
  poweredUp = false;
  kronkPlaced = false;
  hadesPlaced = false;
  digitalWrite(POWERED_LED, LOW);

  kronk.show();
  hades.show();

  large[0].x = -1;
  large[0].y = 1;
  large[1].x = 1;
  large[1].y = 1;
  large[2].x = 0;
  large[2].y = 0;
  large[3].x = -1;
  large[3].y = -1;
  large[4].x = 1;
  large[4].y = -1;

  mediumA[0].x = 0;
  mediumA[0].y = 1;
  mediumA[1].x = 0;
  mediumA[1].y = 0;
  mediumA[2].x = 0;
  mediumA[2].y = -1;
  mediumA[3].x = 0;
  mediumA[3].y = -2;

  mediumB[0].y = 0;
  mediumB[0].x = -1;
  mediumB[1].y = 0;
  mediumB[1].x = 0;
  mediumB[2].y = 0;
  mediumB[2].x = 1;
  mediumB[3].y = 0;
  mediumB[3].x = 2;

  small[0].x = 0;
  small[0].y = 1;
  small[1].x = 0;
  small[1].y = 0;
  small[2].x = 0;
  small[2].y = -1;

  Serial.println("Setup Complete");
}

void loop() {

if(!poweredUp)
{
  state = STARTUP;
}


// State Machine
  switch(state)
  {
    case STARTUP:
      Serial.println("Startup State");
      poweredUp = true;
      
      // Turn on powered LED
      digitalWrite(POWERED_LED, HIGH);

      Serial.println("Powered Up");

      // Wait for user in put
      delay(2000);
      if(digitalRead(KRONK_OR_HADES))
      {
        state = HADES_PLACE;
        Serial.println("Select Hades");
      }
      else
      {
        state = KRONK_PLACE;
        Serial.println("Select Kronk");
      }
      break;
      
    case HADES_PLACE:
      Serial.println("Hades Place State");
      if(!hadesPlaced)
      {
        int select =1;
        byte x, y = 0;
        
        for(byte j = 0; j < 4; j++)
        {
          delay (1000);
          select = digitalRead(SELECT);
          Serial.println("Placing next boat");
        while(select != 0)
        {
          Serial.println("Making Selection");
          Serial.println(j);
          bool posChange = false;
          bool valid = true;
          int xCmd = analogRead(X_MOVE);
          int yCmd = analogRead(Y_MOVE);
          int lightList[5] = {101, 101, 101, 101, 101};
          // Move cursor
          if(xCmd < 50 && x > 0)
          {
            x--;
            posChange = true;
          }
          else if (xCmd > 975 && x < 9)
          {
            x++;
            posChange = true;
          }

          if(yCmd < 50 && y > 0)
          {
            y--;
            posChange = true;
          }
          else if (yCmd > 975 && y < 9)
          {
            y++;
            posChange = true;
          }

          switch(j)
          {
            case 0:
            // large ship
            for(byte i = 0; i < 5; i++)
            {
              lightList[i] = 10*(x+large[i].x) + (y+large[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(hadesField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 1:
            // Medium Verticle ship
            for(byte i = 0; i < 4; i++)
            {
              lightList[i] = 10*(x+mediumA[i].x) + (y+mediumA[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(hadesField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 2:
            // Medium Horizontal ship
            for(byte i = 0; i < 4; i++)
            {
              lightList[i] = 10*(x+mediumB[i].x) + (y+mediumB[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(hadesField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 3:
            // Small Verticle ship
            for(byte i = 0; i < 3; i++)
            {
              lightList[i] = 10*(x+small[i].x) + (y+small[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(hadesField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            default:
              valid = false;
              break;
          }

          // Light Square
          for(byte i = 0; i < 100; i++)
          {
              if(hadesField[i] == SEA)
              {
                hades.setPixelColor(positionMap(i), blue);
              }
              else
              {
                hades.setPixelColor(positionMap(i), purple);
              }
          }

          for(byte i = 0; i < 5; i++)
          {
            if(lightList[i] < 100 && lightList[i] >= 0)
            {
              if(valid)
              {
                hades.setPixelColor(positionMap(lightList[i]), green);
              }
              else
              {
                hades.setPixelColor(positionMap(lightList[i]), red);
              }
            }
          }
          
          hades.show();

          if(posChange)
          {
            delay(400);
          }

          if(valid)
          {
            select = digitalRead(SELECT);
            Serial.println(select);
            if(digitalRead(SELECT) ==0)
            {
              Serial.println("Select Pressed");
              delay(100);
              select = digitalRead(SELECT);
              if(select == 0)
              {
                Serial.println("Select Valid");
                switch(j)
                {
                  case 0:
                      for(byte t = 0; t < 5; t++)
                      {
                        hadesField[lightList[t]] = LARGE_SHIP;
                      }
                      break;
                  case 1:
                      for(byte t = 0; t < 4; t++)
                      {
                        hadesField[lightList[t]] = MEDIUM_A_SHIP;
                      }
                      break;
                  case 2:
                      for(byte t = 0; t < 4; t++)
                      {
                        hadesField[lightList[t]] = MEDIUM_B_SHIP;
                      }
                      break;
                  case 3:
                      for(byte t = 0; t < 3; t++)
                      {
                        hadesField[lightList[t]] = SMALL_SHIP;
                      }
                      break;
                  default:
                    break;
                }
              }
            }
          }
          else
          {
            select = 1;
          }

          delay(100);
        }
        }
        Serial.println("Placed is true");
        hadesPlaced = true;
        // Light Square
          for(byte i = 0; i < 100; i++)
          {
              if(hadesField[i] == SEA)
              {
                hades.setPixelColor(positionMap(i), blue);
              }
              else
              {
                hades.setPixelColor(positionMap(i), purple);
              }
          }
          hades.show();
      }
      else
      {
        Serial.println("Placed");
        delay(500);
      }
      
      if(!kronkPlaced)
      {
        if(digitalRead(KRONK_OR_HADES))
        {
          state = HADES_PLACE;
        }
        else
        {
          state = KRONK_PLACE;
        }
      }
      else
      {
        state = READY;
      }
      break;
    case KRONK_PLACE:
      Serial.println("Kronk Place State");
      if(!kronkPlaced)
      {
        int select =1;
        byte x, y = 0;
        
        for(byte j = 0; j < 4; j++)
        {
          delay (1000);
          select = digitalRead(SELECT);
          Serial.println("Placing next boat");
        while(select != 0)
        {
          Serial.println("Making Selection");
          Serial.println(j);
          bool posChange = false;
          bool valid = true;
          int xCmd = analogRead(X_MOVE);
          int yCmd = analogRead(Y_MOVE);
          int lightList[5] = {101, 101, 101, 101, 101};
          // Move cursor
          if(xCmd < 50 && x > 0)
          {
            x--;
            posChange = true;
          }
          else if (xCmd > 975 && x < 9)
          {
            x++;
            posChange = true;
          }

          if(yCmd < 50 && y > 0)
          {
            y--;
            posChange = true;
          }
          else if (yCmd > 975 && y < 9)
          {
            y++;
            posChange = true;
          }

          switch(j)
          {
            case 0:
            // large ship
            for(byte i = 0; i < 5; i++)
            {
              lightList[i] = 10*(x+large[i].x) + (y+large[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(kronkField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 1:
            // Medium Verticle ship
            for(byte i = 0; i < 4; i++)
            {
              lightList[i] = 10*(x+mediumA[i].x) + (y+mediumA[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(kronkField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 2:
            // Medium Horizontal ship
            for(byte i = 0; i < 4; i++)
            {
              lightList[i] = 10*(x+mediumB[i].x) + (y+mediumB[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(kronkField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            case 3:
            // Small Verticle ship
            for(byte i = 0; i < 3; i++)
            {
              lightList[i] = 10*(x+small[i].x) + (y+small[i].y);
              if(lightList[i] > 99 || lightList[i] < 0)
              {
                valid = false;
              }
              else
              {
                if(kronkField[lightList[i]] != SEA)
                {
                  valid = false;
                }
              }
            }
            break;
            default:
              valid = false;
              break;
          }

          // Light Square
          for(byte i = 0; i < 100; i++)
          {
              if(kronkField[i] == SEA)
              {
                kronk.setPixelColor(positionMap(i), blue);
              }
              else
              {
                kronk.setPixelColor(positionMap(i), purple);
              }
          }

          for(byte i = 0; i < 5; i++)
          {
            if(lightList[i] < 100 && lightList[i] >= 0)
            {
              if(valid)
              {
                kronk.setPixelColor(positionMap(lightList[i]), green);
              }
              else
              {
                kronk.setPixelColor(positionMap(lightList[i]), red);
              }
            }
          }
          
          kronk.show();

          if(posChange)
          {
            delay(400);
          }

          if(valid)
          {
            select = digitalRead(SELECT);
            Serial.println(select);
            if(digitalRead(SELECT) ==0)
            {
              Serial.println("Select Pressed");
              delay(100);
              select = digitalRead(SELECT);
              if(select == 0)
              {
                Serial.println("Select Valid");
                switch(j)
                {
                  case 0:
                      for(byte t = 0; t < 5; t++)
                      {
                        kronkField[lightList[t]] = LARGE_SHIP;
                      }
                      break;
                  case 1:
                      for(byte t = 0; t < 4; t++)
                      {
                        kronkField[lightList[t]] = MEDIUM_A_SHIP;
                      }
                      break;
                  case 2:
                      for(byte t = 0; t < 4; t++)
                      {
                        kronkField[lightList[t]] = MEDIUM_B_SHIP;
                      }
                      break;
                  case 3:
                      for(byte t = 0; t < 3; t++)
                      {
                        kronkField[lightList[t]] = SMALL_SHIP;
                      }
                      break;
                  default:
                    break;
                }
              }
            }
          }
          else
          {
            select = 1;
          }

          delay(100);
        }
        }
        Serial.println("Placed is true");
        kronkPlaced = true;
        // Light Square
          for(byte i = 0; i < 100; i++)
          {
              if(kronkField[i] == SEA)
              {
                kronk.setPixelColor(positionMap(i), blue);
              }
              else
              {
                kronk.setPixelColor(positionMap(i), purple);
              }
          }
          kronk.show();
      }
      else
      {
        Serial.println("Placed");
        delay(500);
      }
      
      if(!kronkPlaced)
      {
        if(digitalRead(KRONK_OR_HADES))
        {
          state = KRONK_PLACE;
        }
        else
        {
          state = KRONK_PLACE;
        }
      }
      else
      {
        state = READY;
      }
      break;

    case READY:
        for(byte i = 0; i < 100; i++)
        {
          hades.setPixelColor(i, blue);
          kronk.setPixelColor(i, blue);
        }
        kronk.show();
        hades.show();
        delay(2500);
        state = HADES_TURN;
      break;
    case HADES_TURN:
      if(digitalRead(KRONK_OR_HADES))
      {
        int select =1;
        byte x, y = 0;
         while(select != 0)
        {
          bool posChange = false;
          bool valid = true;
          int xCmd = analogRead(X_MOVE);
          int yCmd = analogRead(Y_MOVE);
          int lightList[5] = {101, 101, 101, 101, 101};

          // Move cursor
          if(xCmd < 50 && x > 0)
          {
            x--;
            posChange = true;
          }
          else if (xCmd > 205 && x < 9)
          {
            x++;
            posChange = true;
          }

          if(yCmd < 50 && y > 0)
          {
            y--;
            posChange = true;
          }
          else if (yCmd > 205 && y < 9)
          {
            y++;
            posChange = true;
          }

          if(digitalRead(SELECT) == 0)
          {
              delay(100);
              select = digitalRead(SELECT);
              if(select ==0)
              {
                byte pos = 10*x+y;
                switch(kronkField[pos])
                {
                  case SEA:
                    kronkField[pos] = MISS;
                    break;
                  case SMALL_SHIP:
                     kronkShipHealth[3] -= 1;
                     kronkField[pos] = HIT;
                     break;
                   case MEDIUM_B_SHIP:
                     kronkShipHealth[2] -= 1;
                     kronkField[pos] = HIT;
                     break;
                   case MEDIUM_A_SHIP:
                     kronkShipHealth[1] -= 1;
                     kronkField[pos] = HIT;
                     break;
                   case LARGE_SHIP:
                     kronkShipHealth[0] -= 1;
                     kronkField[pos] = HIT;
                     break;
                }
              }
          }

              
          // Light Square
          for(byte i = 0; i < 100; i++)
          {
              if (kronkField[i] == HIT)
              {
                kronk.setPixelColor(i, red);
              }
              else if (kronkField[i] == MISS)
              {
                kronk.setPixelColor(i, white);
              }
              else
              {
                kronk.setPixelColor(i, blue);
              }
          }

          for(byte i = smallShip; i < smallShip + 3; i++)
          {
            byte health = kronkShipHealth[3];
            if(health > 0)
            {
              kronk.setPixelColor(i, white);
              health--;
            }
            else
            {
              kronk.setPixelColor(i, red);
            }
          }


          for(byte i = mediumShip1; i < mediumShip1 + 4; i++)
          {
            byte health = kronkShipHealth[2];
            if(health > 0)
            {
              kronk.setPixelColor(i, white);
              health--;
            }
            else
            {
              kronk.setPixelColor(i, red);
            }
          }

          for(byte i = mediumShip2; i < mediumShip2 + 4; i++)
          {
            byte health = kronkShipHealth[1];
            if(health > 0)
            {
              kronk.setPixelColor(i, white);
              health--;
            }
            else
            {
              kronk.setPixelColor(i, red);
            }
          }

          for(byte i = largeShip; i < largeShip + 5; i++)
          {
            byte health = kronkShipHealth[0];
            if(health > 0)
            {
              kronk.setPixelColor(i, white);
              health--;
            }
            else
            {
              kronk.setPixelColor(i, red);
            }
          }
          hades.show();
          
        }
      }
      else
      {
        delay(200);
      }
      if(kronkShipHealth[0] == 0 && kronkShipHealth[1] == 0 && kronkShipHealth[2] == 0 && kronkShipHealth[3] == 0)
      {
        state = END;
      }
      break;
    case END:
      if(kronkShipHealth[0] == 0 && kronkShipHealth[1] == 0 && kronkShipHealth[2] == 0 && kronkShipHealth[3] == 0)
      {
        for(byte i = 0; i < 100; i++)
        {
          hades.setPixelColor(i, winner[i]);
          kronk.setPixelColor(i, loser[i]);
        }
      }
      else
      {
        for(byte i = 0; i < 100; i++)
        {
          kronk.setPixelColor(i, winner[i]);
          hades.setPixelColor(i, loser[i]);
        }
      }
      kronk.show();
      hades.show();
      while(1)
      {
        delay(5000);
      }
      break;
    default:
      state = STARTUP;
      break;
  }



}

byte positionMap(int pos)
{
  if(pos < 0 || pos > 99)
  {
    return 101;
  }
  return ledSnake[pos];
}

