#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#define BASE 	100

int tot, write = -1;

int
get_num(char *s, int len)
{
	int num = 0, i = 0;
	while (i < len && s[i] >= '0' && s[i] <= '9') {
		num = num * 10 + s[i] - '0';
		i++;
	}
	return num;
}

int
after_char(char *s, int len, char c)
{
	int i = 0;
	while (i < len - 1 && s[i] != c)
		i++;
	if (i < len - 1)
		return i + 1;
	return -1;
}

int
check(char *s, int len)
{
	int id, pos, value;
	switch(s[0]) {
		case 'W':
			id = get_num(s + 6, len - 6);
			pos = after_char(s, len, ' ');
			if (pos == -1) return 0;
			if (s[pos] == 's') {
				if (write != -1) return 0;
				write = id;
				tot += BASE;
			} else if (s[pos] == 'f') {
				if (write != id) return 0;
				write = -1;
			} else return 0;
			break;
		case 'R':
			if (write != -1) return 0;
			id = get_num(s + 6, len - 6);
			pos = after_char(s, len, '[');
			value = after_char(s, len, '=');
			pos = get_num(s + pos, len - pos);
			value = get_num(s + value, len - value);
			if (pos + tot != value)
				return 0;
			break;
		default:
			return 0;
	}
	return 1;
}

int
main()
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int pos = 0;
	int fl = 0;

	fp = fopen("log.txt", "r");

	while ((read = getline(&line, &len, fp)) != -1) {
		if (line[0] == 'S') {
			fl = 1;
			continue;
		}
		if (!fl) continue;
		pos++;
		if (!check(line, len)) {
			printf("LINE %d ERROR!\n%s\n", pos, line);
			return 0;
		}
	}
	printf("PASS\n");
	return 0;
}