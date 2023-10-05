#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <netdb.h>

#define NICKSIZE (size_t)32 
#define DATASIZE (size_t)128

#define DATATYPEINIT 0
#define DATATYPECOMM 1
#define DATATYPEVALUE 2

#define COMMQUIT 0
int sockfd;

typedef enum gameState
{
    Init,
    Finish,
    YourTurn,
    EnemyTurn,
    EnemyQuit,
    Quit
} GameState;

void sgnHandler(int signum)
{
    if(signum == SIGINT)
    {
        
        close(sockfd);
        exit(0);
    }
    if(signum == SIGCHLD)
    {
        wait(0);
    }
}

typedef struct initData
{
    int hasNick;
    char nick[NICKSIZE];
    char addr[32];
    int isStarting; 
} InitData;

typedef struct sendData
{
    int data;
    int dataType;
} SendData;

int main(int argc, char **argv)
{
    GameState state = Init;
    char nick[NICKSIZE];
    char enemyNick[NICKSIZE];
    int currentValue;
    int score; 
    int enemyScore;
    int gaico;
    char enemyaddr[32];
    int isStarting = 0;
    char dataBuff[16];

    struct sockaddr_in bindaddr, sendaddr;
    struct addrinfo hints;
    struct addrinfo *retaddr;
    InitData buffInitData;
    SendData buffSendData;

    socklen_t sendaddrSize = sizeof(sendaddr);
    
    //Obsługa sygnałów
    signal(SIGINT, sgnHandler);
    signal(SIGCHLD, sgnHandler);
    srand((unsigned int)time(NULL));

    //Obsługa argumentów wywołania
    if(argc < 3)
    {
        perror("Bledna liczba argumentow\n");
        perror("Sposob uzycia: ./gra adres port [nick] \n");
        exit(1);
    }
    else if(argc >= 4)
    {
        strcpy(nick,  argv[3]);
    }

    //Tłumaczenia adresu domenowego na adres IP
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_protocol = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    if((gaico = getaddrinfo(argv[1], NULL, &hints, &retaddr)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gaico));
        exit(1);
    }
    struct sockaddr_in *sain = (struct sockaddr_in *)retaddr->ai_addr;
    strcpy(enemyaddr, inet_ntoa(sain->sin_addr));

    //Ustawianie gniazda, strukur
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Blad podczas tworzenia socketu");
        exit(1);
    }
    memset((void *) &bindaddr, 0, sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = htons(atoi(argv[2]));
    bindaddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
    {
        perror("Blad podczas wiazania socketu");
        exit(1);
    }
    memset((void *) &sendaddr, 0, sizeof(sendaddr));
    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(atoi(argv[2]));
    if(inet_aton(enemyaddr, &sendaddr.sin_addr) == 0)
    {
        perror("Bledny adres ip");
        exit(1);
    }

    printf("Gra w 50, wersja B.\n");
    printf("Rozpoczynam gre z %s. Napisz \"koniec\" by zakonczyc lub \"wynik\" by wyswietlic aktualny wynik.\n", enemyaddr);
    printf("Propozycja gry wyslana.\n");
    
    //Główna pętla
    while(state != Quit)
    {
        switch(state)
        {
            //Nowe połączenie
            case Init:
                isStarting = 0;
                score = 0;
                enemyScore = 0;
                currentValue = (rand() % 10) + 1;

                buffInitData.hasNick = 0;
                buffInitData.isStarting = isStarting;
                if(argc >= 4)
                {
                    buffInitData.hasNick = 1;
                    strcpy(buffInitData.nick, nick); 
                }
                strcpy(buffInitData.addr, enemyaddr); 
                
                if(sendto(sockfd, (void *)&buffInitData, sizeof(InitData), 0, (struct sockaddr*) &sendaddr, sendaddrSize) == -1)
                {
                    perror("Blad sendto\n");
                    exit(0);
                }
                fflush(NULL);
                if(recvfrom(sockfd, (void *)&buffInitData, sizeof(InitData), 0, (struct sockaddr*) &sendaddr, &sendaddrSize) == -1)
                {
                    perror("Blad recvfrom");
                    exit(0);
                }

                if(buffInitData.hasNick)
                {
                    strcpy(enemyNick, buffInitData.nick);
                }
                else
                {
                    strcpy(enemyNick, enemyaddr);
                }
                
                if(argc >= 4)
                {
                    buffInitData.hasNick = 1;
                }
                else
                {
                    buffInitData.hasNick = 0;
                    strcpy(nick, buffInitData.addr);
                }

                isStarting = !buffInitData.isStarting;
                buffInitData.isStarting = isStarting;

                strcpy(buffInitData.nick, nick); 
                strcpy(buffInitData.addr, enemyaddr); 

                if(isStarting)
                {
                    if(sendto(sockfd, (void *)&buffInitData, sizeof(InitData), 0, (struct sockaddr*) &sendaddr, sendaddrSize) == -1)
                    {
                        perror("Blad recvfrom\n");
                        exit(0);
                    }
                }

                if(isStarting)
                {
                    printf("%s dolaczyl do gry.\n", enemyNick);
                    printf("Losowa wartosc poczatkowa: %d, podaj kolejna wartosc.\n> ", currentValue);
                    state = YourTurn;
                }
                else
                {
                    state = EnemyTurn;
                } 
                break;
            
            //Tura gracza
            case YourTurn:
                scanf("%s", dataBuff);
                if(!strcmp(dataBuff, "koniec")) 
                {
                    state = Quit;
                    buffSendData.dataType = DATATYPECOMM;
                    buffSendData.data = COMMQUIT;
                    if(sendto(sockfd, (void *)&buffSendData, sizeof(SendData), 0, (struct sockaddr*) &sendaddr, sendaddrSize) == -1)
                    {
                        perror("Blad sendto\n");
                        exit(0);
                    }
                    break;
                }
                if(!strcmp(dataBuff, "wynik")) 
                {
                    printf("Ty %d : %d %s\n> ", score, enemyScore, enemyNick);
                    state = YourTurn;
                    break;
                }
                if(atoi(dataBuff) > currentValue && atoi(dataBuff) <= (currentValue + 10))
                {
                    currentValue = atoi(dataBuff);
                    buffSendData.dataType = DATATYPEVALUE;
                    buffSendData.data = currentValue;
                    if(sendto(sockfd, (void *)&buffSendData, sizeof(SendData), 0, (struct sockaddr*) &sendaddr, sendaddrSize) == -1)
                    {
                        perror("Blad sendto\n");
                        exit(0);
                    }
                    state = EnemyTurn;
                    if(currentValue >= 50)
                    {
                        printf("Wygrana!\n");
                        score++;
                        state = Finish;
                    }
                }
                else
                {
                    printf("Takiej wartosci nie mozesz wybrac!\n> ");
                }
                break;

            //Rozpoczęcie nowej rozgrywki
            case Finish:
                currentValue = (rand() + 1) % 11;
                isStarting = !isStarting;

                if(isStarting)
                {
                    printf("Zaczynamy kolejna rozgrywke.\n");
                    printf("Losowa wartosc poczatkowa: %d, podaj kolejna wartosc.\n> ", currentValue);
                    state = YourTurn;
                }
                else
                {
                    printf("Zaczynamy kolejna rozgrywke, poczekaj na swoja kolej.\n");
                    state = EnemyTurn;
                }
                break;

            //Tura przeciwnika
            case EnemyTurn:
                if(recvfrom(sockfd, (void *)&buffSendData, sizeof(SendData), 0, (struct sockaddr*) &sendaddr, &sendaddrSize) == -1)
                {
                    perror("Blad recvfrom");
                    exit(0);
                }
                if(buffSendData.dataType == DATATYPEVALUE)
                {
                    currentValue = buffSendData.data;
                    if(currentValue >= 50)
                    {
                        printf("%s podal wartosc %d.\n", enemyNick, currentValue);
                    }
                    else
                    {
                        printf("%s podal wartosc %d, podaj kolejna wartosc.\n> ", enemyNick, currentValue);
                        state = YourTurn;
                    }
                }
                else if(buffSendData.dataType == DATATYPECOMM)
                {
                    if(buffSendData.data == COMMQUIT)
                    {
                        state = EnemyQuit;
                    }
                }
                
                if(currentValue >= 50)
                {
                    printf("Przegrana!\n");
                    enemyScore++;
                    state = Finish;
                }
                break;
            //Wyjście przeciwnika
            case EnemyQuit:
                printf("%s zakonczyl gre, mozesz poczekac na kolejnego gracza\n", enemyNick);
                state = Init;
                break;

            default:
                perror("Nie obslugiwany stan programu");
                exit(1);
                break;
        }
    }
    
    close(sockfd);
    return 0;
}