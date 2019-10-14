#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct Room Room;

struct Room {
    char name[13]; 
    char type[15];
    //char connections[6][10];
    Room * connections[6];
    int numConns;
};

char * findCurrentDirectory() { 
    
    struct dirent * dir_entry;
    struct stat statbuf;

    DIR * dr = opendir(".");

    char * workingDir = "";
    time_t moddedTime;

    while ((dir_entry = readdir(dr)) != NULL) { //iterate through every file in the game directory 
    
        if (strstr(dir_entry->d_name, ".rooms") != NULL) { //if the file is a "rooms" directory

            stat(dir_entry->d_name, &statbuf);

            // Set the current working directory If the one being compared was modified after the current one
            // Or if this is the first rooms dir encountered. 
            if ((difftime(statbuf.st_mtim.tv_sec, moddedTime) > 0) || (workingDir == "")) { 

                workingDir = dir_entry->d_name;
                moddedTime = statbuf.st_mtim.tv_sec;
            }
        }            
    }

    closedir(dr);

    printf("Returning %s \n", workingDir);
    return workingDir;
} 

void parseLine(char * line, char* lineParts[]) {

     char* token = strtok(line, " :");
     //token = strtok(NULL, ": ");

     int linePart = 0;

     while (token != NULL) {

        lineParts[linePart] = token;
        //printf("token is %s \n", lineParts[linePart]);

        token = strtok(NULL, " :");
        linePart++;
     }

     //printf("Last linepart is %s \n", lineParts[2]);
}

int findRoom(struct Room rooms[], char* roomToFind, char* charType) { //Takes in the room characteristic type and value and returns its index in the rooms struct

    printf("Finding a %s that matches %s \n", charType, roomToFind); 

    for (int i = 0; i < 7; i++) { 
        char* testRoom;
        printf("Currently on room %s \n", rooms[i].name);

        if (charType == "type") {
            testRoom = rooms[i].type;
            printf("Currently Testing %s \n", testRoom);
        }
        else {
            testRoom = rooms[i].name;
            printf("Current room type is %s \n", rooms[i].type);
        }

        printf("Testing if %s matches %s \n", testRoom, roomToFind);

        if (strcmp(testRoom, roomToFind) == 0) { 
            printf("Found! \n");
            return i;       
        }
    } 

    printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN. \n"); //Fallback if the room provided doesn't exist
}


void initRooms(struct Room *rooms, struct dirent *dir_entry, DIR* dr) {

    //chdir(workingDir); //Move to the most recent directory

    dr = opendir(".");

    int roomInd = 0;

    while ((dir_entry = readdir(dr)) != NULL) { //iterate through every file in the game directory 
        //printf("name of file is %s \n", dir_entry->d_name);

        char * fileName = dir_entry->d_name;

        if (strstr(fileName, "room") != NULL) { //If the file is a room file
            char* roomName = strtok(fileName, "_");
            printf("Found Room name is %s \n", roomName);

            strcpy(rooms[roomInd].name, roomName);

            roomInd++;
        }
    }
    
    //closedir(dr);
}

void readRooms (struct Room *rooms, struct dirent *dir_entry, DIR* dr) {
    
    //For file iteration in directory
    //DIR *dr = opendir(".");
    FILE *fptr;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //For parsing of Connection values
    char *endptr;

    char* lineParts[4];

    int roomNum = 0; 

        
    while ((dir_entry = readdir(dr)) != NULL) { //iterate through every file in the game directory 
        printf("name of file is %s \n", dir_entry->d_name);

        //char * fileName = dir_entry->d_name;

        if (strstr(dir_entry->d_name, "room") == NULL) { //Skip any non-room file
            //printf("Skipping bad files \n");
            continue;
        }

        fptr = fopen(dir_entry->d_name, "r");

        while ((read = getline(&line, &len, fptr)) != -1)  { //while the end of the file is not reached
            
            //printf("The room num is %d \n", roomNum);

            //char * line_ptr = &line;

            parseLine(line, lineParts);

            printf("lineparts[1] is %s \n", lineParts[1]);
            
            if (strcmp(lineParts[1], "TYPE") == 0) {
                strcpy(rooms[roomNum].type, lineParts[2]);
                printf("setting room type as %s \n", rooms[roomNum].type);
            }
            
            else if (strcmp(lineParts[0], "CONNECTION") == 0) {
                int numOfConns = atoi(lineParts[1]); 
                    
                rooms[roomNum].numConns = numOfConns; //Number of connections is the last Connection number in the file: will be in the last parsed connection
                 
                int connectNum = numOfConns - 1; //For use in array indexing

                rooms[roomNum].connections[connectNum] = &(rooms[findRoom(rooms, lineParts[2], "name")]); //Find index of parsed room name, then pass pointer to that room object to the Connections array

                //strncpy(rooms[roomNum].connections[connectNum], lineParts[2], strlen(lineParts[2]-1));

                printf("Setting connection %d to %s \n", connectNum, rooms[roomNum].connections[connectNum]->name);

            }
            /*
               
            else if (strcmp(lineParts[1], "NAME") == 0)  {
                //char* roomName = lineParts[2];

                //printf("Room name found to be %s \n", lineParts[2]);

                strcpy(rooms[roomNum].name, lineParts[2]);
                printf("setting room name as %s \n", rooms[roomNum].name); 
            }

            */

        }

        printf("Within readRooms, a random room conn is %s \n", rooms[1].connections[1]->name);
        roomNum++;
    }
    //closedir(dr);
}

/*
void readConnections(struct Room rooms[]) {
    while ((dir_entry = readdir(dr)) != NULL) { //iterate through every file in the game directory 

        for (int y = 0; y < 7; y++) { //For every Room in the Room array

            char *roomFile = strcat(rooms[y].name, "_ROOM"); 

            if (strcmp(dir_entry->d_name, roomFile) != 0) {continue;} //Only read connections from matching file to Room


            }
        }
    }
}
*/


void printConnections(struct Room rooms[], int roomPos) {

    //int connNum = 0;
    
    //while(rooms[roomPos].connections[++connNum] != NULL); 
    //int connNum = strlen(&rooms[roomPos].connections);

    //int connNum = sizeof(rooms[roomPos].connections) / sizeof(char*); //Determine how many connections there are

    //printf("Finding connections. \n");

    int roomConnNum = rooms[roomPos].numConns;

    for(int j = 0; j < roomConnNum; j++) {
    //while (rooms[roomPos].connections[j] != NULL) {
        if (j == (roomConnNum - 1)) { //If it's the last connection, format correctly. Otherwise, just continue the list
            printf("%s. \n", rooms[roomPos].connections[j]->name);
            break;
        }
        else 
            printf("%s, ", rooms[roomPos].connections[j]->name);
    }    
}


int goToRoom(struct Room rooms[], char* room, char* charType, int stepNum) {

    int roomPos = findRoom(rooms, room, charType);

    printf("CURRENT LOCATION: %s \n", rooms[roomPos].name);
    printf("POSSIBLE CONNECTIONS: ");

    printConnections(rooms, roomPos);

    printf("WHERE TO? >");

    char destination[10];

    scanf("%s", destination);

    printf("Want to go to %s \n", destination);

    if (strcmp(rooms[roomPos].type, "END_ROOM") == 0) {
        printf("End room found!");
        return stepNum;
    }
    else {
        int itrStepNum = goToRoom(rooms, destination, "name", (stepNum++));
    }
}

void playGame(struct Room rooms[]) {

    printf("a random room name is %s \n", rooms[1].name);

    int numSteps = goToRoom(rooms, "START_ROOM", "type", 1);
    
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS! \n YOU TOOK %d STEPS. \n", numSteps);    
}

int main(){    
    
    struct Room rooms[7]; 
    char * workingDir = "";

    struct dirent * dir_entry; 


    //For file iteration in directory
    FILE *fptr;
    char *line = NULL;
    //printf("Entering findcurrent \n");

    workingDir = findCurrentDirectory(); //Find the most recent directory created by buildrooms.c
   
    chdir(workingDir);
    DIR *dr = opendir(".");


    printf("Working dir now is %s \n", workingDir);

    initRooms(rooms, dir_entry, dr); //Initialize room array with room names

    readRooms(rooms, dir_entry, dr); //read all Room types and connections into the rooms array

    playGame(rooms);

    closedir(dr);

    return 0;
}
