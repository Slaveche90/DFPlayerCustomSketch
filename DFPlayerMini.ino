#include "SoftwareSerial.h"
#define Start_Byte        0x7E
#define Version_Byte      0xFF
#define Command_Length    0x06
#define End_Byte          0xEF

//  Returns info with command 0x41 [0x01: info, 0x00: no info]
#define Acknowledge       0x01

SoftwareSerial mySerial(6, 7);  //  RX, TX
byte receive_buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char data;
byte volume = 0x00;
bool mute_state = false;

void execute_CMD(byte CMD, byte Par1, byte Par2) {
  //  Sends the command to the module
  
  //  Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  
  //  Build the command line
  byte Command_line[10] = { Start_Byte, Version_Byte,
                            Command_Length, CMD, Acknowledge, 
                            Par1, Par2, highByte(checksum),
                            lowByte(checksum), End_Byte};

  //  Send the command line to the module
  for(byte k = 0; k < 10; k++) {
    mySerial.write(Command_line[k]);
  }
}

void reset_rec_buf() {
  // Resets the receive_buffer
  for(uint8_t i = 0; i < 10; i++) {
    receive_buffer[i] = 0;
  }
}

bool receive() {
  // Reads the received data
  reset_rec_buf();
  if(mySerial.available() < 10) {
    return false;
  }  
  for(uint8_t i = 0; i < 10; i++) {
    short b = mySerial.read();
    if(b == -1) {
      return false;
    }
    receive_buffer[i] = b;
  }
  
  // when you reset the module in software,
  // receive_buffer elements are shifted
  // to correct that we do the following:
  short b = receive_buffer[0];
  for(uint8_t i = 0; i < 10; i++) {
    if(i == 9) {
      receive_buffer[i] = b;
    }
    else {
      receive_buffer[i] = receive_buffer[i+1];
    }  
  } //  end correcting receive_buffer
  return true;
}

void print_received(bool print_it) {
  // Outputs the returend data from the module
  // To the Serial Monitor
  if(print_it) {
    if(receive()) {
      for(uint8_t i = 0; i < 10; i++) {
        Serial.print(receive_buffer[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    }
  }
  else {
    receive();
  }
}

void module_init() {
  // Resets the module and sets the EQ state, 
  // volume level, and plays first song on the
  // storage device
  execute_CMD(0x0C, 0, 0); // reset the module
  delay(1000);
  print_received(false);
  delay(100);
  Serial.print("SDON\t");
  print_received(true);
  delay(100);
  set_eq(2);
  play_first();
  set_volume(0x09);
}

void set_eq(uint8_t eq) {
  // Sets the state of EQ
  Serial.print("SETEQ\t");
  execute_CMD(0x07, 0, eq); // Sets the EQ
  delay(100);  
  print_received(false);
  delay(100);
  execute_CMD(0x44, 0, 0);  // Get EQ state
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void play_first() {
  // Plays first song on the storage device
  Serial.print("PLYFST\t");
  execute_CMD(0x03, 0, 1); // Play first song
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void set_volume(uint8_t volume) {
  // Sets the volume level
  Serial.print("SETVOL\t");
  execute_CMD(0x06, 0, volume); //  Set volume level
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x43, 0, 0);  //  Get volume level
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void play() {
  // Resumes playing current song
  Serial.print("PLAY\t");
  execute_CMD(0x0D, 0, 0); 
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0); // Get the current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void pause() {
  // Pauses only current song
  Serial.print("PAUSE\t");
  execute_CMD(0x0E, 0, 0);
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0); // Get the current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void play_next() {
  // Plays next song, after which it stops playback
  Serial.print("NEXT\t");
  execute_CMD(0x01, 0, 0);
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0); // Get the current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void play_previous() {
  // Plays previous song, after which it stops playback
  Serial.print("PRE\t");
  execute_CMD(0x02, 0, 0);
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0); // Get the current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void mute() {
  mute_state = !mute_state;

  if(mute_state) {
    execute_CMD(0x43, 0, 0);  //  Get volume level
    delay(100);
    print_received(false);
    delay(100);
    print_received(false);
    delay(100);
    volume = receive_buffer[6];
    
    Serial.print("MUTE\t");
    execute_CMD(0x06, 0, 0x00); //  Set volume level
    delay(100);
    print_received(false);
    delay(100);
    execute_CMD(0x43, 0, 0);  //  Get volume level
    delay(100);
    print_received(false);
    delay(100);
    print_received(true);
    delay(100);
  }
  else {
    Serial.print("VOL\t");
    execute_CMD(0x06, 0, volume); //  Set previous volume level
    delay(100);
    print_received(false);
    delay(100);
    execute_CMD(0x43, 0, 0);  //  Get volume level
    delay(100);
    print_received(false);
    delay(100);
    print_received(true);
    delay(100);
  }
}

void play_in_mp3() {
  // plays song 01 in the mp3 folder
  execute_CMD(0x12, 0, 1);
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x42, 0, 0);
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("MP3fol\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void loop_fol_two_song_one() {
  // plays song 01 in the 02 folder
  execute_CMD(0x17, 2, 1);
  delay(100);
  //Serial.print("\t");
  print_received(false);
  delay(100);
  Serial.print("Loop01\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void loop_current() {
  // loops current playing song
  execute_CMD(0x19, 0, 0);
  delay(100);
  Serial.print("LPCRRNT\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void stop_loop_current() {
  // stops looping the current playing song
  execute_CMD(0x19, 0, 1);
  delay(100);
  Serial.print("STPLP\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void loop_all() {
  // loops all songs, playback one by one
  execute_CMD(0x11, 0, 1);
  delay(100);
  Serial.print("LPALL\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void stop_loop_all() {
  // stops loop all
  execute_CMD(0x11, 0, 0);
  delay(100);
  Serial.print("NOLPA\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void random_play() {
  // random plays all songs, loops all, repeats songs in playback
  execute_CMD(0x18, 0, 0);
  delay(100);
  Serial.print("RNDM\t");
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0);  // Get current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void play_ads() {
  // pause the playback, plays the add,  
  // and after adds is finished playing
  // resumes the playback
  execute_CMD(0x13, 0, 1);
  delay(100);
  Serial.print("ADD\t");
  print_received(true);
  delay(100);
}

void loop_folder_two() {
  // loops all songs in the folder 02
  execute_CMD(0x17, 0, 2);
  delay(100);
  Serial.print("LPFLDR\t");
  print_received(false);
  delay(100);
  print_received(false);
  delay(100);
  execute_CMD(0x4C, 0, 0);  // Get current song played
  delay(100);
  print_received(false);
  delay(100);
  print_received(true);
  delay(100);
}

void stop_all() {
  // stop playback
  execute_CMD(0x16, 0, 0);
  delay(100);
  Serial.print("STOP\t");
  print_received(false);
  delay(100);
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);
}

void query_status() {
  execute_CMD(0x42, 0, 0); // Get status of module
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("STATUS\t");
  print_received(true);
  delay(100);
  
  execute_CMD(0x43, 0, 0); // Get volume level
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("VOLUME\t");
  print_received(true);
  delay(100);
  
  execute_CMD(0x44, 0, 0); // Get EQ status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("EQ\t");
  print_received(true);
  delay(100);
  
  execute_CMD(0x45, 0, 0); // Get playback status
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("PLYBCK\t");
  print_received(true);
  delay(100);

  execute_CMD(0x46, 0, 0); // Get software version
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("SFVER\t");
  print_received(true);
  delay(100);

  execute_CMD(0x48, 0, 0); // Get total number of files on storage device
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("FILES\t");
  print_received(true);
  delay(100);

  execute_CMD(0x4C, 0, 0); // Get current song played
  delay(100);
  print_received(false);
  delay(100);
  Serial.print("CRRTRK\t");
  print_received(true);
  delay(100);
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(1000);
  
  Serial.println("\nInitialization");
  module_init();
}

void loop() {
  print_received(true);
   
  while(Serial.available() > 0) {
    data = Serial.read();
    //Serial.println(data, HEX); //  for debugging
    if(data != "/n") {
      if(data == 'P') {
        Serial.println("\nPlay the song");
        play();
      }
      else if(data == 'p') {
        Serial.println("\nPause the song");
        pause();
      }
      else if(data == 'N') {
        Serial.println("\nPlay next song");
        play_next();
      }
      else if(data == 'R') {
        Serial.println("\nPlay previous song\t");
        play_previous();
      }
      else if(data == 'm') {
        Serial.println("\nPlay in mp3 folder song 1");
        play_in_mp3();
      }
      else if(data == 'M') {
        Serial.println("\nMute/Unmute");
        mute();
      }
      else if(data == '1') { // number one
        Serial.println("\nLoop song 01 in folder 02");
        loop_fol_two_song_one();
      }
      else if(data == 'C') {
        Serial.println("\nLoop one");
        loop_current();
      }
      else if(data == 'c') {
        Serial.println("\nStop loop one");
        stop_loop_current();
      }
      else if(data == 'L') {
        Serial.println("\nLoop all");
        loop_all();
      }
      else if(data == 'l') {
        Serial.println("\nStop loop all");
        stop_loop_all();
      }
      else if(data == 'B') {
        Serial.println("\nRandom play");
        random_play();
      }
      else if(data == 'a') {
        Serial.println("\nPlay adds");
        play_ads();
      }
      else if(data == '2') {
        Serial.println("\nLoop all songs in folder 2");
        loop_folder_two();
      }
      else if(data == 'S') {
        Serial.println("\nStop playback");
        stop_all();
      }
      else if(data == 'D') {
        Serial.println("\nQuerry status of the module");
        query_status();
      }
    }
  }
  delay(100);
}
