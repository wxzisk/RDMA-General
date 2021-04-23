#include <assert.h>
#include <errno.h>
#include <numaif.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <infiniband/verbs.h>
#include <libmemcached/memcached.h>
#include <malloc.h>
#include <time.h>

int main()
{
	int shm_key;
	int size;
	int shmid;
	printf("testing shmget()\n");

	shmid = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666 | SHM_HUGETLB);
	if(shmid == -1)
	{
		switch(errno){
			case EACCES:
				printf("malloc error: Insufficient permissions\n");
				break;
			case EEXIT:
				printf("malloc error: Already exist\n");
				break;
			case EINVAL:
				printf("malloc error: SHMMAX/SHMIN mismatch\n");
				break;
			case ENOMEM:
				printf("malloc error: Insufficient memory\n");
				break;
			default:
				printf("malloc error: a wild SHM error\n");
				break;
		}
		assert(false);
	}
	void* buf = shmat(shmid, NULL, 0);
	if(buf == NULL){
		printf("malloc error: shmat() failed for key\n");
	}
	const unsigned long nodemask = 1;
	int ret = mbind(buf , size, MPOL_BIND, &nodemask, 32, 0);
	if(ret != 0){
		printf("malloc error: mbind() failed for key\n");
	}


	shmid = shmget(shm_key, 0 , 0);
	if(shmid == -1){
		switch(errno){
			case EACCES:
				printf("free erro: Insufficient permissions\n");
				break;
			case ENOENT:
				printf("free error: No such SHM key.\n");
				break;
			default:
				printf("free error: A wild SHM error\n");
				break;
		}
		assert(false);
	}

	ret = shmctl(shmid, IPC_RMID, NULL);
	if(ret != 0)
	{
		printf("SHM free error: shctl() failed for key %d\n", shm_key);
	}

	ret = shmdt(buf);
	if(ret != 0)
	{
		printf("SHM free error : shmdt() failed for key %d\n", shm_key );
	}
	return;
}