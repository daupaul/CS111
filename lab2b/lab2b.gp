
# general plot parameters
set terminal png
set datafile separator ","

set title "Lab2b_1 Throughputs vs threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughputs"
set logscale y 10
set output 'lab2b_1.png'
set key left top

plot \
     "< grep add-m lab_2b_list.csv" using ($2):(1000000000/($6)) \
	title 'mutex add' with linespoints lc rgb 'green', \
     "< grep add-s lab_2b_list.csv" using ($2):(1000000000/($6)) \
	title 'spin add' with linespoints lc rgb 'red',\
	 "< grep list-none-m,[0-9]*,1000,1, lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list' with linespoints lc rgb 'blue',\
     "< grep list-none-s,[0-9]*,1000,1, lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin list' with linespoints lc rgb 'orange'

set title "Lab2b_2: Wait for lock time vs cost per operation"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Aveage time/operation(ns)"
set logscale y 10
set output 'lab2b_2.png'
set key left top

plot \
     "< grep list-none-m,[1,2,4,8][4,6]*,1000,1, lab_2b_list.csv" using ($2):($7) \
	title 'cost per op' with linespoints lc rgb 'blue',\
     "< grep list-none-m,[1,2,4,8][4,6]*,1000,1, lab_2b_list.csv" using ($2):($8) \
	title 'wait time' with linespoints lc rgb 'red'

set title "Lab2b_3: Correct partitioned lists vs threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Aveage time/operation(ns)"
set logscale y 10
set output 'lab2b_3.png'
set key left top

plot \
     "< grep list-id-none lab_2b_list.csv" using ($2):($7) \
	title 'not protected' with points lc rgb 'blue',\
     "< grep list-id-m lab_2b_list.csv" using ($2):($7) \
	title 'mutex' with points lc rgb 'red',\
	 "< grep list-id-s lab_2b_list.csv" using ($2):($7) \
	title 'spin' with points lc rgb 'green'

set title "Lab2b_4 Mutex throughputs vs threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughputs"
set logscale y 10
set output 'lab2b_4.png'
set key left top

plot \
     "< grep list-none-m,[1,2,4,8]2*,1000,1, lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'green', \
     "< grep list-none-m,[0-9]*,1000,4 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'red',\
	 "< grep list-none-m,[0-9]*,1000,8 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue',\
     "< grep list-none-m,[0-9]*,1000,16 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'orange'

set title "Lab2b_5  Spin throughputs vs threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughputs"
set logscale y 10
set output 'lab2b_5.png'
set key left top

plot \
     "< grep list-none-s,[1,2,4,8]2*,1000,1, lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'green', \
     "< grep list-none-s,[0-9]*,1000,4 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'red',\
	 "< grep list-none-s,[0-9]*,1000,8 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue',\
     "< grep list-none-s,[0-9]*,1000,16 lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'orange'