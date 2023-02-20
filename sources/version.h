#ifndef VERSION_H
#define VERSION_H

#ifndef QT_STRINGIFY
#define QT_STRINGIFY2(x) #x
#define QT_STRINGIFY(x) QT_STRINGIFY2(x)
#endif

//Version
#define MAJORVERSION 1
#define MINORVERSION 3
#define PATCHVERSION 1
#define PROGRAMVERSION QT_STRINGIFY(MAJORVERSION.MINORVERSION.PATCHVERSION)
#define PROGRAMVERSION_NUMBER MAJORVERSION,MINORVERSION,PATCHVERSION

#endif // VERSION_H
