#ifndef VERSION_H
#define VERSION_H

#ifndef QT_STRINGIFY
#define QT_STRINGIFY2(x) #x
#define QT_STRINGIFY(x) QT_STRINGIFY2(x)
#endif

//Version
#define MAJORVERSION 1
#define MINORVERSION 5
#define PATCHVERSION 0
#define PROGRAMVERSION QT_STRINGIFY(MAJORVERSION.MINORVERSION.PATCHVERSION)
#define PROGRAMVERSION_NUMBER MAJORVERSION,MINORVERSION,PATCHVERSION
#define GUID "{8F9AD676-E4E5-4EAE-AEE4-34571E76729C}"

#endif // VERSION_H
