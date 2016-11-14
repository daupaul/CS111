SHELL = /bin/bash

default: lab0.c
	gcc -g lab0.c -o lab0
check: lab0
	@echo "File123" > test1
	@echo "File number 456" > test2

	@./lab0 --input=test1 --output=outtest
	@diff test1 outtest > /dev/null && echo -e "Passed test 1, please refer to README file for test detail" || echo -e "Failed test 1, please refer to README file for test detail"

	@./lab0 --input=test2 --output=outtest
	@diff test2 outtest > /dev/null && echo -e "Passed test 2, please refer to README file for test detail" || echo -e "Failed test 2, please refer to README file for test detail"

	@(./lab0 --catch --segfault; true) >& seg1
	@(./lab0 --segfault --catch; true) >& seg2
	@diff seg1 seg2 > /dev/null && echo -e "Passed test 3, please refer to README file for test detail" || echo -e "Failed test 3, please refer to README file for test detail"

	@rm -rf *test* seg?
	@echo "Check completed"
clean:
	@rm -rf lab0 *.o *~
	@echo "Directory cleaned"
dist:
	@rm -rf lab0-504451468.tar.gz
	@tar -cf lab0-504451468.tar.gz lab0.c Makefile README backtrace.png breakpoint.png
