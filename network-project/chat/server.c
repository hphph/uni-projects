#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

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

int n; //Maksymalna liczba wiadomosci, liczba semaforów
void *data; //Wskaźnik na pamięć dzieloną
int semid, shmid;
union semun arg;
struct sembuf singlesembuf;
struct sembuf *allsembuf;


void errorexit(char *errmsg)
{
    perror(errmsg);
    exit(0);
}

//Obsluga odpowiednio CTRL^C i CTRL^Z
void sgnhandle(int signal)
{
    switch(signal)
    {
        //Zakończenie działania programu, "sprzątanie" po pamięci dzielonej, semaforach oraz dealokacja zmiennych.
        case SIGINT:
            printf("\n[Serwer]: dostalem SIGINT => koncze i sprzatam... (odlaczenie: %s, usuniecie: %s)\n", (shmdt(data) == 0)?"OK":"blad", (shmctl(shmid, IPC_RMID, NULL) == 0)?"OK":"blad");
            if(semctl(semid, 0, IPC_RMID) == -1)
            {
                errorexit("Blad semctl, IPC_RMID");
            }
            free(arg.array);
            free(allsembuf);
            exit(0);
            break;
        //Wypisanie
        case SIGTSTP:
            for(int i = 0; i<n; i++)
            {
                allsembuf[i].sem_op = -1;
            }
            if(semop(semid, allsembuf, n) == -1) errorexit("Blad semop");
            int tweetscount = *((int *)data);
            Tweet *tweetsbuf = (Tweet *)(data+2*sizeof(int));
            for(int i = 0; i<n; i++)
            {
                allsembuf[i].sem_op = 1;
            }
            if(semop(semid, allsembuf, n) == -1) errorexit("Blad semop");
            if(tweetscount == 0)
            {
                printf("\nBrak wpisow\n");
                return;
            }
            printf("___________  Twitter 2.0:  ___________\n");
            singlesembuf.sem_flg = 0;
            for(int i = 0; i < tweetscount; i++)
            {
                singlesembuf.sem_op = -1;
                singlesembuf.sem_num = i;
                if(semop(semid, &singlesembuf, 1) == -1) errorexit("Blad semop");
                printf("[%s]: %s [Polubienia: %d]\n", tweetsbuf[i].nick, tweetsbuf[i].message, tweetsbuf[i].likes);
                singlesembuf.sem_op = 1;
                if(semop(semid, &singlesembuf, 1) == -1) errorexit("Blad semop");
            }
            break;
    }
}

int main(int argc, char **argv) 
{
    int currentmsgcount = 0;
    Tweet *tweets;
    key_t ipckey;
    struct shmid_ds shmstats;
    
    //Bledne wywolanie programu
    if(argc < 3)
    {
        printf("Zbyt malo argumentow\n");
        printf("Spsob uzycia: ");
        printf("serwertwitter [sciezka_do_serwera] [maksymalna_liczba_wiadomosci]\n");
        exit(0);
    }

    n = atoi(argv[2]);
    if(n < 1)
    {
        printf("Liczba wiadomosci musi byc wieksza od 0\n");
        exit(0);
    }

    //Obsluga sygnalow
    signal(SIGINT, sgnhandle);
    signal(SIGTSTP, sgnhandle);

    printf("[Serwer]: Twitter 2.0 (wersja C)\n");
    printf("[Serwer]: tworze klucz na podstawie pliku %s... ", argv[1]);
    if((ipckey = ftok(argv[1], 1)) == -1)
    {
        fflush(stdout);
        errorexit("Blad generowania klucza IPC");
    }
    printf("OK (klucz: %ld)\n", (long int) ipckey);

    //Tworzę semafory i wypełniam odpowiednio struktury pomocnicze
    if((semid = semget(ipckey, n, 0600 | IPC_CREAT)) == -1)
    {
        errorexit("Blad semget");
    }
    arg.array = (unsigned short *)malloc(n * sizeof(short));
    allsembuf = (struct sembuf *)malloc(n * sizeof(struct sembuf));
    for(int i = 0; i<n; i++)
    {
        arg.array[i] = 1;
        allsembuf[i].sem_flg = 0;
        allsembuf[i].sem_num = i;
        allsembuf[i].sem_op = 1;
    }
    if((semctl(semid, n, SETALL, arg)) == -1)
    {
        errorexit("Blad semctl, SETALL");
    }

    printf("[Serwer]: tworze segment pamieci wspolnej na %d wpisow po %ldb...)\n", n, sizeof(struct tweet));
    if((shmid = shmget(ipckey, 2 * sizeof(int) + n * sizeof(Tweet), 0600 | IPC_CREAT | IPC_EXCL)) == -1)
    {
        errorexit("Blad shmget");
    }
    if(shmctl(shmid, IPC_STAT, &shmstats) == -1)
    {
        errorexit("Blad shmctl, IPC_STAT");
    }
	printf("\tOK (id: %d, rozmiar: %zub)\n", shmid, shmstats.shm_segsz);
    printf("[Serwer]: dolaczam pamiec wspolna...");
    data = (void *) shmat(shmid, NULL, 0);
    if(data == (void *)-1)
    {
        errorexit("Blad shmat");
    }

    //Uzupełniam pamięć dzieloną zmiennymi i strukturami
    tweets = malloc(n * sizeof(Tweet));
    memcpy(data, &currentmsgcount, sizeof(int));
    data+=sizeof(int);
    memcpy(data, &n, sizeof(int));
    data+=sizeof(int);
    memcpy(data, tweets, n*sizeof(Tweet));
    data-=2*sizeof(int);
    free(tweets);

    printf("OK (adres: %lX)\n", (long int)data);
    printf("[Serwer]: nacisnij Crtl^Z by wyswietlic stan serwisu\n");
    printf("[Serwer]: nacisnij Crtl^C by zakonczyc program\n");
    while(1)
    {
        sleep(1);
    }
    return 0;
}
