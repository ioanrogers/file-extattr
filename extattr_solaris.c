#include "extattr_os.h"

#ifdef EXTATTR_SOLARIS

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

static const mode_t ATTRMODE = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP;

static int
writexattr (const int attrfd,
	    const char *attrvalue,
	    const size_t slen)
{
  int ok = 1;

  if (ftruncate(attrfd, 0) == -1)
    ok = 0;
  if (ok && (write(attrfd, attrvalue, slen) != slen))
    ok = 0;

  return ok ? 0 : -1;
}

static int
readclose (const int attrfd,
	   void *attrvalue,
	   const size_t slen)
{
  int sz = 0;
  int saved_errno = 0;
  int ok = 1;

  if (attrfd == -1)
    ok = 0;

  if (ok)
  {
    if (slen)
    {
      sz = read(attrfd, attrvalue, slen);
    }
    else
    {
      /* Request to see how much data is there. */
      struct stat sbuf;

      if (fstat(attrfd, &sbuf) == 0)
	sz = sbuf.st_size;
      else
	sz = -1;
    }

    if (sz == -1)
      ok = 0;
  }

  if (!ok)
    saved_errno = errno;
  if ((attrfd >= 0) && (close(attrfd) == -1) && !saved_errno)
    saved_errno = errno;
  if (saved_errno)
    errno = saved_errno;

  return ok ? sz : -1;
}

static int
unlinkclose (const int attrdirfd, const char *attrname)
{
  int sz = 0;
  int saved_errno = 0;
  int ok = 1;

  if (attrdirfd == -1)
    ok = 0;

  if (ok && (unlinkat(attrdirfd, attrname, 0) == -1))
    ok = 0;

  if (!ok)
    saved_errno = errno;
  if ((attrdirfd >= 0) && (close(attrdirfd) == -1) && !saved_errno)
    saved_errno = errno;
  if (saved_errno)
    errno = saved_errno;

  return ok ? sz : -1;  
}

static ssize_t
listclose (const int attrdirfd, char *buf, const size_t buflen)
{
  int saved_errno = 0;
  int ok = 1;
  ssize_t len = 0;
  DIR *dirp;

  if (attrdirfd == -1)
    ok = 0;

  if (ok)
    dirp = fdopendir(attrdirfd);

  if (ok)
  {
    struct dirent *de;

    while ((de = readdir(dirp)))
    {
      const size_t namelen = strlen(de->d_name);

      /* Ignore "." and ".." entries */
      if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
	continue;

      if (buflen)
      {
	/* Check for space, then copy directory name + nul into list. */
	if ((len + namelen + 1) > buflen)
	{
	  errno = ERANGE;
	  ok = 0;
	  break;
	}
	else
	{
	  strcpy(buf + len, de->d_name);
	  len += namelen;
	  buf[len] = '\0';
	  ++len;
	}
      }
      else
      {
	/* Seeing how much space is needed? */
	len += namelen + 1;
      }
    }
  }

  if (!ok)
    saved_errno = errno;
  if ((attrdirfd >= 0) && (close(attrdirfd) == -1) && !saved_errno)
    saved_errno = errno;
  if (saved_errno)
    errno = saved_errno;

  return ok ? len : -1;
}

int
solaris_setxattr (const char *path,
		  const char *attrname,
		  const char *attrvalue,
		  const size_t slen,
		  const int flags)
{
  /* XXX: Support overwrite/no overwrite flags */
  int saved_errno = 0;
  int ok = 1;
  int attrfd = attropen(path, attrname, O_RDWR|O_CREAT, ATTRMODE);

  /* XXX: More common code? */
  if (attrfd == -1)
    ok = 0;
  if (ok && (writexattr(attrfd, attrvalue, slen) == -1))
    ok = 0;

  if (!ok)
    saved_errno = errno;
  if ((attrfd >= 0) && (close(attrfd) == -1) && !saved_errno)
    saved_errno = errno;
  if (saved_errno)
    errno = saved_errno;

  return ok ? 0 : -1;
}

int solaris_fsetxattr (const int fd,
		       const char *attrname,
		       const char *attrvalue,
		       const size_t slen,
		       const int flags)
{
  /* XXX: Support overwrite/no overwrite flags */
  int saved_errno = 0;
  int ok = 1;
  int attrfd = openat(fd, attrname, O_RDWR|O_CREAT|O_XATTR, ATTRMODE);

  /* XXX: More common code? */
  if (attrfd == -1)
    ok = 0;
  if (ok && (writexattr(attrfd, attrvalue, slen) == -1))
    ok = 0;

  if (!ok)
    saved_errno = errno;
  if ((attrfd >= 0) && (close(attrfd) == -1) && !saved_errno)
    saved_errno = errno;
  if (saved_errno)
    errno = saved_errno;

  return ok ? 0 : -1;
}

int
solaris_getxattr (const char *path,
		  const char *attrname,
		  void *attrvalue,
		  const size_t slen)
{
  const int attrfd = attropen(path, attrname, O_RDONLY);
  return readclose(attrfd, attrvalue, slen);
}

int
solaris_fgetxattr (const int fd,
		   const char *attrname,
		   void *attrvalue,
		   const size_t slen)
{
  int attrfd = openat(fd, attrname, O_RDONLY|O_XATTR);
  return readclose(attrfd, attrvalue, slen);
}

int
solaris_removexattr (const char *path, const char *attrname)
{
  int attrdirfd = attropen(path, ".", O_RDONLY);
  return unlinkclose(attrdirfd, attrname);
}

int
solaris_fremovexattr (const int fd, const char *attrname)
{
  int attrdirfd = openat(fd, ".", O_RDONLY|O_XATTR);
  return unlinkclose(attrdirfd, attrname);
}

ssize_t
solaris_listxattr (const char *path, char *buf, const size_t buflen)
{
  int attrdirfd = attropen(path, ".", O_RDONLY);
  return listclose(attrdirfd, buf, buflen);
}

ssize_t
solaris_flistxattr (const int fd, char *buf, const size_t buflen)
{
  int attrdirfd = openat(fd, ".", O_RDONLY|O_XATTR);
  return listclose(attrdirfd, buf, buflen);
}

#endif /* EXTATTR_SOLARIS */
