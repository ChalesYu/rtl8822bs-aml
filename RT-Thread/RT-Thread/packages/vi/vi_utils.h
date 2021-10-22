/*
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#ifndef __VI_UTILS_H__
#define __VI_UTILS_H__

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <dfs_posix.h>
#include <dfs_poll.h>
#include <sys/types.h>

#include <mem_sandbox/mem_sandbox.h>

#define BB_VER "latest: 2021-06-17"
#define BB_BT  "Busybox vi for RT-Thread"

//config:config FEATURE_VI_MAX_LEN
//config:   int "Maximum screen width in vi"
//config:   range 256 16384
//config:   default 4096
//config:   depends on VI
//config:   help
//config:     Contrary to what you may think, this is not eating much.
//config:     Make it smaller than 4k only if you are very limited on memory.
#ifdef VI_MAX_LEN
#define CONFIG_FEATURE_VI_MAX_LEN VI_MAX_LEN
#else
#define CONFIG_FEATURE_VI_MAX_LEN 4096
#endif

//config:config FEATURE_VI_ASK_TERMINAL
//config:   bool "Use 'tell me cursor position' ESC sequence to measure window"
//config:   default y
//config:   depends on VI
//config:   help
//config:     If terminal size can't be retrieved and $LINES/$COLUMNS are not set,
//config:     this option makes vi perform a last-ditch effort to find it:
//config:     position cursor to 999,999 and ask terminal to report real
//config:     cursor position using "ESC [ 6 n" escape sequence, then read stdin.
//config:
//config:     This is not clean but helps a lot on serial lines and such.
#ifdef VI_ENABLE_VI_ASK_TERMINAL
#define ENABLE_FEATURE_VI_ASK_TERMINAL 1
#define IF_FEATURE_VI_ASK_TERMINAL(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_ASK_TERMINAL 0
#define IF_FEATURE_VI_ASK_TERMINAL(...)
#endif

//config:config FEATURE_VI_COLON
//config:   bool "Enable \":\" colon commands (no \"ex\" mode)"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Enable a limited set of colon commands for vi. This does not
//config:     provide an "ex" mode.
#ifdef VI_ENABLE_COLON
#define ENABLE_FEATURE_VI_COLON 1
#define IF_FEATURE_VI_COLON(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_COLON 0
#define IF_FEATURE_VI_COLON(...)
#endif

//config:config FEATURE_VI_COLON_EXPAND
//config:   bool "Expand \"%\" and \"#\" in colon commands"
//config:   default y
//config:   depends on FEATURE_VI_COLON
//config:   help
//config:   Expand the special characters \"%\" (current filename)
//config:   and \"#\" (alternate filename) in colon commands.
#ifdef VI_ENABLE_COLON_EXPAND
#define ENABLE_FEATURE_VI_COLON_EXPAND 1
#else
#define ENABLE_FEATURE_VI_COLON_EXPAND 0
#endif

//config:config FEATURE_VI_SEARCH
//config:   bool "Enable search and replace cmds"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Select this if you wish to be able to do search and replace in
//config:     busybox vi.
#ifdef VI_ENABLE_SEARCH
#define ENABLE_FEATURE_VI_SEARCH 1
#define IF_FEATURE_VI_SEARCH(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_SEARCH 0
#define IF_FEATURE_VI_SEARCH(...)
#endif

//config:config FEATURE_VI_READONLY
//config:   bool "Enable -R option and \"view\" mode"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Enable the read-only command line option, which allows the user to
//config:     open a file in read-only mode.
#ifdef VI_ENABLE_READONLY
#define ENABLE_FEATURE_VI_READONLY 1
#define IF_FEATURE_VI_READONLY(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_READONLY 0
#define IF_FEATURE_VI_READONLY(...)
#endif

//config:config FEATURE_VI_SET
//config:   bool "Support for :set"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Support for ":set".
#ifdef VI_ENABLE_SET
#define ENABLE_FEATURE_VI_SET 1
#define IF_FEATURE_VI_SET(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_SET 0
#define IF_FEATURE_VI_SET(...)
#endif

//config:config FEATURE_VI_SETOPTS
//config:   bool "Enable set-able options, ai ic showmatch"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Enable the editor to set some (ai, ic, showmatch) options.
#ifdef VI_ENABLE_SETOPTS
#define ENABLE_FEATURE_VI_SETOPTS 1
#define IF_FEATURE_VI_SETOPTS(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_SETOPTS 0
#define IF_FEATURE_VI_SETOPTS(...)
#endif

//config:
//config:config FEATURE_VI_WIN_RESIZE
//config:   bool "Handle window resize"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Make busybox vi behave nicely with terminals that get resized.
#ifdef VI_ENABLE_WIN_RESIZE
#define ENABLE_FEATURE_VI_WIN_RESIZE 1
#define IF_FEATURE_VI_WIN_RESIZE(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_WIN_RESIZE 0
#define IF_FEATURE_VI_WIN_RESIZE(...)
#endif

//config:config FEATURE_VI_YANKMARK
//config:   bool "Enable yank/put commands and mark cmds"
//config:   default y
//config:   depends on VI
//config:   help
//config:     This will enable you to use yank and put, as well as mark in
//config:     busybox vi.
//config:
#ifdef VI_ENABLE_YANKMARK
#define ENABLE_FEATURE_VI_YANKMARK 1
#define IF_FEATURE_VI_YANKMARK(...) __VA_ARGS__
#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))
#else
#define ENABLE_FEATURE_VI_YANKMARK 0
#define IF_FEATURE_VI_YANKMARK(...)
#endif

//config:config FEATURE_VI_DOT_CMD
//config:   bool "Remember previous cmd and \".\" cmd"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Make busybox vi remember the last command and be able to repeat it.
#ifdef VI_ENABLE_DOT_CMD
#define ENABLE_FEATURE_VI_DOT_CMD 1
#define IF_FEATURE_VI_DOT_CMD(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_DOT_CMD 0
#define IF_FEATURE_VI_DOT_CMD(...)
#endif

//config:config FEATURE_VI_UNDO
//config:   bool "Support undo command 'u'"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Support the 'u' command to undo insertion, deletion, and replacement
//config:     of text.
#ifdef VI_ENABLE_UNDO
#define ENABLE_FEATURE_VI_UNDO 1
#define IF_FEATURE_VI_UNDO(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_UNDO 0
#define IF_FEATURE_VI_UNDO(...)
#endif

//config:config FEATURE_VI_UNDO_QUEUE
//config:   bool "Enable undo operation queuing"
//config:   default y
//config:   depends on FEATURE_VI_UNDO
//config:   help
//config:     The vi undo functions can use an intermediate queue to greatly lower
//config:     malloc() calls and overhead. When the maximum size of this queue is
//config:     reached, the contents of the queue are committed to the undo stack.
//config:     This increases the size of the undo code and allows some undo
//config:     operations (especially un-typing/backspacing) to be far more useful.
//config:config FEATURE_VI_UNDO_QUEUE_MAX
//config:   int "Maximum undo character queue size"
//config:   default 256
//config:   range 32 65536
//config:   depends on FEATURE_VI_UNDO_QUEUE
//config:   help
//config:     This option sets the number of bytes used at runtime for the queue.
//config:     Smaller values will create more undo objects and reduce the amount
//config:     of typed or backspaced characters that are grouped into one undo
//config:     operation; larger values increase the potential size of each undo
//config:     and will generally malloc() larger objects and less frequently.
//config:     Unless you want more (or less) frequent "undo points" while typing,
//config:     you should probably leave this unchanged.
#ifdef VI_ENABLE_UNDO_QUEUE
#define ENABLE_FEATURE_VI_UNDO_QUEUE 1
#define IF_FEATURE_VI_UNDO_QUEUE(...) __VA_ARGS__
#define CONFIG_FEATURE_VI_UNDO_QUEUE_MAX  VI_UNDO_QUEUE_MAX
#else
#define ENABLE_FEATURE_VI_UNDO_QUEUE 0
#define IF_FEATURE_VI_UNDO_QUEUE(...)
#endif

//config:config FEATURE_VI_VERBOSE_STATUS
//config:   bool "Enable verbose status reporting"
//config:   default y
//config:   depends on VI
//config:   help
//config:   Enable more verbose reporting of the results of yank, change,
//config:   delete, undo and substitution commands.
#ifdef VI_ENABLE_VERBOSE_STATUS
#define ENABLE_FEATURE_VI_VERBOSE_STATUS 1
#else
#define ENABLE_FEATURE_VI_VERBOSE_STATUS 0
#endif

//config:config FEATURE_VI_REGEX_SEARCH
//config:   bool "Enable regex in search and replace"
//config:   default n   # Uses GNU regex, which may be unavailable. FIXME
//config:   depends on FEATURE_VI_SEARCH
//config:   help
//config:     Use extended regex search.
#ifdef VI_ENABLE_REGEX_SEARCH
#define ENABLE_FEATURE_VI_REGEX_SEARCH 1
#define IF_FEATURE_VI_REGEX_SEARCH(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_REGEX_SEARCH 0
#define IF_FEATURE_VI_REGEX_SEARCH(...)
#endif

//config:config FEATURE_VI_8BIT
//config:   bool "Allow vi to display 8-bit chars (otherwise shows dots)"
//config:   default n
//config:   depends on VI
//config:   help
//config:     If your terminal can display characters with high bit set,
//config:     you may want to enable this. Note: vi is not Unicode-capable.
//config:     If your terminal combines several 8-bit bytes into one character
//config:     (as in Unicode mode), this will not work properly.
#ifdef VI_ENABLE_8BIT
#define ENABLE_FEATURE_VI_8BIT 1
#define IF_FEATURE_VI_8BIT(...) __VA_ARGS__
#else
#define ENABLE_FEATURE_VI_8BIT 0
#define IF_FEATURE_VI_8BIT(...)
#endif

/*----------------------------------------------------------------*/

#define SET_PTR_TO_GLOBALS(x) do { \
    (*(struct globals**)&ptr_to_globals) = (void*)(x); \
    barrier(); \
} while (0)

/* "Keycodes" that report an escape sequence.
 * We use something which fits into signed char,
 * yet doesn't represent any valid Unicode character.
 * Also, -1 is reserved for error indication and we don't use it. */
enum {
    KEYCODE_UP       =  -2,
    KEYCODE_DOWN     =  -3,
    KEYCODE_RIGHT    =  -4,
    KEYCODE_LEFT     =  -5,
    KEYCODE_HOME     =  -6,
    KEYCODE_END      =  -7,
    KEYCODE_INSERT   =  -8,
    KEYCODE_DELETE   =  -9,
    KEYCODE_PAGEUP   = -10,
    KEYCODE_PAGEDOWN = -11,
    // -12 is reserved for Alt/Ctrl/Shift-TAB
#if 0
    KEYCODE_FUN1     = -13,
    KEYCODE_FUN2     = -14,
    KEYCODE_FUN3     = -15,
    KEYCODE_FUN4     = -16,
    KEYCODE_FUN5     = -17,
    KEYCODE_FUN6     = -18,
    KEYCODE_FUN7     = -19,
    KEYCODE_FUN8     = -20,
    KEYCODE_FUN9     = -21,
    KEYCODE_FUN10    = -22,
    KEYCODE_FUN11    = -23,
    KEYCODE_FUN12    = -24,
#endif
    /* Be sure that last defined value is small enough
     * to not interfere with Alt/Ctrl/Shift bits.
     * So far we do not exceed -31 (0xfff..fffe1),
     * which gives us three upper bits in LSB to play with.
     */
    //KEYCODE_SHIFT_TAB  = (-12)         & ~0x80,
    //KEYCODE_SHIFT_...  = KEYCODE_...   & ~0x80,
    //KEYCODE_CTRL_UP    = KEYCODE_UP    & ~0x40,
    //KEYCODE_CTRL_DOWN  = KEYCODE_DOWN  & ~0x40,
    KEYCODE_CTRL_RIGHT = KEYCODE_RIGHT & ~0x40,
    KEYCODE_CTRL_LEFT  = KEYCODE_LEFT  & ~0x40,
    //KEYCODE_ALT_UP     = KEYCODE_UP    & ~0x20,
    //KEYCODE_ALT_DOWN   = KEYCODE_DOWN  & ~0x20,
    KEYCODE_ALT_RIGHT  = KEYCODE_RIGHT & ~0x20,
    KEYCODE_ALT_LEFT   = KEYCODE_LEFT  & ~0x20,

    KEYCODE_CURSOR_POS = -0x100, /* 0xfff..fff00 */
    /* How long is the longest ESC sequence we know?
     * We want it big enough to be able to contain
     * cursor position sequence "ESC [ 9999 ; 9999 R"
     */
    KEYCODE_BUFFER_SIZE = 16
};

typedef enum {FALSE = 0, TRUE = !FALSE} bool;
typedef int smallint;
typedef unsigned smalluint;

#if defined(_MSC_VER) || defined(__CC_ARM) || defined(__ICCARM__)
#define ALIGN1
#define barrier()
#define F_OK    0
#define R_OK    4
#define W_OK    2
#define X_OK    1
int isblank(int ch);
int isatty (int  fd);
#else
#define ALIGN1 __attribute__((aligned(1)))
/* At least gcc 3.4.6 on mipsel system needs optimization barrier */
#define barrier() __asm__ __volatile__("":::"memory")
#endif

#define vi_strtou strtoul
#define fflush_all() fflush(NULL)

unsigned char vi_mem_init(void);
void vi_mem_release(void);
void *vi_malloc(rt_size_t size);
void *vi_realloc(void *rmem, rt_size_t newsize);
void vi_free(void *ptr);
void* vi_zalloc(size_t size);
char *vi_strdup(const char *s);
char *vi_strndup(const char *s, size_t n);
int64_t read_key(int fd, char *buffer, int timeout);
void *memrchr(const void* ptr, int ch, size_t pos);
char* xasprintf(const char *format, ...);

#ifdef VI_ENABLE_SEARCH
char* strchrnul(const char *s, int c);
#endif

#ifdef VI_ENABLE_COLON
char* last_char_is(const char *s, int c);
#endif

#ifdef VI_ENABLE_SETOPTS
char* skip_whitespace(const char *s);
char* skip_non_whitespace(const char *s);
#endif

int index_in_strings(const char *strings, const char *key);
int safe_read(int fd, void *buf, size_t count);
int safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout);
ssize_t full_write(int fd, const void *buf, size_t len);
ssize_t full_read(int fd, void *buf, size_t len);

#ifdef RT_USING_POSIX_TERMIOS
#include <termios.h>
#define TERMIOS_CLEAR_ISIG      (1 << 0)
#define TERMIOS_RAW_CRNL_INPUT  (1 << 1)
#define TERMIOS_RAW_CRNL_OUTPUT (1 << 2)
#define TERMIOS_RAW_CRNL        (TERMIOS_RAW_CRNL_INPUT|TERMIOS_RAW_CRNL_OUTPUT)
#define TERMIOS_RAW_INPUT       (1 << 3)
int tcsetattr_stdin_TCSANOW(const struct termios *tp);
int get_terminal_width_height(int fd, unsigned *width, unsigned *height);
int set_termios_to_raw(int fd, struct termios *oldterm, int flags);
#endif

//config: TODO for RT-Thread
//config:config FEATURE_VI_USE_SIGNALS
//config:   bool "Catch signals"
//config:   default y
//config:   depends on VI
//config:   help
//config:     Selecting this option will make busybox vi signal aware. This will
//config:     make busybox vi support SIGWINCH to deal with Window Changes, catch
//config:     Ctrl-Z and Ctrl-C and alarms.

#endif
