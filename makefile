OPTS = -g -Wall


bigmoney: shellmain.c shellexec.o input.o pipe.o infolist.o
	gcc ${OPTS} -o bigmoney.out shellmain.c shellexec.o input.o infolist.o pipe.o

shellexec.o: shellexec.c shellexec.h arraylist.h pipe.h infolist.h
	gcc ${OPTS} -c shellexec.c

input.o: input.c input.h
	gcc ${OPTS} -c input.c

arraylist.o: arraylist.c arraylist.h
	gcc ${OPTS} -c arraylist.c

pipe.o:	pipe.c pipe.h
	gcc ${OPTS} -c pipe.c

infolist.o: infolist.c infolist.h
	gcc ${OPTS} -c infolist.c

clean:
	rm -f *.o
	rm -f bigmoney.out
	rm -f *~
