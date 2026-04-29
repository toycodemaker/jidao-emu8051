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

#include "emu8051.h"


void show_emu(const em8051& aCPU) 
{
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

  return EXIT_SUCCESS;
}
