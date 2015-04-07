/* 
 * Author: Stephen Burgos
 * Class: CIS 363, Concurrent Programming
 * Professor: Dr. Joe Hoffert
 * Assignment 2, try 3
 * Description: This program counts the number of 3s in an array by updating a global count
 * 		variable, guarded by a mutex, from local count variables in each thread.
 * Statement: I wrote all of this code myself and did not copy any
		code from the internet or another student.
*/

#include <iostream>
#include <random>
#include <ctime>
#include <sys/time.h>
#include <thread>
#include <mutex>

typedef unsigned long long timestamp_t;

std::mutex myMutex;
const int THREADS = 8; // Number of threads
int count;
const int LENGTH = 5000000;
int myArray[LENGTH];
const int MICROSECONDS_PER_SECOND = 1000000.0L;
int private_count[THREADS];

timestamp_t get_timestamp()
{
	// Precondition: None
	// Postcondition: The current time is returned
	struct timeval now;
	::gettimeofday (&now, NULL);
	return now.tv_usec + static_cast<timestamp_t>(now.tv_sec) * MICROSECONDS_PER_SECOND;
}

void count3s_thread(int id, int * myArray)
{
	// Precondition: None
	// Postcondition: The number of 3s in the array is counted
	// Compute the portion of the array that this thread should work upon
	int searchFor = 3;
	int lengthPerThread = LENGTH / THREADS;
	int start = id * lengthPerThread;
	for(int i = start; i < start + lengthPerThread; ++i)
	{
		if(myArray[i] == searchFor)
		{
			++private_count[id];
		}
	}
	myMutex.lock();
	count += private_count[id];
	myMutex.unlock();
}

void create_array(int * myArray)
{
	// Precondition: None
	// Postcondition: An array of random numbers has been generated
	int generator_min_cint = 2;
	int generator_max_cint = 3;
	std::random_device randomDevice;
	std::mt19937_64 numberGenerated_int(randomDevice());
	std::uniform_int_distribution<> distr(generator_min_cint, generator_max_cint);
	for (int index = 0; index < LENGTH; ++index)
	{
		myArray[index] = distr(numberGenerated_int);
	}
}

void calc_timing(timestamp_t time0, timestamp_t time1)
{
	// Precondition: None
	// Postcondition: The time taken for counting has been printed
	double secs = (time1 - time0) / (MICROSECONDS_PER_SECOND * 1.0);
	long usecs = time1 - time0;
	std::cout << "Time taken (secs): " << secs << ", usecs: " << usecs << std::endl;
}

void print_message()
{
	// Precondition: None
	// Postcondition: The number of 3s and the length of the array has been printed
	std::cout << "The total number of 3s in the array is: " << count << "." << std::endl;
	std::cout << "Size of array: " << LENGTH << "." << std::endl;
}

int main()
{
	create_array(myArray);

	// Count the number of 3s
	std::thread addition_threads[THREADS];
	timestamp_t time0 = get_timestamp();
	for(int id = 0; id < THREADS; ++id)
	{
		addition_threads[id] = std::thread(count3s_thread, id, myArray);
	}
	for (int id = 0; id < THREADS; ++id)
	{
		addition_threads[id].join();
	}
	timestamp_t time1 = get_timestamp();

	print_message();
	calc_timing(time0, time1);
	
	return 0;
}