// Called from entry.S to get us going.
// entry.S already took care of defining envs, pages, vpd, and vpt.

#include <inc/lib.h>

extern void umain(int argc, char **argv);

const volatile struct Env *thisenv;
const char *binaryname = "<unknown>";

void
libmain(int argc, char **argv)
{
    // set thisenv to point at our Env structure in envs[].
    // LAB 3: Your code here.
    // reference inc/env.h
    envid_t eid = 0;//sys_getenvid();
    thisenv = (struct Env *) envs + ENVX(eid);

    // save the name of the program so that panic() can use it
    if (argc > 0)
	binaryname = argv[0];

    // call user main routine
    umain(argc, argv);

    // exit gracefully
    //exit();
}

