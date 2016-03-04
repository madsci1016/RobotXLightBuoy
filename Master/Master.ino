// RobotX RGB Buoy Controller
// Handles receiving commands over network and pipes display data over serial ports to RGB panel drivers. 

// Written by Bill Porter
// www.billporter.info
// 

/********************************
 * Modifications by Gavin Taylor
 * date: 2/8/16
 *******************************/
 
/*
  This code is meant to be run on a 'Master Arduino' connected to 4 slave Arduino's acting as display backpacks. 
  Serial TX line on this Ardunio should be connected to the Serial RX line of the other 4 Arduinos.
  This Demo (used to) flashes out a sequence of solid Red, then Blue, then Green 3 times followed by a Game of Life simulation. 
  
*/


#include <EasyTransfer.h>


/*****************************
     SET COLOR PATTERN HERE
*****************************/
char pattern[] = "RGB";




byte num1[] = {0x00, 0x00, 0x21, 0x41, 0xFF, 0x01, 0x01, 0x00}; //  1
byte num2[] = {0x00, 0x01, 0x63, 0x85, 0x89, 0x71, 0x00, 0x00}; //  2
byte num3[] = {0x00, 0x00, 0x66, 0x81, 0x99, 0x7E, 0x00, 0x00}; //  3
byte num4[] = {0x00, 0x00, 0x18, 0x28, 0x48, 0xFF, 0x08, 0x00}; //  4


// Variables For game of Life
#define SIZEX 16
#define SIZEY 32

byte analogPin = A5;
byte col = 0;
//byte repeat[SIZEX][SIZEY];
byte world[SIZEX][SIZEY][2];
byte  DELAY = 1;
byte colors[] = {0b11100000, 0b00011100, 0b00000011, 0b11111100, 0b11100011, 0b00011111, 0b11111111};

//Get comms ready
//create objects
EasyTransfer ET1, ET2, ET3; 

//I'm gonna implement an adressing scheme. Panels will be address 1,2,3,4.
//Panels will display any data matching their address
//All panels will listen to address 0

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  byte address;
  byte displaydata[16][8];
} packet;


//a big buffer representing the whole screen. GFX functions will write out to this. 
byte displaybuffer[16][32]; 

//Adafruit_GFX gfx(16,32);

void drawPixel(int x, int y, int c){
  
  displaybuffer[x][y] = c;
  
}

//flashing state. 0 =  no everything off
int state;


void identify(int z) {
  
   /******
  *   -- displaying different bit masks based on bitRead...
  *   -- the 4 options are RED(0b225), GREEN(0b28), BLUE(0b3) and WHITE(0xFF) 
  */
  
    packet.address=1;
    packet.displaydata[0][0] = 0b11111100;
    
    for (int i =0; i< 8; i++){
      for (int y =0; y< 8; y++){
       if(bitRead(num1[i],y)) packet.displaydata[8-i+z][y] = 0b11100000;
       else packet.displaydata[8-i+z][y] = 0;
      }
    }
    
    ET1.sendData();
    ET2.sendData();
    ET3.sendData();
    //delay(5000);
    
    //delay(100);
    
    packet.address=2;
    for (int i =0; i< 8; i++){
      for (int y =0; y< 8; y++){
       if(bitRead(num2[i],y)) packet.displaydata[8-i+z][y] = 0b00011100;
       else packet.displaydata[8-i+z][y] = 0;
      }
    }
    
    ET1.sendData();
    ET2.sendData();
    ET3.sendData();
    
    packet.address=3;
    for (int i =0; i< 8; i++){
      for (int y =0; y< 8; y++){
       if(bitRead(num3[i],y)) packet.displaydata[8-i+z][y] = 0b00000011;
       else packet.displaydata[8-i+z][y] = 0;
      }
    }
    
    ET1.sendData();
    ET2.sendData();
   ET3.sendData();
    
    packet.address=4;
    for (int i =0; i< 8; i++){
      for (int y =0; y< 8; y++){
       if(bitRead(num4[i],y)) packet.displaydata[8-i+z][y] = 0b11111111;
       else packet.displaydata[8-i+z][y] = 0;
      }
    }
    
    ET1.sendData();
    ET2.sendData();
    ET3.sendData();
    
    Serial1.flush();
    Serial2.flush();
    Serial3.flush();
     //send sync trigger
      //We are going to send a Pulse down the serial line to tell all drivers to post the data they received. 
    Serial1.end();
    Serial2.end();
    Serial3.end();
    
    
    
    pinMode(18,OUTPUT); //serial 1
    pinMode(16,OUTPUT); //serial 2
    pinMode(14,OUTPUT); //serial 3
    
    digitalWrite(18,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(14,HIGH);
    
    delayMicroseconds(500);
    
    digitalWrite(18,LOW);
    digitalWrite(16,LOW);
    digitalWrite(14,LOW);
    
    delayMicroseconds(1500);
    
  
    
    //restart serial port
    Serial1.begin(57600);
    Serial2.begin(57600);
    Serial3.begin(57600);
  
    delay(15);

}

void setup() {

  
 randomSeed(analogRead(analogPin));
 Serial.begin(9600);
 Serial1.begin(57600);
 Serial2.begin(57600);
 Serial3.begin(57600);
 ET1.begin(details(packet), &Serial1);
 ET2.begin(details(packet), &Serial2);
 ET3.begin(details(packet), &Serial3);
 
  // whew!

  
  Serial.println("Hello");
  
  delay(5000);
  
  for(int z=0; z< 6; z++)  identify(z);
  for(int z=5; z>=0; z--)  identify(z);
  for(int z=0; z< 6; z++)  identify(z);
  for(int z=5; z>=0; z--)  identify(z);
  
  
  delay(2000);
  
 
  Serial.println("RED");
  fillDisplay(0b11100000);
  delay(500);
  Serial.println("GREEN");
  fillDisplay(0b00011100);
  delay(500);
  Serial.println("BLUE");
  fillDisplay(0b00000011);
  delay(500);

  Serial.println("YELLOW");
  fillDisplay(0b11111100);
  delay(500);
 
  fillDisplay(0b00000000);
  delay(1500);
  
  Serial.println("RED");
  fillDisplay(0b11100000);
  delay(500);
  Serial.println("GREEN");
  fillDisplay(0b00011100);
  delay(500);
  Serial.println("BLUE");
  fillDisplay(0b00000011);
  delay(500);

  Serial.println("YELLOW");
  fillDisplay(0b11111100);
  delay(500);

  fillDisplay(0b00000000);
  delay(1500);
  
  
  //runSim();
}

void loop() {
  
  //useful functions.
  //fillDisplay(color), sets all nodes to a single solid color. 
 //runSim(), runs game of life sim.
 
  
  /******************************************
      BLINKY SECTION
  ****************************************/   
  
  //RobotX 2016 Rules, 1 second per color and 2 seconds dark. 
  
    for(int i=0; i<3; i++){
      
        switch (pattern[i]) {
          case 'R':
          Serial.println("RED");
          fillDisplay(0b11100000);
          delay(1000);
          break;
          case 'G':
          Serial.println("GREEN");
          fillDisplay(0b00011100);
          delay(1000);
          break;
          case 'B':
          Serial.println("BLUE");
          fillDisplay(0b00000011);
          delay(1000);
          break;
          case 'Y':
          Serial.println("YELLOW");
          fillDisplay(0b11111100);   // 0b11111100 is RED + GREEN = 252
          delay(1000);
          break;

        }
      }
      fillDisplay(0);

    
    
      delay(1500);
 


delay(500); 

//Right now it's one big loop
//TODO implement scheduler so we can parse messages while blinking things. 




  
  /*
  fillDisplay(0b11100000);
  delay(500);
  fillDisplay(0b00000011);
  delay(500);
  fillDisplay(0b00011100);
  delay(500);
  fillDisplay(0);
  delay(2000);
  fillDisplay(0b11100000);
  delay(500);
  fillDisplay(0b00000011);
  delay(500);
  fillDisplay(0b00011100);
  delay(500);
  fillDisplay(0);
  delay(2000);
  fillDisplay(0b11100000);
  delay(500);
  fillDisplay(0b00000011);
  delay(500);
  fillDisplay(0b00011100);
  delay(500);
  fillDisplay(0);
  
  runSim();
  
  fillDisplay(0);
  delay(500);
*/


}

//run game of life, break down display and send all nodes. 
void runSim(){
  
  beginLife();
  for(int i=0; i<100; i++){
    //run sim frame
    gameOfLife();
    //post frame to our multisegment display
    postFrame();
  
  }
}

//set whole screen to one color.
//use Global address to update all segments at once. 
void fillDisplay(byte color){
  
  for(int y=0; y<16; y++){
    for(int i=0; i<8; i++){
      packet.displaydata[y][i] = color;
      
    }
  }
  
  packet.address = 0;
  ET1.sendData();
  ET2.sendData();
  ET3.sendData();
}

void postFrame(){
  //copy segments out of our world buffer, plane 0
  //send each segment using ET, then sync them together. 
  
  //i could do this in single block copy instructions, but I'd have to re-arrange my arrays to enable that. doin it the long way.
  
  for(int y=0; y<16; y++){
    for(int i=0; i<8; i++){
      packet.displaydata[y][7-i] = world[y][i][0];
      
    }
  }
  
  packet.address=1;
  
  //Serial1.flush();
  ET1.sendData();
  //Serial2.flush();
  ET2.sendData();
 // Serial3.flush();
  ET3.sendData();
  
  for(int y=0; y<16; y++){
    for(int i=0; i<8; i++){
      packet.displaydata[y][7-i] = world[y][i+8][0];
      
    }
  }
  
  packet.address=2;
  
  //Serial1.flush();
  ET1.sendData();
 // Serial2.flush();
  ET2.sendData();
 // Serial3.flush();
  ET3.sendData();
  
  for(int y=0; y<16; y++){
    for(int i=0; i<8; i++){
      packet.displaydata[y][7-i] = world[y][i+16][0];
      
    }
  }
  
  packet.address=3;
  
 // Serial1.flush();
  ET1.sendData();
//  Serial2.flush();
  ET2.sendData();
//  Serial3.flush();
  ET3.sendData();
  
  for(int y=0; y<16; y++){
    for(int i=0; i<8; i++){
      packet.displaydata[y][7-i] = world[y][i+24][0];
      
    }
  }
  
  packet.address=4;
  
 // Serial1.flush();
  ET1.sendData();
 // Serial2.flush();
  ET2.sendData();
  //Serial3.flush();
  ET3.sendData();
  
  Serial1.flush();
  Serial2.flush();
  Serial3.flush();
    //send sync trigger
    //We are going to send a Pulse down the serial line to tell all drivers to post the data they received. 
  Serial1.end();
  Serial2.end();
  Serial3.end();
  
  
  
  pinMode(18,OUTPUT); //serial 1
  pinMode(16,OUTPUT); //serial 2
  pinMode(14,OUTPUT); //serial 3
  
  digitalWrite(18,HIGH);
  digitalWrite(16,HIGH);
  digitalWrite(14,HIGH);
  
  delayMicroseconds(500);
  
  digitalWrite(18,LOW);
  digitalWrite(16,LOW);
  digitalWrite(14,LOW);
  
  delayMicroseconds(1500);
  

  
  //restart serial port
  Serial1.begin(57600);
  Serial2.begin(57600);
  Serial3.begin(57600);
 
 //delay(20);
  
}

void gameOfLife() {
  lifeAndDeath();
  //display here only posts data to our displaybuffer
  //actually, screw that. Let's have our postFrame() function copy directly out of world buffer. 
  //displayCurrentLife();
}


void beginLife() {
  long density = 42; 
  density += random(15);
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if (random(100) < density) {
        world[i][j][0] = 0b11100000;
      }
      else {
        world[i][j][0] = 0;
      }
      world[i][j][1] = 0;
    }
  }
}

void lifeAndDeath() {
  // Birth and death cycle
  for (int x = 0; x < SIZEX; x++) {
    for (int y = 0; y < SIZEY; y++) {
      // Default is for cell to stay the same
      world[x][y][1] = world[x][y][0];
      int count = neighbours(x, y);
      if (count == 3 && world[x][y][0] == 0) {
        // A new cell is born
        world[x][y][1] =  colors[random(6)] ; //0b00000011;
      }
      if ((count < 2 || count > 3) && world[x][y][0] >= 1) {
        // Cell dies
        world[x][y][1] = 0;
      }
    }
  }
  // Copy next generation into place
  for (int x = 0; x < SIZEX; x++) {
    for (int y = 0; y < SIZEY; y++) {
     // repeat[x][y] = world[x][y][0];
      world[x][y][0] = world[x][y][1];
    }
  }
}

void displayCurrentLife() {
  // Display current generation
  int color=0;
  int _counter = 0;
  int _death   = 0;
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if(world[i][j][0]) {
        //drawPixel(i,j, randColor(7));
        /* need to fix this color = matrix.Color333(world[i][j][0] >> 5, world[i][j][0] >> 2, (world[i][j][0] & 0b00000011) << 1);
        matrix.drawPixel(i*2, j*2, color);
        matrix.drawPixel(i*2+1, j*2, color);
        matrix.drawPixel(i*2, j*2+1, color);
        matrix.drawPixel(i*2+1, j*2+1, color);
      } 
      else {
        //drawPixel(i,j,cBlack);
        //matrix.drawPixel(i, j, matrix.Color333(0, 0, 0));
        color=0;
        matrix.drawPixel(i*2, j*2, color);
        matrix.drawPixel(i*2+1, j*2, color);
        matrix.drawPixel(i*2, j*2+1, color);
        matrix.drawPixel(i*2+1, j*2+1, color);*/
      } 
      world[i][j][1] = 0;
    }
  }
  delay(DELAY);
  //fillMatrix(cBlack);
  //matrix.fillScreen(matrix.Color333(0, 0, 0));
}

int neighbours(int x, int y) {
  
  int count=0;
  
  if (world[(x + 1) % SIZEX][y][0]) count ++;
  if (world[x][(y + 1) % SIZEY][0]) count ++;
  if (world[(x + SIZEX - 1) % SIZEX][y][0]) count ++;
  if (world[x][(y + SIZEY - 1) % SIZEY][0]) count ++;
  if (world[(x + 1) % SIZEX][(y + 1) % SIZEY][0]) count ++;
  if (world[(x + SIZEX - 1) % SIZEX][(y + 1) % SIZEY][0]) count ++; 
  if (world[(x + SIZEX - 1) % SIZEX][(y + SIZEY - 1) % SIZEY][0]) count ++;
  if (world[(x + 1) % SIZEX][(y + SIZEY - 1) % SIZEY][0]) count ++;
  
  
  return count;
}

