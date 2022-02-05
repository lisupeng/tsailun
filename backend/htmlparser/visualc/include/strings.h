/*Dummy file to satisfy source file dependencies on Windows platform*/
#ifdef linux
  // do nothing
#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64)
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  #define inline __inline
#endif
