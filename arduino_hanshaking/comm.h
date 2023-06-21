#define MAX_MESSAGE_LENGTH 128
#include <WString.h>

struct Message{
   char label;
   char content[MAX_MESSAGE_LENGTH];
   unsigned int mlen;
};

#define HANDLE_HANDSHAKE(message, id) if(message.label == 'u') send_line_val(cstr_message('i', id));

Message int_message(char label, int value);
Message float_message(char label, float value);
Message string_message(char label, String str);
Message cstr_message(char label, char * cstr);
void send_line_val(Message m);
Message wait_for_line_val();
