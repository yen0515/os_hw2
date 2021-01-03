#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHM_KEY 123456
#define SHMSZ 17

int main()
{
    int shmid = 0, i = 0;
    key_t key;
    char *shm = NULL;
    int ptr[(SHMSZ - 1)];
    
    // Segment key.
    
    key = SHM_KEY;
    
    // Create the segment.
    
    if( ( shmid = shmget( key, SHMSZ, IPC_CREAT | 0666 ) ) < 0 )
    {
        perror( "shmget" );
        exit(1);
    }
    
    // Attach the segment to the data space.
    
    if( ( shm = shmat( shmid, NULL, 0 ) ) == (char *)-1 )
    {
        perror( "shmat" );
        exit(1);
    }
    
    // Initialization.
    
    memset( shm, 0, SHMSZ );
    
    // Wait other processes to change the memory content.
    // *shm == 0, nothing.
    // *shm == 1, something changed.
    // *shm == 2, bye.
    
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
            printf("\033[1;32m[server] The value is %d\033[0m\n", *shm);
        else if (cmd == 2) {
            printf("Input new value: ");
            memset( ptr, 0, ( SHMSZ - 1 ) );
            scanf("%d", ptr);
            memcpy(shm, ptr, ( SHMSZ - 1 ));
        }
        else
            break;          
        sleep(1);
    }
    
    return 0;
}

/*while(1) {
        int cmd;

        printf("\n");
        printf("1: Show the value\n");
        printf("2: Modify the value\n");
        printf("3: Exit\n");
        printf("Enter commands: ");
        scanf("%d", &cmd);

        if (cmd == 1)
            printf("\033[1;32m[server] The value is %d\033[0m\n", ptr[0]);
        else if (cmd == 2) {
            printf("Input new value: ");
            scanf("%d", &ptr[0]);
        }
        else
            break;        
    } */
