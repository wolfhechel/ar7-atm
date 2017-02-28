#ifndef _INC_CPREMAP_C
#define _INC_CPREMAP_C

#ifdef __ADAM2
static inline void osfuncDataCacheHitInvalidate(void *ptr, int Size)
  {
  asm(" cache  17, (%0)" : : "r" (ptr));
  }

static inline void osfuncDataCacheHitWriteback(void *ptr, int Size)
  { 
  asm(" cache  25, (%0)" : : "r" (ptr));
  }

#else
  #define osfuncDataCacheHitInvalidate(MemPtr, Size)   HalDev->OsFunc->DataCacheHitInvalidate(MemPtr, Size)
  #define osfuncDataCacheHitWriteback(MemPtr, Size)    HalDev->OsFunc->DataCacheHitWriteback(MemPtr, Size)
#endif

/*
#define osfuncDataCacheHitInvalidate(ptr, Size) asm(" cache  17, (%0)" : : "r" (ptr))
#define osfuncDataCacheHitWriteback(ptr, Size)  asm(" cache  25, (%0)" : : "r" (ptr))
*/



#endif
