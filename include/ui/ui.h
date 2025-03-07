#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <termios.h>
#include "herror.h"
#include "dynmem.h"

#include "ui.c"

int prompPassword(char *password, const char *msg);
char *prompNormalRequest(char *message);

#endif // !UI_H