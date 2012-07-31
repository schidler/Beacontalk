// Beacontalk - peer to peer chat-program that sends data over Wi-Fi without associations.
// Copyright (C) 2012 Rick van de Loo

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>

#include "beaconpacket.h"

#define SCREEN_HEIGHT 100

int i, j, checkdouble;
char nick[10], message[500], concatenated[512], text[100][512];
char *dev;

void shiftarray(){
	/* clears screen */
	for (i = SCREEN_HEIGHT; i > 0; i--){
		printf("\n");
	}
	/* clears any double messages, a guid would be nicer here */ 
	checkdouble = 0;
	for (i = 1; i < 5; i++){
		if (strcmp(text[0], text[i]) == 0){
			checkdouble = 1;
		}
	}
	if (checkdouble){
		text[0][0] = 0;
	}
	/* deletes the last entry in the array when it is full */
	if (text[0][0] != 0 && text[1][0] != 0 && text[99][0] != 0){
		text[99][0] = 0;
	}
	/* moves the messages up in the array */
	if (text[0][0] != 0){
		for (i = 99; i >= 0; i--){
			if (text[i][0] != 0 && text[i+1][0] == 0){
				for (j = 0; j < 512; j++){
					text[i+1][j] = text[i][j];
				}
				text[i][0] = 0;
			}
		}
	}
	/* prints the messages */
	for (i = 99; i > 0; i--){
		if (text[i][0] != 0){
			printf("%s", text[i]);
		}
	}
	fputs("enter message: ", stdout);
	fflush(stdout);
}

void printmessage(){

//	shiftarray();
	fgets(message, sizeof message, stdin);
	strcpy(concatenated, nick);
	strcat(concatenated, ": ");
	strcat(concatenated, message);
	/* sends the package a bunch of times, this is just fire and forget */
	for (i = 100; i > 0; i--){
		send_beaconpacket(concatenated);
	}
}	

int main(int argc, char *argv[]){

	/* defines the wireless device that will be used */
	if (argc < 2){
	printf("enter wireless interface as argument\n");
		return(0);
	}
	dev = argv[1];

	/* defines the nickname that will be send with your messages */
	fputs("nick (max 10 char): ", stdout);
	fflush(stdout);
	fgets(nick, 10, stdin);	
	for (i = 0; i < sizeof(nick); i++){
		if (nick[i] == '\n'){
			nick[i] = '\0';
		}
	}
	
	/* makes sure no random garbage is stored in the array */
	for (i = 99; i >= 0; i--){
		text[i][0] = 0;
	}

	/* this function is a callback for the threads in threads.c */
	void threads(int thread_choice){
		switch (thread_choice){
			case 1:
				/* listens for incoming messages */
				get_beaconpacket();
				break;

			case 2:
				/* takes care of the sending and displaying of messages */
				while (1){	
					printmessage();
				}
				break;

			default:
				printf("ERROR: non valid input. Check the (*f)(x) in threads.c\n");
		}
	}

	/* initiates the threading */
	thread_init(threads);

}
