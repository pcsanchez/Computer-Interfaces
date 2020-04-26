#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

// Variable Definition.
DCB config;
COMMTIMEOUTS touts;
HANDLE serial;

// Auxiliary array storing the text representation all the digits.
char* nums[10] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
char bufferRxD;

int count;

void main() {
    // Opening the communication with the Serial Driver.
    serial = CreateFile("COM4",GENERIC_WRITE | GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    // Checking for successful creation.
    if (serial!=INVALID_HANDLE_VALUE) {
        printf("Serial interface opened!\n");

        // Serial port configuration via the DCB structure.
        GetCommState(serial, &config);
        config.BaudRate=9600;
		config.ByteSize=8;
		config.fBinary=1;
		config.fParity=0;
		config.StopBits=ONESTOPBIT;
        SetCommState(serial, &config);

        touts.ReadTotalTimeoutConstant = 100;
        SetCommTimeouts(serial, &touts);

        // End the program if a '.' is read.
        while(bufferRxD != '.') {
            ReadFile(serial, &bufferRxD,1,&count,NULL);
            if(count!=0) {
                // If the read byte is a number, print the text representation of the number.
                // If the read byte is a lowercase letter, print its uppercase form.
                // If the read byte is an uppercase letter, print it.
                if(bufferRxD >= '0' && bufferRxD <='9') {
                    printf("%s", nums[bufferRxD - '0']);
                } else if(bufferRxD >='a' && bufferRxD <= 'z') {
                    printf("%c", bufferRxD-32);
                } else if(bufferRxD >= 'A' && bufferRxD <= 'Z') {
                    printf("%c", bufferRxD);
                }
            }
        }
    }
    printf("\nSerial Interface Closed\n");
    CloseHandle(serial);
}
