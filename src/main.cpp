/* 8051 emulator
 * Copyright 2006 Jari Komppa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * (i.e. the MIT License)
 *
 * main.cpp(original emu.c)
 * Curses-based emulator front-end
 * ---------------------------------------------------------------------------
 * Significant refactoring and modifications by Liu Qitao (2026).
 * Copyright (c) 2026 Liu Qitao.
 * This file remains under the terms of the original MIT License.
 */

#include <iostream>
#include <print>
#ifdef _MSC_VER
#include <windows.h>
#undef MOUSE_MOVED
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <curses.h>
#else
#include "curses.h"
#endif

#include "emu8051.h"

//unsigned char history[HISTORY_LINES * (128 + 64 + sizeof(int))];

// current line in the history cyclic buffers
int historyline = 0;
// last known columns and rows; for screen resize detection
int oldcols, oldrows;
// are we in single-step or run mode
int runmode = 0;
// current run speed, lower is faster
int speed = 6;

// instruction count; needed to replay history correctly
unsigned int icount = 0;

// current clock count
unsigned int clocks = 0;

// old port out values
int pout[4] = {0};

int breakpoint = -1;

// returns time in 1ms units
int getTick() 
{
#ifdef _MSC_VER
  return GetTickCount();
#else
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
}

void emu_sleep(int value) 
{
#ifdef _MSC_VER
  Sleep(value);
#else
  usleep(value * 1000);
#endif
}


void show_emu(const em8051& aCPU) 
{
  //if (aCPU->mPC < 200)
  std::println("PC: {:4x}, ASM Command:{}",
               aCPU.mPC,
               aCPU.decode(aCPU.mPC));
  std::println("The Command delay Tick is {}", aCPU.mTickDelay);
}


int main(int parc, char** pars) 
{
  em8051 emu{"r.ihx"};

  // Minimum core Loop
  int num = 1;
  while (--num || std::cin >> num) {
    //emu.tick();
    emu.do_op();

    show_emu(emu);
  }
  //}while (true);

  return EXIT_SUCCESS;
}
