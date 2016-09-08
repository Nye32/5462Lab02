

CC = gcc


ALL:FTPC FTPS


FTPC:ftpc.c
	$(CC) ftpc.c -o ftpc


FTPS:ftps.c
	$(CC) ftps.c -o ftps


clean:
	rm ftpc ftps
