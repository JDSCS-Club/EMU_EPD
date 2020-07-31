/*
 * retarget.h
 *
 *  Created on: Feb 4, 2020
 *      Author: RobetKim
 */

#ifndef _RETARGET_H__
#define _RETARGET_H__

#include "main.h"
#include <sys/stat.h>

void RetargetInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif /* INC_RETARGET_H_ */
