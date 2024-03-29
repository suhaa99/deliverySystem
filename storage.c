#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

#define	ASSERT(exp)		{ if (!(exp)) \
						{ printf("가정(" #exp")이 소스파일 %s %d번째 줄에서 실패.\n"\
							, __FILE__, __LINE__), exit(1);}}

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;

static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password

storage_t	storage;		//x,y가 없을 때 변수로 사용하기 위함. 


// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	
	//set all the member variable as an initial value
	deliverySystem[x][y].building = 0;
	deliverySystem[x][y].room = 0;
	deliverySystem[x][y].cnt = 0;
	deliverySystem[x][y].passwd[PASSWD_LEN+1] = 0;	
	deliverySystem[x][y].context = NULL;
	
	storedCnt--;
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	//사용자가 넣은 비밀번호 변수 
	char inputpasswd[PASSWD_LEN+1];
	
	printf("input password for (%d, %d) storage : ", x, y);
	scanf("%4s", &inputpasswd);
	//마스터 비밀번호와 같은지 비교
	if(inputpasswd != masterPassword) 
	{
		//사용자가 넣은 비밀번호와 실제 비밀번호가 같은지 비교 
		if(inputpasswd != deliverySystem[x][y].passwd)
		{
			return -1;
		}
	}
	return 0;
}	



// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i, j;
	//파일 열기 
	FILE *fp;
	fp = fopen("storage.txt", "w");
	if(fp == NULL)
	{
		printf("cannot open the file.\n");
		return -1;
	}	
	
	for(i=0;i<systemSize[0];i++) {
		for(j=0;j<systemSize[1];j++)
			fprintf(fp, "%d %d %s %d %d %d %d %s %s", systemSize[0], systemSize[1], 
											  masterPassword, i, j, 
											  deliverySystem[i][j].building, deliverySystem[i][j].room, 
											  deliverySystem[i][j].passwd, 
											  deliverySystem[i][j].context);
	}
	fclose(fp);
	
	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	//make file NULL
	FILE *fp = NULL;
	fp=fopen(filepath, "r");
	char c;

	//check open&close: FILE *fopen(STORAGE_FILEPATH, "r");
	if (fp == NULL)
	{
		return -1;
	}
	
	//malloc
	int i, j;
	//apply the locker's column&row
	int inputrow, inputcolumn;
		
	deliverySystem = (storage_t**)malloc(systemSize[0]*sizeof(storage_t*));
	for(i=0;i<systemSize[0];i++) {
		deliverySystem[i]= (storage_t*)malloc(systemSize[1]*sizeof(storage_t));
	}
	
	fscanf(fp, "%d %d %s", &systemSize[0], &systemSize[1], masterPassword);
	
	for(i=0;i<systemSize[0];i++) {
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].context = (char *)malloc(100 * sizeof(char));
	}
	
	while(fscanf(fp, "%d %d", &inputrow, &inputcolumn)==2){
		fscanf(fp, "%d %d %s %s", deliverySystem[inputrow][inputcolumn].building, deliverySystem[inputrow][inputcolumn].room, deliverySystem[inputrow][inputcolumn].passwd, deliverySystem[inputrow][inputcolumn].context);
		printf("%d %d %s %d %d %d %d %s %s", systemSize[0], systemSize[1], 
											  masterPassword, inputrow, inputcolumn, 
											  deliverySystem[inputrow][inputcolumn].building, deliverySystem[inputrow][inputcolumn].room, 
											  deliverySystem[inputrow][inputcolumn].passwd, 
											  deliverySystem[inputrow][inputcolumn].context);
	}
	
	
	fclose(fp);
	
	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i;
	
	for(i=0; i<systemSize[0]; i++)
	{
		free(deliverySystem[i]);		//일중포인터 FREE시킴 
	}
	free(deliverySystem);				//이중포인터 FREE시킴 
	deliverySystem = NULL; 			
	free(storage.context);				//context pointer FREE시킴 
	storage.context = NULL;
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
		int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	//context 포인터에 메모리 할당 
	deliverySystem[x][y].context = (char*)malloc((strlen(msg+1)) * sizeof(char));
	
	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	deliverySystem[x][y].cnt=1;
	deliverySystem[x][y].context = msg; 
	deliverySystem[x][y].passwd[PASSWD_LEN+1] = passwd[PASSWD_LEN+1];
	
	//cell을 차지하는 변수 cnt 증가 
	storedCnt++;
	
	return 0; 
}                                                                 
	

//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	//비밀번호 체크 
	if(inputPasswd(x,y)!=0) {
		printf("password does not match!");
		return -1;
	}
	printf(" ---------------> extracting the storage (%d, %d)...\n", x, y);
	//비밀번호 체크
	printStorageInside(x,y);
	//storage 다시 초기화 시킴 
	initStorage(x,y);
	//메모리 반납 
	free(deliverySystem[x][y].context);
	//cell에 차지하고 있던 cnt 빼기 
	storedCnt--;
					
	return 0;
	
}                                   
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	
	int i, j;
	int x, y;	//x->nBuilding, y->nRoom 
	int cnt=0; 
	
	for (i=0;i<systemSize[0];i++)
	{
		for (j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[x][y].building == nBuilding && deliverySystem[x][y].room == nRoom)
			{
				printf(" ---------> Found a package in (%d, %d)", x, y);
				cnt++;
			}
		}
	}
	return cnt; 
}
