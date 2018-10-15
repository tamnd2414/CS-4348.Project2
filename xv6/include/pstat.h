#ifndef _PSTAT_H
#define _PSTAT_H
#include "param.h"

struct pstat
{
  int inuse[NPROC];
  int tickets[NPROC];
  int pid[NPROC];
  int ticks[NPROC];
};

#endif 
