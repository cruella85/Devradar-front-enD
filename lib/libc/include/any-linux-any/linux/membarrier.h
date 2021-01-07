#ifndef _LINUX_MEMBARRIER_H
#define _LINUX_MEMBARRIER_H

/*
 * linux/membarrier.h
 *
 * membarrier system call API
 *
 * Copyright (c) 2010, 2015 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * enum membarrier_cmd - membarrier system call command
 * @MEMBARRIER_CMD_QUERY:   Query the set of supported commands. It returns
 *                          a bitmask of valid commands.
 * @MEMBARRIER_CMD_GLOBAL:  Execute a memory barrier on all running threads.
 *                          Upon return from system call, the caller thread
 *                          is ensured that all running threads have passed
 *                          through a state where all memory accesses to
 *                          user-space addresses match program order between
 *                          entry to and return from the system call
 *                          (non-running threads are de facto in such a
 *                          state). This covers threads from all processes
 *                          running on the system. This command returns 0.
 * @MEMBARRIER_CMD_GLOBAL_EXPEDITED:
 *                          Execute a memory barrier on all running threads
 *                          of all processes which previously registered
 *                          with MEMBARRIER_CMD_REGISTER_GLOBAL_EXPEDITED.
 *                          Upon return from system call, the caller thread
 *                          is ensured that all running threads have passed
 *                          through a state where all memory accesses to
 *                          user-space addresses match program order between
 *                          entry to and return from the system call
 *                          (non-running threads are de facto in such a
 *                          state). This only covers threads from processes
 *                          which registered with
 *                          MEMBARRIER_CMD_REGISTER_GLOBAL_EXPEDITED.
 *                          This command returns 0. Given that
 *                          registration is about the intent to receive
 *                          the barriers, it is valid to invoke
 *                          MEMBARRIER_CMD_GLOBAL_EXPEDITED from a
 *                          non-registered process.
 * @MEMBARRIER_CMD_REGISTER_GLOBAL_EXPEDITED:
 *                          Register the process intent to receive
 *                          MEMBARRIER_CMD_GLOBAL_EXPEDITED memory
 *                          barriers. Always returns 0.
 * @MEMBARRIER_CMD_PRIVATE_EXPEDITED:
 *                          Execute a memory barrier on each running
 *                          thread belonging to the same process as the current
 *                          thread. Upon return from system call, the
 *                          caller thread is ensured that all its running
 *                          threads siblings have passed through a state
 *                          where all memory accesses to user-space
 *                          addresses match program order between entry
 *                          to and return from the system call
 *                          (non-running threads are de facto in such a
 *                          state). This only covers threads from the
 *                          same process as the caller thread. This
 *                          command returns 0 on success. The
 *                          