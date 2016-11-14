

default: lab2_add.c lab2_list.c SortedList.c
	@gcc -o lab2_add -g lab2_add.c -pthread -lrt -Wall
	@gcc -o lab2_list -g lab2_list.c SortedList.c -pthread -lrt -Wall
build: lab2_add.c lab2_list.c SortedList.c
	@gcc -o lab2_add -g lab2_add.c -pthread -lrt -Wall
	@gcc -o lab2_list -g lab2_list.c SortedList.c -pthread -lrt -Wall
graphs:
	@gnuplot lab2b.gp
profile: lab2_list Makefile
	-rm -f ./raw.gperf
	LD_PRELOAD=/u/ee/ugrad/daupo/CS111/lab2a/lib/libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	./pprof --text ./lab2_list ./raw.gperf > profile.gperf
	./pprof --list=thread_func ./lab2_list ./raw.gperf >> profile.gperf
	rm -f ./raw.gperf
clean:
	@rm -rf lab2_add
	@rm -rf lab2_list
	@rm -rf *.o
tests: lab2_add lab_2b_list.csv lab2_list
	-./lab2_add --threads=1 --iterations=100 >>lab_2b_list.csv
	-./lab2_add --threads=1 --iterations=1000 >>lab_2b_list.csv
	-./lab2_add --threads=1 --iterations=10000 >>lab_2b_list.csv
	-./lab2_add --threads=1 --iterations=100000 >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=100 >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=1000 >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=10000 >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=100000 >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=100 >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=1000 >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=10000 >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=100000 >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=100 >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=1000 >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=10000 >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=100000 >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=100 >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=1000 >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=10000 >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=100000 >>lab_2b_list.csv
	
	-./lab2_add --threads=1 --iterations=10000 --sync=m >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=10000 --sync=m >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=10000 --sync=m >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=10000 --sync=m >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=10000 --sync=m >>lab_2b_list.csv
	-./lab2_add --threads=1 --iterations=10000 --sync=s >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=10000 --sync=s >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=10000 --sync=s >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=10000 --sync=s >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=10000 --sync=s >>lab_2b_list.csv
	-./lab2_add --threads=1 --iterations=10000 --sync=c >>lab_2b_list.csv
	-./lab2_add --threads=2 --iterations=10000 --sync=c >>lab_2b_list.csv
	-./lab2_add --threads=4 --iterations=10000 --sync=c >>lab_2b_list.csv
	-./lab2_add --threads=8 --iterations=10000 --sync=c >>lab_2b_list.csv
	-./lab2_add --threads=12 --iterations=10000 --sync=c >>lab_2b_list.csv
	# generate time per operation vs number of iterations
	-./lab2_list --threads=1 --iterations=10 >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=100 >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=10000 >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=20000 >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=10 >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=100 >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=10 >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=100 >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=10 >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=100 >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10 >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=100 >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 >> lab_2b_list.csv

	# generate insert conflicts
	-./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab_2b_list.csv
	
	# measure the performance
	-./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m  >> lab_2b_list.csv

	-./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s  >> lab_2b_list.csv

	-./lab2_list --threads=1 --iterations=1000 --list=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=1 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=1000 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=24 --iterations=1000 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=4 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=8 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=16 --sync=m >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=16 --sync=m >> lab_2b_list.csv

	-./lab2_list --threads=1 --iterations=1000 --list=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=1 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=16 --iterations=1000 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=24 --iterations=1000 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=4 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=8 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=1 --iterations=1000 --list=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=2 --iterations=1000 --list=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=4 --iterations=1000 --list=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=8 --iterations=1000 --list=16 --sync=s >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --list=16 --sync=s >> lab_2b_list.csv

tarball:
	@rm -rf lab2b-504451468.tar.gz
	@tar -zcf lab2b-504451468.tar.gz *.c Makefile README.txt *.png *.h *.csv *.gperf lab2b.gp
