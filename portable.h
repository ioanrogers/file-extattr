#ifndef EXTATTR_PORTABLE_H
#define EXTATTR_PORTABLE_H

/* OS detection */
#include "extattr_os.h"

struct hv;

/* Portable extattr functions */
static inline int
portable_setxattr (const char *path,
                   const char *attrname,
                   const void *attrvalue,
                   const size_t slen,
                   struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return setxattr(path, attrname, attrvalue, slen, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  return bsd_setxattr(path, attrname, attrvalue, slen /* XXX: flags? */);
#elif defined(EXTATTR_SOLARIS)
  return solaris_setxattr(path, attrname, attrvalue, slen, 0 /* XXX: flags? */);
#else
  return linux_setxattr(path, attrname, attrvalue, slen, flags);
#endif
}

static inline int
portable_fsetxattr (const int fd,
                    const char *attrname,
                    const void *attrvalue,
                    const size_t slen,
                    struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return fsetxattr(fd, attrname, attrvalue, slen, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  return bsd_fsetxattr(fd, attrname, attrvalue, slen /* XXX: flags? */);
#elif defined(EXTATTR_SOLARIS)
  return solaris_fsetxattr(fd, attrname, attrvalue, slen, 0 /* XXX: flags? */);
#else
  return linux_fsetxattr(fd, attrname, attrvalue, slen, flags);
#endif
}

static inline int
portable_getxattr (const char *path,
                   const char *attrname,
                   void *attrvalue,
                   const size_t slen,
                   struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return getxattr(path, attrname, attrvalue, slen, 0, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  /* XXX: flags? Namespace? */
  return extattr_get_file(path, EXTATTR_NAMESPACE_USER, attrname, attrvalue, slen);
#elif defined(EXTATTR_SOLARIS)
  return solaris_getxattr(path, attrname, attrvalue, slen /* XXX: flags? */);
#else
  return linux_getxattr(path, attrname, attrvalue, slen, flags);
#endif
}

static inline int
portable_fgetxattr (const int fd,
                    const char *attrname,
                    void *attrvalue,
                    const size_t slen,
                    struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return fgetxattr(fd, attrname, attrvalue, slen, 0, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  /* XXX: flags? Namespace? */
  return extattr_get_fd(fd, EXTATTR_NAMESPACE_USER, attrname, attrvalue, slen);
#elif defined(EXTATTR_SOLARIS)
  return solaris_fgetxattr(fd, attrname, attrvalue, slen /* XXX: flags? */);
#else
  return linux_fgetxattr(fd, attrname, attrvalue, slen, flags);
#endif
}

static inline ssize_t
portable_lenxattr (const char *path, const char *attrname, struct hv *flags)
{
#ifdef BSD
  /* XXX: flags? Namespace? */
  return extattr_get_file(path, EXTATTR_NAMESPACE_USER, attrname, NULL, 0);
#else
  /* XXX: Can BSD use this too? Maybe once namespacing sorted. */
  return portable_getxattr(path, attrname, NULL, 0, flags);
#endif
}

static inline int
portable_flenxattr (int fd, const char *attrname, struct hv *flags)
{
#ifdef BSD
  /* XXX: flags? Namespace? */
  return extattr_get_fd(fd, EXTATTR_NAMESPACE_USER, attrname, NULL, 0);
#else
  /* XXX: Can BSD use this too? Maybe once namespacing sorted. */
  return portable_fgetxattr(fd, attrname, NULL, 0, flags);
#endif
}

static inline int
portable_removexattr (const char *path, const char *name, struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return removexattr(path, name, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  /* XXX: flags? Namespace? */
  return extattr_delete_file(path, EXTATTR_NAMESPACE_USER, name);
#elif defined(EXTATTR_SOLARIS)
  return solaris_removexattr(path, name /* XXX: flags? */);
#else
  return linux_removexattr(path, name, flags);
#endif
}

static inline int
portable_fremovexattr (const int fd, const char *name, struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return fremovexattr(fd, name, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  /* XXX: flags? Namespace? */
  return extattr_delete_fd(fd, EXTATTR_NAMESPACE_USER, name);
#elif defined(EXTATTR_SOLARIS)
  return solaris_fremovexattr(fd, name /* XXX: flags? */);
#else
  return linux_fremovexattr(fd, name, flags);
#endif
}

static inline int
portable_listxattr(const char *path,
                   char *buf,
                   const size_t slen,
                   struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return listxattr(path, buf, slen, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  return bsd_listxattr(path, buf, slen /* XXX: flags? */);
#elif defined(EXTATTR_SOLARIS)
  return solaris_listxattr(path, buf, slen /* XXX: flags? */);
#else
  return linux_listxattr(path, buf, slen, flags);
#endif
}

static inline int
portable_flistxattr(const int fd,
                    char *buf,
                    const size_t slen,
                    struct hv *flags)
{
#ifdef EXTATTR_MACOSX
  return flistxattr(fd, buf, slen, 0 /* XXX: flags? */);
#elif defined(EXTATTR_BSD)
  return bsd_flistxattr(fd, buf, slen /* XXX: flags? */);
#elif defined(EXTATTR_SOLARIS)
  return solaris_flistxattr(fd, buf, slen /* XXX: flags? */);
#else
  return linux_flistxattr(fd, buf, slen, flags);
#endif
}

#endif /* EXTATTR_PORTABLE_H */
