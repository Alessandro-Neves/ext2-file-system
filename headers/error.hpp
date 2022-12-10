
#ifndef _ERRORX_H_
#define _ERRORX_H_

#include <cstddef>

class Error
{
public:
  const char *message = NULL;
  Error(const char *message)
  {
    this->message = message;
  }
};

class FileManagerInfo
{
public:
  const char *message = NULL;
  FileManagerInfo(const char *message)
  {
    this->message = message;
  }
};

#endif