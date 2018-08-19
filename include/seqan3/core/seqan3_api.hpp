
#pragma once

// #define SEQAN3_AS_LIBRARY 1

#define SEQAN3_EXTERN_TEMPLATE 1

#ifndef SEQAN3_AS_LIBRARY
    #warning("SEQAN3_AS_LIBRARY UNDEFINED")
    #ifndef SEQAN3_API
    #define SEQAN3_API inline
    #endif

    #ifndef SEQAN3_EXTERN_TEMPLATE
    // #undef SEQAN3_EXTERN_TEMPLATE
    #endif

#else
    // #warning("SEQAN3_AS_LIBRARY DEFINED")
    #ifndef SEQAN3_API
    #define SEQAN3_API extern
    #endif

    #ifndef SEQAN3_EXTERN_TEMPLATE
    #define SEQAN3_EXTERN_TEMPLATE 1
    #endif

#endif
