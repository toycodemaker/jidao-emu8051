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
 * emu.c
 * Curses-based emulator front-end
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
extern "C" {
#include "emu8051.h"
}

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
int pout[4] = { 0 };

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



void emu_sfrwrite_SBUF(struct em8051 *aCPU, uint8_t aRegister)
{
    aCPU->serial_out_remaining_bits = 8;
}

uint8_t emu_sfrread(struct em8051 *aCPU, uint8_t aRegister)
{
  std::print("Unsupported Function: {}", __FUNCTION__ );
  return 0;
}


void hello(struct em8051 *aCPU, int changeto)
{
}


void show_emu(const em8051* aCPU)
{
  char ASMcommand[256]{};
  decode((em8051*)aCPU, aCPU->mPC, ASMcommand);
  //if (aCPU->mPC < 200)
  std::println("PC: {:4x}, ASM Command:{}", 
      aCPU->mPC, 
      ASMcommand
  );
}

int main(int parc, char ** pars)
{
  struct em8051 emu;
  int ticked = 1;

  memset(&emu, 0, sizeof(emu));
  emu.mCodeMemMaxIdx = 65536-1;
  emu.mCodeMem     = (unsigned char*)calloc(emu.mCodeMemMaxIdx+1, sizeof(unsigned char));
  emu.mExtDataMaxIdx = 65536-1;
  emu.mExtData     = (unsigned char*)calloc(emu.mExtDataMaxIdx+1, sizeof(unsigned char));
  emu.mUpperData   = (unsigned char*)calloc(128, sizeof(unsigned char));
  emu.except       = &hello;
  emu.xread = NULL;
  emu.xwrite = NULL;

  emu.sfrwrite[REG_SBUF] = emu_sfrwrite_SBUF;

  emu.sfrread[REG_P0] = emu_sfrread;
  emu.sfrread[REG_P1] = emu_sfrread;
  emu.sfrread[REG_P2] = emu_sfrread;
  emu.sfrread[REG_P3] = emu_sfrread;

  reset(&emu, 1);
  load_obj(&emu, "r.ihx");

  // Minimum core Loop
  int num = 1;
  while (--num || std::cin >> num) {
    int targettime;
    unsigned int targetclocks;
    targetclocks = 0;
    targettime = getTick();

    do
    {
      /*
      if (opt_step_instruction)
      {
        ticked = 0;
        while (!ticked)
        {
          targetclocks--;
          clocks += 12;
          ticked = tick(&emu);
        }
      }
      else
      {
        targetclocks--;
        clocks += 12;
        ticked = tick(&emu);
      }
      */
      {
        targetclocks--;
        clocks += 12;
        ticked = tick(&emu);
      }

    }
    while (targettime > getTick() && targetclocks > 0);

    while (targettime > getTick())
    {
      emu_sleep(1);
    }

    show_emu(&emu);
  }
  //}while (true);

  return EXIT_SUCCESS;
}

int readbyte(FILE * f)
{
    char data[3];
    data[0] = fgetc(f);
    data[1] = fgetc(f);
    data[2] = 0;
    return strtol(data, NULL, 16);
}

int load_obj(struct em8051 *aCPU, char *aFilename)
{
    FILE *f;
    if (aFilename == 0 || aFilename[0] == 0)
        return -1;
    f = fopen(aFilename, "r");
    if (!f) return -1;
    if (fgetc(f) != ':')
    {
	  fclose(f);
        return -2; // unsupported file format
    }
    while (!feof(f))
    {
        int recordlength;
        int address;
        int recordtype;
        int checksum;
        int i;
        recordlength = readbyte(f);
        address = readbyte(f);
        address <<= 8;
        address |= readbyte(f);
        recordtype = readbyte(f);
        if (recordtype == 1)
            return 0; // we're done
        if (recordtype != 0)
            return -3; // unsupported record type
        checksum = recordtype + recordlength + (address & 0xff) + (address >> 8); // final checksum = 1 + not(checksum)
        for (i = 0; i < recordlength; i++)
        {
            int data = readbyte(f);
            checksum += data;
            aCPU->mCodeMem[address + i] = data;
        }
        i = readbyte(f);
        checksum &= 0xff;
        checksum = 256 - checksum;
        if (i != (checksum & 0xff))
            return -4; // checksum failure
        while (fgetc(f) != ':' && !feof(f)) {} // skip newline
    }
	  fclose(f);
    return -5;
}
