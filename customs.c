/*
*	Alec Uyematsu
*	CS 305 HW4
*	10/31/2020
*
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>


/**
 * customs.c
 *
 * This program simulates how long it will take for people to clear
 * customs in a United States airport.
 *
 */

#define TRUE ('/'/'/')   // a silly way to say 1
#define FALSE ('^'^'^')  // a silly way to say 0


//This struct describes a single node in a linked list that is used in
//this program to simulate a group waiting in line to talk to a
//customs agent.  
//
typedef struct group_struct group;
struct group_struct {
    int adults;  // how many adults are in the group
    int children; // how many children are in the group
    int usa;  // TRUE if native, FALSE otherwise
    group *next;
    group *prev;
};

//This struct is the head of the queue.  It contains information about
//the customs agent that is serving this group of travelers.
typedef struct agent_struct {
    int timecard; //minutes this agent has worked so far
    int avail;  //minutes until this agent will be available to serve the next group
    
    group *head;
    group *tail;
} agent;

// This struct stores the extra data for "Part 2: Analysis & Optimization"
typedef struct stats_struct {
    int total_time;    // Total time worked by all agents (minutes)
    int total_payroll; // Payroll cost for all agents (dollars)
    int avg_wait_time; // Average wait time over all groups (minutes)
    int max_wait_time; // Max wait time over all groups (minutes)
} stats;

/**
 * create_group
 *
 * creates a new group of travelers to pass through customs with
 * random values
 *
 * Important:  caller must deallocate the memory
 *
 * Return:
 *   the newly created group
 */
group *create_group() {
    group *newbie = (group *)malloc(sizeof(group));

    //These are random values that are supposed to be some
    //approximation of what is typically present

    //adults:  at least 1, no more than 3
    newbie->adults = 1 + rand() % 3;

    //children: usually 0, occasionally several
    newbie->children = (rand() % 4) + (rand() % 4) - 2;
    if (newbie->children < 0) newbie->children = 0;

    //citizen of USA 80% of the time
    newbie->usa = ((rand() % 5) == 0) ? FALSE : TRUE;

    //empty list atm
    newbie->next = NULL;
    newbie->prev = NULL;
    
    return newbie;
}//create_group

/**
 * create_agent
 *
 * creates a new customs agent which is also the head of a queue
 *
 * Important:  caller must deallocate the memory
 *
 * Return:
 *   the newly created agent
 */
agent *create_agent() {
    agent *newbie = (agent *)malloc(sizeof(agent));
    newbie->timecard = 0;
    newbie->avail = 0;
    newbie->head = NULL;
    newbie->tail = NULL;
    return newbie;    
}//create_agent

/**
  proc_time
 *
 * estimates how much time it will take to process a group
 *
 * Parameters:
 *   grp - group to estimate for
 *
 * Return:
 *   estimated time in minutes 
 */
int proc_time(group *grp) {
    //one minutes for each adult
    int result = grp->adults;

    //foreign adults take twice as long
    if (! grp->usa) {
        result *= 2;
    }
    
    //half minute for each child (min: 1 min)
    result += (1 + grp->children) / 2;

    return result;
}//proc_time



/**
 * enqueue
 *
 * adds a new group to an agent's line
 *
 * Parameters:
 *   agt - the agent to add to
 *   grp - the group to add
 */
void enqueue(agent *agt, group *grp) {
	
	if(agt->tail != NULL){
		agt->tail->next = grp;
	}
	agt->tail = grp;
	if(agt->head == NULL){
		agt->head = grp;
	}
}//enqueue

/**
 * dequeue
 *
 * removes the next group from an agent's queue
 *
 * Parameters:
 *   agt - the agent to remove next group from
 *
 * Returns:
 *   the group that was removed or NULL for empty/bad queue
 */
group *dequeue(agent *agt) {
  
	group *grp;
	if(agt->head == NULL){
		return grp;
	}
	grp = agt->head;
	agt->head = agt->head->next;
	if(agt->head == NULL){
		agt->tail = NULL;
	}
    return grp;
}//dequeue

/**
 * calc_time
 *
 * calculates the time it takes a given set of agents to process all
 * the groups in their queues
 *
 * Parameters:
 *   num_agents - number of agents 
 *   agents - the array of agents
 *
 * Returns:
 *   Total time (in minutes) it takes for the agents to process all
 *   the groups
 */
stats *calc_time(int num_agents, agent **agents) {
   //initializing variables
   int max_time = 0;
   int time1 = 0;
   int average_wait_time = 0;
   int max_wait_time = 0;
   int total_work_time = 0;
   int counter = 0;
   int last_wait_time = 0;
   int all_wait_time = 0;
	//bad parameters
	if(num_agents == 0 || NULL){
		return 0;
    	}
	if(*agents == NULL){
		return 0;
	}
	else{
	   //for llop goes through all agents
	   for(int i = 0; i < num_agents; i++){	
		time1 = 0; //resets time1
		//repeats while the head of the queue exists
		while(agents[i]->head != NULL){
			all_wait_time += proc_time(agents[i]->head); //adds all the wait time without the reset		
			time1 += proc_time(agents[i]->head); //adds up total time for agent[i]
			last_wait_time = proc_time(agents[i]->head);//gets the last wait time
			*dequeue(agents[i]);
			counter++; //counts how many times while loop runs
		}	
			free(agents[i]->head);

			//if the time just taken is greater than the max replace 
			if(time1 > max_time){
				max_time  = time1;		
			}	
				
	   }
	int total_pay = 0;
	int remaining_time = 0;
	//payroll
	if(max_time < 480){ //480 minutes = 8 hours
		total_pay = .33*max_time*num_agents; //$20/60 minutes
	}
	//for overtime
	else{
		remaining_time = max_time - 480;
		total_pay = 160*num_agents + .5 *remaining_time*num_agents; //160 = $20*8 hours, .5 = $30/60mins
	}
	//finding other stats 
	total_work_time = max_time*num_agents; 
	max_wait_time = max_time-last_wait_time;
	average_wait_time = (all_wait_time - last_wait_time)/counter;

	//airport_stats struct
	stats *airport_stats;
	airport_stats->total_time = max_time;
	airport_stats->total_payroll = total_pay;
	airport_stats->avg_wait_time = average_wait_time;
	airport_stats->max_wait_time = max_wait_time;
        return airport_stats;
	}
	//return airport_stats;	
}//calc_time


//let's do this!
void main(int argc, char* argv[]) {

    srand(time(0));
    int num_agents = 10;
    int num_groups = 1000;

    //Create the agents
    agent **agents = (agent **)malloc(num_agents * sizeof(agent));
    for(int i = 0; i < num_agents; ++i) {
        agents[i] = create_agent();
    }
    
    //create the groups and add to the agents lines
    for(int i = 0; i < num_groups; ++i) {
        enqueue(agents[ i % num_agents ], create_group());
    }

    //report time elapsed
    /* int elapsed = calc_time(num_agents, agents);
    printf("time elapsed: %d hours %d minutes \n",
           elapsed / 60, elapsed % 60);
    */
    stats *total_stats = calc_time(num_agents, agents);
    printf("total time: %d minutes\n", total_stats->total_time);
    printf("total payroll: %d dollars\n", total_stats->total_payroll);
    printf("average wait time: %d minutes\n", total_stats->avg_wait_time);
    printf("max wait time: %d minutes \n", total_stats->max_wait_time);

    //report stats
	
    //cleanup
    for(int i = 0; i < num_agents; ++i) {
        free(agents[i]);
    }
    free(agents);
    
    
}//main
