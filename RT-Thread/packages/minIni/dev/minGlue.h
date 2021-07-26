/*  Glue functions for the minIni library, based on the C/C++ stdio library
 *
 *  Or better said: this file contains macros that maps the function interface
 *  used by minIni to the standard C/C++ file I/O functions.
 *
 *  By CompuPhase, 2008-2014
 *  This "glue file" is in the public domain. It is distributed without
 *  warranties or conditions of any kind, either express or implied.
 */

/* map required file I/O types and functions to the standard C library */
#include <dfs_posix.h>

#define INI_BUFFERSIZE  256       /* maximum line length, maximum path length */

#define INI_FILETYPE                    int
#define ini_openread(filename,file)     ((*(file) = open((filename),O_RDONLY, 0)) >= 0)
#define ini_openwrite(filename,file)    ((*(file) = open((filename),O_WRONLY | O_CREAT /*O_TRUNC*/, 0)) >= 0) 
#define ini_close(file)                 (close(*(file)) == 0)
//#define ini_read(buffer,size,file)      (read(*(file), (buffer),(size)) > 0)
#define ini_write(buffer,file)          write(*(file), (buffer), strlen(buffer))
#define ini_rename(source,dest)         rename((source), (dest))
#define ini_remove(filename)            unlink(filename)

#define INI_FILEPOS                     off_t
#define ini_tell(file,pos)              (*(pos) = lseek(*(file), 0, SEEK_CUR))
#define ini_seek(file,pos)              lseek(*(file), *(pos), SEEK_SET)


static int ini_read(char *buffer, int size, INI_FILETYPE *file)
{
  int numread = size;
  char *eol;

  if ((numread = read(*file, buffer, size)) == 0) {
     return 0;                   /* at EOF */
  }
  if ((eol = strchr(buffer, '\n')) == NULL)
    eol = strchr(buffer, '\r');
  if (eol != NULL) {
    /* terminate the buffer */
    *++eol = '\0';
    /* "unread" the data that was read too much */
    lseek(*file, - (int)(numread - (size_t)(eol - buffer)), SEEK_CUR);
  } /* if */
  return 1;
}
