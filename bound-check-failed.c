#include <stdio.h>
#include <stdlib.h>

#include <mqueue.h>

#define domain_count 8

typedef int mqt_t;

struct ipc_thread_info {
	mqt_t id;
};

struct ipc_thread_info bla[8] = {
	{0},
	{1},
};

int get_domain(mqt_t mq)
{
	int i = 0;
	while (i++ < domain_count) {
		printf("%d\n", bla[i].id);
		if (bla[i].id == mq) {
			return i;
		}
	}
	return 0;
}

int main()
{
	get_domain(11);
}
