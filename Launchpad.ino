/*
  Arduino x Ableton Launchpad Interaction
  Harlan Batchelor
  
  Required libraries:
  USB_Host_Shield_2.0 - https://github.com/felis/USB_Host_Shield_2.0
  USBH_MIDI - https://github.com/YuuichiAkagawa/USBH_MIDI
  
  Use the left/right buttons to switch between programs.
*/

/* include libraries needed */
#include <SPI.h>
#include <usbhub.h>
#include <usbh_midi.h>

USB Usb;
USBH_MIDI Midi(&Usb);

uint8_t green = 0x30;
uint8_t red = 0x03;
uint8_t orange = 0x13;
uint8_t yellow = 0x32;

boolean newInput = true;
uint8_t midiInput;

int currentProgram = 1;

/* Program specific variables */
/* Paint */
uint8_t currColor = green;
boolean refreshPaint = false;
uint8_t lp_paint[8][8][2]={
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}}
}; //these 2d arrays essentially store the state of each of the buttons on the 8x8 launchpad grid

/* Blocks */
uint8_t lp_blocks[8][8]={
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80}
};

/* Visualizer */
int col_values[8] = {0,0,0,0,0,0,0,0};

/* Keyboard */
int notes[8][8];
int frequency = 31;
int noteLength = 150;
boolean refreshKeyboard = false;
uint8_t sharps[12] = {0x80,0x80,0x90,0x80,0x90,0x80,0x80,0x90,0x80,0x90,0x80,0x90}; //keyboard pattern starting at B

/* Sequencer */
uint8_t lp_sequencer[8][8][2]={
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}},
  {{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00},{0x80, 0x00}}
};
int currCol = 0;
int delaySize = 300;
int delayDecrement = delaySize;
int tones[8] = {131,147,165,175,196,220,247,262};

void setup(){
  Serial.begin(115200); //midi baud rate
  if(Usb.Init() == -1){ //if the USB can't initialize
    while(1); //panic and stop existing
  }
  delay(200);
  for(int r=0;r<8;r++){
    for(int c=0;c<8;c++){
      Serial.println(frequency);
      notes[r][c] = frequency;
      frequency = (frequency * 1.059460646483)+0.5;
    }
  } 
}

/* Main program loop */
void loop(){
  Usb.Task();
  newInput = false;
  uint8_t buf[64]; //our raw midi data buffer
  uint16_t  rcvd;
  if(Usb.getUsbTaskState() == USB_STATE_RUNNING){
    /* If input was received and it was a "note on" signal */
     if((Midi.RecvData( &rcvd,  buf) == 0) && (buf[1] == 0x7f)){
       /* change what program is running. 6A is left and 6B is right. */
       if(buf[0] == 0x6A){
         if(currentProgram > 0){
           currentProgram--;
           if(currentProgram == 1){
             refreshPaint = true;
           }
           refreshKeyboard = true;
           wipe(); //clear launchpad for the next program
           Serial.println(currentProgram);
         }
       }else if(buf[0] == 0x6B){
         if(currentProgram < 5){
           currentProgram++;
           wipe();
           Serial.println(currentProgram);
         }
       }else{
         midiInput = buf[0]; //buf[0] is the button that was pressed
         newInput = true; //this will tell the program function whether or not to run input stuff
       }
     }
  }
  /* run the currently selected program. happens every loop. */
  switch(currentProgram){
           case 1:
           paint(midiInput, newInput, refreshPaint); //pass any input to the program function
           break;
           case 2:
           blocks(midiInput, newInput);
           break;
           case 3:
           visualizer();
           break;
           case 4:
           keyboard(midiInput, newInput, refreshKeyboard);
           break;
           case 5:
           sequencer(midiInput, newInput);
           break;
     }
}

/* Function to change launchpad button states */
void note(uint8_t note, uint8_t state, uint8_t color, int iter){
  uint8_t buf[3]; //create our midi buffer
  buf[0] = state; //note state (on/off)
  buf[1] = note; //note to play (light up in this case)
  buf[2] = color; //velocity of note (launchpad uses this for color)
  /* sometimes notes 'stick', so there's the option to send the data multiple times for reliability */
  for(int i = 0;i<iter;i++){
    Midi.SendData(buf);
  }
}

/* function to turn off all the launchpad's lights */
void wipe(){
  for(int i=0;i<0x7f;i++){
    note(i, 0x80, 0x00, 10);
  }
}


/* ------Program 1: Paint------ 
  Select the color from the buttons on the right,
  and simply tap a button in the 8x8 grid to make
  it the selected color. Tap a lit block while having
  the same color selected to erase it.
*/
void paint(uint8_t input, boolean newIn, boolean refresh){
  if(currentProgram == 1){
  /* light up our selection buttons */
  note(0x08, 0x90, red,5);
  note(0x18, 0x90, orange,5);
  note(0x28, 0x90, yellow,5);
  note(0x38, 0x90, green,5);
  note(0x78, 0x90, currColor, 5);
  if(refresh){
    for(int y=0;y<8;y++){
      for(int x=0;x<8;x++){
        note((y*16)+x,lp_paint[y][x][0],lp_paint[y][x][1],5);
      }
    }
    refresh= false;
  }
  if(newIn){ //if the input received is new
    if(input == 0x78){
      wipe();
      for(int y=0;y<8;y++){
        for(int x=0;x<8;x++){
          lp_paint[y][x][0] = 0x80; //off
          lp_paint[y][x][1] = 0x00; //blank color
        }
      }
      /* color selection */
    }else if(input == 0x08){
      currColor = red;
    }else if(input == 0x18){
      currColor = orange;
    }
    else if(input == 0x28){
      currColor = yellow;
    }
    else if(input == 0x38){
      currColor = green;
    }else{
      /* 'painting' */
      uint8_t state;
      uint8_t color;
      if(lp_paint[ input/16 ][ input%16 ][1] == currColor){ //if block is already the same color, turn it off
        state = 0x80; //off
        color = 0x00; //blank color
      }else{
        state = 0x90; //on
        color = currColor; //currently selected color
      }
      lp_paint[ input/16 ][ input%16 ][0] = state; //set the state of the pressed button
      lp_paint[ input/16 ][ input%16 ][1] = color; //set the color of the pressed button
      note(input, state, currColor, 15); //light up the button on the launchpad
    }
  }
  }
}


/*------Program 2: Blocks------
  A simple arcade-ish falling blocks demo.
  press any button to drop a block there.
  Blocks will stack, and any full horizontal
  line will dissapear. Like ghetto Tetris.
*/
void blocks(uint8_t input, boolean newIn){
  if(currentProgram == 2){
    int blocksInRow[8] = {0,0,0,0,0,0,0,0}; //keep track of blocks in each row (for full line)
    int freeze[8][8] = { //tell the for loop not to change blocks it has already changed when it moves to the next row
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0}
    };
    note(0x08, 0x90, green, 5);
      if(newIn){ //if we have new input
        lp_blocks[ input/16 ][ input%16 ] = 0x90; //set the input as pressed in the 2d array
        if(input == 0x08){ //clear the launchpad
          for(int y=0;y<8;y++){
            for(int x=0;x<8;x++){
              lp_blocks[y][x] = 0x80; //0x80 is the note off code
              note((y*16)+x,0x80,green,5);
            }
          }
        }
      }
      
      /* count the blocks in every row */
      for(int y=0;y<8;y++){
        for(int x=0;x<8;x++){
          if(lp_blocks[y][x] == 0x90){
            blocksInRow[y]++;
          }
        }
      }
      
      /* clear any full rows */
      for(int i=0;i<8;i++){
        if(blocksInRow[i] == 8){
          for(int r=0;r<8;r++){
            lp_blocks[i][r] = 0x80;
          }
        }
      }
      
      /* write frame */
      for(int y=0;y<8;y++){
        for(int x=0;x<8;x++){
          note((y*16)+x,lp_blocks[y][x],green,5); //(y*16)+x formula to change 2d array x/y to launchpad button number
          /* if the current block can still fall, move it down one*/
          if((lp_blocks[y][x] == 0x90) && (y < 7) && (freeze[y][x] != 1) && (lp_blocks[y+1][x] == 0x80)){
              lp_blocks[y][x] = 0x80;
              lp_blocks[y+1][x] = 0x90;
              freeze[y][x] = 0;
              freeze[y+1][x] = 1;
          }
        }
     }
  }
}


/*------Program 3: Visualizer------
  A simple program that can visualize music
  in a very basic form. Can also visualize basically
  anything that can be plugged into analog port 0, but
  it's geared toward audio.
*/
void visualizer(){
  if(currentProgram == 3){
   int mappedValue = map(analogRead(0)*5, 0, 255, 0, 7); //multiplying input by 4 to get larger values
   for(int i=0;i<7;i++){ //for every column
      col_values[i] = col_values[i+1]; //move the column left one
      column_vis(i); //display column
   }
        
   col_values[7] = mappedValue; //set out current (right-most) column
   column_vis(7);
  }
}
/* function to show columns for the visualizer */
void column_vis(int column){
  int height = col_values[column];
  for(int i=0;i<(8-height);i++){
    note((i*16)+column,0x80,0x00,5);
  }
  uint8_t init = 0x70;
  uint8_t color;
  
  /* light up each block in the column, with different colors depending on the block */
  while(height >= 0){
    if(init > 0x50){
      color = green; //bottom is green
    }
    else if(init > 0x30){
      color = yellow;
    }
    else if(init > 0x10){
      color = orange;
    }
    else{
      color = red; //top is red
    }
    note(init+column, 0x90, color,5);
    init -= 0x10;
    height--;
  }
}

/*------Program 4: Keyboard------
  Turns the launchpad into a simple
  keyboard to play tones
*/
void keyboard(uint8_t input, boolean newIn, boolean refresh){
  if(refresh){
    /* If the user just selected this program, show the keyboard pattern 
      (loop the keyboard pattern array to show lights until there aren't any more lights)
    */
    int lightIndex = 0;
    for(int r=0;r<8;r++){
      for(int c=0;c<8;c++){
        note((r*16)+c, sharps[lightIndex], orange, 5);
        if(lightIndex < 11){ //if we haven't reached the end of the pattern array
          lightIndex++; //increment the index
        }else{
          lightIndex = 0; //if we've reached the end of the pattern, start over at 0 to repeat
        }
      }
    }
    refreshKeyboard = false;
    lightIndex = 0;
  }
  if(newIn){ //if there's new input
      if(input == 0x69){ //if the down button is pressed
        if(noteLength > 0){
          noteLength -= 10;
        }else{
          noteLength = 10;
        }
      }else if(input == 0x68){ //if the up button is pressed
        noteLength += 10;
      }
      tone(8, notes[input/16][input%16], noteLength); //play a tone of the current length
      Serial.println(notes[input/16][input%16]);
  }
}

/*------Program 5: Sequencer------
  A simple line scrolling horizontally
  to play 'notes' that are on in the grid.
  Connect a buzzer to pin 8, or even normal
  speakers, to hear the tones.
*/
void sequencer(uint8_t input, boolean newIn){
  if(currentProgram == 5){
    if(newIn){
      if(input == 0x68){ //decrease the delay (speed up)
        if(delaySize > 2){
          delaySize-=50;
          delayDecrement = delaySize;
        }else{
          delaySize = 4; //make sure our delay isn't 0 so we don't get division errors
        }
      }else if(input == 0x69){ //increase the delay (slow down)
        delaySize+=50;
        delayDecrement = delaySize;
      }else if((lp_sequencer[ input/16 ][ input%16 ][1] != orange) && (input%16 != currCol)){
        note(input, 0x90, orange, 10);
        lp_sequencer[ input/16 ][ input%16 ][0] = 0x90;
        lp_sequencer[ input/16 ][ input%16 ][1] = orange;
      }else{
        note(input, 0x80, 0x00, 10);
        lp_sequencer[ input/16 ][ input%16 ][0] = 0x80;
        lp_sequencer[ input/16 ][ input%16 ][1] = 0x00;
      }
    }
    
     if(delayDecrement == 0){
       if(currCol == 0){
         column_seq(0, 0x90); //turn on current column
         column_seq(7, 0x80); //turn off previous column
         currCol++;
       }else if(currCol == 7){
         column_seq(7, 0x90);
         column_seq(6, 0x80);
         currCol = 0;
       }else{
         column_seq(currCol, 0x90);
         column_seq(currCol-1, 0x80);
         currCol++;
       }
       delayDecrement = delaySize;
     }else{
       delayDecrement--;
     }
  }
}
/* show column function for sequencer */
void column_seq(int col, uint8_t state){
  for(int i=0;i<8;i++){
    if(lp_sequencer[i][col][1] != orange){
      if(lp_sequencer[i][col][1] != red){
        note((i*16)+col, state, green, 10);
        lp_sequencer[i][col][1] = green;
      }else{
        //if the light was previously red, change it back to orange
        note((i*16)+col, 0x90, orange, 10);
        lp_sequencer[i][col][1] = orange;
      }
    }else{
      if(col == currCol){ //only show the note being played if it's in the current column. prevents odd behaviour.
        //if the light was previously orange, change to to red to show "note playing"
        note((i*16)+col, state, red, 10);
        lp_sequencer[i][col][1] = red;
        tone(8, tones[map(i,0,7,7,0)], (delaySize+2)/2);
      }
    }
  }
}
  
