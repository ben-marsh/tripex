#ifndef __EXP2DLL_H_
#define __EXP2DLL_H_
#pragma once
//#pragma message("including exp2dll.h")
struct mphostent {
	  char *h_name;	      /* official name of host */
	  char **h_aliases;   /* alias list */
	  short  h_addrtype;	  /* host address type	*/
	  short  h_length;	    /* length of	address	*/
	  char **h_addr_list; /* list of addresses	from name server */
};


class Exp2Dll {
public:
    char*           ObjectName;
	char*			FunctionName;
    unsigned long   Function;
    unsigned long   Ordinal;
    Exp2Dll*        Next;
    
	Exp2Dll(char*, char*, unsigned long);
	Exp2Dll(char*, char*, unsigned long,unsigned long);
    Exp2Dll(char*, unsigned long, unsigned long);

    virtual ~Exp2Dll();
};

#endif