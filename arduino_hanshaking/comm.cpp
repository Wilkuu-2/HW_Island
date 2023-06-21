/*=======================================================*
 * Simple serial communication protocol 
 *
 * Copyright 2023, Group Tantor (CreaTe M8 2023)
 * You may use the code under the terms of the MIT license
 *=======================================================*/

#include <HardwareSerial.h>
#include <WString.h>

// Header Guard 
#ifndef COMM_CPP
#define COMM_CPP 

// This is how long the message can be 
#define MAX_MESSAGE_LENGTH 128

/* 
 * The Message struct is the base of the protocol:
 * function: {                 Sent Portion                }{Bookkeeping  }
 * var:      [label][content (c style string)              ](string lenght)
 * size:     <1byte><--------MAX_MESSAGE_LENGTH bytes------><----2bytes--->
 *
 * example messages: 
 *           [t][Lorem ipsum\0] (12 bytes)
 *           [a][1\0] (2 bytes)
 *           [c][69.420\0] (7 bytes)
 * */
struct Message{
   char label;
   char content[MAX_MESSAGE_LENGTH];
   unsigned int mlen;
};


// Creates a message with a integer value 
Message int_message(char label, int value){
  Message m = {0};
  m.label = label; 
  sprintf(m.content, "%d", 2); // Print value in the buffer 
  m.mlen = strlen(m.content);  // Measure the resulting length
  return m; 
}

// Creates a message with a float value 
Message float_message(char label, float value){
  Message m = {0};
  m.label = label;
  sprintf(m.content, "%.5f", value); // Print value into the buffer with 5 decimals
  m.mlen = strlen(m.content);        // Measure the resulting lenght 

  return m;
}

// Creates a message with a c++/Arduino string as content  
Message string_message(char label, String str){
  Message m = {0};
  m.label = label;
  m.mlen = str.length(); // See the length of the string 
  if(m.mlen > MAX_MESSAGE_LENGTH -1){ // When the string is too long
    strcpy(m.content, str.substring(0,MAX_MESSAGE_LENGTH-1).c_str()); // Cut the string off 
    m.mlen = MAX_MESSAGE_LENGTH -1;
  } else { // When str is shorter than the maximum
    strcpy(m.content, str.c_str());  // Copy normally
  }
  return m;
}

// Creates a message with a c style string as content 
Message cstr_message(char label, char * cstr){
  Message m = {0};
  m.label = label;
  m.mlen = strlen(cstr);
  strcpy(m.content, cstr);
  return m;  
}


// Sends a message over serial 
void send_message(Message m){
    Serial.print(m.label);
    Serial.println(m.content);
}



// Reads serial byte by byte to and pieces a message together
//     WARNING: This blocks the arduino from doing anything else
Message wait_for_message(){
  Message m = {0}

  while(m.mlen < MAX_MESSAGE_LENGTH){
    char ch = ' ';
    
    // Because arduino does not allow bytewise reads, we have to gaslight the compiler into thinking that we are reading into a 1 byte long buffer
    if (Serial.available() && Serial.readBytes((char *)(&ch),1) < 1)
        continue; // Don't check or write the character when no character recieved
    
    // newlines and carriage returns are the breaking characters for this protocol
    if(ch == '\n' || ch == '\r'){
        break;  
    }

    // Write into buffer 
    m.content[m.mlen] = ch; 
    m.mlen ++;     
  }
  
  // Return z-message when reading a stray breaking character
  if(m.mlen < 2){
      return int_message('z', -1);
  }
  
  return m; 
   
}

// Handshake
// Responds to hanshake requests and returns true if the request was a handshake request
inline bool handle_handshake(Message m, const char * id){
    if(m.label = 'u'){
        send_message(cstr_message('i',id));
        return true; 
    }

    return false;
}

#endif
