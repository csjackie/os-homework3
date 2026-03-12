# user inputs oss[-h][-n][-s][-t][-i][-f]
# oss initializes system clock
# oss initializes message queue
# oss sets up message queue
# oss loops to launch (max s) child processes:
	# fork()
	# exec()
	# output process table
	# output/send oss info to screen/log file
# oss loops:
	# increment clock
	# check to see if child terminated
# once all child processes completed:
# output summary (total number of processes launched, number of times messages were sent from oss)
