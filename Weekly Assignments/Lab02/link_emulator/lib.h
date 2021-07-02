#ifndef LIB
#define LIB

#define MAX_LEN 50

#define TYPE_FILENAME 0
#define TYPE_ACK 1
#define TYPE_FILESIZE 2
#define TYPE_FILECONTENT 3

#define FILENAME "tema.txt"
#define NEWFILENAME "recv_tema.txt"

typedef struct {
  int  len;
  char payload[MAX_LEN];
} msg;

typedef struct {
  int message_type;
  char payload[MAX_LEN - sizeof(int)];
} DetailedMessage;

void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);

int send_message(const msg* m); 
// negative in the event of a failure, or the number of bytes

int recv_message(msg* r);
// blocks the program until the other entity "sends" and the data is transmitted
// when recv ends, r contains the transmitted message


#endif

