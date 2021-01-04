#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHM_KEY 123456
#define SHM_SIZE 17
 
int main()
{
    int shmid = 0, i = 0;
    key_t key;
    char *shm = NULL;
    int ptr[(SHM_SIZE - 1)];
    
    key = SHM_KEY;
    
    if( ( shmid = shmget( key, SHM_SIZE, IPC_CREAT | 0666 )) < 0 )
    {
        perror( "shmget" );
        exit(1);
    }
    
    if( ( shm = shmat( shmid, NULL, 0 )) == (char *) - 1 )
    {
        perror( "shmat" );
        exit(1);
    }

    memset(shm, 0, ( SHM_SIZE - 1 ));
    printf("\033[1;32m[client] The value is %d\033[0m\n", *shm);

    while(1)
    {
        int cmd;
        printf("\n");
        printf("1: Show the value\n");
        printf("2: Modify the value\n");
        printf("3: Exit\n");
        printf("Enter commands: ");
        scanf("%d", &cmd);

        if (cmd == 1)
            printf("\033[1;32m[client] The value is %d\033[0m\n", *shm);
        else if (cmd == 2) {
            printf("Input new value: ");
            memset( ptr, 0, ( SHM_SIZE - 1 ));
            scanf("%d", ptr);
            memcpy(shm, ptr, ( SHM_SIZE - 1 ));
        }
        else
            break;          
        sleep(1);
    }
    
    return 0;
} 