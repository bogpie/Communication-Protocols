#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int countCorruptMessages;

int calculateStringParity(char *string)
{
	int length = strlen(string);
	int result = -1;
	for (int idLetter = 0; idLetter < length; idLetter++)
	{
		char c = string[idLetter];
		unsigned char mask = 1 << 7;
		int idBit;
		int value = ((c & mask) != 0);
		if (result == -1)
		{
			result = value; // daca este primul bit din intreg mesajul
		}
		else if (result == 0 || result == 1)
		{
			result ^= value;
		}
		else
			printf("something's wrong with the parity calculation\n");

		mask >>= 1;

		for (idBit = 0; idBit < 7; idBit++)
		{
			int value = ((c & mask) != 0);
			result ^= value;
			mask >>= 1;
		}
	}
	return result;
}
int receiveMessageHelper(msg *adrMessage)
{
	my_payload newMessage;
	int res = recv_message(adrMessage);
	if (res < 0)
	{
		perror("[RECEIVER] Receive error. Exiting.\n");
		return -1;
	}
	newMessage = *((my_payload *)adrMessage->payload);

	printf("[RECEIVER] Received payload of content <%s>, with a parity of %d\n",
		   newMessage.payload,
		   newMessage.parity);
	if(newMessage.parity!=0 && newMessage.parity!=1)
	{
		printf("[RECEIVER] Sent parity value was corrupted!\n");
	}

	int actualParity = calculateStringParity(newMessage.payload);
	printf("[RECEIVER] Actual parity is %d\n", actualParity);

	if (newMessage.parity != actualParity)
	{
		printf("[RECEIVER] Found corrupt message! \n");
		++countCorruptMessages;
	}

	printf("\n");

	return 0;
}

int main(void)
{
	msg r;
	int i, res;

	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	for (i = 0; i < COUNT; i++)
	{
		// wait for message

		/*
		res = recv_message(&r);
		if (res < 0)
		{
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		}*/

		receiveMessageHelper(&r);

		//printf("[RECEIVER] Received payload of content <%s>\n", r.payload);

		// send dummy ACK
		res = send_message(&r);
		if (res < 0)
		{
			perror("[RECEIVER] Send ACK error. Exiting.\n");
			return -1;
		}
	}

	printf("[RECEIVER] Found %d corrupt messages out of %d\n\n,", countCorruptMessages, COUNT);

	printf("[RECEIVER] Finished receiving..\n\n");

	return 0;
}
