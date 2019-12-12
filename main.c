#include <stdio.h>
#include <stdlib.h>
#include "storage.h"

#define N_BUILDING				10
#define N_FLOOR					10
#define N_ROOM					10

#define STORAGE_FILEPATH 		"storage.txt"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int getIntegerInput(void)
{
	int input = -1;
	scanf("%d", &input);
	fflush(stdin);
	
	return input;
}




int buildingValidityCheck(int nBuilding, int nRoom)
{
	if (nBuilding <= 0 || nBuilding > N_BUILDING)
	{
		return -1;
	}
	
	if (nRoom/100 <= 0 || nRoom/100 > N_FLOOR)
	{
		return -1;
	}
	
	if (nRoom%100 <= 0 || nRoom%100 > N_ROOM)
	{
		return -1;
	}
	
	return 0;
}


int main(int argc, char *argv[]) {
	
	
	int cmd;
	int x, y, nBuilding, nRoom;
	char msg[MAX_MSG_SIZE+1];
	char passwd[PASSWD_LEN+1];
	
	//1. initialize the delivery system
	if (str_createSystem(STORAGE_FILEPATH) != 0)
	{
		printf("failed to open storage config file! (%s)\n", STORAGE_FILEPATH);
		return -1;
	}
	
	printf("------- Unmanned Delivery Storage System START -------\n");
	
	
	str_printStorageStatus();
	
	#if 0
	//user mode
	do
	{
		printf("\n\n");
		printf("----------------------------------------------\n");
		printf("1. put a package\n");
		printf("2. get my package\n");
		printf("3. check the storage status\n");
		printf("4. find my package\n");
		printf("0. exit\n");
		printf("----------------------------------------------\n");
		printf("command : ");
		
		cmd = getIntegerInput();	
		
		switch(cmd)
		{
			case 0:
				printf(" -----------> exiting the program....\n");
				break;
				
			case 1:
				str_printStorageStatus();
				
				printf(" - storage row : ");
				x = getIntegerInput();
				printf(" - storage column : ");
				y = getIntegerInput();
				
				if (str_checkStorage(x,y) != 0)
				{
					printf(" -----------> Storage (%i,%i) is already occupied or invalid!\n", x, y);
					continue;
				}
				
				printf(" - building # : ");
				nBuilding = getIntegerInput();
				printf(" - room # : ");
				nRoom = getIntegerInput();
				
				if (buildingValidityCheck(nBuilding, nRoom) != 0)
				{
					printf(" -----------> Invalid building/room number (%i,%i)\n", nBuilding, nRoom);
					continue;
				}
				
				printf(" - message (max : 100 characters) : ");
				scanf("%100s", msg);
				fflush(stdin);
				
				passwd[0] = 0;
				printf(" - password : ");
				scanf("%4s", passwd);
				fflush(stdin);
				
				printf(" -----------> processing to put the package...\n\n");
				
				if (str_pushToStorage(x, y, nBuilding, nRoom, msg, passwd) == 0)
				{
					printf(" -----------> Succeeded to put package for (%i, %i) user in (%i,%i), msg : %s passwd : %s\n", nBuilding, nRoom, x, y, msg, passwd);
				}
				else
				{
					printf(" -----------> Failed to put the package in the storage!\n");
				}
				break;
				
			case 2:
				printf(" - storage row : ");
				x = getIntegerInput();
				printf(" - storage column : ");
				y = getIntegerInput();
				
				if (str_checkStorage(x,y) <= 0)
				{
					printf(" -----------> Storage (%i,%i) is empty or invalid!\n", x, y);
					continue;
				}
				if (str_extractStorage(x,y) != 0)
				{
					printf(" -----------> Failed to extract my package!\n");
				}
				break;
				
			case 3:
				str_printStorageStatus();
				break;
				
			case 4:
				printf(" - building # : ");
				nBuilding = getIntegerInput();
				printf(" - room # : ");
				nRoom = getIntegerInput();
				
				if (buildingValidityCheck(nBuilding, nRoom) != 0)
				{
					printf(" -----------> Invalid building/room number (%i,%i)\n", nBuilding, nRoom);
					continue;
				}
				
				
				
				if (str_findStorage(nBuilding, nRoom) == 0)
				{
					printf(" -----------> Failed to find my package!\n");
				}
				
				break;
				
			default:
				printf(" -----------> Wrong cmd!! input again!..\n");
				break;
		}
		
		if (str_backupSystem(STORAGE_FILEPATH) != 0)
		{
			printf("[WARNING] failed to backup the delivery system!\n");
		}
	} while (cmd != 0);
	#endif
	
	str_freeSystem();
	
	return 0;
}
