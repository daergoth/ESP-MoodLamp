#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H

// Comment out to disable debug mode
#define DEBUG

#ifdef DEBUG 

#define debugLog(x) Serial.print(x);
#define debugLogln(x) Serial.println(x);

#else

#define debugLog(x)
#define debugLogln(x)

#endif

#endif
