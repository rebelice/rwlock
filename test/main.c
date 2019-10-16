#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "inc/rwlock.h"

#define ASIZE 		100
#define BASE 		100

int Array[ASIZE];
rwlock_t lock;
int finished;
int READER_NUM = 5, WRITER_NUM = 1;

void *
writer(void *arg)
{
	int id = *(int *)arg;
	for (int i = 0; i < 5; i++) {
		sleep(0);
		rw_write_lock(&lock);
		printf("Writer%d starts to write\n", id);
		for (int j = 0; j < ASIZE; j++) {
			Array[j] += BASE;
		}
		printf("Writer%d finished\n", id);
		rw_write_unlock(&lock);
	}
	sleep(0);
}

void *
reader(void *arg)
{
	int id = *(int *)arg;
	int loop_start = id * (ASIZE / READER_NUM);
	int loop_end = loop_start + ASIZE / READER_NUM;
	while (!finished) {
		rw_read_lock(&lock);
		for (int i = loop_start; i < loop_end; i++)
			printf("Reader%d: Array[%d]=%d\n", id, i, Array[i]);
		rw_read_unlock(&lock);
		sleep(0);
	}
}

int
char2int(char *s) {
	int len = strlen(s), num = 0;
	for (int i = 0; i < len; i++) {
		if (s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] - '0';
		else return -1;
	}
	return num;
}

void
init(int argc, char *argv[])
{
	if (argc > 2) {
		READER_NUM = char2int(argv[1]);
		WRITER_NUM = char2int(argv[2]);
		if (READER_NUM == -1) READER_NUM = 5;
		if (WRITER_NUM == -1) WRITER_NUM = 1;
	}
	printf("Reader:%d\nWriter:%d\n", READER_NUM, WRITER_NUM);
	finished = 0;
	for (int i = 0; i < ASIZE; i++)
		Array[i] = i;
}

int
main(int argc, char *argv[])
{
	init(argc, argv);
	int x;
	rw_lock_init(&lock);
	pthread_t rthread_pool[READER_NUM], wthread_pool[WRITER_NUM];
	int rid[READER_NUM], wid[WRITER_NUM];

	printf("Start!\n");

	for (int i = 0; i < READER_NUM; i++) {
		rid[i] = i;
		x = pthread_create(&rthread_pool[i], NULL, reader, &rid[i]);
		if (x != 0) {
			printf("Thread creation failed!\n");
			return 0;
		}
	}

	for (int i = 0; i < WRITER_NUM; i++) {
		wid[i] = i;
		x = pthread_create(&wthread_pool[i], NULL, writer, &wid[i]);
		if (x != 0) {
			printf("Thread creation failed!\n");
			return 0;
		}
	}

	for (int i = 0; i < WRITER_NUM; i++) {
		x = pthread_join(wthread_pool[i], NULL);
		if (x != 0) {
			printf("Thread join failed!\n");
			return 0;
		}
		finished = 1;
	}

	for (int i = 0; i < READER_NUM; i++) {
		x = pthread_join(rthread_pool[i], NULL);
		if (x != 0) {
			printf("Thread join failed!\n");
			return 0;
		}
	}

	return 0;
}