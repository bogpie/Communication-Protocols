#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int receiveMessageHelper(msg * adrPassedMessage,int output)
{
  DetailedMessage detailedMessage;
  if (recv_message(adrPassedMessage)<0){
    perror("Receive message");
    return -1;
  }
  detailedMessage = * ( (DetailedMessage*) adrPassedMessage->payload );

  printf("[recv] Got msg of type <%d> with payload: <%s> and a length of <%d>, sending ACK...\n",
   detailedMessage.message_type,
   detailedMessage.payload,
   adrPassedMessage->len);
  

  if(detailedMessage.message_type == TYPE_FILECONTENT)
  {
    lseek(output,0,SEEK_END);
    write(output,detailedMessage.payload,strlen(detailedMessage.payload));
  }

  return 0;
}

void sendAck(msg * adrPassedMessage)
{
  // Send ACK:
  sprintf(adrPassedMessage->payload,"%s", "ACK");
  adrPassedMessage->len = strlen(adrPassedMessage->payload) + 1;
  send_message(adrPassedMessage);
  printf("[recv] ACK sent\n");
}

int main(int argc,char** argv){
  msg passedMessage;
  init(HOST,PORT);

  int output = open("recv_tema.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (recv_message(&passedMessage)<0){
    perror("Receive message");
    return -1;
  }
  printf("[recv] Got msg with payload: <%s>, with a length of %d, sending ACK...\n", passedMessage.payload,passedMessage.len);
  sendAck(&passedMessage);

  //Receive previously sent file name
  int returnCode = receiveMessageHelper(&passedMessage,output);
  if(returnCode!=0) return returnCode;
  sendAck(&passedMessage);

  //Receive previously sent file size
  returnCode = receiveMessageHelper(&passedMessage,output);
  if(returnCode!=0) return returnCode;
  sendAck(&passedMessage);


  while(1)
  {
    //Receive previously sent file content
    returnCode = receiveMessageHelper(&passedMessage,output);


    if(returnCode!=0)
    {
      break;
    } 
    sendAck(&passedMessage);
    
  }

  close(output);

  return 0;
}
