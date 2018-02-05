#define A 0
#ifdef A
print("A")
#endif

#define B 4
#ifndef B
print("2")
#endif

#undef B
#ifdef A
#ifndef B
print("3")
#endif
#endif