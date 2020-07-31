#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

// Declaration of struct used for rooms
struct room
{
	char name [9];
	char type [16];
	int connections [6];
	int numberOfConnections;
};

// Function declarations
int graphNotComplete(struct room rooms[]);
void addRandomConnection(struct room rooms[]);

int main()
{	
	// Possible room names
	char *roomName0 = "Garden";
	char *roomName1 = "Bedroom";
	char *roomName2 = "Kitchen";
	char *roomName3 = "Entryway";
	char *roomName4 = "Foyer";
	char *roomName5 = "Cellar";
	char *roomName6 = "Attic";
	char *roomName7 = "Chapel";
	char *roomName8 = "Bathroom";
	char *roomName9 = "Vault";
	
	// Sentinel Values (To show rooms have already been picked)
	int sentinel0 = 0;
	int sentinel1 = 0;
	int sentinel2 = 0;
	int sentinel3 = 0;
	int sentinel4 = 0;
	int sentinel5 = 0;
	int sentinel6 = 0;
	int sentinel7 = 0;
	int sentinel8 = 0;
	int sentinel9 = 0;
	int successOrNo;

	// String variables
	char *roomName = "ROOM NAME: ";
	char *connection = "CONNECTION ";
	char *colon = ": ";
	char *roomType = "ROOM TYPE: ";
	char buffer[1024] = "";
	char *newLine = "\n";
	char connectionNumAsString[5];

	// Make folder with process ID in name
	int processID = getpid();
	char pidString[8] = "";
	memset(pidString, '\0', sizeof(char)*8);
	char directory[64] = "";
	memset(directory, '\0', sizeof(char)*64);
	sprintf(pidString, "%d", processID);
	strcat(directory, "lindsada.rooms.");
	strcat(directory, pidString);
	mkdir(directory, 0755);

	// More variables
	char filePath[128] = "";
	time_t t;
	srand((unsigned) time(&t));
	int i;
	int j;
	int k;
	ssize_t nwritten;
	struct room rooms[7];
	
	for (i = 0; i < 7;)
	{
		successOrNo = 99;
		memset(filePath, '\0', sizeof(char)*128);
		strcat(filePath, directory);
		strcat(filePath, "/");
		// This case statement chooses a random number, and a room based on that.
		// If the room has already been selected, it skips the rest of the function and doesn't increment the counter.
		switch (rand() % 10)
		{
			case 0:
				// If not already selected, make file path equal to the name, and store name in array.
				// Set sentinel value to avoid double picking.
				// Increment i (this is not done in the for loop).
				// Upon failure, set successOrNo variable.
				// All other cases arecomparable.
				if (sentinel0 == 0)
				{
					strcat(filePath, roomName0);
					strcat(rooms[i].name, roomName0);
					sentinel0 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 1:
				if (sentinel1 == 0)
				{
					strcat(filePath, roomName1);
					strcat(rooms[i].name, roomName1);
					sentinel1 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 2:
				if (sentinel2 == 0)
				{
					strcat(filePath, roomName2);
					strcat(rooms[i].name, roomName2);
					sentinel2 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 3:
				if (sentinel3 == 0)
				{
					strcat(filePath, roomName3);
					strcat(rooms[i].name, roomName3);
					sentinel3 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 4:
				if (sentinel4 == 0)
				{
					strcat(filePath, roomName4);
					strcat(rooms[i].name, roomName4);
					sentinel4 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 5:
				if (sentinel5 == 0)
				{
					strcat(filePath, roomName5);
					strcat(rooms[i].name, roomName5);
					sentinel5 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 6:
				if (sentinel6 == 0)
				{
					strcat(filePath, roomName6);
					strcat(rooms[i].name, roomName6);
					sentinel6 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 7:
				if (sentinel7 == 0)
				{
					strcat(filePath, roomName7);
					strcat(rooms[i].name, roomName7);
					sentinel7 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 8:
				if (sentinel8 == 0)
				{
					strcat(filePath, roomName8);
					strcat(rooms[i].name, roomName8);
					sentinel8 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
			case 9:
				if (sentinel9 == 0)
				{
					strcat(filePath, roomName9);
					strcat(rooms[i].name, roomName9);
					sentinel9 = 1;
					i++;
				}
				else
				{
					successOrNo = 0;
				}
				break;
		}

		// If the roomselection went well, it assigns a room type.
		if (successOrNo == 99)
		{
			if (i == 1)
			{
				strcpy(rooms[i-1].type, "START_ROOM");
			}
			else if (i == 7)
			{
				strcpy(rooms[i-1].type, "END_ROOM");
			}
			else
			{
				strcpy(rooms[i-1].type, "MID_ROOM");
			}
		}

	}

	// Modularized graph filling methods. Description below.
	// As long as the graph is incomplete, it adds a connection.
	while (graphNotComplete(rooms) == 1)
	{
		addRandomConnection(rooms);
	}
	
	// For each room...
	for (i = 0; i < 7; i++)
	{
		// Set up the file path.
		memset(filePath, '\0', sizeof(char)*128);
		strcat(filePath, directory);
		strcat(filePath, "/");
		strcat(filePath, rooms[i].name);
		
		// Open the file.
		int fileDescriptor = open(filePath, O_WRONLY | O_CREAT, 0555);
		
		// Feed in room name title and value.
		memset(buffer, '\0', sizeof(char)*1024);
		strcat(buffer, roomName);
		strcat(buffer, rooms[i].name);
		strcat(buffer, newLine);

		// Feed in connection title and value (for all connections).
		for (k = 0; k < rooms[i].numberOfConnections; k++)
		{
			strcat(buffer, connection);
			memset(connectionNumAsString, '\0', sizeof(char)*5);
			sprintf(connectionNumAsString, "%d", k+1);
			strcat(buffer, connectionNumAsString);
			strcat(buffer, colon);
			strcat(buffer, rooms[rooms[i].connections[k]].name);
			strcat(buffer, newLine);
		}

		// Feed in room type title and value.
		strcat(buffer, roomType);
		strcat(buffer, rooms[i].type);

		// Write it all to the file.
		nwritten = write(fileDescriptor, buffer, strlen(buffer) * sizeof(char));
	}

	return 0;
}

int graphNotComplete(struct room rooms[])
{
	// Basically, check each room. If any have less than three connections, return 1.
	// If all have three or more,  return 0.
	int i;
	for (i = 0; i < 7; i++)
	{
		if (rooms[i].numberOfConnections < 3)
		{
			return 1;
		}
	}
	return 0;
}


void addRandomConnection(struct room rooms[])
{
	// Variables
	time_t t;
        srand((unsigned) time(&t));
	int i;
	int k;
	int roomA;
	int roomB;
	int alreadyExists;

	// Loop forever (connections sometimes fail).
	// Upon a successful add, the function will terminate.
	while (1)
	{
		// Choose two random indexes. Initialize sentinel variable.
		roomA = rand() % 7;
		roomB = rand() % 7;
		alreadyExists = 0;

		// Check if connection already exists. Set variable if so.
		for (k = 0; k < rooms[roomA].numberOfConnections; k++)
		{
			if (rooms[roomA].connections[k] == roomB)
			{
				alreadyExists = 1;
			}
		}

		// If it doesn't exist, and neither room has 6 or more connections, add the connection.
		if (rooms[roomB].numberOfConnections < 6 && rooms[roomA].numberOfConnections < 6 && roomA != roomB && alreadyExists == 0)
		{
			// Make both rooms recognize each other, and increment both connection counts.
			rooms[roomA].connections[rooms[roomA].numberOfConnections] = roomB;
			rooms[roomB].connections[rooms[roomB].numberOfConnections] = roomA;
			rooms[roomA].numberOfConnections++;
			rooms[roomB].numberOfConnections++;
			return;
		}
	}
	return;
}
