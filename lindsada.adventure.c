#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

// Declaration of struct used for rooms
struct room
{
	char name [9];
	char type [16];
	char connections [6][15];
	int numberOfConnections;
};

// Global variables to set up pthreading and mutex.
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread;

// Pthread function
void *threadFunction(void *arg)
{
	// Variables
	ssize_t nwritten;
	struct tm *theTM;
	time_t rawtime;
	char *timeFileName = "currentTime.txt";
	char buffer [64];
	
	// Lock thread before proceeeding
	pthread_mutex_lock(&myMutex);

	// Open file
	int fileDescriptor = open(timeFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	// Flush buffer
	memset(buffer, '\0', 64);

	// Write time into file using buffer
	time(&rawtime);
	theTM = localtime(&rawtime);
	strftime(buffer, 64, "%l:%M%P, %A, %B %d, %Y", theTM);
	nwritten = write(fileDescriptor, buffer, strlen(buffer));

	// Close file, unlock mutex
	close(fileDescriptor);
	pthread_mutex_unlock(&myMutex);
}

int main()
{
	// Variables
	char *input;
	char *timeFileName = "currentTime.txt";
	size_t size = 10;
	ssize_t nwritten;
	input = malloc(size * sizeof(char));
	memset(input, '\0', sizeof(char)*10);
	struct room rooms[7];
	char *timeString = "time";
	DIR* newestRoomsDir;
	char targetDirPrefix[32] = "lindsada.rooms.";
	char newestDirName[256];
	memset(newestDirName, '\0', sizeof(newestDirName));
	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;
	int newestDirTime = -1;
	int fileDescriptor;	
	int noPthread = 1;
	int i;
	int j;
	int timeCommand;
	int numberOfSteps = 0;
	struct dirent* currentFile;
	char currentFileName[64];
	char *buffer;
	size_t bufferSize = 64;
	FILE* filePointer;
	buffer = (char *)malloc(bufferSize * sizeof(char));
	char *nameBuffer[10];
	char str1 [20];
	char str2 [20];
	char str3 [20];
	char steps [100][10];
	
	// Find most recently created rooms direectory.
	// Open current directory.
	dirToCheck = opendir(".");
	if (dirToCheck > 0)
	{
		// While there are still directories to check...
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			// If directory includes necessary prefix...
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				// Get attributes
				stat(fileInDir->d_name, &dirAttributes);
				
				// If the time is newer than what we have, use that instead.
				if ((int)dirAttributes.st_mtime > newestDirTime)
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}
	}
	closedir(dirToCheck);

	// Open newest rooms directory, read all its files.
	newestRoomsDir = opendir(newestDirName);
	i = 0;

	// While there are files to read...
	while ((currentFile = readdir(newestRoomsDir))!= NULL)
	{
		// Other than current and parent directories...
		if (strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0)
		{
			// Initialize number of connections to 0.
			rooms[i].numberOfConnections = 0;
			// Flush filename variable.
			memset(currentFileName, '\0', sizeof(currentFileName));
			// Make file name out of directory name and current file name.
			sprintf(currentFileName, "%s/%s", newestDirName, currentFile->d_name);
			// Open file.
			filePointer = fopen(currentFileName, "r");
			// Flush buffer and string variables.
			memset(buffer, '\0', bufferSize);
			memset(str1, '\0', 20);
			memset(str2, '\0', 20);
			memset(str3, '\0', 20);

			// While fscanf doesn't fail... (read all lines)
			// Read three strings delimited by white space
			while (fscanf(filePointer,"%s %s %s", str1, str2, str3) == 3)
			{
				// If it's the name, put it in name
				if (strcmp(str2,"NAME:") == 0)
				{
					sprintf(rooms[i].name, "%s\0", str3);
				}
				// If it's a connection, add it
				if (strcmp(str1,"CONNECTION") == 0)
				{
					memset(rooms[i].connections[rooms[i].numberOfConnections],'\0', 15);
					sprintf(rooms[i].connections[rooms[i].numberOfConnections], "%s\0", str3);
					rooms[i].numberOfConnections++;
				}
				// If it's the type, put it in type
				if (strcmp(str2,"TYPE:") == 0)
				{
					sprintf(rooms[i].type, "%s\0", str3);
				}
				// Flush variables
				memset(str1, '\0', 20);
				memset(str2, '\0', 20);
				memset(str3, '\0', 20);
			}
			// Close file, incremeent i
			fclose(filePointer);
			i++;
		}
	}

	// Free buffer space, close directory.
	free(buffer);
	closedir(newestRoomsDir);

	// Find start room, output prompt using that.
	for (i = 0; i < 7; i++)
	{
		if (strcmp(rooms[i].type, "START_ROOM") == 0)
			break;
	}
	printf("CURRENT LOCATION: %s\n", rooms[i].name);
	printf("POSSIBLE CONNECTIONS: ");
	// Loop through connections and output each.
	for (j = 0; j < rooms[i].numberOfConnections; j++)
	{
		printf(rooms[i].connections[j]);
		if (j < rooms[i].numberOfConnections-1)
		{
			printf(", ");
		}
		else
		{
			printf(".\n");
		}
	}
	printf("WHERE TO? >");




	// Endless loop of prompts until user quits.
	while (1)
	{
		// Assume a step has been made (we will fix this below if one hasn't).
		numberOfSteps++;
		// timeCommand is a sentinel showing if the time command was used.
		// I initialize it to 0 at the beginning of each loop.
		timeCommand = 0;
		// Initialize input buffer and current step list character array.
		memset(steps[numberOfSteps-1], '\0', 10);
		memset(input, '\0', sizeof(char)*10);

		// Get input, remove newline character
		getline(&input, &size, stdin);
		input[strlen(input)-1] = '\0';

		// Add input to step list
		memset(steps[numberOfSteps-1], '\0', 10);
		strcpy(steps[numberOfSteps-1], input);

		// If the input was "time"
		if (strcmp(input, timeString) == 0)
		{
			// Initilize the pthread mutex
			pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
			// Make pthread if there isn't one yet.
			if (noPthread == 1)
			{
				noPthread = pthread_create(&thread, NULL, threadFunction, NULL);
			}

			// Wait for pthread to finish, then continue.
			pthread_join(thread, NULL);
			pthread_mutex_destroy(&myMutex);

			// Time is not a step, so decrement steps variable.
			numberOfSteps--;

			// Lock mutex.
			pthread_mutex_lock(&myMutex);

			// Open file, read, print, and close.
			filePointer = fopen(timeFileName, "r");
			memset(buffer, '\0', bufferSize);
			getline(&buffer, &bufferSize, filePointer);
			printf("\n%s\n", buffer);
			fclose(filePointer);

			// Unlock mutex and set timeCommand sentinel value.
			pthread_mutex_unlock(&myMutex);
			timeCommand = 1;
		}

		// Preserve current room value for use below.
		int currentRoomValue = i;
		
		// Only do this if the time command wasn't used.
		if (timeCommand == 0)
		{
			// Find the input word in the room list.
			for (i = 0; i < 7; i++)
			{
				if (strcmp(input, rooms[i].name) == 0)
					break;
			}

			// Find the input word in the connection list.
			for (j = 0; j < rooms[currentRoomValue].numberOfConnections; j++)
			{
				if (strcmp(input, rooms[currentRoomValue].connections[j]) == 0)
					break;
			}

			// If it was not present in one of those, reject it.
			if (i == 7 || j == rooms[currentRoomValue].numberOfConnections)
			{
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
				i = currentRoomValue;

				// Again, not a step. Decrement.
				numberOfSteps--;
			}
		}

		// If they get to the end, show ending dialog and exit.
		if (strcmp(rooms[i].type,"END_ROOM") == 0)
		{
			printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", numberOfSteps);
			for (j = 0; j < numberOfSteps; j++)
			{
				printf("%s\n", steps[j]);
			}
			free(input);
			pthread_exit(NULL);
			return 0;
		}

		// Output prompt again, and loop.
		printf("\nCURRENT LOCATION: %s\n", rooms[i].name);
		printf("POSSIBLE CONNECTIONS: ");
		for (j = 0; j < rooms[i].numberOfConnections; j++)
		{
			printf(rooms[i].connections[j]);
			if (j < rooms[i].numberOfConnections-1)
			{
				printf(", ");
			}
			else
			{
				printf(".\n");
			}
		}
		printf("WHERE TO? >");
	}

	// Closing up shop...
	free(input);
	pthread_exit(NULL);
	return 0;
}
