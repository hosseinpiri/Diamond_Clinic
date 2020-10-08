 #include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "random.c"  // for generate random numbers we can reference our random.c file
#include "DiamondClinic_Code_V5.h"



FILE* output_file, * clinic_file, * elevator_file; 
int dummy;

/********************************************************************************************
Notes:
Current assumptions (some of which we'll want to improve later):
- elevators move at a constant velocity at all time; we do not model acceleration/deceleration right now. 
********************************************************************************************/
int main()
{
	int i, j, keepgoing, repnum, elevator_avail, floor_this_person_is_going_to, elevator_index;
	double minutes_before_appt_time, unif;
	event next_event;
	person next_in_Line;
	person next_in_elevator;


	// This seeds the Mersenne Twister Random Number Generator
	unsigned long long init[4] = { 0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL }, length = 4;
	init_by_array64(init, length);
	
	Open_and_Read_Files();

	for (repnum = 0; repnum < REPS; repnum++)
  	{
    Initialize_Rep();
	Load_Lobby_Arrivals();  //this includes patients, staff, and doctors
    keepgoing=1;
	a=0;
	b=0;
	c=0;
	t=0;
	l=0;
	kj=0;
	cou=0;
	coun=0;
	var=0;
	

		while (keepgoing)

		
        //int ii ;
		//for (ii=0;ii<0;ii++)
		{	


			next_event = event_head->Event; // going to check each event one by one
			Remove_Event(&event_head);

			if (num_events_on_calendar == 0) //this indicates this simulation replication is over
				keepgoing = 0;

			if (next_event.event_type == CLINIC_DEPARTURE)
			{	totl++;

				elevator_avail = Elevator_Available(elevators, people[next_event.entity_type][next_event.entity_index].to_floor); // when the patient is comming back to the LOBBY, if there is an elevator available in his level
				if (elevator_avail) //this means someone arrives to an idle elevator at their floor.
				{
					elevator_index = elevator_avail - 1;
					elevators[elevator_index].idle = 0;
					elevators[elevator_index].num_people++;
					elevators[elevator_index].direction = DOWN; //assume now that we are just going down to lobby
					floor_this_person_is_going_to = 0;
					elevators[elevator_index].floor_to[floor_this_person_is_going_to] = 1;
					elevators[elevator_index].next_floor = floor_this_person_is_going_to;
					num_in_line_from_clinic[people[next_event.entity_type][next_event.entity_index].to_floor] --;
					elevators[elevator_index].idle = 1; //back to idle condition
					cou ++;
				}
				
				else // if there is no elevator in this level
					{	
					Load_Event_Person(&hall_head[DOWN][people[next_event.entity_type][next_event.entity_index].to_floor],next_event.time, next_event.entity_type, next_event.entity_index); // add this person to wait list in that level
					num_in_line_from_clinic[people[next_event.entity_type][next_event.entity_index].to_floor] ++; // this indicator is capturing length of queue in each level
					for (i = 0; i < NUM_ELEVATORS; i++) // check elevator's distance from this person. We split this into different scenarios
					    {if (elevators[i].next_floor>=people[next_event.entity_type][next_event.entity_index].to_floor) // when the elevator is in the higher level compare to the person
                            {travel_time = elevator_travel_time_per_floor *(-1) *(people[next_event.entity_type][next_event.entity_index].to_floor-elevators[i].next_floor);
							elevators[i].time_to_reach = elevators[i].elevator_clock + elevators[i].elevator_time[1] + elevators[i].elevator_time[0] + travel_time - next_event.time;}
                        if ((elevators[i].next_floor<people[next_event.entity_type][next_event.entity_index].to_floor)&&(elevators[i].idle==0)) //when the elevator is in the lower level compare to the person and its not idle
						    {travel_time = elevator_travel_time_per_floor * (people[next_event.entity_type][next_event.entity_index].to_floor+elevators[i].next_floor);
                        	elevators[i].time_to_reach = elevators[i].elevator_clock + elevators[i].elevator_time[1] + elevators[i].elevator_time[0] + travel_time - next_event.time;}//when the elevator is in the lower level compare to the person and its idle
                        if ((elevators[i].next_floor<people[next_event.entity_type][next_event.entity_index].to_floor)&&(elevators[i].idle==1))
                            {travel_time = elevator_travel_time_per_floor *(people[next_event.entity_type][next_event.entity_index].to_floor-elevators[i].next_floor);
                            elevators[i].time_to_reach = elevators[i].elevator_time[1] + elevators[i].elevator_time[0]+travel_time; // this is the time that it takes for each elevator to take the person
                            }

                        }

					t++;
					}
					wait_time = 999; // initialize wait_time to something big
					for (int k=0;k<NUM_ELEVATORS;k++)
					{wait_time = min(round(wait_time),round(elevators[k].time_to_reach)); //find the shortest wait time
					}
					for (int k=0;k<NUM_ELEVATORS;k++)
						{ if (round(wait_time) == round(elevators[k].time_to_reach)) // find the elevator k with the shortest wait time for this specific person
							

						
						{	b++;
							//elevators[k].elevator_going_to = people[next_event.entity_type][next_event.entity_index].to_floor;
						// if elevator k is in the higher floor compare to the person we dont do anything because we know that that elevator have the ability to come down and take the person in the middle of its way
						if (elevators[k].next_floor < people[next_event.entity_type][next_event.entity_index].to_floor) // if elevator k is in the lower floor compare to the person 
							{l++;
                            if (elevators[k].elevator_going_to<people[next_event.entity_type][next_event.entity_index].to_floor)
							    {elevators[k].elevator_going_to = people[next_event.entity_type][next_event.entity_index].to_floor;} // we use ELEVATOR_GOING_TO to tell that elevator that there is a person in TO_FLOOR that you need to take care of.
                            if (elevators[k].idle==1) // if elevator is idle and in lower level we need to send it to the person right away.
								{a++;
                               elevators[k].next_floor = elevators[k].elevator_going_to; 
                                elevators[k].elevator_clock = next_event.time;
                                elevators[k].elevator_up_time += elevators[k].elevator_time[0];
                                elevators[k].elevator_down_time += elevators[k].elevator_time[1];
                                elevators[k].elevator_time[0]=0; //reset elevator wait time when there is nobody in the waiting list
                                elevators[k].elevator_time[1]=0;
                               elevators[k].elevator_time[0] += (travel_time + DOOR_TIME);
							   
                             //int story=20 ;
                             //int elevators[i].counter=0;
                               elevators[k].idle=0;
                               people[next_event.entity_type][next_event.entity_index].elevator_ind = k;
                               Load_Event(&event_head,elevators[k].elevator_clock+elevators[k].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list. this means that elevator is in the level of the person and ready to move in DOWN direction and take that person
                                 
								} 
                               
							}
					break;	}
					}
				}

			if (next_event.event_type == PERSON_ARRIVES_LOBBY)
			{
				//if there is an elevator waiting open, then get in it; othwerise, have to wait
				elevator_avail = Elevator_Available(elevators, LOBBY); //returns elevator index + 1 if elevator is avail
				if (elevator_avail)  //this means someone arrives to an idle elevator at their floor which is Lobby.
					{elevator_index= elevator_avail - 1; // change to elevator_index which is from 0 to 4;
					Load_Event_Elevator(&elevator_head[elevator_index],next_event.time, next_event.entity_type, next_event.entity_index,people[next_event.entity_type][next_event.entity_index].to_floor); // add person to the elevator list
					elevators[elevator_index].num_people++;

					elevators[elevator_index].elevator_up_time += elevators[elevator_index].elevator_time[0]; //hp: We need to take care of last 4 people as well. I am still thinking moving this chunk to the end part of this chunk might be better.   // to track up time 
					elevators[elevator_index].elevator_down_time += elevators[elevator_index].elevator_time[1]; // to track down time
					elevators[elevator_index].elevator_time[0]=0; //reset elevator wait time when there is nobody in the waiting list . This will help us track time for each elevator each time it becomes idle and someone enter it
					elevators[elevator_index].elevator_time[1]=0;
					coun++;
					//hp: This scenario takes care of the case when this person is the last one who arrives lobby, after whom the lobby will be empty. 
					if (event_head->next->Event.event_type !=1) //hp: don't we need to check timing of this event?
						{for (w=0;w<NUM_ELEVATORS;w++)
							{if ((elevators[w].num_people!=0)&&(elevators[w].next_floor==LOBBY))
								{
                        			elevators[w].ultimate_number = elevators[w].num_people; // we use this ULTIMATE_NUMBER to track number of people in the elevator
									elevators[w].idle = 0; // Start moving
									elevators[w].direction = UP; // in UP direction from LOBBY
                       // int story=20 ; //hp: we need to uncomment this.
                    				elevators[w].counter=0; // Start counting number of DOOR_OPEN till top floor for this elevator
									for (i = 0; i < elevators[w].ultimate_number; i++)     // check each person one by one in elevator list
										{next_in_elevator = elevator_head[w]->Person; // take that person from the elevator list
										Remove_Event_Elevator(&elevator_head[w]);
										next_event.entity_type=next_in_elevator.person_type; // take the type
										next_event.entity_index=next_in_elevator.index; // take the index
										next_event.time=next_in_elevator.time; //hp: Is this the same with people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time?  // take the time
										floor_this_person_is_going_to = next_in_elevator.to_floor; // check where that person wanna go
										
										elevators[w].floor_to[floor_this_person_is_going_to] = 1;
										elevators[w].next_floor = floor_this_person_is_going_to; // change next floor to where that person wanna go. Its notable to say that our elevator list is sorted by floors. so first its going to go to lower levels and going up accordingly
										people[next_event.entity_type][next_event.entity_index].elevator_ind = w; // take index of elevator
										elevators[w].current_floor=0; // everytime we set this to LOBBY because we want to capture travel time for each person in the elevator
										elevators[w].elevator_clock = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time; // This is because the idea of time capture in idle situation for elevator. when elevator is idle it doesnt know what time it is. But when someone step into the elevator suddenly elevator has the time and thats the arrival time for that specific person 
										//if (i == elevators[elevator_index].num_people-1) // this is last person in the elevator
										//	{k=1;}
							
                            
                            		if (floor_this_person_is_going_to!=story) // we use this if to capture how many times door open in the way UP
                             		   elevators[w].counter ++; //hp: let us put this in {} to make the code more readable.
                            			story=floor_this_person_is_going_to;
										travel_time = elevator_travel_time_per_floor * (elevators[w].next_floor-elevators[w].current_floor) + elevators[w].counter* DOOR_TIME; // calculate travel time for each person in the elevator
               
							//elevators[elevator_index].elevator_time[0] += travel_time ; // add time to up direction for this elevator
							//people[next_event.entity_type][next_event.entity_index].elevator_travel_time = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time + travel_time; 
                            people[next_event.entity_type][next_event.entity_index].elevator_travel_time = travel_time - DOOR_TIME/2; // calculate elevator travel time for each person in
							people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time + travel_time - DOOR_TIME/2  ; // calculate total time to the clinic for each person. this is the time that the person reach to his clinic
							Load_Event(&event_head,max(people[next_event.entity_type][next_event.entity_index].start_time, people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic)+people[next_event.entity_type][next_event.entity_index].appointment, CLINIC_DEPARTURE , next_in_elevator.person_type, next_in_elevator.index); // load Clinic_Departure for each person . this is the event when the person is done with his doctor and want to go back to lobby and leave the system
							kj++;
                            elevators[w].num_people -- ; // we are done with that person so we remove it from the elevator
							//if (k==1) // if this is last person then load event as 
							if (((i == elevators[w].ultimate_number-1)&&(elevators[w].elevator_going_to<people[next_event.entity_type][next_event.entity_index].to_floor))) //hp: Amir, look how I have condensed this. // this if condition is for when we have someone else requested a elevator at CLINICAL_DEPARTURE event and we need to send the elevator now to that floor

							{
							elevators[w].elevator_time[0] += travel_time + elevators[w].counter * DOOR_TIME;
							Load_Event(&event_head,elevators[w].elevator_clock+elevators[w].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list AP: add this one to revised time for this load event AND last person in the list from lobby wanna go the higher level than the person who requested before from CLINICAL_DEPARTURE
                            int story=20 ;
                            elevators[w].counter=0;
							//Load_Event(&event_head,travel_time, ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list
							}
							if ((i == elevators[w].ultimate_number-1)&&(elevators[w].elevator_going_to>people[next_event.entity_type][next_event.entity_index].to_floor))  // add ELVEVATOR_ARRIVAL event into the list AP: add this one to revised time for this load event AND last person in the list from lobby wanna go the lower level than the person who requested before from CLINICAL_DEPARTURE
                                {travel_time = elevator_travel_time_per_floor * (elevators[w].elevator_going_to-elevators[w].current_floor);
                                elevators[w].elevator_time[0] += travel_time + (elevators[w].counter+1) * DOOR_TIME; //hp: we need another Door_time here as well. Look to the +1 I have added.
                                elevators[w].next_floor = elevators[w].elevator_going_to; // (erase +1) here we say that beacuse the last person in the lobby wanna go to the level lower than ELEVATOR_GOING_TO we need to update this
                                elevators[w].elevator_going_to = 0;
                                Load_Event(&event_head, elevators[w].elevator_clock+elevators[w].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list
                               int story=20 ;
                            	elevators[w].counter=0; // start counting for DOOR_OPEN
                                }

							}
						}	

					}

				}	
				

					
					if (elevators[elevator_index].num_people == 4) //hp: Do we need to add || lobby is empty? // when elevator is full OR there is no one else in the lobby elevator can start to move
					//if (elevators[elevator_index].num_people == 4)
						{
                        elevators[elevator_index].ultimate_number = elevators[elevator_index].num_people; // we use this ULTIMATE_NUMBER to track number of people in the elevator
						elevators[elevator_index].idle = 0; // Start moving
						elevators[elevator_index].direction = UP; // in UP direction from LOBBY
                       // int story=20 ; hp: why you have commented this.
                    	elevators[elevator_index].counter=0; // Start counting number of DOOR_OPEN till top floor for this elevator
						{for (i = 0; i < elevators[elevator_index].ultimate_number; i++) // check each person one by one in elevator list
							{next_in_elevator = elevator_head[elevator_index]->Person; // take that person from the elevator list
							Remove_Event_Elevator(&elevator_head[elevator_index]);
							next_event.entity_type=next_in_elevator.person_type; // take the type
							next_event.entity_index=next_in_elevator.index; // take the index
							next_event.time=next_in_elevator.time;//hp: do you mean people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time?  // take the time
							floor_this_person_is_going_to = next_in_elevator.to_floor; // check where that person wanna go
                            
							elevators[elevator_index].floor_to[floor_this_person_is_going_to] = 1;
							elevators[elevator_index].next_floor = floor_this_person_is_going_to; // change next floor to where that person wanna go. Its notable to say that our elevator list is sorted by floors. so first its going to go to lower levels and going up accordingly // AP: ADD +1 here to check 
							people[next_event.entity_type][next_event.entity_index].elevator_ind = elevator_index; // take index of elevator
							elevators[elevator_index].current_floor=0; // everytime we set this to LOBBY because we want to capture travel time for each person in the elevator
							elevators[elevator_index].elevator_clock = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time; // This is because the idea of time capture in idle situation for elevator. when elevator is idle it doesnt know what time it is. But when someone step into the elevator suddenly elevator has the time and thats the arrival time for that specific person 
							//if (i == elevators[elevator_index].num_people-1) // this is last person in the elevator
							//	{k=1;}
							
                            
                            if (floor_this_person_is_going_to!=story) // we use this if to capture how many times door open in the way UP
                                {elevators[elevator_index].counter ++;} //hp: I put this in {}
                            story=floor_this_person_is_going_to;
							travel_time = elevator_travel_time_per_floor * (elevators[elevator_index].next_floor-elevators[elevator_index].current_floor) + elevators[elevator_index].counter* DOOR_TIME; // calculate travel time for each person in the elevator
               
							//elevators[elevator_index].elevator_time[0] += travel_time ; // add time to up direction for this elevator
							//people[next_event.entity_type][next_event.entity_index].elevator_travel_time = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time + travel_time; 
                            people[next_event.entity_type][next_event.entity_index].elevator_travel_time = travel_time - DOOR_TIME/2; // calculate elevator travel time for each person in
							people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time + travel_time - DOOR_TIME/2  ; // calculate total time to the clinic for each person. this is the time that the person reach to his clinic
							Load_Event(&event_head,max(people[next_event.entity_type][next_event.entity_index].start_time, people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic)+people[next_event.entity_type][next_event.entity_index].appointment, CLINIC_DEPARTURE , next_in_elevator.person_type, next_in_elevator.index); // load Clinic_Departure for each person . this is the event when the person is done with his doctor and want to go back to lobby and leave the system
							kj++;
                            elevators[elevator_index].num_people -- ; // we are done with that person so we remove it from the elevator
							//if (k==1) // if this is last person then load event as 
							if (((i == elevators[elevator_index].ultimate_number-1)&&(elevators[elevator_index].elevator_going_to<people[next_event.entity_type][next_event.entity_index].to_floor))) //hp: Look how I have shortened this. // this if condition is for when we have someone else requested a elevator at CLINICAL_DEPARTURE event and we need to send the elevator now to that floor

							{
							elevators[elevator_index].elevator_time[0] += travel_time + elevators[elevator_index].counter * DOOR_TIME;
							Load_Event(&event_head,elevators[elevator_index].elevator_clock+elevators[elevator_index].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list AP: add this one to revised time for this load event AND last person in the list from lobby wanna go the higher level than the person who requested before from CLINICAL_DEPARTURE
                            int story=20 ;
                            elevators[elevator_index].counter=0;
							//Load_Event(&event_head,travel_time, ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list
							}
							if ((i == elevators[elevator_index].ultimate_number-1)&&(elevators[elevator_index].elevator_going_to>people[next_event.entity_type][next_event.entity_index].to_floor))  // add ELVEVATOR_ARRIVAL event into the list AP: add this one to revised time for this load event AND last person in the list from lobby wanna go the lower level than the person who requested before from CLINICAL_DEPARTURE
                                {travel_time = elevator_travel_time_per_floor * (elevators[elevator_index].elevator_going_to-elevators[elevator_index].current_floor);
                                elevators[elevator_index].elevator_time[0] += travel_time + (elevators[elevator_index].counter+1) * DOOR_TIME; //hp: I have addeda nother DOOR_TIME
                                elevators[elevator_index].next_floor = elevators[elevator_index].elevator_going_to; // here we say that beacuse the last person in the lobby wanna go to the level lower than ELEVATOR_GOING_TO we need to update this
                                elevators[elevator_index].elevator_going_to = 0;
                                Load_Event(&event_head, elevators[elevator_index].elevator_clock+elevators[elevator_index].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list
                               int story=20 ;
                            	elevators[elevator_index].counter=0; // start counting for DOOR_OPEN
                                }

							}
						}	

					}
					
				}			
				else
				{
					Load_Event_Person(&hall_head[UP][LOBBY],people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time, next_event.entity_type, next_event.entity_index); //hp:I revised this time from EPSILOn to people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time.  // add a person without elevator to waiting linked list in the LOBBY
					people_queue_lobby ++; // increase lobby queue
                     // use this as a indicator in one of if conditions		
				}
				
			}

			if (next_event.event_type == ELEVATOR_ARRIVAL)  //elevator opens doors at its highest floor
			{
			elevator_index = people[next_event.entity_type][next_event.entity_index].elevator_ind;
			if (elevators[elevator_index].next_floor == LOBBY ) // if this level is LOBBY we are done
                {   elevators[elevator_index].elevator_time[1] += elevators[elevator_index].counter* DOOR_TIME;
                    story = 20;
                    elevators[elevator_index].counter= 0;
                    for (i=0;i<elevators[elevator_index].num_people;i++)
                        {Remove_Event_Elevator(&elevator_head[elevator_index]); // all people gonna leave the elevator now
                        }
					// we are in LOBBY so we need to update these parameters
	                elevators[elevator_index].current_floor = 0;
                    elevators[elevator_index].next_floor = 0;
                    //elevators[elevator_index].elevator_going_to=0; 
                    elevators[elevator_index].num_people=0;

				
	                while ((people_queue_lobby!=0)&&(elevators[elevator_index].num_people<ELEVATOR_CAPACITY)) // we are going to check if we have a person waiting in a queue at LOBBY as we have a elevator in lobby in idle condition
		                {next_in_Line = hall_head[UP][LOBBY]->Person; // take person from lobby's queue
		                Remove_Event_Person(&hall_head[UP][LOBBY]);
		                people_queue_lobby -- ;	                
                        elevators[elevator_index].num_people++;
                        elevators[elevator_index].direction = UP;
                        next_event.entity_index = next_in_Line.index;
                        next_event.entity_type = next_in_Line.person_type;
                        floor_this_person_is_going_to = people[next_event.entity_type][next_event.entity_index].to_floor;
                        Load_Event_Elevator(&elevator_head[elevator_index],next_event.time, next_event.entity_type , next_event.entity_index,people[next_event.entity_type][next_event.entity_index].to_floor); // add person to the elevator list
		                }	
                     if (elevators[elevator_index].num_people==0)// if there is no one in the elevator
                        {elevators[elevator_index].idle=1; // make the elevator idle and wait for next person in the lobby
                        ind=0;
						}
	                 if (elevators[elevator_index].num_people!=0) // if there is a person in the elevator
	                    {elevators[elevator_index].ultimate_number = elevators[elevator_index].num_people; // capture number of people in it
                        for (int i = 0; i < elevators[elevator_index].ultimate_number; i++) // check for each person in the elevator and take them to their floors
                            {next_in_elevator = elevator_head[elevator_index]->Person;
                            Remove_Event_Elevator(&elevator_head[elevator_index]);
                            next_event.entity_type=next_in_elevator.person_type;
                            next_event.entity_index=next_in_elevator.index;
                            next_event.time=next_in_elevator.time;
                            floor_this_person_is_going_to = next_in_elevator.to_floor;
                            elevators[elevator_index].floor_to[floor_this_person_is_going_to] = 1;
                            elevators[elevator_index].next_floor = floor_this_person_is_going_to;
                            people[next_event.entity_type][next_event.entity_index].elevator_ind = elevator_index; 
                            elevators[elevator_index].current_floor=0;
                            people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time=(elevators[elevator_index].elevator_time[1]+elevators[elevator_index].elevator_time[0]+elevators[elevator_index].elevator_clock)-people[next_in_Line.person_type][next_in_Line.index].arrive_to_elevator_time;
							coun++; // calculate wait time in UP direction for each person 

                            if (floor_this_person_is_going_to!=story) // capture number of DOOR_OPEN
                                elevators[elevator_index].counter++;
                            story=floor_this_person_is_going_to;
                            travel_time = elevator_travel_time_per_floor * (elevators[elevator_index].next_floor-elevators[elevator_index].current_floor + elevators[elevator_index].counter* DOOR_TIME);   // calculate travel time 
                            people[next_event.entity_type][next_event.entity_index].elevator_travel_time = travel_time -DOOR_TIME/2;
							people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic = people[next_event.entity_type][next_event.entity_index].arrive_to_elevator_time + travel_time + people[next_event.entity_type][next_event.entity_index].elevator_wait_time - DOOR_TIME/2; 
							Load_Event(&event_head,max(people[next_event.entity_type][next_event.entity_index].total_time_to_get_clinic, people[next_event.entity_type][next_event.entity_index].start_time)+people[next_event.entity_type][next_event.entity_index].appointment, CLINIC_DEPARTURE , next_event.entity_type, next_event.entity_index);   //hp: max // Add CLINIC_DEPARTURE event for each person in the elevator
							kj++;
                            elevators[elevator_index].num_people -- ;
							if (((i == elevators[elevator_index].ultimate_number-1)&&(elevators[elevator_index].elevator_going_to==0))||((i == elevators[elevator_index].ultimate_number-1)&&(elevators[elevator_index].elevator_going_to<people[next_event.entity_type][next_event.entity_index].to_floor))) // Same as what we had before 
							{
							elevators[elevator_index].elevator_time[0] += travel_time +elevators[elevator_index].counter* DOOR_TIME;
                            int story=20 ;
                            elevators[elevator_index].counter=0;
							Load_Event(&event_head, elevators[elevator_index].elevator_clock+elevators[elevator_index].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index);  // add ELVEVATOR_ARRIVAL event into the list
							}
							if ((i == elevators[elevator_index].ultimate_number-1)&&(elevators[elevator_index].elevator_going_to>people[next_event.entity_type][next_event.entity_index].to_floor))
                                {travel_time = elevator_travel_time_per_floor * (elevators[elevator_index].elevator_going_to-elevators[elevator_index].current_floor);
                                elevators[elevator_index].elevator_time[0] += travel_time + elevators[elevator_index].counter * DOOR_TIME;
                                elevators[elevator_index].next_floor = elevators[elevator_index].elevator_going_to;
                                elevators[elevator_index].elevator_going_to = 0;
                                Load_Event(&event_head,elevators[elevator_index].elevator_clock+ elevators[elevator_index].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index); 
                                int story=20 ;
                                elevators[elevator_index].counter=0;
                                } 

				            }
				        }
					if ((elevators[elevator_index].num_people==0)&&(elevators[elevator_index].elevator_going_to!=0))
			        {
						travel_time = elevator_travel_time_per_floor * (elevators[elevator_index].elevator_going_to-elevators[elevator_index].current_floor);
                                elevators[elevator_index].elevator_time[0] += travel_time + elevators[elevator_index].counter * DOOR_TIME;
                                elevators[elevator_index].next_floor = elevators[elevator_index].elevator_going_to; //erase +1
                                elevators[elevator_index].elevator_going_to = 0;
                                Load_Event(&event_head,elevators[elevator_index].elevator_clock+ elevators[elevator_index].elevator_time[0], ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index); 
					}
                }
			else
			{
			
			while ((num_in_line_from_clinic[elevators[elevator_index].next_floor]!=0) && (elevators[elevator_index].num_people<4)) // while elevator is going down and in this level find people in the queue and has capacity take the people from the queue
					{next_in_Line = hall_head[DOWN][elevators[elevator_index].next_floor]->Person; // take the person from the wait list
                    if (story != elevators[elevator_index].next_floor)
                        {elevators[elevator_index].counter++;
						}
                    if (elevators[elevator_index].counter>4)
                        {printf("Counter Error position 1");} //hp: helps debugging.
                    story = elevators[elevator_index].next_floor; // use in counting DOOR_OPEN
                    people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time_down=(elevators[elevator_index].elevator_time[1]+ (elevators[elevator_index].counter* DOOR_TIME) + elevators[elevator_index].elevator_time[0]+elevators[elevator_index].elevator_clock)-max(people[next_in_Line.person_type][next_in_Line.index].total_time_to_get_clinic,people[next_in_Line.person_type][next_in_Line.index].start_time) -people[next_in_Line.person_type][next_in_Line.index].appointment; //hp: Amir, this was causing an error. The very last one had a + rather than -, so it was adding the appointment time. Also, I added that maximization.  // calculating wait time in DOWN direction 
					if (people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time_down<0) 
                        {printf("Negative wait_time in the down direction \n");} //hp: Let us add stuff like this to the code so that we can detect any possible abnormality.
                   



                    cou++; //hp: Amir, please add an explanation about what thsi variable is taking care of.
					Remove_Event_Person(&hall_head[DOWN][elevators[elevator_index].next_floor]);
					num_in_line_from_clinic[elevators[elevator_index].next_floor] -- ; // decrease number of people in the line
					Load_Event_Elevator(&elevator_head[elevator_index],next_event.time, next_in_Line.person_type, next_in_Line.index,LOBBY); // add people to elevator's list
                    elevators[elevator_index].num_people ++; // add people's count in the elevator
                    last_indicator = 1; //hp: Amir, let us add an explanation to this.

					}
			 // use this indicator in counting DOOR_OPEN


		// if this level is not LOBBY we need to load another event for next floor to check there for people in queue


			if ((elevators[elevator_index].num_people==4) && (num_in_line_from_clinic[elevators[elevator_index].next_floor]!=0)) // if there IS people waiting in the queue in this level but elevator HAS NOT capacity to take those people 
				{	number = num_in_line_from_clinic[elevators[elevator_index].next_floor];
					for (p=0;p<number;p++) // for every person in that queue

						{//printf("%d\t",number);
						next_in_Line = hall_head[DOWN][elevators[elevator_index].next_floor]->Person;
						Remove_Event_Person(&hall_head[DOWN][elevators[elevator_index].next_floor]);
						num_in_line_from_clinic[elevators[elevator_index].next_floor] -- ; // we imaginary erase them from wait list but we are going to add them again and check them again in next EPSILON time
						Load_Event(&event_head, next_event.time+ EPSILON + DOOR_TIME , CLINIC_DEPARTURE , next_in_Line.person_type, next_in_Line.index); //hp: I changed the time of this.  // we need to make another CLINIC_DEPARTURE to check for other elevators because this one was full
						}
				}
			
			 // get the elevator index from the last person inside it 
			elevators[elevator_index].next_floor=elevators[elevator_index].next_floor-1; // its start to moving down and check for people in each floor
            elevators[elevator_index].elevator_time[1] += elevator_travel_time_per_floor;
			Load_Event(&event_head, next_event.time+elevator_travel_time_per_floor + DOOR_TIME*last_indicator , ELEVATOR_ARRIVAL , next_event.entity_type, next_event.entity_index); // here we imaginary stop in each level so elevator time in DOWN direction needs to update by one level distance
            last_indicator = 0;
			}
		}
		//Print_Calendar();
		} //end while keepgoing
		 // records all stats for each replication 

		//record KPIs from this replication

	}  //end for each rep
	//print stats results
	Print_Calendar();
	fclose(output_file);
}
/********************************************************************************************
Open_and_Read_Files() opens all output and input files, and reads the latter into appropriate data stpructures 
********************************************************************************************/
void Open_and_Read_Files()
{
	
	char buf[1024];  //holds line of data at a time

	int i, row, col, clinic_index;


	output_file=fopen("/Users/hosseinpiri/Desktop/Diamond_clinic/Diamond_Elevator_Project/output.txt","w");
	clinic_file=fopen("/Users/hosseinpiri/Desktop/Diamond_clinic/Diamond_Elevator_Project/Clinic_Input_File.csv","r");
	elevator_file=fopen("/Users/hosseinpiri/Desktop/Diamond_clinic/Diamond_Elevator_Project/Elevator_Input_File.csv","r");
	
	//READ IN CLINIC INFO information sheet one line at a time
	row = 0;
	while (fgets(buf, 1024, clinic_file))
	{
		if (row == 0 || row == 1)   //get past first two header rows
		{
			row++;
			continue;
		}
		if (row >= NUM_CLINICS + 2) //the +2 for the two header rows
			break;

		clinic_index = row - 2;
		char* field = strtok(buf, ",");
		for (col = 0; col < 4; col++)  //read in first 5 columns (through and including num_docs)
		{
			if (col > 0)
				field = strtok(NULL, ",");
			switch (col)
			{
			case 0:
				clinics[clinic_index].index = atoi(field);
				break;
			case 1:
				clinics[clinic_index].floor = atoi(field);
				break;
			case 2:
				clinics[clinic_index].num_staff = atoi(field);
				break;
			case 3:
				clinics[clinic_index].num_docs = atoi(field);
				break;

			default:
				// should never get here
				//printf("Should never get to this default in switch\n");
				//exit(0);
				break;
			}
		}

		for (i = 0; i < clinics[clinic_index].num_docs; i++)
		{
			field = strtok(NULL, ",");
			clinics[clinic_index].num_pats[i] = atoi(field);
			field = strtok(NULL, ",");
			clinics[clinic_index].doc_start_times[i] = atof(field);
			field = strtok(NULL, ",");
			clinics[clinic_index].doc_end_times[i] = atof(field);

			people[DOCTOR][total_docs].person_type = DOCTOR;
			people[DOCTOR][total_docs].index = total_docs;
			people[DOCTOR][total_docs].clinic = clinic_index;
			people[DOCTOR][total_docs].to_floor = clinics[clinic_index].floor;
			people[DOCTOR][total_docs].start_time = clinics[clinic_index].doc_start_times[i];
			people[DOCTOR][total_docs].end_time = clinics[clinic_index].doc_end_times[i]; 
			people[DOCTOR][total_docs].appointment = clinics[clinic_index].doc_end_times[i]; // add them to the list when the want to come back 
			 //sms: remember to load doctor and staff end times into event calendar for them calling an elevator from their clinic floor back down to lobby at end of day.

			total_docs++;
		}
		row++;
	}

	fclose(clinic_file);

	//READ IN ELEVATOR INFO information sheet one line at a time
	row = 0;
	while (fgets(buf, 1024, elevator_file))
	{
		if (row == 0 || row == 1)   //header row
		{
			row++;
			continue;
		}
		if (row >= NUM_ELEVATORS + 2)  //the +2 for the two header rows
			break;

		//if we are here, then we are still reading the rows of data
		char* field = strtok(buf, ",");
		for (i = 0; i < BUILDING_HOURS; i++)
		{	
			//the first strtok above just reads in the elevator index...we already know that from the row, so just do the next read
			field = strtok(NULL, ",");
			elevators[row - 2].floor_idle[i] = atoi(field);
		}
		row++;
	}

	fclose(elevator_file);
}
 
/********************************************************************************************
Initialize_Rep() resets all counters, other initial conditions at the start of each simulation replication
********************************************************************************************/
void Initialize_Rep()
{  
    int story=20 ;
    int counter=0;
	int i, j;

	tnow = 0;
	num_events_on_calendar = 0; // @ t=0 no one wait for elevator in any direction in any floor

	for (i = 0; i < NUM_DIRECTIONS; i++)
		for (j = 0; j < NUM_FLOORS; j++)
			people_waiting_elevator[i][j] = 0;

	for (i=0;i<NUM_FLOORS;i++)
		{num_in_line_from_clinic[i]=0;
		}
	for (i = 0; i < NUM_ELEVATORS; i++) // @ t=0 all elevators are at their idle situations and their t=0 floor idle
	{	
		elevators[i].num_people = 0;
		elevators[i].idle = 1; // idle can get binary values , 1 == True
		elevators[i].current_floor = elevators[i].floor_idle[0]; //set the current floor of elevators as the idle floor assigned at time 0.
		elevators[i].elevator_going_to = 0;
		elevators[i].counter = 0; 

		for (j = 0; j < NUM_FLOORS; j++)
		{
			elevators[i].floor_to[j] = 0;
		}

		elevator_head[i] = NULL; // Still no person linked to elevator
	}

	for (i = 0; i < NUM_DIRECTIONS; i++) 
	{
		for (j = 0; j < NUM_FLOORS; j++)
		{
			//num_wait_floor[i][j] = 0;   
			hall_head[i][j] = NULL; //list of people waiting for elevators for any direction in any floor is empty now
			num_events_on_headhall = 0 ;// total number of people in line is zero
			num_events_on_elevator=0 ; // 
			people_queue_lobby =0; // people waiting in a queue at lobby
		}
	}
} //end Initialize()

/********************************************************************************************
Load_Lobby_Arrivals() loads all of the first-of-day Lobby elevator arrival events onto the event calendar.
These will be some offset based on the patients' scheduled appointment times, and the staff and doctors' scheduled start times. 
********************************************************************************************/
void Load_Lobby_Arrivals()
{
	int i, j, k, clinic_floor;
	double minutes_before_start_time, unif, appointment_duration;

	//first, load all doctor info and their arrival times
	//note, some of the doctor structure info was loaded when we read the clinic input file
	for (i = 0; i < total_docs; i++)
	{
		minutes_before_start_time = Normal(AVG_MINS_BEFORE_APPT, SD_MINS_BEFORE_APPT); //random offset for arriving relative to appointment time
		people[DOCTOR][i].arrive_to_elevator_time = max(people[DOCTOR][i].start_time - minutes_before_start_time, 0); //time 0 is when the doors open, so can't arrive before then
		Load_Event(&event_head, people[DOCTOR][i].arrive_to_elevator_time, PERSON_ARRIVES_LOBBY, DOCTOR, i); //load event for doctor into our list
	}

	//next, load all staff and first-of-day patient lobby arrival times for the day onto event calendar
	for (i = 0; i < NUM_CLINICS; i++) //loop over each clinic
	{
		clinic_floor = clinics[i].floor;
		//load staff info and their arrival times
		for (j = 0; j < clinics[i].num_staff; j++)
		{
			people[STAFF][total_staff].person_type = STAFF;   // First we want to add STAFF into our people struct
			people[STAFF][total_staff].index = total_staff;
			people[STAFF][total_staff].clinic = i;
			people[STAFF][total_staff].to_floor = clinic_floor;
			people[STAFF][total_staff].start_time = STAFF_START_TIME;
			people[STAFF][total_staff].end_time = STAFF_END_TIME;
			people[STAFF][total_staff].appointment = STAFF_END_TIME; //  add to the list when they are done as departure clinic event
			minutes_before_start_time = Normal(AVG_MINS_BEFORE_APPT, SD_MINS_BEFORE_APPT); //random offset for arriving relative to appointment time
			people[STAFF][total_staff].arrive_to_elevator_time = max(people[STAFF][total_staff].start_time - minutes_before_start_time, 0); //time 0 is the when the doors open, so can't arrive before then
			Load_Event(&event_head, people[STAFF][total_staff].arrive_to_elevator_time, PERSON_ARRIVES_LOBBY, STAFF, total_staff);

			total_staff++;
		}

		//load patient info and their arrival times
		for (j = 0; j < clinics[i].num_docs; j++) //loop over each doctor working today in this clinic
		{
			//we assume for now that each patient a doctor will see that day has an appointment duration of (doc end time - doc start time)/num_pats
			appointment_duration = (clinics[i].doc_end_times[j] - clinics[i].doc_start_times[j]) / clinics[i].num_pats[j];
			for (k = 0; k < clinics[i].num_pats[j]; k++)  //loop over each patient this doc will see today in this clinic
			{
				people[PATIENT][total_pats].person_type = PATIENT;
				people[PATIENT][total_pats].index = total_pats;
				people[PATIENT][total_pats].clinic = i;   // indexing patients
				people[PATIENT][total_pats].to_floor = clinic_floor;   //when entering lobby, the to floor is the clinic they are going to
				people[PATIENT][total_pats].start_time = clinics[i].doc_start_times[j] + k * appointment_duration;
				people[PATIENT][total_pats].appointment = appointment_duration; // assume uniform duration time for each patient
				unif = Unif(); // uniform number generates to see if patient will show up or not ! 
				if (unif < NO_SHOW_PROB) // threshold 
					people[PATIENT][total_pats].no_show = 1;
				else
					people[PATIENT][total_pats].no_show = 0;

				//only add an arrival to the event calendar for this patient if they show up
				if (people[PATIENT][total_pats].no_show == 0) 
				{
					minutes_before_start_time = Normal(AVG_MINS_BEFORE_APPT, SD_MINS_BEFORE_APPT); //random offset for arriving relative to appointment time
					people[PATIENT][total_pats].arrive_to_elevator_time = max(people[PATIENT][total_pats].start_time - minutes_before_start_time, 0); //time 0 is the when the doors open, so can't arrive before then
					Load_Event(&event_head, people[PATIENT][total_pats].arrive_to_elevator_time, PERSON_ARRIVES_LOBBY, PATIENT, total_pats);
				}
				total_pats++; // move on to next patient ( increment patient index )
			} //end for k looping through number of this doctors patients
		}//end for j looping through each doctor in this clinic
	}//end for i looping through each clinic``

	//sms test
	;
	dummy = 0;
}
/********************************************************************************************
Load_Event() inserts a new event into the event calendar (a linked list), maintaining the chronological order
********************************************************************************************/
void Load_Event(struct event_node** head_ref, double time, int event_type, int entity_type, int index)
{
	struct event_node* current;
	struct event_node* event_ptr;

	event_ptr = (struct event_node*)malloc(sizeof(struct event_node));
	event_ptr->Event.time = time;
	event_ptr->Event.event_type = event_type;
	event_ptr->Event.entity_type = entity_type;
	event_ptr->Event.entity_index = index;

	/* Special case for inserting at the head  */
	if (*head_ref == NULL || (*head_ref)->Event.time >= event_ptr->Event.time)
	{
		event_ptr->next = *head_ref;
		*head_ref = event_ptr;
	}
	else
	{
		/* Locate the node before the point of insertion */   
		current = *head_ref;
		while (current->next != NULL && current->next->Event.time < event_ptr->Event.time)
			current = current->next;

		event_ptr->next = current->next;
		current->next = event_ptr;
	}
	num_events_on_calendar++;
}




/********************************************************************************************
Print_Calendar() prints all events currently in the event calendar to an external file
********************************************************************************************/
void Print_Calendar()
{
	int i;
	struct event_node* event_ptr;

	event_ptr = (struct event_node*)malloc(sizeof(struct event_node));

	for (i = 0; i < num_events_on_calendar; i++)
	{
		if (i == 0)
			event_ptr = event_head;
		else
			event_ptr = event_ptr->next;

		fprintf(output_file, "%.2f\t%d\t%d\t%d\n", event_ptr->Event.time, event_ptr->Event.event_type, event_ptr->Event.entity_type, event_ptr->Event.entity_index);
	}
	dummy = 0;
	fprintf(output_file,"End of This Calender\n");
}

/********************************************************************************************
Remove_Event() removes the head of the event calendar; i.e., deletes the event that was scheduled to occur next.
This is called after the code above already obtained that next event to process in the simulation. 
********************************************************************************************/
void Remove_Event(struct event_node** head_ref)
{
	struct event_node* temp;

	if (*head_ref == NULL)
	{
		printf("head_ref should never be NULL when calling Remove_Event\n");
		exit(0);
	}

	temp = *head_ref;
	*head_ref = temp->next;
	free(temp);
	num_events_on_calendar--;
}

/********************************************************************************************
Elevator_Available() is called when someone is trying to get on an elevator at floor "floor".  It checks
if/how many elevators are sitting idle at that floor.  If there are n > 0 elevators to choose from, we assume 
the person will choose any of them with prob 1/n each. 
Output from this function would be 0 if there is no elevator available in the same floor in idle situation
OR a number from 1 to NUM_ELEVATORS indicate which elevator is avaiable for pickup.
********************************************************************************************/
int Elevator_Available(elevator elevs[], int floor)
{
	int i, num_avail = 0;
	double unif, equal_probs;

	for (i = 0; i < NUM_ELEVATORS; i++)
	{
		if ((elevs[i].idle == 1) && (elevs[i].current_floor == floor))
			num_avail++;
	}

	if (num_avail == 0) // no idle elevator in the same floor 
		return 0;
	else //pick one of the available ones with equal prob
	{
		equal_probs = 1.0 / num_avail;
		unif = Unif();

		num_avail = 0;
		for (i = 0; i < NUM_ELEVATORS; i++)
		{
			if ((elevs[i].idle == 1) && (elevs[i].current_floor == floor))
			{
				num_avail++;
				if (unif < num_avail * equal_probs)
					return i + 1;  //the +1 makes it so that a "true" value (i.e., > 0) is returned even if the elevator with index 0 is chosen.
			}
		}
	}
return 0;
} 

/********************************************************************************************
Load_Event_Person() inserts a new event into the event calendar (a linked list), maintaining the chronological order
********************************************************************************************/
void Load_Event_Person(struct person_node** head_ref, double time,int person_type, int index)
{
	struct person_node* current;
	struct person_node* person_ptr;

	person_ptr = (struct person_node*)malloc(sizeof(struct person_node));
	person_ptr->Person.time = time;
	person_ptr->Person.person_type = person_type;
	person_ptr->Person.index = index;

	/* Special case for inserting at the head  */
	if (*head_ref == NULL || (*head_ref)->Person.time >= person_ptr->Person.time)
	{
		person_ptr->next = *head_ref;
		*head_ref = person_ptr;
	}
	else
	{
		/* Locate the node before the point of insertion */   
		current = *head_ref;
		while (current->next != NULL && current->next->Person.time < person_ptr->Person.time)
			current = current->next;

		person_ptr->next = current->next;
		current->next = person_ptr;
	}
	num_events_on_headhall ++;
}

/********************************************************************************************
Remove_Event() removes the head of the event calendar; i.e., deletes the event that was scheduled to occur next.
This is called after the code above already obtained that next event to process in the simulation. 
********************************************************************************************/
void Remove_Event_Person(struct person_node** head_ref)
{
	struct person_node* temp;

	if (*head_ref == NULL)
	{
		printf("head_ref should never be NULL when calling Remove_Event\n");
		exit(0);
	}

	temp = *head_ref;
	*head_ref = temp->next;
	free(temp);
	num_events_on_headhall--;
	
}

/********************************************************************************************
Load_Event_Elevator() inserts a new event into the event calendar (a linked list), maintaining the chronological order
********************************************************************************************/
void Load_Event_Elevator(struct person_node** head_ref, double time,int person_type, int index, int floor_to)
{
	struct person_node* current;
	struct person_node* person_ptr;

	person_ptr = (struct person_node*)malloc(sizeof(struct person_node));
	person_ptr->Person.time = time;
	person_ptr->Person.person_type = person_type;
	person_ptr->Person.index = index;
	person_ptr->Person.to_floor = floor_to;


	/* Special case for inserting at the head  */
	if (*head_ref == NULL || (*head_ref)->Person.to_floor >= person_ptr->Person.to_floor)
	{
		person_ptr->next = *head_ref;
		*head_ref = person_ptr;
	}
	else
	{
		/* Locate the node before the point of insertion */  // here we want to locate them based on their floor 
		current = *head_ref;
		while (current->next != NULL && current->next->Person.to_floor < person_ptr->Person.to_floor)
			current = current->next;

		person_ptr->next = current->next;
		current->next = person_ptr;
	}
	num_events_on_elevator ++;
}

/********************************************************************************************
Remove_Event() removes the head of the event calendar; i.e., deletes the event that was scheduled to occur next.
This is called after the code above already obtained that next event to process in the simulation. 
********************************************************************************************/
void Remove_Event_Elevator(struct person_node** head_ref)
{
	struct person_node* temp;

	if (*head_ref == NULL)
	{
		printf("head_ref should never be NULL when calling Remove_Event\n");
		exit(0);
	}

	temp = *head_ref;
	*head_ref = temp->next;
	free(temp);
	num_events_on_elevator--;
}
