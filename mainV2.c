#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "random.c"  // for generate random numbers we can reference our random.c file
#include "main1.h"



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

	// This seeds the Mersenne Twister Random Number Generator
	unsigned long long init[4] = { 0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL }, length = 4;
	init_by_array64(init, length);
	
	Open_and_Read_Files();

	for (repnum = 0; repnum < REPS; repnum++)
  	{
    Initialize_Rep();
	Load_Lobby_Arrivals();  //this includes patients, staff, and doctors

	keepgoing = 1;
#if 0
		while (keepgoing)
		{
			//get next event off the top of the linked list
			next_event = event_head->Event;
			Remove_Event(&event_head);

			if (num_events_on_calendar == 0) //this indicates this simulation replication is over
				keepgoing = 0;
			else if (next_event.event_type == CLINIC_DEPARTURE)
			{
				//sms: to be filled in later
			}
			else if (next_event.event_type == PERSON_ARRIVES_LOBBY)
			{
				//if there is an elevator waiting open, then get in it; othwerise, have to wait
				elevator_avail = Elevator_Available(elevators, LOBBY); //returns elevator index + 1 if elevator is avail
				if (elevator_avail)  //this means someone arrives to an idle elevator at their floor.
				{
					elevator_index = elevator_avail - 1;
					elevators[elevator_index].idle = 0;
					elevators[elevator_index].num_people++;
					elevators[elevator_index].direction = UP;
					floor_this_person_is_going_to = people[next_event.entity_type][next_event.entity_index].to_floor;
					elevators[elevator_index].floor_to[floor_this_person_is_going_to] = 1;
					elevators[elevator_index].next_floor = floor_this_person_is_going_to;

					Send_Elevator(elevator_index);  //SMS: CONTINUE HERE
				}
				else
				{
					people_waiting_elevator[UP][LOBBY]++;
					//add this person's node to hall_head linked list. 
					//SMS: CONTINUE HERE
				}
			}

			else if (next_event.event_type == ELEVATOR_ARRIVAL)  //elevator opens doors at a floor
			{
				num_off = 0;
				num_on = 0;
				//see if/which people get off
				elev_num = next_event.elevator_num;
				elevators[elev_num].current_floor = elevators[elev_num].next_floor; //update current floor
				arrival_floor = elevators[elev_num].current_floor;
				//see if anyone on the elevator is getting off on this floor.  
				for (i = 0; i < elevators[elev_num].num_people; i++)
				{
					//does the next person on the linked list get off here?
					if (i == 0)
						person_ptr = elevator_head[elev_num];
					else
						person_ptr = person_ptr->next;

					if (person_ptr == NULL)
					{
						printf("Shouldn't have person_ptr == NULL.  In ``if (next_event.event_type == ELEVATOR_ARRIVAL)'' part of code.  Exiting program.\n");
						exit(0);
					}

					if (person_ptr->Person.to_floor == arrival_floor)  //this person is getting off on this floor
					{
						Elevator_to_Clinic(person_ptr->Person);
					}

					//remove this person who got off Elevator from the linked list of people on the elevator
					Remove_from_Elevator(elevator_head[elev_num], person_ptr);

					num_off++;
				}//end checking each person on elevator for if they are getting off on this floor or not.


				elevators[elev_num].num_people -= num_off;

				//if elevator just emptied out, see if it is supposed to go pick up a request that was made prior to anyone on the current
				//floor making the request; otherwise assign it idle status (for now).  by doing so, anyone getting on in the next step
				//of the logic can go in whichever direction they want to take it.
				//change of plans...for now skip checking if someone else on different floor requested earlier...i'm giving priority
				//to those loading from the current floor.

				//update if elevator still has people in it (in which case it will only pick up new passengers if they are traveling in same direction).

				//see if/which people get on
				if (elevators[elev_num].num_people == 0)
				{
					elevators[elev_num].direction = NO_DIRECTION;

					switch (hall_head)
						​{
							case ((hall_head[UP][arrival_floor] == NULL) && (hall_head[DOWN][arrival_floor] == NULL)): //means no one waiting to get on at this floor; see if other floors need elevator
								Elevator_Available(elev_num); //this will check if this elevator should now go to any other floor waiting for it, or to it's idle parking location

								break;

							  case ((hall_head[UP][arrival_floor] != NULL) && (hall_head[DOWN][arrival_floor] == NULL)):
								  direction = UP;
								   break;

							  case ((hall_head[UP][arrival_floor] == NULL) && (hall_head[DOWN][arrival_floor] != NULL)):
								  direction = DOWN;
								  break;

							  case ((hall_head[UP][arrival_floor] != NULL) && (hall_head[DOWN][arrival_floor] != NULL)):
								  //both directions have people waiting to get on.  see which direction was pressed first and give that priority
								  if ((hall_head[UP][arrival_floor]->Person.arrive_to_elevator_time) < (hall_head[DOWN][arrival_floor]->Person.arrive_to_elevator_time))
									  direction = UP;
								  else
									  direction = DOWN;
								  break;

								default:
									// default statements

					}

					elevators[elev_num].direction = direction;



					while (elevators[elev_num].num_people < ELEVATOR_CAPACITY)
					{
						while (hall_head[direction][arrival_floor] != NULL)
						{
							Remove_from_Hall(elevator_head[elev_num], person_ptr);
							elevators[elev_num].num_people++;

						}




						num_on++;
					}

					elevators[next_event.elevator_num].num_people += num_on;
					if (elevators[next_event.elevator_num].num_people == 0)
					{
						elevators[next_event.elevator_num].idle == 1;
						//see if this elevator needs to go serve any other calls, if not, go park at idle floor. 

					}
					//else
					{
					}
				}
			}//end if next event is ELEVATOR_ARRIVAL

			else  //shouldn't get to this condition
			{
				printf("Shouldn't get to the else condition for checking event types");
				exit(0);
			}

		} //end while keepgoing
		 
#endif
		//record KPIs from this replication

	}  //end for each rep
	
	fclose(output_file);
}

/********************************************************************************************
Open_and_Read_Files() opens all output and input files, and reads the latter into appropriate data structures 
********************************************************************************************/
void Open_and_Read_Files()
{
	
	char buf[1024];  //holds line of data at a time

	int i, row, col, clinic_index;

	output_file=fopen("/Users/amirparizi/Desktop/UBC-Master/Diamond_Elevator_Project/output.txt","w");
	clinic_file=fopen("/Users/amirparizi/Desktop/UBC-Master/Diamond_Elevator_Project/Clinic_Input_File.csv","r");
	elevator_file=fopen("/Users/amirparizi/Desktop/UBC-Master/Diamond_Elevator_Project/Elevator_Input_File.csv","r");
	
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
			people[DOCTOR][total_docs].end_time = clinics[clinic_index].doc_end_times[i];  //sms: remember to load doctor and staff end times into event calendar for them calling an elevator from their clinic floor back down to lobby at end of day.

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
	int i, j;

	tnow = 0;
	num_events_on_calendar = 0; // @ t=0 no one wait for elevator in any direction in any floor

	for (i = 0; i < NUM_DIRECTIONS; i++)
		for (j = 0; j < NUM_FLOORS; j++)
			people_waiting_elevator[i][j] = 0;


	for (i = 0; i < NUM_ELEVATORS; i++) // @ t=0 all elevators are at their idle situations and their t=0 floor idle
	{
		elevators[i].num_people = 0;
		elevators[i].idle = 1; // idle can get binary values , 1 == True
		elevators[i].current_floor = elevators[i].floor_idle[0]; //set the current floor of elevators as the idle floor assigned at time 0.
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
			//num_wait_floor[i][j] = 0;    What do you mean by this value? 
			hall_head[i][j] = NULL; //list of people waiting for elevators for any direction in any floor is empty now
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
				people[PATIENT][total_pats].start_time = clinics[i].doc_start_times[j] + k * appointment_duration; // assume uniform duration time for each patient
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
	Print_Calendar();
	dummy = 0;
}

/********************************************************************************************
Send_Elevator() inserts onto the event calendar the time that the elevator will arrive time to the next floor it is headed to, either
because that's the next floor someone on the inside has pressed, or because it is first picking someone up
based on a hall call.
********************************************************************************************/
void Send_Elevator(int elevator)
{

	//sms: CONTINUE HERE

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

#if 0
/********************************************************************************************
Remove_from_Elevator() is called when someone is trying to get out of the elevator and to remove preson from the list
********************************************************************************************/


void Remove_from_Elevator(struct event_node* head_refelevator_head[elev_num], person_ptr);


//removes the head of the linked list, as it is in order
void Remove_Person(struct person_node* head_ref)
{
	struct person_node* temp;

	temp = head_ref;
	head_ref = temp->next;
	free(temp);
}

/********************************************************************************************
Process_Lobby_Arrival() ....
********************************************************************************************/

void Process_Lobby_Arrival(event lobby_arrival)
{
	patient pat;
	//for now i'm assuming fifo line feeding multiple elevators
	pats_waiting_elevator[LOBBY]++;
	


}

#endif