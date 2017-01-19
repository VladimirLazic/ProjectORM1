#include<stdbool.h>	   //type bool
#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

//A global variable that hold the information if the clients is connected or not
bool clientConnected = false;

//Two global variables to be used while creating a socket and connection
int sock;
struct sockaddr_in server;

//declarations of client functions
void Login();
void Logout();
void SendPinCode();
void PinChange();
void CheckStatus();
void GetCash();

//definitions

//A function that connects a client to the server
void Login() {
	char clientName[DEFAULT_BUFLEN] , serverResponse[DEFAULT_BUFLEN];
	int readSize;
	//If client is already connected exit the function
	if(clientConnected) {
		printf("Error: You are already loged in!\n");
		return;
	}

	//Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error: Connection failed, server already in use\n");
        return;
    }
	printf("Connected to the server, begining the login process\n");

	//Loggin in to the server
	//I for Login
	if(send(sock , "I" , sizeof(char) , 0) < 0) {
		puts("Unable to log in to the server");
		return;
	}

	//Requesting client ID
	printf("Please enter your user name: ");
	scanf("%s" , clientName);
	clientName[strlen(clientName)] = NULL;
    printf("You have entered username: %s\n" , clientName);

    printf("Sending username to server!\n");
	//Sending the client name to the server
	if(send(sock , clientName , strlen(clientName) , 0) < 0) {
		puts("Unable to send client name to the server");
		return;
	}

	printf("Waiting server response...\n");
	if((readSize = recv(sock , serverResponse , DEFAULT_BUFLEN , 0)) < 0) {
		printf("Error: Unable to recive server response!\n");
		return;
	}
	printf("Server response: %s\n" , serverResponse);

	//Registering the connection
    if(strcmp(serverResponse , "SUCCESS") == 0) {
		clientConnected = true;
	} else {
		clientConnected = false;
	}
}

//A function that disconnects a server from a client
void Logout() {
	//If the client isn't connected exit the function
	if(!clientConnected) {
		printf("Error: You can't logout because you're not connected\n");
		return;
	}

	//Notifying the server that the client wishes to logout
	//O for Logout
	if(send(sock , "O" , sizeof(char) , 0) < 0) {
		puts("Unable to notify server");
		return;
	}

	close(sock);
	clientConnected = false;
	return 0;
}

void SendPinCode() {
	char clientPIN[4];
	int readSize;
	char serverResponse[DEFAULT_BUFLEN];

	//If the client isn't connected exit the function
	if(!clientConnected) {
		printf("Error: You can't send PIN because you're not connected\n");
		return;
	}

	//Asking the client to enter his PIN Code
	printf("Please enter your PIN Code: ");
	scanf("%s" , clientPIN);
	clientPIN[4] = NULL;					//removing juck from the end of string

	//Notifying the server that the client wants to verify his PIN
	//V for verification
	if(send(sock , "V" , sizeof(char) , 0) < 0) {
		puts("Unable to make request!");
		return;
	}

	//Sending the PIN in form of string to the server
	if(send(sock , clientPIN , strlen(clientPIN), 0) < 0)
    {
        puts("Send failed");
        return;
    } else {
		printf("Sending successful!\n");
	}

	printf("Waiting server verification...\n");
	if((readSize = recv(sock , serverResponse , DEFAULT_BUFLEN , 0)) < 0) {
		printf("Error: Unable to recive server response!\n");
		return;
	}
	serverResponse[readSize] = NULL;
	printf("Server response: %s\n" , serverResponse);
}

void PinChange() {
	char newPIN[4] , clientMessage[5];
	int readSize;
	char serverResponse[DEFAULT_BUFLEN];

	//If the client isn't connected exit the function
	if(!clientConnected) {
		printf("Error: You can't change PIN because you're not connected\n");
		return;
	}

	//Asking the client to enter a new PIN
	printf("Please enter your new PIN: ");
	scanf("%s" , newPIN);
	newPIN[4] = NULL;						//removing junk from the end of string

	//Notifying the server that the client wants to change his PIN
	//N for new PIN
	if(send(sock , "N" , sizeof(char) , 0) < 0) {
		puts("Unable to make request!");
		return;
	}

	//Sending the new PIN in form of string to the server
	if(send(sock , newPIN , strlen(newPIN), 0) < 0)
    {
        puts("Send failed");
        return;
    } else {
		printf("Sending successful!\n");
	}

	printf("Waiting server verification...\n");
	if((readSize = recv(sock , serverResponse , DEFAULT_BUFLEN , 0)) < 0) {
		printf("Error: Unable to recive server response!\n");
		return;
	}
	printf("Server response: %s\n" , serverResponse);

}

void CheckStatus() {
	char serverResponse[DEFAULT_BUFLEN];
	int readSize;

	//If the client isn't connected exit the function
	if(!clientConnected) {
		printf("Error: You can't check your status because you're not connected\n");
		return;
	}

	//Requesting a response from the server
	//B for balance
	if(send(sock , "B" , sizeof(char) , 0) < 0) {
		puts("Failed to make a request!");
		return;
	}

	printf("Waiting server response...\n");
	if((readSize = recv(sock , serverResponse , DEFAULT_BUFLEN , 0)) < 0) {
		printf("Error: Unable to recive server response!\n");
		return;
	}
	serverResponse[readSize] = NULL;
	printf("Your current account balance is: %s\n" , serverResponse);

}

void GetCash() {
	char serverResponse[DEFAULT_BUFLEN] , clientMessage[DEFAULT_BUFLEN];
	int readSize , withdrawalAmount;

	//If the client isn't connected exit the function
	if(!clientConnected) {
		printf("Error: You can't get cash because you're not connected\n");
		return;
	}

	//Creating clientMessage
	printf("Please eneter your desired withdrawal amount: ");
	scanf("%d" , &withdrawalAmount);
	snprintf(clientMessage , 10 , "%d" , withdrawalAmount);

	//Notifying the server that the client wants to get cash
	//G for get cash
	if(send(sock , "G" , sizeof(char) , 0) < 0) {
		puts("Unable to make request!");
		return;
	}

	//Sending information to the server
	if(send(sock , clientMessage , strlen(clientMessage) , 0) < 0) {
		printf("Unable to send information to the server\n");
		return;
	}

	printf("Waiting server response...\n");
	if((readSize = recv(sock , serverResponse , DEFAULT_BUFLEN , 0)) < 0) {
		printf("Error: Unable to recive server response!\n");
		return;
	}
	serverResponse[readSize] = NULL;
	printf("Your withdrawal was a : %s\n" , serverResponse);
}

int main() {

	//Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

	//Filing in the information about the desired server
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    char clientFunction[DEFAULT_BUFLEN];
    printf("Welcome\n");
	printf("Operations available:\n1. Login\n2. Logout\n3. SendPinCode\n");
	printf("4. PinChange\n5. CheckStatus\n6. GetCash\n");
	do {
		char clientFunction[DEFAULT_BUFLEN];
		printf("Please select your desired operation: ");

		scanf("%s" , clientFunction);
		clientFunction[strlen(clientFunction)] = NULL;		//making sure there is no junk at the end of the string
		//TODO: check which function a client wishes to make a call and do it

		if( strcmp(clientFunction , "Login") == 0 || strcmp(clientFunction , "login") == 0 ) {
			printf("You have selected operation: Login\n");
			Login();
		} else if( strcmp(clientFunction , "Logout") == 0 || strcmp(clientFunction , "logout") == 0 ) {
			printf("You have selected operation: Logout\n");
			Logout();
		} else if( strcmp(clientFunction , "SendPinCode") == 0 || strcmp(clientFunction , "sendpincode") == 0 ) {
			printf("You have selected operation: SendPinCode\n");
			SendPinCode();
		} else if( strcmp(clientFunction , "PinChange") == 0 || strcmp(clientFunction , "pinchange") == 0 ) {
			printf("You have selected operation: PinChange\n");
			PinChange();
		} else if( strcmp(clientFunction , "CheckStatus") == 0 || strcmp(clientFunction , "checkstatus") == 0 ) {
			printf("You have selected operation: CheckStatus\n");
			CheckStatus();
		} else if( strcmp(clientFunction , "GetCash") == 0 || strcmp(clientFunction , "getcash") == 0 ) {
			printf("You have selected operation: GetCash\n");
			GetCash();
		} else {
			printf("You have selected an invalid operation! Please try again!\n");
		}
	}while(true);
}
