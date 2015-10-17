*****************************************************************************************************************************
*****************************************************************************************************************************
	
	NAME		:	Deep Ghosh
	GTID		:	903067423
	COURSE	:	CS 6210 FALL 15
	
	README file for PROJECT 1	
	
*****************************************************************************************************************************
*****************************************************************************************************************************

1. LINUX PLATFORM USED:

		-	Ubuntu 14.04 LTS 64 bit OS 

2. HOW IS THE PRE-EMPTIVE SCHEDULER IMPLEMENTED?
		
		- The scheduler is a Round Robin. I have used an array to store all the threads. I have used a pointer to the current thread element to track the thread being processed. Next thread in the list is picked by moving down the array in a round robin fashion. I have an alternate solution with queues implemented. This solution fails for one test case and has been submitted via T-Square.
		- Context switching is used to swap the threads in a round robin fashion.
		
3. HOW TO COMPILE THE CODE?

		- The code has been submitted through the Udacity website. 
		- Make files have not been requested for submission, but can be made available if necessary.
		- The only important thing is that the <gtthread.h>, <gtthread_mutex.c> and <gtthread_sched.c> files must be included to use the GTTHREAD Library
		- Submitted files through TSquare as well.
		
4. HOW TO AVOID DEADLOCK IN PHILOSOPHER DINNING?

		- The easiest way to avoid deadlock in Dining Philosopher is to use Djikstra's method.Following Djikstra's method it is ensured that every philosopher reaches for the fork with the smaller id nearest to them. This ensures that at least the last philosopher in the table will reach for the same fork as the first philosopher in the table. This avoids a deadlock by as the last philosopher will not pick up his other fork till he gets the lower id ed fork. This will allow the philosopher near him to pick up both forks and start eating.
		
5. COMMENTS ON THE PROJECT:

		- The project should extend to include more comprehensive scheduling algorithms.
		- I had a really bad time trying to find out why my code was failing in one single scenario in Udacity. My alternate code passed all the other available test cases on Udacity and in the piazza resource section. It would be really good if the test cases in Udacity could be made available for debugging.

*****************************************************************************************************************************
*****************************************************************************************************************************