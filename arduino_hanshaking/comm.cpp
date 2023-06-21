#include "comm.h"
#include <HardwareSerial.h>

Message int_message(char label, int value){
  Message m = {0};
  m.label = label;
  sprintf(m.content, "%d", 2);
  m.mlen = strlen(m.content);

  return m; 
}

Message float_message(char label, float value){
  Message m = {0};
  m.label = label;
  sprintf(m.content, "%.5f", value);
  m.mlen = strlen(m.content);

  return m;
}

Message string_message(char label, String str){
  Message m = {0};
  m.label = label;
  m.mlen = str.length();
  if(m.mlen > MAX_MESSAGE_LENGTH -1){
    strcpy(m.content, str.substring(0,MAX_MESSAGE_LENGTH-2).c_str());
    m.mlen = MAX_MESSAGE_LENGTH -1;
  } else {
    strcpy(m.content, str.c_str());  
  }
  return m;
}

Message cstr_message(char label, char * cstr){
  Message m = {0};
  m.label = label;
  m.mlen = strlen(cstr);
  strcpy(m.content, cstr);
  return m;  
}

void send_line_val(Message m){
    Serial.print(m.label);
    Serial.println(m.content);
}


Message wait_for_line_val(){
  char buf[128] = {0};
  size_t mlen = 0;

  while(mlen < MAX_MESSAGE_LENGTH){
    char ch = ' ';
    if (Serial.readBytes((char *)(&ch),1) < 1){
        continue;
    }

    if(ch == '\n' || ch == '\r'){
        break;  
    }
    
    buf[mlen] = ch; 
    
    mlen ++;     
  }

  if(mlen < 2){
      return int_message('z', -1);
  }
   
}
