#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *args[])
{
	struct pstat *st;
	st = malloc(sizeof(*st));
	if(getpinfo(st) == 0)
	{
		printf(1, "First process pid: %d", st->pid[0]);
	}else
		printf(1, "Shit");
	exit();
}
