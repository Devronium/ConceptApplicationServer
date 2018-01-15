#ifndef __BUILTINS_H
#define __BUILTINS_H

void *BUILTINADDR(void *PIF, const char *name, unsigned char *is_private);
void BUILTININIT(void *PIF);
void BUILTINOBJECTS(void *PIF, const char *classname);
void BUILTINDONE();

#endif
