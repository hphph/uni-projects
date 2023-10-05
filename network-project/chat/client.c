#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define MAX_MSG_LEN 128
#define MAX_NICK_LEN 32

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
};

typedef struct tweet
{
    char nick[MAX_NICK_LEN];
    char message[MAX_MSG_LEN];
    unsigned int likes;
} Tweet;

void errorexit(char *errmsg)
{
    perror(errmsg);
    exit(0);
}

int main(int argc, char **argv)  
{
    void *data; //Wskaźnik na pamięć dzieloną
    int semcount; 
    int n;//Maksymalna liczba wiadomosci
    int currentmsgcount;
    Tweet *tweets;
    key_t ipckey;
    int shmid, semid;
    char input;
    struct sembuf singlesembuf;
    struct sembuf *allsembuf;
    union semun arg;
    arg.buf = (struct semid_ds *)malloc(sizeof(struct semid_ds));
    //Kontrola argumentow wejscia
    if(argc < 3)
    {
        printf("Zbyt malo argumentow\n");
        printf("Spsob uzycia: ");
        printf("klienttwitter [sciezka_do_serwera] [nick]\n");
        exit(0);
    }

    //Generowanie klucza i pobieranie wskaznika na pamiec wspoldzielona
    if((ipckey = ftok(argv[1], 1)) == -1)
    {
        fflush(stdout);
        errorexit("Blad generowania klucza IPC");
    }
    if((shmid = shmget(ipckey, 0, 0)) == -1)
    {
        errorexit("Blad shmget");
    }
    data = (void *) shmat(shmid, NULL, 0);
    if(data == (void *)-1)
    {
        errorexit("Blad shmat");
    }

    //Zaladowanie semaforow
    if((semid = semget(ipckey, 0, 0)) == -1)
    {
        errorexit("Blad semget");
    }
    if(semctl(semid, 0, IPC_STAT, arg) == -1)
    {
        errorexit("Blad semctl");
    }
    semcount = arg.buf->sem_nsems;
    allsembuf = (struct sembuf *) malloc(semcount * sizeof(struct sembuf));
    singlesembuf.sem_flg = 0;
    for(int i = 0; i<semcount; i++)
    {
        allsembuf[i].sem_flg = 0;
        allsembuf[i].sem_num = i;
        allsembuf[i].sem_op = -1;
    }
    if(semop(semid, allsembuf, semcount) == -1)
    {
        errorexit("Blad semop");
    }
    
    //Załadowanie danych z pamięci dzielonej
    currentmsgcount = *((int *) data);
    data+=sizeof(int);
    n = *((int *) data);
    data+=sizeof(int);
    tweets = (Tweet *)data;
    data -= 2*sizeof(int);

    printf("Twitter 2.0 wita!\n");
    printf("[Wolnych %d wpisow (na %d)]\n", n - currentmsgcount, n);
    for(int i = 0; i < currentmsgcount; i++)
    {
        printf("%d. %s [Autor: %s, Polubienia: %d]\n", i+1, tweets[i].message, tweets[i].nick, tweets[i].likes);
    }
    for(int i = 0; i<semcount; i++)
    {
        allsembuf[i].sem_op = 1;
    }
    if(semop(semid, allsembuf, semcount) == -1)
    {
        errorexit("Blad semop");
    }
    printf("Podaj akcje (N)owy wpis, (L)ike\n> ");
    input = getchar();
    if(input == 'N')
    {
        char msginput[MAX_MSG_LEN];
        printf("Napisz co ci chodzi po głowie:\n> ");
        getchar();
        fgets(msginput, MAX_MSG_LEN, stdin);

        for(int i = 0; i<semcount; i++)
        {
            allsembuf[i].sem_op = -1;
        }
        if(semop(semid, allsembuf, semcount) == -1)
        {
            errorexit("Blad semop");
        }
        currentmsgcount = *((int *) data);
        if(currentmsgcount >= n)
        {
            printf("Nie mozna utworzyc nowego wpisu, pamiec na wpisy zostala wyczerpana.\n");
            for(int i = 0; i<semcount; i++)
            {
                allsembuf[i].sem_op = 1;
            }
            if(semop(semid, allsembuf, semcount) == -1)
            {
                errorexit("Blad semop");
            }
            free(arg.buf);
            free(allsembuf);
            exit(0);
        }
        (*((int *)data))++;
        for(int i = 0; i<semcount; i++)
        {
            allsembuf[i].sem_op = 1;
        }
        if(semop(semid, allsembuf, semcount) == -1)
        {
            errorexit("Blad semop");
        }
        
        singlesembuf.sem_op = -1;
        singlesembuf.sem_num = currentmsgcount;
        if(semop(semid, &singlesembuf, 1) == -1)
        {
            errorexit("Blad semop");
        }
        //Linia do testowania
        //sleep(10);
        strcpy(tweets[currentmsgcount].message, msginput);
        strcpy(tweets[currentmsgcount].nick, argv[2]);
        singlesembuf.sem_op = 1;
        if(semop(semid, &singlesembuf, 1) == -1)
        {
            errorexit("Blad semop");
        }
    }
    else if(input == 'L')
    {
        int tweetliked;
        printf("Który wpis chcesz polubic\n> ");
        scanf("%d", &tweetliked);
        for(int i = 0; i<semcount; i++)
        {
            allsembuf[i].sem_op = -1;
        }
        if(semop(semid, allsembuf, semcount) == -1)
        {
            errorexit("Blad semop");
        }
        currentmsgcount = *((int *) data);
        for(int i = 0; i<semcount; i++)
        {
            allsembuf[i].sem_op = 1;
        }
        if(semop(semid, allsembuf, semcount) == -1)
        {
            errorexit("Blad semop");
        }
        if(tweetliked < 1 || tweetliked > currentmsgcount)
        {
            printf("Nie istnieje wpis o podanym numerze.\n");
            free(arg.buf);
            free(allsembuf);
            exit(0);
        }
        singlesembuf.sem_op = -1;
        singlesembuf.sem_num = tweetliked-1;
        if(semop(semid, &singlesembuf, 1) == -1)
        {
            errorexit("Blad semop");
        }
        tweets[tweetliked-1].likes++;
        singlesembuf.sem_op = 1;
        if(semop(semid,  &singlesembuf, 1) == -1)
        {
            errorexit("Blad semop");
        }
    }
    else
    {
        printf("Akcja o podanej literze nie jest zdefiniowana.\nProsze sprobowac ponownie.\n");
    }

    free(arg.buf);
    free(allsembuf);
    return 0;
}
