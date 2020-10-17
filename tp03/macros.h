#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define OK 1
#define ERROR -1
#define TRIES 3
#define TIMEOUT 3
#define RESET_ALARM 0



// Mesh Macros
#define FLAG        0x7E
#define A           0x03
#define SET         0x03
#define UA          0x07
#define SET_BCC     A ^ SET
#define UA_BCC      A ^ UA

// Control Macros (there is another way of defining them)
#define C_I0        0x00
#define C_I1        0x40
#define C_RR0       0x05
#define C_RR1       0x85
#define C_REJ0      0x01
#define C_REJ1      0x81

//Byte Stuffing
#define ESCAPE      0x7D
#define ESCAPE_ESC  0x5D
#define ESCAPE_FLAG 0x5E
