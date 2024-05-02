#ifndef SHARED_MEMORIES
#define SHARED_MEMORIES




struct shared_memory {
    int columnNumber;
    char message[100];
};


#define TSM_KEY 0x1478

typedef struct shared_memory* TSM;



int get_shared_memory(int key, int memory_struct_size){


    // For get the memory
    int sm_id = shmget(key, memory_struct_size, IPC_CREAT | 0777);
    if (sm_id == -1){
        perror("Memory Get Error\n");
        exit(-1);
    }

    return sm_id;
}


#endif
