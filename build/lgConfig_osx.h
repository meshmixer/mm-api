#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wswitch"

#define memcpy_s(a,b,c,d) memcpy(a,c,d)

static void osx_sprintf_s(char *buffer, size_t size, const char * format, ...)
{
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
}
#define sprintf_s osx_sprintf_s

#define lgRASSERT(x)

static void lgDevelBreak()
{
}

namespace rms {
  class BinarySerializer;
}
