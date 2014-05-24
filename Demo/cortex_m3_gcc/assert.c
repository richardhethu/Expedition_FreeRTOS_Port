#include "basic_io.h"
#include "assert.h"

void vAssertCalled(const char *pcString, long lineNo) {
    panic("%s, %d\r\n", pcString, lineNo);
}
