lab4: part1.c part2.c
	@gcc -g -lmraa -Wall part1.c -o part1 -lm
	@gcc -g -lmraa -Wall part2.c -o part2 -lm -lpthread
clean:
	@rm log_1.txt
	@rm log_2.txt
	@rm part1
	@rm part2
test:
	@./part1
	@./part2
tarball:
	@tar -czf lab4-504451468.tar.gz part1.c part2.c log_1.txt log_2.txt Makefile
