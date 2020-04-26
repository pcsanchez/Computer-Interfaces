#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

// Defining some literals.
#define S_COUNT 6
#define TRANS_COUNT 6

DCB config;
COMMTIMEOUTS touts;
HANDLE serial;
char bufferRxD;
int count;

char ch = 0;

int acum = 0;

int trans = 0;
int prevS = 0;
int currS = 0;

int auxVector[6] = {10, 11, 12, 13, 14, 15};
int chrTrans[TRANS_COUNT] = {0, 8, 27, 13, 6};
int matrixTrans[S_COUNT][TRANS_COUNT] = {
                                        {0, 99, 99, 0, 1},
                                        {1, 99, 99, 5, 2},
                                        {2, 99, 99, 5, 3},
                                        {3, 99, 99, 5, 4},
                                        {4, 99, 99, 5, 4},
                                        {5, 0, 0, 0, 0}};

int calcTrans(char chr) {
    int trans = 0;
    if((chr >= '0' && chr <= '9') || (chr >= 'A' && chr <='F') || (chr >= 'a' && chr <= 'f'))
        return(4);
    for (trans = 3; trans > 0; --trans) {
        if(chr == chrTrans[trans]){
            break;
        }
    }
    return trans;
}

int nextState(int state, int trans) {
    return matrixTrans[state][trans];
}

int getNumerical() {
    if(ch>='0' && ch<='9')
        return ch - '0';
    if(ch>='A' && ch<='Z')
        return auxVector[ch - 'A'];
    if(ch>='a' && ch<='z')
        return auxVector[ch - 'a'];
}

int executeState(int state) {
    switch(state) {
        case 0:
            break;
        case 1:
            acum = 0;
            acum *= 16;
            acum += (getNumerical());
            printf("%c", ch);
            break;
        case 2:
            acum *= 16;
            acum += getNumerical();
            printf("%c", ch);
            break;
        case 3:
            acum *= 16;
            acum += getNumerical();
            printf("%c", ch);
            break;
        case 4:
            acum *= 16;
            acum += getNumerical();
            printf("%c", ch);
            break;
        case 5:
            printf("\nEl numero convertido es %d\n", acum);
            return 0;
        case 99:
            printf("\n<<<Capture was Canceled>>\n");
            return 0;
    }

    return state;
}

void main() {

	serial=CreateFile("COM4",GENERIC_WRITE | GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(serial != INVALID_HANDLE_VALUE) {
        printf("Serial Interface Opened!\n");
        GetCommState(serial, &config);
        config.BaudRate = 9600;
        config.ByteSize = 8;
        config.fBinary = 1;
        config.fParity = 0;
        config.StopBits = ONESTOPBIT;
        SetCommState(serial, &config);

        touts.ReadTotalTimeoutConstant=100;
        SetCommTimeouts(serial, &touts);

        while(ch != '.') {
            ReadFile(serial, &ch,1,&count,NULL);
            if(count!=0) {
                if(ch != '.') {
                    trans = calcTrans(ch);
                    if(trans) {
                        prevS = currS;
                        currS = nextState(prevS, trans);
                        if(prevS != currS) {
                            currS = executeState(currS); 
                        }
                    }
                }
            }
        }
    }
	CloseHandle(serial);
}