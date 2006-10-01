#include "extattr_os.h"

#ifdef EXTATTR_LINUX

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "flags.h"

static const char NAMESPACE_DEFAULT[] = "user";

static char *
flags2namespace (struct hv *flags)
{
  const size_t NAMESPACE_KEYLEN = strlen(NAMESPACE_KEY);
  SV **psv_ns;
  char *ns = NULL;

  if (flags && (psv_ns = hv_fetch(flags, NAMESPACE_KEY, NAMESPACE_KEYLEN, 0)))
  {
    char *s;
    STRLEN len;

    s = SvPV(*psv_ns, len);
    ns = malloc(len + 1);
    if (ns)
    {
      strncpy(ns, s, len);
      ns[len] = '\0';
    }
  }
  else
  {
    ns = strdup(NAMESPACE_DEFAULT);
  }

  return ns;
}

static char *
qualify_attrname (const char *attrname, struct hv *flags)
{
  char *res = NULL;
  char *ns;
  size_t reslen;

  ns = flags2namespace(flags);
  if (ns)
  {
    reslen = strlen(ns) + strlen(attrname) + 2; /* ns + "." + attrname + nul */
    res = malloc(reslen);
  }

  if (res)
    snprintf(res, reslen, "%s.%s", ns, attrname);

  if (ns)
    free(ns);

  return res;
}

int
linux_setxattr (const char *path,
                const char *attrname,
                const char *attrvalue,
                const size_t slen,
                struct hv *flags)
{
  int ret;
  char *q;
  File_ExtAttr_setflags_t setflags;
  int xflags = 0;

  setflags = File_ExtAttr_flags2setflags(flags);
  switch (setflags)
  {
  case SET_CREATEIFNEEDED: break;
  case SET_CREATE:         xflags |= XATTR_CREATE; break;
  case SET_REPLACE:        xflags |= XATTR_REPLACE; break;
  }

  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = setxattr(path, q, attrvalue, slen, xflags);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

int
linux_fsetxattr (const int fd,
                 const char *attrname,
                 const char *attrvalue,
                 const size_t slen,
                 struct hv *flags)
{
  int ret;
  char *q;
  File_ExtAttr_setflags_t setflags;
  int xflags = 0;

  setflags = File_ExtAttr_flags2setflags(flags);
  switch (setflags)
  {
  case SET_CREATEIFNEEDED: break;
  case SET_CREATE:         xflags |= XATTR_CREATE; break;
  case SET_REPLACE:        xflags |= XATTR_REPLACE; break;
  }

  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = fsetxattr(fd, q, attrvalue, slen, xflags);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

int
linux_getxattr (const char *path,
                const char *attrname,
                void *attrvalue,
                const size_t slen,
                struct hv *flags)
{
  int ret;
  char *q;

  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = getxattr(path, q, attrvalue, slen);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

int
linux_fgetxattr (const int fd,
                 const char *attrname,
                 void *attrvalue,
                 const size_t slen,
                 struct hv *flags)
{
  int ret;
  char *q;

  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = fgetxattr(fd, q, attrvalue, slen);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

int
linux_removexattr (const char *path,
                   const char *attrname,
                   struct hv *flags)
{
  int ret;
  char *q;

  /* XXX: Other flags? */
  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = removexattr(path, q);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

int
linux_fremovexattr (const int fd,
                    const char *attrname,
                    struct hv *flags)
{
  int ret;
  char *q;

  /* XXX: Other flags? */
  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = fremovexattr(fd, q);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
}

ssize_t
linux_listxattr (const char *path,
                 char *buf,
                 const size_t buflen,
                 struct hv *flags)
{
#if 0
  /* XXX: We need some kind of hash returned here: { namespace => attrname } */
  int ret;
  char *q;

  /* XXX: Other flags? */
  q = qualify_attrname(attrname, flags);
  if (q)
  {
    ret = listxattr(path, buf, buflen);
    free(q);
  }
  else
  {
    ret = -1;
    errno = ENOMEM;
  }

  return ret;
#else
  return listxattr(path, buf, buflen);
#endif
}

ssize_t
linux_flistxattr (const int fd,
                  char *buf,
                  const size_t buflen,
                  struct hv *flags)
{
  return flistxattr(fd, buf, buflen);
}

#endif /* EXTATTR_LINUX */
