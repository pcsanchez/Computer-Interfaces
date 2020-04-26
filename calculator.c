#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

// Variable Definition
#define TRANS_COUNT 7
#define STATE_COUNT 15

DCB config;
COMMTIMEOUTS touts;
HANDLE serial;
char bufferTxD;
int count;

char chr;
float acum1 = 0.0;
float acum2 = 0.0;
float res = 0.0;
float aux = 0.1;

int count;

// Buffer for storing the string representation of the operation's result.
int helper = 0;
char buffer[64];

// Enum strucuture to save the operation to execute.
enum Operator{Sum, Substraction, Multiplication, Division};
enum Operator oper;

// State Machine Variables
int trans = 0;
int current_state = 0;
int previous_state = 0;

// Transition Vector and Transition Matrix.
int transtion_characters[TRANS_COUNT] = {0, '(', ')', '=', '.', 5, 6};
int transition_matrix[STATE_COUNT][TRANS_COUNT] ={
                                                {0, 1, 0, 0, 0, 0, 0},
                                                {1, 1, 1, 1, 4, 2, 1},
                                                {2, 2, 2, 2, 4, 3, 2},
                                                {3, 3, 3, 3, 4, 2, 3},
                                                {4, 4, 4, 4, 4, 5, 7},
                                                {5, 5, 5, 5, 5, 6, 7},
                                                {6, 6, 6, 6, 6, 5, 7},
                                                {7, 7, 7, 7, 10, 8, 7},
                                                {8, 8, 8, 8, 10, 9, 8},
                                                {9, 9, 9, 9, 10, 8, 9},
                                                {10, 10, 13, 10, 10, 11, 10},
                                                {11, 11, 13, 11, 11, 12, 11},
                                                {12, 12, 13, 12, 12, 11, 12},
                                                {13, 13, 13, 14, 13, 13, 13},
                                                {14, 0, 0, 0, 0, 0, 0}};

// Function that tries to match chr with a character in the
// transition vector. If the character is a digit, a 5 is returned.
// If the character is an algebraic operation, a 6 is returned.
// The function then tries to match with the remaining characters,
// if there is no match, a 0 is returned.
int CalcTrans(char chr) {
    int trans = 0;
    if (chr >= '0' && chr <= '9') {
        return 5;
    }
    switch(chr) {
        case'+':
        case'-':
        case'*':
        case'/':
            return 6;
    }
    for(trans = 4; trans > 0; --trans){
        if(chr==transtion_characters[trans])
            break;
    }
    return trans;
}

// Returns the next state with regards to the current state
// and the given transition.
int NextState(int state, int transition) {
    return transition_matrix[state][transition];
}

// Performs the nececary actions for each state, this is
// the principal logic of the program.
int ExecuteState(int state) {
    switch(state) {
        case 0:
            break;
        case 1:
            acum1 = 0.0;
            acum2 = 0.0;
            aux = 0.1;
            printf("%c", chr);
            break;
        case 2:
            acum1*=10;
            acum1+=(chr - '0');
            printf("%c", chr);
            break;
        case 3:
            acum1*=10;
            acum1+=(chr - '0');
            printf("%c", chr);
            break;
        case 4:
            if(previous_state == 1) {
                printf("0");
            }
            printf("%c", chr);
            break;
        case 5:
            acum1 += (aux * (chr - '0'));
            aux /= 10;
            printf("%c", chr);
            break;
        case 6:
            acum1 += (aux * (chr - '0'));
            aux /= 10;
            printf("%c", chr);
            break;
        case 7:
            if(previous_state == 4) {
                printf("0");
            }
            switch(chr) {
                case'+':
                    oper=Sum;
                    break;
                case'-':
                    oper=Substraction;
                    break;
                case'*':
                    oper=Multiplication;
                    break;
                case'/':
                    oper=Division;
                    break;
            }
            aux = 0.1;
            printf("%c", chr);
            break;
        case 8:
            acum2*=10;
            acum2+=(chr - '0');
            printf("%c", chr);
            break;
        case 9:
            acum2*=10;
            acum2+=(chr - '0');
            printf("%c", chr);
            break;
        case 10:
            if(previous_state == 7) {
                printf("0");
            }
            printf("%c", chr);
            break;
        case 11:
            acum2 += (aux * (chr - '0'));
            aux /= 10;
            printf("%c", chr);
            break;
        case 12:
            acum2 += (aux * (chr - '0'));
            aux /= 10;
            printf("%c", chr);
            break;
        case 13:
            if(previous_state == 10) {
                printf("0");
            }
            printf("%c", chr);
            break;
        case 14:
            switch(oper) {
                case Sum:
                    res = acum1 + acum2;
                    break;
                case Substraction:
                    res = acum1-acum2;
                    break;
                case Multiplication:
                    res = acum1*acum2;
                    break;
                case Division:
                    if(acum2) {
                        res = acum1/acum2;
                    } else {
                        printf("\nDivision by 0 is not possible\n");
                        return 0;
                    }
                    break;
            }
            printf("%c\n", chr);
            sprintf(buffer,"%f",res);
			for(helper = 0; helper < strlen(buffer); ++helper) {
				WriteFile(serial,&buffer[helper],1,&count,NULL);
			}
            return 0;
    }

    return state;
}

void main() {
    // Opening the communication with the Serial Driver.
    serial=CreateFile("COM4",GENERIC_WRITE | GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    // Checking for successful creation.
    if (serial != INVALID_HANDLE_VALUE) {
        printf("Serial Interface Opened\n");
        // Serial port configuration via the DCB structure.
        GetCommState(serial, &config);
        config.BaudRate = 9600;
        config.ByteSize = 8;
        config.fBinary = 1;
        config.fParity = 0;
        config.StopBits = ONESTOPBIT;
        SetCommState(serial, &config);

        touts.ReadTotalTimeoutConstant=100;
        SetCommTimeouts(serial, &touts);

        // The program stops when an ESC character is received.
        while(chr != 27) {
			ReadFile(serial, &chr,1,&count,NULL);
            if(count!=0) {
                if(chr != 27) {
                    trans = CalcTrans(chr);
                    if(trans) {
                        previous_state = current_state;
                        current_state = NextState(previous_state, trans);
                        if(previous_state != current_state) {
                            current_state = ExecuteState(current_state);
                        }
                    }
                }
            }
        }
    }
    CloseHandle(serial);
}