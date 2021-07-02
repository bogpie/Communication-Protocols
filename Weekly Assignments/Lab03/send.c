#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

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

int sendMessageHelper(int frameCounter, msg *adrMessage)
{
	my_payload newMessage;
	newMessage.parity = calculateStringParity(adrMessage->payload);

	memcpy(newMessage.payload, adrMessage->payload, strlen(adrMessage->payload) + 1);
	memcpy(adrMessage->payload, &newMessage, adrMessage->len);
	printf("[SENDER] Sending frame no. %d of content <%s> and parity %d\n", frameCounter, newMessage.payload, newMessage.parity);
	int res = send_message(adrMessage);
	if (res < 0)
	{
		perror("[SENDER] Send error. Exiting.\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	msg t;
	int i, res;

	printf("[SENDER] Starting.\n");
	init(HOST, PORT);

	printf("[SENDER]: BDP=%d\n", atoi(argv[1]));

	int w = atoi(argv[1]) * 1000 / (8 * sizeof(msg));

	printf("[SENDER] Sending %d frames without requiring confirmation\n", w);
	for (i = 0; i < w; i++)
	{
		memcpy(t.payload, "testing", strlen("testing") + 1);

		t.len = MSGSIZE;
		sendMessageHelper(i, &t);
	}

	for (i = w; i < COUNT; ++i)
	{
		//wait for ack
		res = recv_message(&t);
		if (res < 0)
		{
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
		printf("[SENDER] Received ack\n");

		//sending next frame
		memcpy(t.payload, "testing", strlen("testing") + 1);
		t.len = strlen(t.payload) + 1;
		
		sendMessageHelper(i, &t);
	}
	printf("[SENDER] Expecting the rest of the ack's..\n");

	for (int i = 0; i < w; ++i)
	{
		//wait for ack
		res = recv_message(&t);

		if (res < 0)
		{
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
		printf("[SENDER] Received ack\n");
	}

	return 0;
}
