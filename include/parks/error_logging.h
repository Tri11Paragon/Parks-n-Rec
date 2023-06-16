//
// Created by brett on 6/13/23.
//

#ifndef PARKSNREC_ERROR_LOGGING_H
#define PARKSNREC_ERROR_LOGGING_H

#include <blt/std/logging.h>
#ifdef __linux__
    #include <execinfo.h>
#endif
// TODO: add to BLT

namespace blt::logging {
    
    /**
     * prints the current stack trace
     * @param remove number of calls to remove from head. Defaults to remove the call to this function.
     * @param max_up number of calls to lookup
     */
     template<int max_up = 20>
    inline void printStackTrace(int remove = 2) {
#ifdef __linux__
        void *array[max_up];
        int size;
        
        // get void*'s for all entries on the stack
        size = backtrace(array, 10);
        auto** names = backtrace_symbols(array, size);
        BLT_ERROR("Stacktrace:");
        for (int i = remove; i < size; i++)
            BLT_ERROR("%d. (%s)", i, names[i]);
        free(names);
#else
        BLT_WARN("Stacktrace not implemented for platforms other than linux!");
#endif
    }

    inline void printErrorLog(const char* what){
        // TODO: stacktrace
        BLT_ERROR("--------{Begin Error Log}--------");
        BLT_ERROR("Error Desc: %s", what);
        printStackTrace();
        BLT_ERROR("--------{ End Error Log }--------");
    }

}

#endif //PARKSNREC_ERROR_LOGGING_H
