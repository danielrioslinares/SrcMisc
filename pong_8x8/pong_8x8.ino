//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%% [DESCRIPTION:PONG MADE WITH A LED MATRIX, 4 BUTTONS AND A LCD DISPLAY] //%%//%%//%%//%%//%%//%%
//%%//%%//%%//%% [AUTHOR:DANIEL RÍOS LINARES] %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%% [DEVELOPMENT HOURS:~33HOURS] %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%

//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%  [Setup] %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
#include "EEPROM.h" //EEPROM library

#include "LedControl.h" // MAX7219 library
 LedControl lc=LedControl(8,10,9,1);
 //LedControl lc=LedControl(8,7,9,1); LOAD TO BOARD
  #define SCROLL_PONG {0x7E, 0x48, 0x48, 0x78, 0x00, 0x3C, 0x42, 0x42, 0x3C, 0x00, 0x7E, 0x30, 0x0C, 0x7E, 0x00, 0x3C, 0x42, 0x4A, 0x2C, 0x00}
  int SCROLL_REFERENCE[50]={26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16,-17,-18,-19,-20,-21,-22,-23};

#include "LiquidCrystal.h" // LCD library
 LiquidCrystal lcd(7,6,5,4,3,2);
 //LiquidCrystal lcd(12,11,10,5,4,3,2); LOAD TO BOARD
  byte right_arrow[8] = {B00000, B00100, B00110, B11111, B11111, B00110, B00100, B00000};
  int MENU_CURSORLVL1=0; int MENU_CURSORLVL2=0; int MENU_CURSORLVL3=0; // cursor for menu
  int OLD_MENU_CURSORLVL1=0; int OLD_MENU_CURSORLVL2=0; int OLD_MENU_CURSORLVL3=0; // old cursor for menu
  int MENU_PAGE=1; int OLD_MENU_PAGE=1; // page of the menu
  bool MENU_REFRESH=0; // refresh menu
  int MENU_DISPLAY; // delay (speed) of the ball displayed
  
  int START_GAME1P=0; int START_GAME2P=0; // start the round
  int DIFFICULTY; // difficulty of the solo game version
  
  bool PAUSE; // pause game
  int PAUSE_OFF=0;

  int BALL_PERMISSION; int OLD_BALL_PERMISSION; // multiplier of the speed ball (CPU load decrease)
  int BALL_ANGLE; int OLD_BALL_ANGLE; // angle of the ball in the screen
  int BALL_X; int BALL_Y; int OLD_BALL_X; int OLD_BALL_Y; int OLD_OLD_BALL_X; int OLD_OLD_BALL_Y; // ball coordinates
  int BALL_DELAY=EEPROM.read(0x02); int OLD_BALL_DELAY; // ball speed (inverse)
  int BALL_TRACK=EEPROM.read(0x01);
  int P1_SCORE; int OLD_P1_SCORE; int P2_SCORE; int OLD_P2_SCORE; // score
  
  int P1_POS; int OLD_P1_POS; int P2_POS; int OLD_P2_POS; // position of the players
  int P1_LENGTH; int P2_LENGTH; // length of the players
  bool P1_PERMISSION; bool P2_PERMISSION; // loop player 1 control

  bool CHEAT_ACTIVATED=0; // activate cheats & tricks
  bool CHEAT_ENDLESS; // extra length cheats
  bool CHEAT_BALL_DELAY; // maximum speed
  bool CHEAT_CPUDUEL;
  

void setup() {
  lc.shutdown(0,false); // LED matrix setup
  lc.setIntensity(0,5);
  lc.clearDisplay(0);
  lcd.begin(16,2); // LCD display setup
  lcd.createChar(0, right_arrow);
  pinMode(A0, INPUT_PULLUP); pinMode(A1, INPUT_PULLUP); pinMode(A2, INPUT_PULLUP); pinMode(A3, INPUT_PULLUP); pinMode(A4, INPUT_PULLUP);
}

//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%  [Loop]  %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%

void loop() {
  ScrollPongText();
  P1_SCORE=0; P2_SCORE=0;
  if(digitalRead(A1)==LOW) {if(digitalRead(A2)==LOW) {CHEAT_ACTIVATED=1;}}
  if(CHEAT_ACTIVATED==0) {if(BALL_DELAY<10) {BALL_DELAY=10;}}
  LCDMenu();
    lcd.clear();
    delay(1000);
    lcd.setCursor(0,0); lcd.print("  P1 "); lcd.print(P1_SCORE); lcd.print(" - "); lcd.print(P2_SCORE); lcd.print(" P2");
  while(START_GAME1P==1) {GameSolo();} // 1 player game
  while(START_GAME2P==1) {GameVersus();} // 2 player game
}
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%  [LCD Menu]  %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%

void LCDMenu() {
  while(START_GAME2P+START_GAME1P==0) {
    if(MENU_PAGE==1) {LCDMenuLvl1();}
    if(MENU_PAGE==2) {LCDMenuLvl2();}
    if(MENU_PAGE==3) {LCDMenuLvl3();}
  }
}

void LCDMenuLvl1() { // Page 1 of the menu
  if(OLD_MENU_CURSORLVL1!=MENU_CURSORLVL1) {lcd.clear();}
  lcd.setCursor(0,0); lcd.write(byte(0));
  if(MENU_CURSORLVL1==0) {lcd.setCursor(1,0); lcd.print("1 player"); lcd.setCursor(0,1); lcd.print("2 players");}
  if(MENU_CURSORLVL1==1) {lcd.setCursor(1,0); lcd.print("2 players"); lcd.setCursor(0,1); lcd.print("Options");}
  if(MENU_CURSORLVL1==2) {lcd.setCursor(1,0); lcd.print("Options");}
  OLD_MENU_CURSORLVL1=MENU_CURSORLVL1; OLD_MENU_PAGE=MENU_PAGE;
  while(digitalRead(A0)==LOW) {MENU_CURSORLVL1=OLD_MENU_CURSORLVL1-1;}
  while(digitalRead(A1)==LOW) {MENU_CURSORLVL1=OLD_MENU_CURSORLVL1+1;}
  while(digitalRead(A2)==LOW) {}
  while(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE+1; OLD_MENU_CURSORLVL2=-1;}
  if(MENU_CURSORLVL1<0) {MENU_CURSORLVL1=2;}
  if(MENU_CURSORLVL1>2) {MENU_CURSORLVL1=0;}
}

void LCDMenuLvl2() { //Page 2 of the menu
  if(MENU_CURSORLVL1==0) {
    if(OLD_MENU_CURSORLVL2!=MENU_CURSORLVL2) {lcd.clear(); lc.clearDisplay(0);}
    lcd.setCursor(0,0); lcd.print("Platform size:");
    lcd.setCursor(0,1); lcd.write(byte(0));
    if(MENU_CURSORLVL2==0) {lcd.setCursor(1,1); lcd.print("Tiny (2)");}
    if(MENU_CURSORLVL2==1) {lcd.setCursor(1,1); lcd.print("Normal (3)");}
    if(MENU_CURSORLVL2==2) {lcd.setCursor(1,1); lcd.print("Large (4)");}
    if(CHEAT_ENDLESS==1) {if(MENU_CURSORLVL2==3) {lcd.setCursor(1,1); lcd.print("Gigant (8!)");}}
    OLD_MENU_CURSORLVL2=MENU_CURSORLVL2; OLD_MENU_PAGE=MENU_PAGE;
    while(digitalRead(A0)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2-1;}
    while(digitalRead(A1)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2+1;}
    while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL1=-1;}
    while(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE+1; if(CHEAT_ENDLESS==0) {P1_POS=(7-P1_LENGTH)/2+1; P2_POS=(7-P2_LENGTH)/2+1; P1_LENGTH=MENU_CURSORLVL2+2; P2_LENGTH=MENU_CURSORLVL2+2;} if(CHEAT_ENDLESS==1) {P1_POS=0; P2_POS=0; P1_LENGTH=8; P2_LENGTH=8;} OLD_BALL_X=6; OLD_BALL_Y=(7-P1_LENGTH)/2+1; BALL_X=1; BALL_Y=(7-P1_LENGTH)/2+1; BALL_ANGLE=2; P1_SCORE=0; P2_SCORE=0; lcd.clear(); RenderPlatform();}
    if(MENU_CURSORLVL2<0) {MENU_CURSORLVL2=2+CHEAT_ENDLESS;}
    if(MENU_CURSORLVL2>2+CHEAT_ENDLESS) {MENU_CURSORLVL2=0;}
  }
  if(MENU_CURSORLVL1==1) {
    if(OLD_MENU_CURSORLVL2!=MENU_CURSORLVL2) {lcd.clear();}
    lcd.setCursor(0,0); lcd.print("Platform size:");
    lcd.setCursor(0,1); lcd.write(byte(0));
    if(MENU_CURSORLVL2==0) {lcd.setCursor(1,1); lcd.print("Tiny (2)");}
    if(MENU_CURSORLVL2==1) {lcd.setCursor(1,1); lcd.print("Normal (3)");}
    if(MENU_CURSORLVL2==2) {lcd.setCursor(1,1); lcd.print("Large (4)");}
    if(CHEAT_ENDLESS==1) {if(MENU_CURSORLVL2==3) {lcd.setCursor(1,1); lcd.print("Gigant (8!)");}}
    OLD_MENU_CURSORLVL2=MENU_CURSORLVL2; OLD_MENU_PAGE=MENU_PAGE;
    while(digitalRead(A0)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2-1;}
    while(digitalRead(A1)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2+1;}
    while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL1=-1;}
    while(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE+1; if(CHEAT_ENDLESS==0) {P1_POS=(7-P1_LENGTH)/2+1; P2_POS=(7-P2_LENGTH)/2+1; P1_LENGTH=MENU_CURSORLVL2+2; P2_LENGTH=MENU_CURSORLVL2+2;} if(CHEAT_ENDLESS==1) {P1_POS=0; P2_POS=0; P1_LENGTH=8; P2_LENGTH=8;} OLD_BALL_X=1; OLD_BALL_Y=(7-P1_LENGTH)/2+1; BALL_X=1; BALL_Y=(7-P1_LENGTH)/2+1; BALL_ANGLE=2; P1_SCORE=0; P2_SCORE=0; START_GAME2P=1; lcd.clear(); RenderPlatform();}
    if(MENU_CURSORLVL2<0) {MENU_CURSORLVL2=2+CHEAT_ENDLESS;}
    if(MENU_CURSORLVL2>2+CHEAT_ENDLESS) {MENU_CURSORLVL2=0;}
  }
  
  if(MENU_CURSORLVL1==2) {
    if(OLD_MENU_CURSORLVL2!=MENU_CURSORLVL2) {lcd.clear();}
    lcd.setCursor(0,0); lcd.write(byte(0));
    if(MENU_CURSORLVL2==0) {lcd.setCursor(1,0); lcd.print("Ball speed"); lcd.setCursor(0,1); lcd.print("Ball Tracking"); lc.clearDisplay(0);}
    if(MENU_CURSORLVL2==1) {lcd.setCursor(1,0); lcd.print("Ball Tracking"); if(CHEAT_ACTIVATED==1) {lcd.setCursor(0,1); lcd.print("Cheats"); lc.clearDisplay(0);}}
    if(MENU_CURSORLVL2==2) {lcd.setCursor(1,0); lcd.print("Cheats");}
    OLD_MENU_CURSORLVL2=MENU_CURSORLVL2; OLD_MENU_PAGE=MENU_PAGE;
    while(digitalRead(A0)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2-1;}
    while(digitalRead(A1)==LOW) {MENU_CURSORLVL2=OLD_MENU_CURSORLVL2+1;}
    while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL1=-1;}
    while(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE+1; P1_LENGTH=8; P2_LENGTH=8; P1_POS=0; P2_POS=0; OLD_BALL_X=3; OLD_BALL_Y=3; BALL_X=3; BALL_Y=3; BALL_ANGLE=3; if(MENU_CURSORLVL2==1) {MENU_CURSORLVL3=EEPROM.read(0x01);} if(MENU_CURSORLVL2==0) {MENU_CURSORLVL3=EEPROM.read(0x02);} lcd.clear();}
    if(MENU_CURSORLVL2<0) {MENU_CURSORLVL2=1+CHEAT_ACTIVATED;}
    if(MENU_CURSORLVL2>1+CHEAT_ACTIVATED) {MENU_CURSORLVL2=0;}
  }
}

void LCDMenuLvl3() { //Page 3 of the menu
  if(MENU_CURSORLVL1==0) {
    if(OLD_MENU_CURSORLVL3!=MENU_CURSORLVL3) {lcd.clear();}
    lcd.setCursor(0,0); lcd.print("Cpu skills:");
    lcd.setCursor(0,1); lcd.write(byte(0));
    if(MENU_CURSORLVL3==0) {lcd.setCursor(1,1); lcd.print("Easy");}
    if(MENU_CURSORLVL3==1) {lcd.setCursor(1,1); lcd.print("Normal");}
    if(MENU_CURSORLVL3==2) {lcd.setCursor(1,1); lcd.print("Hard");}
    if(MENU_CURSORLVL3==3) {lcd.setCursor(1,1); lcd.print("Impossible");}
    OLD_MENU_CURSORLVL3=MENU_CURSORLVL3; OLD_MENU_PAGE=MENU_PAGE;
    while(digitalRead(A0)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3-1;}
    while(digitalRead(A1)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3+1;}
    while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL2=-1; lcd.clear();}
    while(digitalRead(A3)==LOW) {DIFFICULTY=MENU_CURSORLVL3; START_GAME1P=1;}
    if(MENU_CURSORLVL3<0) {MENU_CURSORLVL3=3;}
    if(MENU_CURSORLVL3>3) {MENU_CURSORLVL3=0;}
  }
  
  if(MENU_CURSORLVL1==2) {
    if(MENU_CURSORLVL2==0) {
      lcd.setCursor(0,0); lcd.print("Up & Down"); lcd.setCursor(0,1); lcd.print("Velocity: "); lcd.print(BALL_DELAY); MENU_DISPLAY=BALL_DELAY;
      OLD_BALL_DELAY=BALL_DELAY;
      if(digitalRead(A0)==LOW) {
        if(CHEAT_BALL_DELAY==0) {if(BALL_DELAY>10) {BALL_DELAY=OLD_BALL_DELAY-1; BALL_PERMISSION=BALL_DELAY-1;}}
        if(CHEAT_BALL_DELAY==1) {if(BALL_DELAY>1) {BALL_DELAY=OLD_BALL_DELAY-1; BALL_PERMISSION=BALL_DELAY-1;}}
      }
      if(digitalRead(A1)==LOW) {if(BALL_DELAY<30) {BALL_DELAY=OLD_BALL_DELAY+1; BALL_PERMISSION=BALL_DELAY-1;}}
      if(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL1=-1; EEPROM.write(0x02, BALL_DELAY);}
      GameDemo();
    }
    if(MENU_CURSORLVL2==1) {
      if(OLD_MENU_CURSORLVL3!=MENU_CURSORLVL3) {lcd.clear();}
      lcd.setCursor(0,0); lcd.print("Ball tracking:");
      lcd.setCursor(0,1); lcd.write(byte(0));
      if(MENU_CURSORLVL3==0) {lcd.setCursor(1,1); lcd.print("No");}
      if(MENU_CURSORLVL3==1) {lcd.setCursor(1,1); lcd.print("Yes");}
      if(MENU_CURSORLVL3==2) {lcd.setCursor(1,1); lcd.print("Double");}
      OLD_MENU_CURSORLVL3=MENU_CURSORLVL3; OLD_MENU_PAGE=MENU_PAGE;
      while(digitalRead(A0)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3-1;}
      while(digitalRead(A1)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3+1;}
      while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL2=-1; MENU_CURSORLVL2=0; lcd.clear(); EEPROM.write(0x01, MENU_CURSORLVL3); BALL_TRACK=MENU_CURSORLVL3;}
      while(digitalRead(A3)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL2=-1; MENU_CURSORLVL2=0; lcd.clear(); EEPROM.write(0x01, MENU_CURSORLVL3); BALL_TRACK=MENU_CURSORLVL3;}
      BALL_TRACK=MENU_CURSORLVL3;
      if(MENU_CURSORLVL3<0) {MENU_CURSORLVL3=2;}
      if(MENU_CURSORLVL3>2) {MENU_CURSORLVL3=0;}
      GameDemo();
    }
    if(MENU_CURSORLVL2==2) {
      if(OLD_MENU_CURSORLVL3!=MENU_CURSORLVL3) {lcd.clear();}
      lcd.setCursor(0,0); lcd.print("Cheats:");
      lcd.setCursor(0,1); lcd.write(byte(0));
      if(MENU_CURSORLVL3==0) {lcd.setCursor(1,1); if(CHEAT_ENDLESS==1) {lcd.print("(ON) ");} else {lcd.print("(OFF)");} lcd.print("Loop stuck");}
      if(MENU_CURSORLVL3==1) {lcd.setCursor(1,1); if(CHEAT_BALL_DELAY==1) {lcd.print("(ON) ");} else {lcd.print("(OFF)");} lcd.print("Overclock");}
      if(MENU_CURSORLVL3==2) {lcd.setCursor(1,1); if(CHEAT_CPUDUEL==1) {lcd.print("(ON) ");} else {lcd.print("(OFF)");} lcd.print("CPU Duel [1P]");}
      OLD_MENU_CURSORLVL3=MENU_CURSORLVL3; OLD_MENU_PAGE=MENU_PAGE;
      while(digitalRead(A0)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3-1;}
      while(digitalRead(A1)==LOW) {MENU_CURSORLVL3=OLD_MENU_CURSORLVL3+1;}
      while(digitalRead(A2)==LOW) {MENU_PAGE=OLD_MENU_PAGE-1; OLD_MENU_CURSORLVL2=-1;}
      while(digitalRead(A3)==LOW) {
        if(MENU_CURSORLVL3==0) {if(CHEAT_ENDLESS==0) {CHEAT_ENDLESS=1;} else {CHEAT_ENDLESS=0;}}
        if(MENU_CURSORLVL3==1) {if(CHEAT_BALL_DELAY==0) {CHEAT_BALL_DELAY=1;} else {CHEAT_BALL_DELAY=0; BALL_DELAY=10; EEPROM.write(0x02, BALL_DELAY);}}
        if(MENU_CURSORLVL3==2) {if(CHEAT_CPUDUEL==0) {CHEAT_CPUDUEL=1;} else {CHEAT_CPUDUEL=0;}}
        while(digitalRead(A3)==LOW) {}
      }
      if(MENU_CURSORLVL3<0) {MENU_CURSORLVL3=2;}
      if(MENU_CURSORLVL3>2) {MENU_CURSORLVL3=0;}
    }
  }
}

void PauseMenu() {
  if(MENU_REFRESH==1) {lcd.clear(); MENU_REFRESH=0;}
  lcd.setCursor(0,0); lcd.print("Pause "); lcd.print("P1 "); lcd.print(P1_SCORE); lcd.print("-"); lcd.print(P2_SCORE); lcd.print(" P2");
  if(digitalRead(A4)==LOW) {PAUSE=0; PAUSE_OFF=1; while(digitalRead(A4)==LOW) {lcd.clear();}}
}

//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%// [LED Matrix] //%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%

void GameSolo() {

  if(DIFFICULTY==0) {
    if(BALL_PERMISSION==BALL_DELAY) {
      if(BALL_Y<7-P2_LENGTH+2) {
        if(BALL_Y>1) {if(BALL_X<5) {if(P2_POS!=BALL_Y) {P2_POS=BALL_Y+random(-1,1);}}}
      }
    }
    if(CHEAT_CPUDUEL==1) {
      if(BALL_PERMISSION==BALL_DELAY) {
        if(BALL_Y<7-P1_LENGTH+2) {
          if(BALL_Y>1) {if(BALL_X<5) {if(P1_POS!=BALL_Y) {P1_POS=BALL_Y+random(-1,1);}}}
        }
      }
    }
  }
  
  if(DIFFICULTY==1) {
    if(BALL_PERMISSION==BALL_DELAY) {
      if(BALL_Y<7-P2_LENGTH+2) {
        if(BALL_Y>1) {if(P2_POS!=BALL_Y) {P2_POS=BALL_Y+random(-1,1);}}
      }
    }
    if(CHEAT_CPUDUEL==1) {
      if(BALL_PERMISSION==BALL_DELAY) {
        if(BALL_Y<7-P1_LENGTH+2) {
          if(BALL_Y>1) {if(P1_POS!=BALL_Y) {P1_POS=BALL_Y+random(-1,1);}}
        }
      }
    }
  }

  if(DIFFICULTY==2) {
    if(BALL_PERMISSION==BALL_DELAY) {
      if(BALL_Y<7-P2_LENGTH+2) {
        if(BALL_Y>1) {if(P2_POS!=BALL_Y) {P2_POS=BALL_Y-1;}}
      }
    }
    if(CHEAT_CPUDUEL==1) {
      if(BALL_PERMISSION==BALL_DELAY) {
        if(BALL_Y<7-P1_LENGTH+2) {
          if(BALL_Y>1) {if(P1_POS!=BALL_Y) {P1_POS=BALL_Y-1;}}
        }
      }
    }
  }

  
  if(DIFFICULTY==3) {
    if(BALL_Y<7-P2_LENGTH+2) {P2_POS=BALL_Y;}
    if(CHEAT_CPUDUEL==1) {if(BALL_Y<7-P1_LENGTH+2) {P1_POS=BALL_Y;}}
  }
  GameVersus();
}

void GameVersus() {
  MovingPlatform();
  if(digitalRead(A4)==LOW) {PAUSE=1; MENU_REFRESH=1; while(digitalRead(A4)==LOW) {}}
  while(PAUSE==1) {PauseMenu();}
  ScoreCount();
  if(PAUSE_OFF==1) {lcd.setCursor(0,0); lcd.print("  P1 "); lcd.print(P1_SCORE); lcd.print(" - "); lcd.print(P2_SCORE); lcd.print(" P2"); PAUSE_OFF=0;}
  if(BALL_PERMISSION==BALL_DELAY) {
    MovingBall();
    ScoreCount();
    BALL_PERMISSION=0;
  }
  RenderBall();
  delay(3);
  BALL_PERMISSION=BALL_PERMISSION+1;
  lc.clearDisplay(0);
}

void GameDemo() {
  MovingPlatform();
    if(BALL_PERMISSION==BALL_DELAY) {
      lc.clearDisplay(0); RenderPlatform();
      MovingBall();
      BALL_PERMISSION=0;
      RenderBall();
    }
    OLD_BALL_PERMISSION=BALL_PERMISSION;
    RenderBall();
    delay(3);
    BALL_PERMISSION=OLD_BALL_PERMISSION+1;
    if(MENU_DISPLAY!=BALL_DELAY) {if(BALL_DELAY<10 ){lcd.clear();}}
}

void MovingPlatform() {
  RenderPlatform();
  OLD_P1_POS=P1_POS;
  OLD_P2_POS=P2_POS;
  if(P1_PERMISSION==1) {
    if(digitalRead(A0)==LOW) {if(P1_POS>0) {P1_POS=OLD_P1_POS-1; P1_PERMISSION=0;}}
    if(digitalRead(A1)==LOW) {if(P1_POS<7-P1_LENGTH+1) {P1_POS=OLD_P1_POS+1; P1_PERMISSION=0;}}
  }
  if(P2_PERMISSION==1) {
    if(digitalRead(A2)==LOW) {if(P2_POS>0) {P2_POS=OLD_P2_POS-1; P2_PERMISSION=0;}}
    if(digitalRead(A3)==LOW) {if(P2_POS<7-P2_LENGTH+1) {P2_POS=OLD_P2_POS+1; P2_PERMISSION=0;}}
  }
  if(digitalRead(A0)==HIGH) {if(digitalRead(A1)==HIGH) P1_PERMISSION=1;}
  if(START_GAME1P==0) {if(digitalRead(A2)==HIGH) {if(digitalRead(A3)==HIGH) P2_PERMISSION=1;}}
}

void MovingBall(){
  OLD_OLD_BALL_X=OLD_BALL_X; OLD_OLD_BALL_Y=OLD_BALL_Y;
  OLD_BALL_X=BALL_X; OLD_BALL_Y=BALL_Y; OLD_BALL_ANGLE=BALL_ANGLE;
  // wall collision
  if(BALL_ANGLE<0) {
    if(BALL_Y==0) {BALL_ANGLE=OLD_BALL_ANGLE-2;}
    if(BALL_Y==7) {BALL_ANGLE=OLD_BALL_ANGLE+2;}
  }
  if(BALL_ANGLE>0) {
    if(BALL_Y==0) {BALL_ANGLE=OLD_BALL_ANGLE+2;}
    if(BALL_Y==7) {BALL_ANGLE=OLD_BALL_ANGLE-2;}
  }
  // platform collision
  if(BALL_X==6) {
    if(P1_POS==BALL_Y) {
      if(BALL_ANGLE==-1) {BALL_ANGLE=1;}
      if(BALL_ANGLE==-2) {BALL_ANGLE=1;}
      if(BALL_ANGLE==-3) {BALL_ANGLE=2;}
      if(BALL_Y==7) {BALL_ANGLE=1;}
      if(BALL_Y==0) {BALL_ANGLE=3;}
    }
    if(P1_LENGTH>2) {
      for(int i=1; i<P1_LENGTH-1; i++) {
        if(BALL_Y==P1_POS+i) {
          if(BALL_ANGLE==-1) {BALL_ANGLE=1;}
          if(BALL_ANGLE==-2) {BALL_ANGLE=2;}
          if(BALL_ANGLE==-3) {BALL_ANGLE=3;}
        }
      }
    }
    if(P1_POS+P1_LENGTH-1==BALL_Y) {
      if(BALL_ANGLE==-1) {BALL_ANGLE=2;}
      if(BALL_ANGLE==-2) {BALL_ANGLE=3;}
      if(BALL_ANGLE==-3) {BALL_ANGLE=3;}
      if(BALL_Y==7) {BALL_ANGLE=1;}
      if(BALL_Y==0) {BALL_ANGLE=3;}
    }
  }
  if(BALL_X==1) {
    if(P2_POS==BALL_Y) {
      if(BALL_ANGLE==1) {BALL_ANGLE=-1;}
      if(BALL_ANGLE==2) {BALL_ANGLE=-1;}
      if(BALL_ANGLE==3) {BALL_ANGLE=-2;}
      if(BALL_Y==7) {BALL_ANGLE=-1;}
      if(BALL_Y==0) {BALL_ANGLE=-3;}
    }
    if(P2_LENGTH>2) {
      for(int i=1; i<P2_LENGTH-1; i++) {
        if(BALL_Y==P2_POS+i) {
          if(BALL_ANGLE==1) {BALL_ANGLE=-1;}
          if(BALL_ANGLE==2) {BALL_ANGLE=-2;}
          if(BALL_ANGLE==3) {BALL_ANGLE=-3;}
        }
      }
    }
    if(P2_POS+P2_LENGTH-1==BALL_Y) {
      if(BALL_ANGLE==1) {BALL_ANGLE=-2;}
      if(BALL_ANGLE==2) {BALL_ANGLE=-3;}
      if(BALL_ANGLE==3) {BALL_ANGLE=-3;}
      if(BALL_Y==7) {BALL_ANGLE=-1;}
      if(BALL_Y==0) {BALL_ANGLE=-3;}
    }
  }

  if(BALL_ANGLE==1) {BALL_X=OLD_BALL_X-1; BALL_Y=OLD_BALL_Y-1;} // moving function of angle
  if(BALL_ANGLE==2) {BALL_X=OLD_BALL_X-1; BALL_Y=OLD_BALL_Y;}
  if(BALL_ANGLE==3) {BALL_X=OLD_BALL_X-1; BALL_Y=OLD_BALL_Y+1;}
  if(BALL_ANGLE==0) {BALL_X=OLD_BALL_X; BALL_Y=OLD_BALL_Y;}
  if(BALL_ANGLE==-1) {BALL_X=OLD_BALL_X+1; BALL_Y=OLD_BALL_Y-1;}
  if(BALL_ANGLE==-2) {BALL_X=OLD_BALL_X+1; BALL_Y=OLD_BALL_Y;}
  if(BALL_ANGLE==-3) {BALL_X=OLD_BALL_X+1; BALL_Y=OLD_BALL_Y+1;}
}

void RenderPlatform() {
  for(int i=P1_POS; i<P1_POS+P1_LENGTH; i++) {lc.setLed(0,7,i, true);}
  for(int j=P2_POS; j<P2_POS+P2_LENGTH; j++) {lc.setLed(0,0,j, true);}
}

void RenderBall() {
  if(BALL_TRACK>1) {lc.setLed(0,OLD_OLD_BALL_X,OLD_OLD_BALL_Y, true);}
  if(BALL_TRACK>0) {lc.setLed(0,OLD_BALL_X,OLD_BALL_Y, true);}
  lc.setLed(0,BALL_X,BALL_Y, true);
}

void ScoreCount() {
  OLD_P1_SCORE=P1_SCORE; OLD_P2_SCORE=P2_SCORE;
  if(BALL_X==0) {P1_SCORE=OLD_P1_SCORE+1; delay(1000); BALL_ANGLE=2; BALL_X=6; BALL_Y=(7-P1_LENGTH)/2+2; OLD_BALL_X=6; OLD_BALL_Y=(7-P1_LENGTH)/2+2; P1_POS=(7-P1_LENGTH)/2+1; P2_POS=(7-P2_LENGTH)/2+1; MENU_REFRESH=1;}
  if(BALL_X==7) {P2_SCORE=OLD_P2_SCORE+1; delay(1000); BALL_ANGLE=-2; BALL_X=1; BALL_Y=(7-P2_LENGTH)/2+2; OLD_BALL_X=1; OLD_BALL_Y=(7-P2_LENGTH)/2+2; P1_POS=(7-P1_LENGTH)/2+1; P2_POS=(7-P2_LENGTH)/2+1; MENU_REFRESH=1;}
  if(MENU_REFRESH==1) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("  P1 "); lcd.print(P1_SCORE); lcd.print(" - "); lcd.print(P2_SCORE); lcd.print(" P2");
    delay(1000);
    MENU_REFRESH=0;
  }
  if(P1_SCORE==3) {lcd.clear(); lcd.print("P1 WINS!"); delay(1000); START_GAME1P=0; START_GAME2P=0; MENU_CURSORLVL1=0; MENU_CURSORLVL2=0; MENU_CURSORLVL3=0;}
  if(P2_SCORE==3) {lcd.clear(); lcd.print("P2 WINS!"); delay(1000); START_GAME1P=0; START_GAME2P=0; MENU_CURSORLVL1=0; MENU_CURSORLVL2=0; MENU_CURSORLVL3=0;}
}

//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%% [Others] %%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%
//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%//%%

void ScrollPongText() {
  byte PHRASE_PONG[1][20]={SCROLL_PONG};
  for (int i=0; i<sizeof(PHRASE_PONG)/20; i++){
    for (int j=50; j>=20; j--) {
      for (int k=0; k<20; k++){lc.setRow(0,SCROLL_REFERENCE[k+j-20],PHRASE_PONG[i][k]);} // render column i and row k moving left with j
      delay(50); // animation delay speed
      lc.clearDisplay(0);
    }
    if(i == sizeof(PHRASE_PONG)/20) i=0;
  }
}
