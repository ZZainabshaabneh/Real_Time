#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "shared_memories.h"
#include "header.h"
#include <stdbool.h>
#include "local.h"
#define maxWordLength 50
#define maxLineLength 500
#define maxLines 1000
#define maxWordsInLine 100
#define maxEncodedLine 10000
#define maxDecodedLine 1000
#define SHM_SIZE 100000

char messageColumns[maxWordsInLine][maxLineLength];//array for the columns
char messageEncoded[maxWordsInLine][maxEncodedLine];//array for the encoded columns
char messageDecoded[maxWordsInLine][maxDecodedLine];//array for the decoded columns
char receivedMessage[maxWordsInLine][maxLineLength];//array for received message with original ordered lines
char messageDecodedByMaster[maxWordsInLine][maxLineLength];//
char chatchedColumnsBySpy[maxWordsInLine][maxEncodedLine];//by spies
char chatchedColumnsByR[maxWordsInLine][maxEncodedLine];//by receiver
char receiverColumns[maxWordsInLine][maxEncodedLine];//spy master without duplicated data
char masterColumns[maxWordsInLine][maxEncodedLine];//spy master without duplicated data
int catchedIndexR=0;//receiver
int rColumnsIndex=0;//receiver after dropping
int catchedIndexSpies=0;//num of catched columns by spies
int masterColumnsIndex=0;//after dropping duplicated
int Lines=0;//at the end of decoding 
int maxColumns=0;//num of columns
int maxLinesNum=0;
int columnSuffix=0;//column index->suffix 
int num_successful_operations=0;
int num_failed_operations = 0;


//defined functions
void inputFile();//1-3 parts
void encode_column(char *message, int columnIndex);//part 4 
void decode_column(char *message,int id);//part 5
void handle_successful_dencoding();//last part
void handle_failed_dencoding();//last part

void inputFile(){
    char array1[maxLines][maxLineLength];
    char array2[maxWordsInLine][maxLineLength];
    
    FILE* file=fopen("sender.txt","r");
    if(file==NULL){
        printf("Failed to open sender file");
        exit(1);
    }
    int row=0;
    char line[10000];
    while(fgets(line,sizeof(line),file)!=NULL){
        maxLinesNum++;
        char *word=strtok(line,"\n");
        while(word!=NULL){
            strcpy(array1[row],word);
            row++;
            word=strtok(NULL,"\n");
        }
    }
    fclose(file);
    
    int maxCol=0;
    //printf("=========Tokeninzing==========\n");
    for(int i=0;i<row;i++){
        char line[10000];
        strcpy(line,array1[i]);
        char *word=strtok(line," \t");
        int flag=0;
        while(word!=NULL){
            flag++;
            word=strtok(NULL," \t");
        }
        if(maxCol<flag){//to get max columns
            maxCol=flag;
        }
    }
    for(int i=0;i<row;i++){
        char line[10000];
        strcpy(line,array1[i]);
        char *word=strtok(line," \t");
        int flag=0;
        while(word!=NULL){
            strcat(array2[flag],word);
            strcat(array2[flag]," ");
            flag++;
            word=strtok(NULL," \t");
        }

        if(maxCol>flag){//to add alright for spaces
            int dif=maxCol-flag;
            for(int j=0;j<dif;j++){
                strcat(array2[flag],"alright");
                strcat(array2[flag]," ");
                flag++;
            }
        }
    }
    for(int i =0;i<maxCol;i++){
        strcpy(messageColumns[i],array2[i]);//storing in main array
        //printf("messageColumns[%d]=%s\n",i,messageColumns[i]);
    }
    maxColumns=maxCol;
}

void encode_column( char *message, int columnIndex) {
    char line[maxLineLength];
    strcpy(line,message);

    char *ptr=strtok(line," \t");
    int columnSize=0;
    while(ptr!=NULL){
        columnSize++;
        ptr=strtok(NULL," \t");
    }
    int index=0;
    strcpy(line,message);

    ptr=strtok(line," \t");
    char column[columnSize][maxWordLength];
    char encoded[columnSize][maxEncodedLine];

    while(ptr!=NULL){
        strcpy(column[index],ptr);
        index++;
        ptr=strtok(NULL," \t");
    }
    int doSuffix=0;
    for (int i = 0; i < columnSize; i++) {
        char* word = column[i];
        char* encodedWord = encoded[i];

        int len = strlen(word);
        int modifier ;
        int isNumber=0;
        char encodedNumber[len*7];
        strcpy(encodedNumber,"");
        for (int j = 0; j < len; j++) {
            modifier= (columnIndex + 1) * (j + 1);
            if (word[j] >= 'A' && word[j] <= 'Z') {
                encodedWord[j] = 'A' + ((word[j] - 'A' + modifier) % 26);
                   }
            else if (word[j] >= 'a' && word[j] <= 'z') {
                encodedWord[j] = 'a' + ((word[j] - 'a' + modifier) % 26);
                   }
            else if (word[j]=='!') {
                encodedWord[j] = '1';
                   }
            else if (word[j]=='?') {
                encodedWord[j] = '2';
                   }
            else if (word[j]==',') {
                encodedWord[j] = '3';
                   }
            else if (word[j]==';') {
                encodedWord[j] = '4';
                   }
            else if (word[j]==':') {
                encodedWord[j] = '5';
                   }
            else if (word[j]=='%') {
                encodedWord[j] = '6';
                   }
            else if (word[j] >= '0' && word[j] <= '9') {
                isNumber=1;
                int number = word[j] - '0';
                number = 1000000 - number;
                char temp[7];
                sprintf(temp, "%d", number);
                int tempLength=strlen(temp);
                
                if(tempLength!=7){// to make them the same length
                    strcpy(encodedNumber,"0");
                    strcat(encodedNumber,temp);
                    
                }else{
                    strcat(encodedNumber,temp);
                }
            }else{
                encodedWord[j] = word[j];
            }
        }
        int numLength=len*(7);
        if(isNumber==1){
            for(int k=0;k<numLength;k++){
                encodedWord[k]=encodedNumber[k];
            }
            encodedWord[numLength] = '\0';
        }else{
            encodedWord[len] = '\0';
        }
        if(i==(columnSize-1)){
            doSuffix=1;
        }
    }
    if(doSuffix==1){//adding suffix
        char *suffix=encoded[columnSize];
        char tempSuffix[maxWordsInLine];
        char temp0[maxWordsInLine];
        strcpy(tempSuffix,"0");//strcat
        sprintf(temp0,"%d",columnIndex);
        strcat(tempSuffix,temp0);

        int suffixLength=strlen(tempSuffix);
        for(int j=0;j<suffixLength;j++){
            suffix[j]=tempSuffix[j];
        }
        suffix[suffixLength]='\0';
        printf("suffix=%s\n",suffix);
    }
    int columnSizeEncoded=columnSize+1;
    //char encodedColumn[maxEncodedLine];
    for(int i=0;i<columnSizeEncoded;i++){//storing in messageEncoded array according to columnIndex
        //printf("Encoded word=%s\n",encoded[i]);
        strcat(messageEncoded[columnIndex],encoded[i]);
        strcat(messageEncoded[columnIndex]," ");
    }
    printf("column=%d original=%s\n",columnIndex,message);
    printf("column=%d encoded:%s\n",columnIndex,messageEncoded[columnIndex]);
}

void decode_column( char *message,int id) {
    char line[10000];
    strcpy(line,message);

    char *ptr=strtok(line," \t");

    int columnSize=0;
    while(ptr!=NULL){
        columnSize++;
        ptr=strtok(NULL," \t");
    }
    int index=0;
    strcpy(line,message);

    ptr=strtok(line," \t");
    char column[columnSize][maxLineLength];
    char decoded[columnSize][maxLineLength];
    while(ptr!=NULL){
        strcpy(column[index],ptr);
        index++;
        ptr=strtok(NULL," \t");
    }
    char *suffixPtr=column[columnSize-1];
    char suffix[maxWordsInLine];
    int suffixLength=strlen(column[columnSize-1]);
    suffixPtr[0]='\0';
    for(int i=0;i<(suffixLength-1);i++){
        suffixPtr[i]=suffixPtr[i+1];
    }
    suffixPtr[suffixLength-1]='\0';
    int columnIndex=atoi(column[columnSize-1]);

    columnSuffix=columnIndex;
    for (int i = 0; i < columnSize; i++) {
        char* word = column[i];
        char* decodedWord = decoded[i];

        int len = strlen(word);
        int modifier;
        int isNumber=0;

        if(len>=7){
            int wantedLength=0;
            char numStr[7];
            strcpy(numStr,"");
            char digit[1];
            int digitNum=0;

            for(int j=0;j<len;j++){
                char *numPtr=numStr;
                if (word[j] >= '0' && word[j] <= '9') {
                    isNumber=1;
                    numPtr[j]=word[j];
                    wantedLength+=1;
                    if(wantedLength==7){
                        int num=atoi(numStr);
                        num=1000000-num;
                        sprintf(decodedWord + digitNum, "%d", num);
                        digitNum++;
                        wantedLength=0;
                        strcpy(numStr,"");
                    }
                }

            }
            decodedWord[digitNum+1] = '\0';
        }
        if(isNumber!=1){
            for (int j = 0; j < len; j++) {
            modifier= (columnIndex + 1) * (j + 1);
            if (word[j] >= 'A' && word[j] <= 'Z') {
                decodedWord[j] = 'Z' + ((word[j] - 'Z' - modifier) % 26);
                   }
            else if (word[j] >= 'a' && word[j] <= 'z') {
                decodedWord[j] = 'z' + ((word[j] - 'z' - modifier) % 26);
                   }
            else if (word[j]=='1') {
                decodedWord[j] = '!';
                   }
            else if (word[j]=='2') {
                decodedWord[j] = '?';
                   }
            else if (word[j]=='3') {
                decodedWord[j] = ',';
                   }
            else if (word[j]=='4') {
                decodedWord[j] = ';';
                   }
            else if (word[j]=='5') {
                decodedWord[j] = ':';
                   }
            else if (word[j]=='6') {
                decodedWord[j] = '%';
                   }
                   else{
                decodedWord[j] = word[j];
                //printf("%s\n",word);
                }
            }
            decodedWord[len] = '\0';
        }
        //printf("for i=%d ,D=%s \n",i,decodedWord);messageDecodedByMaster
    }
    if(id==1){
        for(int i=0;i<columnSize;i++){//storing in messageEncoded array according to columnIndex
        strcat(messageDecoded[columnIndex],decoded[i]);
        strcat(messageDecoded[columnIndex]," ");
        }
    }else{
        for(int i=0;i<columnSize;i++){//storing in messageEncoded array according to columnIndex
            strcat(messageDecodedByMaster[columnIndex],decoded[i]);
            strcat(messageDecodedByMaster[columnIndex]," ");
        }
    }
}

void swap(char* shm_ptr, int shm_size) {
    srand(time(NULL));  // Initialize random number generator with current time
    // Generate two random offsets within the shared memory
    int offset1 = rand() % shm_size;
    int offset2 = rand() % shm_size;
    // Swap the data between the two offsets
    char temp;
    temp = shm_ptr[offset1];
    shm_ptr[offset1] = shm_ptr[offset2];
    shm_ptr[offset2] = temp;
    printf("Swapped data at offsets %d and %d\n", offset1, offset2);
}
void handle_successful_dencoding(int shm_id) {
    num_successful_operations--;
    if(num_successful_operations==0){
        //remove_shared_memory(shm_id);
        exit(0);
    }
}

void handle_failed_dencoding(int shm_id) {
    num_failed_operations--;
    if(num_failed_operations==0){
        printf("Simulation terminated: Maximum failed decoding operations reached.\n");
        //remove_shared_memory(shm_id);
        exit(0);
    }
    printf("Failed encoding operation. Counter: %d\n", num_failed_operations);
}


///=============================creating shared memory functions======================
int create_shared_memory(key_t key) {
    int shm_id;
    // Create the shared memory segment
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    return shm_id;
}
char* attach_shared_memory(int shm_id) {
    char* shm_ptr;
    // Attach to the shared memory segment
    shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*) -1) {
        perror("shmat");
        exit(1);
    }
    return shm_ptr;
}
void write_shared_memory(char* shm_ptr, const char* message) {
    // Write data to the shared memory
    strncpy(shm_ptr, message, SHM_SIZE);
    //printf("Writing %s is done successfull!\n",message);
}
void read_shared_memory(const char* shm_ptr) {
    // Read and print the data from the shared memory
    printf("Data read from shared memory: %s\n", shm_ptr);
}
void detach_shared_memory(char* shm_ptr) {
    // Detach from the shared memory segment
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(1);
    }
}
void remove_shared_memory(int shm_id) {
    // Remove the shared memory segment
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }
}
//=========================handling queue==================
struct msg {
    char column[maxWordsInLine][maxWordLength];
};
void accessSharedMemory(const char* shm_ptr, int msqid, int id) {//id=1 for receiver and 2 for spies
    srand(time(NULL));
    // Randomly select an index in the shared memory
    int index = rand() % maxColumns;
    
    // Access the encoded message at the selected index
    char encodedMsg[maxWordLength];
    strncpy(encodedMsg, &shm_ptr[index * maxWordLength], maxWordLength);
    encodedMsg[maxWordLength - 1] = '\0';

    // Send the encoded message to the master spy process using a message queue
    struct msg msg1;
    strncpy(msg1.column[0], encodedMsg, maxWordLength - 1);
    msg1.column[0][maxWordLength - 1] = '\0';
    
    msgsnd(msqid, &msg1, strlen(msg1.column[0]) + 1, 0);  // Send the message
    if(strcmp(encodedMsg,"")!=0){//if encodedMsg is not empty 
           if(id==2){
           	   strcpy(chatchedColumnsBySpy[catchedIndexSpies],encodedMsg);
           	   catchedIndexSpies++;
           }else{
               strcpy(chatchedColumnsByR[catchedIndexR],encodedMsg);
               catchedIndexR++;
           }
    }
}
//==============master spy tasks==============
bool isColumnExist(const char column[maxEncodedLine],int id) {
    if(id==2){
        for(int i=0;i<masterColumnsIndex;i++){
            if(strcmp(column,masterColumns[i])==0){
            return true;
            }
        }
        return false;
    }else{
        for(int i=0;i<rColumnsIndex;i++){
            if(strcmp(column,receiverColumns[i])==0){
            return true;
            }
        }
    }
    
}
void saveUniqueColumn(char column[maxEncodedLine],int id) {
    if(id==2){
        if(!isColumnExist(column, id)) {
            strcpy(masterColumns[masterColumnsIndex], column);
            masterColumnsIndex++;
        }
    }else{
        if(!isColumnExist(column, id)) {
            strcpy(receiverColumns[rColumnsIndex], column);
            rColumnsIndex++;
        }
    }
}
void reorderColumns(char columns[maxWordsInLine][maxWordLength], int numColumns) {
    int sorted = 0;
    while (sorted==0) {
        sorted = 1;
        for (int i = 0; i < numColumns - 1; i++) {
            char array1[maxWordsInLine][maxWordLength];
            char array2[maxWordsInLine][maxWordLength];
            char column1[maxEncodedLine];
            strcpy(column1,columns[i]);
            char column2[maxEncodedLine];
            strcpy(column2,columns[i+1]);
            char *word=strtok(column1," \t");
            int columnLength=0;
        	while(word!=NULL){
        	    strcpy(array1[columnLength],word);
                columnLength++;
                word=strtok(NULL," \t");
            }
            int columnNumber1=atoi(array1[columnLength]);
            char *word2=strtok(column2," \t");
            columnLength=0;
        	while(word2!=NULL){
        	    strcpy(array2[columnLength],word2);
                columnLength++;
                word2=strtok(NULL," \t");
            }
            int columnNumber2=atoi(array2[columnLength]);
            if (columnNumber1 > columnNumber2) {
                // Swap columns
                char temp[maxEncodedLine];
                strcpy(temp, columns[i]);
                strcpy(columns[i], columns[i + 1]);
                strcpy(columns[i + 1], temp);
                sorted = 0; // Set sorted flag to 0 since a swap occurred
            }
        }
    }
}
//====================================================================================
void recover(char array[maxWordsInLine][maxWordLength],int num){//num is column numbers
    char all[maxWordsInLine][maxWordLength];
    int linesNum=0;
    for(int i=0;i<maxColumns;i++){
        char column[maxEncodedLine];//
        strcpy(column,messageDecoded[i]);

        char *word=strtok(column," \t");
        int flag=0;
        while(word!=NULL){

            if(strcmp(word,"alright")==0){
                strcat(all[flag],"");
            }else{
                strcat(all[flag],word);
                strcat(all[flag]," ");
            }
            flag++;
            if(linesNum<flag){
                linesNum=flag;
            }
            word=strtok(NULL," \t");
        }
    }
    Lines=linesNum;
    for(int i=0;i<linesNum;i++){
        strcpy(array[i],all[i]);
    }
    int dif=num-linesNum;
    for(int d=0;d<dif;d++){
        strcpy(array[(linesNum-1)+d],"");
    }
}

bool compare(){
    FILE* fp1 = fopen("sender.txt", "rb");
    FILE* fp2 = fopen("Receiver.txt", "rb");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Failed to open file\n");
        return false;
    }
    int ch1, ch2;
    bool isEqual = true;

    while ((ch1 = fgetc(fp1)) != EOF && (ch2 = fgetc(fp2)) != EOF) {
        if (ch1 != ch2) {
            isEqual = false;
            break;
        }
    }

    // Check if both files reached the end at the same time
    if (ch1 != EOF || ch2 != EOF) {
        isEqual = false;
    }

    fclose(fp1);
    fclose(fp2);

    return isEqual;
}
//===========================================
void writetofile(){
    FILE* fp2 = fopen("Receiver.txt", "w");
    if (fp2 == NULL) {
        printf("Failed to open file\n");
        exit(0);
    }
    for(int i=0;i<Lines;i++){
        char temp[maxWordsInLine];
        strcpy(temp,messageDecoded[i]);
        fprintf(fp2,messageDecoded[i]);
        fprintf(fp2,"\n");
    }
    fclose(fp2);
}
int main(){

    inputFile();
    int helpersNum=0;
    int spiesNum=0;
    FILE* file=fopen("input.txt","r");
    if(file==NULL){
        printf("Error: input file couldn't open\n");
    }else{
        char line[10000];
        int flag=0;
        while(fgets(line,sizeof(line),file)!=NULL){
            char *word=strtok(line,"\n");
            char temp[10];
            if(flag==0){
                strcpy(temp,word);
                helpersNum=atoi(temp);
                flag++;
            }else if(flag==1){
                strcpy(temp,word);
                spiesNum=atoi(temp);
                flag++;
            }else if(flag==2){
                strcpy(temp,word);
                num_failed_operations=atoi(temp);
                flag++;
            }else if(flag==3){
                strcpy(temp,word);
                num_successful_operations=atoi(temp);
            }
            word=strtok(NULL,"\n");
        }
    }
    fclose(file);
    
    pid_t sender_pid, receiver_pid, master_spy_pid;
    pid_t helper_pids[helpersNum];
    pid_t spy_pids[spiesNum];
    pid_t senderChildren[maxColumns];
    
    //===========================shared memo def===================
    key_t key;
    int shm_id;
    char* shm_ptr;//where we will define the encoded columns
    // Generate a unique key for the shared memory segment
    key = ftok(".", 'x');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    // Create the shared memory segment
    shm_id = create_shared_memory(key);
    
    //=======================semaphore==========
    int semaphore_id;
    int semaphore_key = 1234;  // Unique key for the semaphore set
    // Create a semaphore set with one semaphore for shared memory access
    semaphore_id = semget(semaphore_key, 1, IPC_CREAT | 0666);
    if (semaphore_id == -1) {
        perror("semget");
        exit(1);
    }
    // Initialize the semaphore with a value of 1 (unlocked)
    semctl(semaphore_id, 0, SETVAL, 1);
    //=====================queue====================
    key_t msqKey = ftok("path/to/keyfile", 'M');
    int msqid = msgget(msqKey, IPC_CREAT | 0666);
    
    //
    //create the sender process
    sender_pid=fork();
    if(sender_pid==0){
        printf("Sender process created! with pid=%d\n",getpid());
        sleep(2);
        //========================sender process tasks===================
        for(int k=0;k<maxColumns;k++){
            senderChildren[k]=fork();
            if(senderChildren[k]==0){
                printf("Sender Child no%d created!\n",k);
                //===========encoding columns==========
                char message[maxLineLength];
                strcpy(message,messageColumns[k]);
                encode_column(message,k);
                printf("Column no%d encoded successfully!\n",k);
                shm_ptr = attach_shared_memory(shm_id);
                write_shared_memory(shm_ptr, messageEncoded[k]);//writing encoded column on the shared memo
                detach_shared_memory(shm_ptr);
                printf("Writing to shared memory is done successfully!\n");
                sleep(2);
                exit(0);
             }else{
                printf("Failed to created Sender Child no%d\n",k);
	 }
        }
        exit(0);
    }else if(sender_pid>0){
        receiver_pid=fork();
        if(receiver_pid==0){
            printf("Receiver process created!\n");
            sleep(2);
            //========================receiver process tasks===================
            shm_ptr = attach_shared_memory(shm_id);//attaching the ptr to the shared memo
            while(1){// Continuously access the shared memory and send encoded messages
                accessSharedMemory(shm_ptr, msqid,1);//1 for receiver
                usleep(500000);  // Sleep for 500 milliseconds before accessing again
                
    	}
    	detach_shared_memory(shm_ptr);
    	// Remove the message queue
    	msgctl(msqid, IPC_RMID, NULL);
    	for(int r=0;r<catchedIndexR;r++){//saving data without duplication
    	    char message[maxLineLength];
                strcpy(message,chatchedColumnsByR[r]);
                saveUniqueColumn(message,1);
    	}
    	reorderColumns(receiverColumns,rColumnsIndex);
    	for(int m=0;m<rColumnsIndex;m++){
    	    char *msgptr=receiverColumns[m];
    	    decode_column(msgptr,1);
    	}
    	recover(messageDecoded,rColumnsIndex);
    	writetofile();
	exit(0);
        }else if(receiver_pid>0){
            //creating master spy
            master_spy_pid=fork();
            if(master_spy_pid==0){
            //====================master spy process tasks==============
                printf("Master spy process created!\n");
                for(int r=0;r<masterColumnsIndex;r++){//saving data without duplication
    	        char message[maxLineLength];
                    strcpy(message,chatchedColumnsBySpy[r]);
                    saveUniqueColumn(message,2);
    	    }
    	    reorderColumns(masterColumns,masterColumnsIndex);
    	    for(int m=0;m<masterColumnsIndex;m++){
    	        char *msgptr=masterColumns[m];
    	        decode_column(msgptr,2);
    	    }
    	    recover(messageDecodedByMaster,masterColumnsIndex);
                sleep(2);
                exit(0);
            }else if(master_spy_pid>0){
                //creating the helper processes
                for(int i=0;i<helpersNum;i++){
                    helper_pids[i]=fork();
                    if(helper_pids[i]==0){
                        printf("Helper process no %d created!\n",i);
                        sleep(2);
                        //========================helper processestasks===================
                        // Attach to the shared memory segment
                        shm_ptr = attach_shared_memory(shm_id);
                        swap(shm_ptr,SHM_SIZE);
                        printf("Swaping done successfully!\n");
                        // Detach from the shared memory segment
    		detach_shared_memory(shm_ptr);
    		printf("Detaching is done\n");
                        exit(0);
                    }else if(helper_pids[i]<0){
                        printf("Failed to create helper process no %d\n",i);
                    }
                }
                //creating the spy processes
                for(int i=0;i<spiesNum;i++){
                    spy_pids[i]=fork();
                    if(spy_pids[i]==0){
                        printf("Spy process no %d created!\n",i);
                        sleep(2);
                        //========================spy processes tasks===================
                        srand(getpid());  // Seed the random number generator with the process ID
                        shm_ptr = attach_shared_memory(shm_id);//attaching the ptr to the shared memo
                        // Continuously access the shared memory and send encoded messages
    		while(1){
    		    accessSharedMemory(shm_ptr, msqid,2);
    		    usleep(500000);  // Sleep for 500 milliseconds before accessing again
    		    
    		}
    		detach_shared_memory(shm_ptr);
    		// Remove the message queue
    		msgctl(msqid, IPC_RMID, NULL);
                        exit(0);
                    }else if(spy_pids[i]<0){
                        printf("Failed to create spy process no %d\n",i);
                    }
                }
                //===========================parent process tasks===============
                printf("Parent process waiting...\n");//waiting for termination of processes 
                sleep(2);
                if(compare()){
                    printf("Receiver received and decoded the same message!\n");
                    handle_successful_dencoding(shm_id);
                }//for receiver and sender
                else{
                    printf("Receiver failed to receive the same message\n");
                    handle_failed_dencoding(shm_id);
                }
                for(int i=0;i<helpersNum;i++){
                    wait(10);
                }
                for(int i=0;i<spiesNum;i++){
                    wait(10);
                }
                remove_shared_memory(shm_id);
                exit(0);
            }else{
                printf("Failed to create master spy process\n");
            }
        }else{
            printf("Failed to create receiver process\n");
        }
    }else{
        printf("Failed to create sender process\n");
    }
    return 0;
}



















