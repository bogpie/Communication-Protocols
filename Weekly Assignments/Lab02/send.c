#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"
#include <errno.h> /* perror() */

#define HOST "127.0.0.1"
#define PORT 10000


void fatal(char *mesaj_eroare)
{
  perror(mesaj_eroare);
  exit(0);
}

int calculateFileSize(int input)
{
  int start = lseek(input, (size_t)0, SEEK_SET);
  int end = lseek(input, (size_t)0, SEEK_END);
  lseek(input, (size_t)0, SEEK_SET);
  int size = end - start;
  return size;
}

void sendMessageHelper(int type, msg *adrPassedMessage, int size, char* string)
{
  DetailedMessage detailedMessage;
  detailedMessage.message_type = type;

  if (type == TYPE_FILESIZE)
  {
    detailedMessage.message_type = TYPE_FILESIZE;
    sprintf(detailedMessage.payload, "%d", size);
    adrPassedMessage->len = sizeof(int) + strlen(detailedMessage.payload) + 1;
  }
  else
  {
    memcpy(detailedMessage.payload, string, strlen(string) + 1);
    adrPassedMessage->len = sizeof(int) + strlen(string) + 1; // also counting the file type integer and the file terminator
  }

  memcpy(adrPassedMessage->payload, &detailedMessage, adrPassedMessage->len);
  printf("[send] Sending message: <%s> ...\n", detailedMessage.payload);
  send_message(adrPassedMessage);
}

int checkResponse(msg *adrPassedMessage)
{
  if (recv_message(adrPassedMessage) < 0)
  {
    perror("Receive error ...");
    return -1;
  }
  printf("[send] Got reply with payload: %s, with a length of %d \n\n", adrPassedMessage->payload, adrPassedMessage->len);
  return 0;
}

int main(int argc, char **argv)
{
  printf("Testing for a maximum message length of %d..\n\n",MAX_LEN);

  init(HOST, PORT);
  msg passedMessage;

  //Send dummy:
  printf("[send] Sending dummy...\n");
  sprintf(passedMessage.payload, "%s", "This is a dummy text");
  passedMessage.len = strlen(passedMessage.payload) + 1;
  send_message(&passedMessage);
  // Check response:
  checkResponse(&passedMessage);

  //Open file and calculate size
  int input = open(FILENAME, O_RDONLY, S_IRUSR);
  int size = calculateFileSize(input);

  //Send file name
  sendMessageHelper(TYPE_FILENAME, &passedMessage, size, FILENAME);
  int returnCode = checkResponse(&passedMessage);
  if (returnCode != 0)
    return returnCode;

  //Send file size
  sendMessageHelper(TYPE_FILESIZE, &passedMessage, size, FILENAME);
  returnCode = checkResponse(&passedMessage);
  if (returnCode != 0)
    return returnCode;

  //Send file content
  int noBytes;

  char *buf = malloc(size);
  while (1)
  {
    memset(buf,0,strlen(buf));
    noBytes = read(input, buf, MAX_LEN - sizeof(int));
    if (noBytes == 0)
    {
      break;
    }
    else if (noBytes < 0)
    {
      fatal("Read error");
    }
    printf("Read content is:\n<%s>\n\n", buf);
    size = strlen(buf);
    sendMessageHelper(TYPE_FILECONTENT, &passedMessage, size, buf);
    returnCode = checkResponse(&passedMessage);
    if (returnCode != 0)
      return returnCode;
  }

  close(input);

  return 0;
}
