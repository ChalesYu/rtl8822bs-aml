/*
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#include "vi_utils.h"

#define DBG_TAG "vi"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <rtconfig.h>
#ifndef VI_SANDBOX_SIZE_KB
#define VI_SANDBOX_SIZE_KB   20 /* KB */
#endif

static const char *vi_outof_momory_warning = "vi sandbox runs out of memory, please enlarge VI_SANDBOX_SIZE_KB";

int index_in_strings(const char *strings, const char *key)
{
    int j, idx = 0;

    while (*strings) {
        /* Do we see "key\0" at current position in strings? */
        for (j = 0; *strings == key[j]; ++j) {
            if (*strings++ == '\0') {
                //bb_error_msg("found:'%s' i:%u", key, idx);
                return idx; /* yes */
            }
        }
        /* No.  Move to the start of the next string. */
        while (*strings++ != '\0')
            continue;
        idx++;
    }
    return -1;
}

#ifdef VI_ENABLE_COLON
/* Find out if the last character of a string matches the one given */
char* last_char_is(const char *s, int c)
{
    if (!s[0])
        return NULL;
    while (s[1])
        s++;
    return (*s == (char)c) ? (char *) s : NULL;
}
#endif

#if defined(_MSC_VER) || defined(__CC_ARM) || defined(__ICCARM__)
void *memrchr(const void* ptr, int ch, size_t pos)
{
    char *end = (char *)ptr+pos-1;
    while (end != ptr)
    {
        if (*end == ch)
            return end;
        end--;
    }
    return (*end == ch)?(end):(NULL);
}

#ifndef __ICCARM__
int isblank(int ch)
{
    if (ch == ' ' || ch == '\t')
        return 1;
    return 0;
}
#endif
#endif

#ifdef VI_ENABLE_SETOPTS
char* skip_whitespace(const char *s)
{
    /* In POSIX/C locale (the only locale we care about: do we REALLY want
     * to allow Unicode whitespace in, say, .conf files? nuts!)
     * isspace is only these chars: "\t\n\v\f\r" and space.
     * "\t\n\v\f\r" happen to have ASCII codes 9,10,11,12,13.
     * Use that.
     */
    while (*s == ' ' || (unsigned char)(*s - 9) <= (13 - 9))
        s++;

    return (char *) s;
}

char* skip_non_whitespace(const char *s)
{
    while (*s != '\0' && *s != ' ' && (unsigned char)(*s - 9) > (13 - 9))
        s++;

    return (char *) s;
}
#endif

ssize_t safe_read(int fd, void *buf, size_t count)
{
    ssize_t n;

    for (;;) {
        n = read(fd, buf, count);
        if (n >= 0 || errno != EINTR)
            break;
        /* Some callers set errno=0, are upset when they see EINTR.
         * Returning EINTR is wrong since we retry read(),
         * the "error" was transient.
         */
        errno = 0;
        /* repeat the read() */
    }

    return n;
}

/*
 * Read all of the supplied buffer from a file.
 * This does multiple reads as necessary.
 * Returns the amount read, or -1 on an error.
 * A short read is returned on an end of file.
 */
ssize_t full_read(int fd, void *buf, size_t len)
{
    ssize_t cc;
    ssize_t total;

    total = 0;

    while (len) {
        cc = safe_read(fd, buf, len);

        if (cc < 0) {
            if (total) {
                /* we already have some! */
                /* user can do another read to know the error code */
                return total;
            }
            return cc; /* read() returns -1 on failure. */
        }
        if (cc == 0)
            break;
        buf = ((char *)buf) + cc;
        total += cc;
        len -= cc;
    }

    return total;
}

ssize_t safe_write(int fd, const void *buf, size_t count)
{
    ssize_t n;

    for (;;) {
        n = write(fd, buf, count);
        if (n >= 0 || errno != EINTR)
            break;
        /* Some callers set errno=0, are upset when they see EINTR.
         * Returning EINTR is wrong since we retry write(),
         * the "error" was transient.
         */
        errno = 0;
        /* repeat the write() */
    }

    return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 if error was seen
 * on the very first write.
 */
ssize_t full_write(int fd, const void *buf, size_t len)
{
    ssize_t cc;
    ssize_t total;

    total = 0;

    while (len) {
        cc = safe_write(fd, buf, len);

        if (cc < 0) {
            if (total) {
                /* we already wrote some! */
                /* user can do another write to know the error code */
                return total;
            }
            return cc;  /* write() returns -1 on failure. */
        }

        total += cc;
        buf = ((const char *)buf) + cc;
        len -= cc;
    }

    return total;
}

/* Wrapper which restarts poll on EINTR or ENOMEM.
 * On other errors does perror("poll") and returns.
 * Warning! May take longer than timeout_ms to return! */
int safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
    while (1) {
        int n = poll(ufds, nfds, timeout);
        if (n >= 0)
            return n;
        /* Make sure we inch towards completion */
        if (timeout > 0)
            timeout--;
        /* E.g. strace causes poll to return this */
        if (errno == EINTR)
            continue;
        /* Kernel is very low on memory. Retry. */
        /* I doubt many callers would handle this correctly! */
        if (errno == ENOMEM)
            continue;
        printf("poll");
        return n;
    }
}

static mem_sandbox_t vi_sandbox = RT_NULL;

unsigned char vi_mem_init(void)
{
    vi_sandbox = mem_sandbox_create(VI_SANDBOX_SIZE_KB * 1024);
    if(vi_sandbox == RT_NULL)
    {
        LOG_E("vi sandbox create error");
        return 0;
    }
    else
    {
        return 1;
    }
}

void vi_mem_release(void)
{
    mem_sandbox_delete(vi_sandbox);
}

void *vi_malloc(rt_size_t size)
{
    void * p;
    p = mem_sandbox_malloc(vi_sandbox, size);
    if(p == RT_NULL)
    {
        LOG_E(vi_outof_momory_warning);
        RT_ASSERT(p != RT_NULL);
        return RT_NULL;
    }
    return p;
}

void *vi_realloc(void *rmem, rt_size_t newsize)
{
    void *p;
    p = mem_sandbox_realloc(vi_sandbox, rmem, newsize);
    if(p == RT_NULL && newsize != 0)
    {
        LOG_E(vi_outof_momory_warning);
        RT_ASSERT(p != RT_NULL);
        return RT_NULL;
    }
    return p;
}

void vi_free(void *ptr)
{
    mem_sandbox_free(vi_sandbox, ptr);
}

void* vi_zalloc(size_t size)
{
    void *ptr = vi_malloc(size);
    rt_memset(ptr, 0, size);
    return ptr;
}

char *vi_strdup(const char *s)
{
    void *p;
    p = mem_sandbox_strdup(vi_sandbox, s);
    if(p == RT_NULL)
    {
        LOG_E(vi_outof_momory_warning);
        RT_ASSERT(p != RT_NULL);
        return RT_NULL;
    }
    return p;
}

char *vi_strndup(const char *s, size_t n)
{
    void *p;
    p = mem_sandbox_strndup(vi_sandbox, s, n);
    if(p == RT_NULL)
    {
        LOG_E(vi_outof_momory_warning);
        RT_ASSERT(p != RT_NULL);
        return RT_NULL;
    }
    return p;
}

int64_t read_key(int fd, char *buffer, int timeout)
{
    struct pollfd pfd;
    const char *seq;
    int n;

    /* Known escape sequences for cursor and function keys.
     * See "Xterm Control Sequences"
     * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
     */
    static const char esccmds[] ALIGN1 = {
        'O','A'        |0x80, (char) KEYCODE_UP      ,
        'O','B'        |0x80, (char) KEYCODE_DOWN    ,
        'O','C'        |0x80, (char) KEYCODE_RIGHT   ,
        'O','D'        |0x80, (char) KEYCODE_LEFT    ,
        'O','H'        |0x80, (char) KEYCODE_HOME    ,
        'O','F'        |0x80, (char) KEYCODE_END     ,
#if 0
        'O','P'        |0x80, (char) KEYCODE_FUN1    ,
        /* [ESC] ESC O [2] P - [Alt-][Shift-]F1 */
        /* ESC [ O 1 ; 2 P - Shift-F1 */
        /* ESC [ O 1 ; 3 P - Alt-F1 */
        /* ESC [ O 1 ; 4 P - Alt-Shift-F1 */
        /* ESC [ O 1 ; 5 P - Ctrl-F1 */
        /* ESC [ O 1 ; 6 P - Ctrl-Shift-F1 */
        'O','Q'        |0x80, (char) KEYCODE_FUN2    ,
        'O','R'        |0x80, (char) KEYCODE_FUN3    ,
        'O','S'        |0x80, (char) KEYCODE_FUN4    ,
#endif
        '[','A'        |0x80, (char) KEYCODE_UP      ,
        '[','B'        |0x80, (char) KEYCODE_DOWN    ,
        '[','C'        |0x80, (char) KEYCODE_RIGHT   ,
        '[','D'        |0x80, (char) KEYCODE_LEFT    ,
        /* ESC [ 1 ; 2 x, where x = A/B/C/D: Shift-<arrow> */
        /* ESC [ 1 ; 3 x, where x = A/B/C/D: Alt-<arrow> - implemented below */
        /* ESC [ 1 ; 4 x, where x = A/B/C/D: Alt-Shift-<arrow> */
        /* ESC [ 1 ; 5 x, where x = A/B/C/D: Ctrl-<arrow> - implemented below */
        /* ESC [ 1 ; 6 x, where x = A/B/C/D: Ctrl-Shift-<arrow> */
        /* ESC [ 1 ; 7 x, where x = A/B/C/D: Ctrl-Alt-<arrow> */
        /* ESC [ 1 ; 8 x, where x = A/B/C/D: Ctrl-Alt-Shift-<arrow> */
        '[','H'        |0x80, (char) KEYCODE_HOME    , /* xterm */
        '[','F'        |0x80, (char) KEYCODE_END     , /* xterm */
        /* [ESC] ESC [ [2] H - [Alt-][Shift-]Home (End similarly?) */
        /* '[','Z'        |0x80, (char) KEYCODE_SHIFT_TAB, */
        '[','1','~'    |0x80, (char) KEYCODE_HOME    , /* vt100? linux vt? or what? */
        '[','2','~'    |0x80, (char) KEYCODE_INSERT  ,
        /* ESC [ 2 ; 3 ~ - Alt-Insert */
        '[','3','~'    |0x80, (char) KEYCODE_DELETE  ,
        /* [ESC] ESC [ 3 [;2] ~ - [Alt-][Shift-]Delete */
        /* ESC [ 3 ; 3 ~ - Alt-Delete */
        /* ESC [ 3 ; 5 ~ - Ctrl-Delete */
        '[','4','~'    |0x80, (char) KEYCODE_END     , /* vt100? linux vt? or what? */
        '[','5','~'    |0x80, (char) KEYCODE_PAGEUP  ,
        /* ESC [ 5 ; 3 ~ - Alt-PgUp */
        /* ESC [ 5 ; 5 ~ - Ctrl-PgUp */
        /* ESC [ 5 ; 7 ~ - Ctrl-Alt-PgUp */
        '[','6','~'    |0x80, (char) KEYCODE_PAGEDOWN,
        '[','7','~'    |0x80, (char) KEYCODE_HOME    , /* vt100? linux vt? or what? */
        '[','8','~'    |0x80, (char) KEYCODE_END     , /* vt100? linux vt? or what? */
#if 0
        '[','1','1','~'|0x80, (char) KEYCODE_FUN1    , /* old xterm, deprecated by ESC O P */
        '[','1','2','~'|0x80, (char) KEYCODE_FUN2    , /* old xterm... */
        '[','1','3','~'|0x80, (char) KEYCODE_FUN3    , /* old xterm... */
        '[','1','4','~'|0x80, (char) KEYCODE_FUN4    , /* old xterm... */
        '[','1','5','~'|0x80, (char) KEYCODE_FUN5    ,
        /* [ESC] ESC [ 1 5 [;2] ~ - [Alt-][Shift-]F5 */
        '[','1','7','~'|0x80, (char) KEYCODE_FUN6    ,
        '[','1','8','~'|0x80, (char) KEYCODE_FUN7    ,
        '[','1','9','~'|0x80, (char) KEYCODE_FUN8    ,
        '[','2','0','~'|0x80, (char) KEYCODE_FUN9    ,
        '[','2','1','~'|0x80, (char) KEYCODE_FUN10   ,
        '[','2','3','~'|0x80, (char) KEYCODE_FUN11   ,
        '[','2','4','~'|0x80, (char) KEYCODE_FUN12   ,
        /* ESC [ 2 4 ; 2 ~ - Shift-F12 */
        /* ESC [ 2 4 ; 3 ~ - Alt-F12 */
        /* ESC [ 2 4 ; 4 ~ - Alt-Shift-F12 */
        /* ESC [ 2 4 ; 5 ~ - Ctrl-F12 */
        /* ESC [ 2 4 ; 6 ~ - Ctrl-Shift-F12 */
#endif
        /* '[','1',';','5','A' |0x80, (char) KEYCODE_CTRL_UP   , - unused */
        /* '[','1',';','5','B' |0x80, (char) KEYCODE_CTRL_DOWN , - unused */
        '[','1',';','5','C' |0x80, (char) KEYCODE_CTRL_RIGHT,
        '[','1',';','5','D' |0x80, (char) KEYCODE_CTRL_LEFT ,
        /* '[','1',';','3','A' |0x80, (char) KEYCODE_ALT_UP    , - unused */
        /* '[','1',';','3','B' |0x80, (char) KEYCODE_ALT_DOWN  , - unused */
        '[','1',';','3','C' |0x80, (char) KEYCODE_ALT_RIGHT,
        '[','1',';','3','D' |0x80, (char) KEYCODE_ALT_LEFT ,
        /* '[','3',';','3','~' |0x80, (char) KEYCODE_ALT_DELETE, - unused */
        0
    };
    pfd.fd = fd;
    pfd.events = POLLIN;
    buffer++; /* saved chars counter is in buffer[-1] now */

 start_over:
    errno = 0;
    n = (unsigned char)buffer[-1];
    if (n == 0) {
        /* If no data, wait for input.
         * If requested, wait TIMEOUT ms. TIMEOUT = -1 is useful
         * if fd can be in non-blocking mode.
         */
        if (timeout >= -1) {
            if (safe_poll(&pfd, 1, timeout) == 0) {
                /* Timed out */
                errno = EAGAIN;
                return -1;
            }
        }
        /* It is tempting to read more than one byte here,
         * but it breaks pasting. Example: at shell prompt,
         * user presses "c","a","t" and then pastes "\nline\n".
         * When we were reading 3 bytes here, we were eating
         * "li" too, and cat was getting wrong input.
         */
        n = safe_read(fd, buffer, 1);
        if (n <= 0)
            return -1;
    }

    {
        unsigned char c = buffer[0];
        n--;
        if (n)
            memmove(buffer, buffer + 1, n);
        /* Only ESC starts ESC sequences */
        if (c != 27) {
            buffer[-1] = n;
            return c;
        }
    }

    /* Loop through known ESC sequences */
    seq = esccmds;
    while (*seq != '\0') {
        /* n - position in sequence we did not read yet */
        int i = 0; /* position in sequence to compare */

        /* Loop through chars in this sequence */
        while (1) {
            /* So far escape sequence matched up to [i-1] */
            if (n <= i) {
                int read_num;
                /* Need more chars, read another one if it wouldn't block.
                 * Note that escape sequences come in as a unit,
                 * so if we block for long it's not really an escape sequence.
                 * Timeout is needed to reconnect escape sequences
                 * split up by transmission over a serial console. */
                if (safe_poll(&pfd, 1, 50) == 0) {
                    /* No more data!
                     * Array is sorted from shortest to longest,
                     * we can't match anything later in array -
                     * anything later is longer than this seq.
                     * Break out of both loops. */
                    goto got_all;
                }
                errno = 0;
                read_num = safe_read(fd, buffer + n, 1);
                if (read_num <= 0) {
                    /* If EAGAIN, then fd is O_NONBLOCK and poll lied:
                     * in fact, there is no data. */
                    if (errno != EAGAIN) {
                        /* otherwise: it's EOF/error */
                        buffer[-1] = 0;
                        return -1;
                    }
                    goto got_all;
                }
                n++;
            }
            if (buffer[i] != (seq[i] & 0x7f)) {
                /* This seq doesn't match, go to next */
                seq += i;
                /* Forward to last char */
                while (!(*seq & 0x80))
                    seq++;
                /* Skip it and the keycode which follows */
                seq += 2;
                break;
            }
            if (seq[i] & 0x80) {
                /* Entire seq matched */
                n = 0;
                /* n -= i; memmove(...);
                 * would be more correct,
                 * but we never read ahead that much,
                 * and n == i here. */
                buffer[-1] = 0;
                return (signed char)seq[i+1];
            }
            i++;
        }
    }
    /* We did not find matching sequence.
     * We possibly read and stored more input in buffer[] by now.
     * n = bytes read. Try to read more until we time out.
     */
    while (n < KEYCODE_BUFFER_SIZE-1) { /* 1 for count byte at buffer[-1] */
        int read_num;
        if (safe_poll(&pfd, 1, 50) == 0) {
            /* No more data! */
            break;
        }
        errno = 0;
        read_num = safe_read(fd, buffer + n, 1);
        if (read_num <= 0) {
            /* If EAGAIN, then fd is O_NONBLOCK and poll lied:
             * in fact, there is no data. */
            if (errno != EAGAIN) {
                /* otherwise: it's EOF/error */
                buffer[-1] = 0;
                return -1;
            }
            break;
        }
        n++;
        /* Try to decipher "ESC [ NNN ; NNN R" sequence */
        if ((ENABLE_FEATURE_VI_ASK_TERMINAL)
         && n >= 5
         && buffer[0] == '['
         && buffer[n-1] == 'R'
         && isdigit((unsigned char)buffer[1])
        ) {
            char *end;
            unsigned long row, col;

            row = strtoul(buffer + 1, &end, 10);
            if (*end != ';' || !isdigit((unsigned char)end[1]))
                continue;
            col = strtoul(end + 1, &end, 10);
            if (*end != 'R')
                continue;
            if (row < 1 || col < 1 || (row | col) > 0x7fff)
                continue;

            buffer[-1] = 0;
            /* Pack into "1 <row15bits> <col16bits>" 32-bit sequence */
            row |= ((unsigned)(-1) << 15);
            col |= (row << 16);
            /* Return it in high-order word */
            return ((int64_t) col << 32) | (uint32_t)KEYCODE_CURSOR_POS;
        }
    }
 got_all:

    if (n <= 1) {
        /* Alt-x is usually returned as ESC x.
         * Report ESC, x is remembered for the next call.
         */
        buffer[-1] = n;
        return 27;
    }

    /* We were doing "buffer[-1] = n; return c;" here, but this results
     * in unknown key sequences being interpreted as ESC + garbage.
     * This was not useful. Pretend there was no key pressed,
     * go and wait for a new keypress:
     */
    buffer[-1] = 0;
    goto start_over;
}

static int vasprintf(char **string_ptr, const char *format, va_list p)
{
    int r;
    va_list p2;
    char buf[128];

    va_copy(p2, p);
    r = vsnprintf(buf, 128, format, p);
    va_end(p);

    /* Note: can't use xstrdup/xmalloc, they call vasprintf (us) on failure! */

    if (r < 128) {
        va_end(p2);
        *string_ptr = vi_strdup(buf);
        return (*string_ptr ? r : -1);
    }

    *string_ptr = vi_malloc(r+1);
    r = (*string_ptr ? vsnprintf(*string_ptr, r+1, format, p2) : -1);
    va_end(p2);

    return r;
}

// Die with an error message if we can't malloc() enough space and do an
// sprintf() into that space.
char* xasprintf(const char *format, ...)
{
    va_list p;
    int r;
    char *string_ptr;

    va_start(p, format);
    r = vasprintf(&string_ptr, format, p);
    va_end(p);
    if (r < 0)
        printf("die_memory_exhausted"); //bb_die_memory_exhausted();
    return string_ptr;
}


#ifdef RT_USING_POSIX_TERMIOS
static int wh_helper(int value, int def_val, const char *env_name, int *err)
{
    /* Envvars override even if "value" from ioctl is valid (>0).
     * Rationale: it's impossible to guess what user wants.
     * For example: "man CMD | ...": should "man" format output
     * to stdout's width? stdin's width? /dev/tty's width? 80 chars?
     * We _cant_ know it. If "..." saves text for e.g. email,
     * then it's probably 80 chars.
     * If "..." is, say, "grep -v DISCARD | $PAGER", then user
     * would prefer his tty's width to be used!
     *
     * Since we don't know, at least allow user to do this:
     * "COLUMNS=80 man CMD | ..."
     */
    char *s = getenv(env_name);
    if (s) {
        value = atoi(s);
        /* If LINES/COLUMNS are set, pretend that there is
         * no error getting w/h, this prevents some ugly
         * cursor tricks by our callers */
        *err = 0;
    }

    if (value <= 1 || value >= 30000)
        value = def_val;
    return value;
}

int get_terminal_width_height(int fd, unsigned *width, unsigned *height)
{
    struct winsize win;
    int err;
    int close_me = -1;

    if (fd == -1) {
        if (isatty(STDOUT_FILENO))
            fd = STDOUT_FILENO;
        else
        if (isatty(STDERR_FILENO))
            fd = STDERR_FILENO;
        else
        if (isatty(STDIN_FILENO))
            fd = STDIN_FILENO;
        else
            close_me = fd = open("/dev/tty", O_RDONLY);
    }

    win.ws_row = 0;
    win.ws_col = 0;
    /* I've seen ioctl returning 0, but row/col is (still?) 0.
     * We treat that as an error too.  */
    err = ioctl(fd, TIOCGWINSZ, &win) != 0 || win.ws_row == 0;
    if (height)
        *height = wh_helper(win.ws_row, 24, "LINES", &err);
    if (width)
        *width = wh_helper(win.ws_col, 80, "COLUMNS", &err);

    if (close_me >= 0)
        close(close_me);

    return err;
}

int tcsetattr_stdin_TCSANOW(const struct termios *tp)
{
    return tcsetattr(STDIN_FILENO, TCSANOW, tp);
}

static int get_termios_and_make_raw(int fd, struct termios *newterm, struct termios *oldterm, int flags)
{
//TODO: slattach, shell read might be adapted to use this too: grep for "tcsetattr", "[VTIME] = 0"
    int r;

    memset(oldterm, 0, sizeof(*oldterm)); /* paranoia */
    r = tcgetattr(fd, oldterm);
    *newterm = *oldterm;

    /* Turn off buffered input (ICANON)
     * Turn off echoing (ECHO)
     * and separate echoing of newline (ECHONL, normally off anyway)
     */
    newterm->c_lflag &= ~(ICANON | ECHO | ECHONL);
    if (flags & TERMIOS_CLEAR_ISIG) {
        /* dont recognize INT/QUIT/SUSP chars */
        newterm->c_lflag &= ~ISIG;
    }
    /* reads will block only if < 1 char is available */
    newterm->c_cc[VMIN] = 1;
    /* no timeout (reads block forever) */
    newterm->c_cc[VTIME] = 0;
/* IXON, IXOFF, and IXANY:
 * IXOFF=1: sw flow control is enabled on input queue:
 * tty transmits a STOP char when input queue is close to full
 * and transmits a START char when input queue is nearly empty.
 * IXON=1: sw flow control is enabled on output queue:
 * tty will stop sending if STOP char is received,
 * and resume sending if START is received, or if any char
 * is received and IXANY=1.
 */
    if (flags & TERMIOS_RAW_CRNL_INPUT) {
        /* IXON=0: XON/XOFF chars are treated as normal chars (why we do this?) */
        /* dont convert CR to NL on input */
        newterm->c_iflag &= ~(IXON | ICRNL);
    }
    if (flags & TERMIOS_RAW_CRNL_OUTPUT) {
        /* dont convert NL to CR+NL on output */
        newterm->c_oflag &= ~(ONLCR);
        /* Maybe clear more c_oflag bits? Usually, only OPOST and ONLCR are set.
         * OPOST  Enable output processing (reqd for OLCUC and *NL* bits to work)
         * OLCUC  Map lowercase characters to uppercase on output.
         * OCRNL  Map CR to NL on output.
         * ONOCR  Don't output CR at column 0.
         * ONLRET Don't output CR.
         */
    }
    if (flags & TERMIOS_RAW_INPUT) {
#ifndef IMAXBEL
# define IMAXBEL 0
#endif
#ifndef IUCLC
# define IUCLC 0
#endif
#ifndef IXANY
# define IXANY 0
#endif
        /* IXOFF=0: disable sending XON/XOFF if input buf is full
         * IXON=0: input XON/XOFF chars are not special
         * BRKINT=0: dont send SIGINT on break
         * IMAXBEL=0: dont echo BEL on input line too long
         * INLCR,ICRNL,IUCLC: dont convert anything on input
         */
        newterm->c_iflag &= ~(IXOFF|IXON|IXANY|BRKINT|INLCR|ICRNL|IUCLC|IMAXBEL);
    }
    return r;
}

int set_termios_to_raw(int fd, struct termios *oldterm, int flags)
{
    struct termios newterm;

    get_termios_and_make_raw(fd, &newterm, oldterm, flags);
    return tcsetattr(fd, TCSANOW, &newterm);
}

#endif /* RT_USING_POSIX_TERMIOS */

#if defined(VI_ENABLE_SEARCH) && !defined(__GNUC__)
char* strchrnul(const char *s, int c)
{
    while (*s != '\0' && *s != c)
        s++;
    return (char*)s;
}
#endif
