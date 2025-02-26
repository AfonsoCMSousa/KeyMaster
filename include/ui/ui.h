#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <termios.h>
#include "herror.h"
#include "dynmem.h"

#include "ui.c"

int pomrpPassword(char *password);
char *prompNormalRequest(char *message);

#endif // !UI_H