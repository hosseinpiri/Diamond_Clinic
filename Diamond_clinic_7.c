//#define _CRT_SECURE_NO_DEPRECATE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "random.c" 
#include "DiamondClinic_Code_V5_sms.h"

FILE* output_file, * clinic_file, * elevator_file; 
int dummy;

/********************************************************************************************
Notes:
Current assumptions (some of which we'll want to improve later):
- elevators move at a constant velocity at all time; we do not model acceleration/deceleration right now. 
********************************************************************************************/

/********************************************************************************************
Notes:
elevators[i].elevator_time[0]: This variable measures the total time that elevator goes up.
elevators[i].elevator_time[1]: This variable measures the total time that elevator goes down.
********************************************************************************************/
int main()
{
    int my_type, i,ii,my_index,elevator_call,lobby_arrival, elevator_avail_lobby, j, keepgoing, repnum, elevator_avail, person_to_floor, elevator_index;
    int juju=0, offload_total=0, offload_non_lobby=0;
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
    
        // Bunch of parameters that we use for testing and debugging the code.
        lobby_arrival=0;
        elevator_avail_lobby=0;

        
        keepgoing=1; // if the event list is empty, we will make this keepgoing=0.
        while (keepgoing)
        //for (ii=0;ii<20000;ii++)
        {   
            
            next_event = event_head->Event; // going to check each event one by one
            Remove_Event(&event_head);
    

                         

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
Part 1: This part takes care of the events when a person arrives lobby 
111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
            if (next_event.event_type == PERSON_ARRIVES_LOBBY)
            {   lobby_arrival++;
                tnow=next_event.time;
                person_to_floor=people[next_event.entity_type][next_event.entity_index].to_floor;
                person_direction=people[next_event.entity_type][next_event.entity_index].direction;
                // Sanity check
                if (person_direction==DOWN)
                    {printf("directrion error 1");}
                //if there is an elevator waiting open, then get in it; othwerise, have to wait
                elevator_avail = Elevator_Available(elevators, LOBBY,UP); //returns elevator index + 1 if elevator is avail.  The +1 is so that if elevator index 0 is available, this expression evaluates as true
                if (elevator_avail)  //this means someone arrives to an idle elevator at their floor which is Lobby.
                {   elevator_avail_lobby++;
                    elevator_index= elevator_avail - 1; // change to elevator_index which is from 0 to 4;
                    Load_Event_Elevator(&elevator_head[elevator_index],tnow, next_event.entity_type, next_event.entity_index,person_to_floor, person_direction); // add person to the elevator list
                    people[next_event.entity_type][next_event.entity_index].elevator_start_travel_time=tnow;
                    elevators[elevator_index].num_people++;
                    elevators[elevator_index].idle = NO;
                    elevators[elevator_index].final_destination=max(elevators[elevator_index].final_destination,person_to_floor);   
                     
                    // If there is one person in the elevator, we define the Send_ELAVATOR event that will start moving the elevator after doors gets closed.
                    // The advantage is that if a person arrives in the meantime that the door is getting closed, they can get on the elevator.
                    if (elevators[elevator_index].num_people==1)
                        {   elevators[elevator_index].direction=person_direction;
                            elevators[elevator_index].elevator_time[person_direction]=DOOR_TIME+elevators[elevator_index].elevator_time[person_direction];
                            Load_Event(&event_head, tnow+DOOR_TIME+EPSILON, SEND_ELEVATOR, ELEVATOR, elevator_index); // EPSILON takes care of the time that the DOOR_TIME is zero.
                        }
                    //The following scenario takes care of the case when this person is the last one who arrives at lobby, after whom the lobby will be empty. 
                    //sms: i'm not sure why the following is something that needs checking.  see my comment in next section that checks about elevator_capacity 
                }           
                else // If there is no idle elevator at LOBBY.
                {
                    Load_Event_Person(&hall_head[person_direction][LOBBY],tnow, next_event.entity_type, next_event.entity_index);  // add a person without elevator to waiting linked list in the LOBBY
                    queue_size[person_direction][LOBBY]++;
                    people[next_event.entity_type][next_event.entity_index].line_up_time=tnow;
                }  
            }    


////////////////////////////////////////////////////////////////////////////////////
//Part 2: This part takes care of send elevators
////////////////////////////////////////////////////////////////////////////////////
            if (next_event.event_type == SEND_ELEVATOR)
            {// Start of send elevator up
                tnow=next_event.time;
                elevator_index=next_event.entity_index;
                elevator_direction=elevators[elevator_index].direction;
                elevators[elevator_index].idle = NO; // Start moving
                //elevators[elevator_index].current_floor=elevators[elevator_index].next_floor;
                if ( elevator_direction== UP)
                    {elevators[elevator_index].current_floor=elevators[elevator_index].current_floor+1;}
                if (elevator_direction == DOWN)
                    {elevators[elevator_index].current_floor=elevators[elevator_index].current_floor-1;}
                if ((elevators[elevator_index].current_floor>9)||(elevators[elevator_index].current_floor<0))
                    {printf("Floor error: %d \t",elevators[elevator_index].current_floor);}
                elevators[elevator_index].elevator_time[elevator_direction]=time_per_floor+elevators[elevator_index].elevator_time[elevator_direction];
                Load_Event(&event_head,tnow+time_per_floor, ELEVATOR_ARRIVAL , ELEVATOR, elevator_index);

            } // End of Send elevator up
                
/*3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333
Part 3 : This part takes care of the events when an elevator arrives at a floor. 
3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333*/ 
            if (next_event.event_type == ELEVATOR_ARRIVAL)  //elevator arrives at a floor.
            {     
                tnow=next_event.time;
                elevator_index = next_event.entity_index;
                elevator_direction=elevators[elevator_index].direction;
                door_open=NO;

                // First, offload people that are already in the elevator.
                if (elevators[elevator_index].num_people>0)
                {
                    next_in_elevator=elevator_head[elevator_index]->Person;
                    while ((elevators[elevator_index].num_people>0)&&(next_in_elevator.to_floor==elevators[elevator_index].current_floor))
                    {
                        //printf("Offloading is getting done properly \n");
                        offload_total++; // For debugging purposes.
                        people[next_in_elevator.person_type][next_in_elevator.index].time_to_get_clinic=tnow+DOOR_TIME; // This still makes sense if the floor is lobby or a lab instead of a clinic. I mean the time that person gets off the elevator.
                        people[next_in_elevator.person_type][next_in_elevator.index].elevator_travel_time=people[next_in_elevator.person_type][next_in_elevator.index].time_to_get_clinic-people[next_in_elevator.person_type][next_in_elevator.index].elevator_start_travel_time+people[next_in_elevator.person_type][next_in_elevator.index].elevator_travel_time;
                        // Sanity check
                        if (people[next_in_elevator.person_type][next_in_elevator.index].elevator_travel_time<0)
                            {printf("Elevator travel time error \n");}
                        //sms: I need discussing with you on how to include appointment_wait_time.
                        if (elevators[elevator_index].current_floor != LOBBY)
                        {  offload_non_lobby++;
                            // load Clinic_Departure for each person . this is the event when the person is done with his doctor and want to go back to lobby and leave the system
                            // hp: note that if we consider travel to other floors as well, say for example to a lab after the clinic, then we need to re-define parameters like appointment duration. The following line is one of the places where this matters. You can re-define these parameters in the "Clinic_Departure" only if they would not travel to lobby after their appointment.
                            Load_Event(&event_head,max(people[next_in_elevator.person_type][next_in_elevator.index].start_time, people[next_in_elevator.person_type][next_in_elevator.index].time_to_get_clinic)+people[next_in_elevator.person_type][next_in_elevator.index].appointment_duration, CLINIC_DEPARTURE, next_in_elevator.person_type, next_in_elevator.index);
                        } 
                        Remove_Event_Elevator(&elevator_head[elevator_index]);
                        elevators[elevator_index].num_people --;

                        if (elevators[elevator_index].num_people>0)
                            {next_in_elevator=elevator_head[elevator_index]->Person;}
                        door_open=YES;
                    }
                }
                
                if (elevators[elevator_index].current_floor == LOBBY )
                    {elevator_direction=UP;
                    elevators[elevator_index].direction = elevator_direction;
                    }
                
                // Load people in that floor who are goin in the elevator's direction.
                while ((queue_size[elevator_direction][elevators[elevator_index].current_floor]>0) && (elevators[elevator_index].num_people<ELEVATOR_CAPACITY)) 
                    {
                        next_in_Line = hall_head[elevator_direction][elevators[elevator_index].current_floor]->Person; // take the person from the wait list
                        people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time=tnow- people[next_in_Line.person_type][next_in_Line.index].line_up_time+DOOR_TIME+people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time; // calculating wait time in DOWN elevator_direction.  //hp: Amir, this was causing an error. The very last one had a + rather than -, so it was adding the appointment time. Also, I added that maximization.  
                        Load_Event_Elevator(&elevator_head[elevator_index],tnow+DOOR_TIME, next_in_Line.person_type, next_in_Line.index,people[next_in_Line.person_type][next_in_Line.index].to_floor,people[next_in_Line.person_type][next_in_Line.index].direction); // add people to elevator's list
                        people[next_in_Line.person_type][next_in_Line.index].elevator_start_travel_time=tnow+DOOR_TIME;
                        elevators[elevator_index].num_people ++; // update people's count in the elevators
                        Remove_Event_Person(&hall_head[elevator_direction][elevators[elevator_index].current_floor]);
                        queue_size[elevator_direction][elevators[elevator_index].current_floor] -- ; // decrease number of people in the queue of each floor.
                        door_open=YES; 
                    }

                
                
                /// Update elevator destination in UP direction, and change elevator_direction if required.

                if (elevators[elevator_index].direction==UP)
                {
                    if (elevators[elevator_index].num_people>0)
                    {   //Update the final destination w.r.t. the people inside the elevator.
                        next_in_elevator=elevator_head[elevator_index]->Person;
                        elevators[elevator_index].final_destination=9;
                        //max(elevators[elevator_index].final_destination,next_in_elevator.to_floor);
                    }

                    for (i = NUM_FLOORS-1; i > elevators[elevator_index].final_destination; i--)
                    {   //Update the final destination w.r.t. the people waiting at upper floors to go to lobby.
                        if (queue_size[DOWN][i]>0)
                        {
                            counter=0;
                            for (j=0;j<NUM_ELEVATORS;j++)
                            {
                                if (elevators[j].final_destination>=i)
                                    {counter++;}
                            }
                            if (counter==0)
                            {
                                   if (elevators[elevator_index].current_floor!=LOBBY)
                                    {elevators[elevator_index].final_destination=max(elevators[elevator_index].final_destination,i);// This is one of the places that makes elevator time distribution non-consistent.
                                    }
                                if (elevators[elevator_index].current_floor==LOBBY)
                                    {elevator_avail = Elevator_Available(elevators, LOBBY,UP);
                                    my_index=elevator_avail-1;
                                    elevators[my_index].final_destination=max(elevators[my_index].final_destination,i);
                                    if (my_index!=elevator_index)
                                        {
                                            Load_Event(&event_head,tnow+DOOR_TIME, SEND_ELEVATOR , ELEVATOR, my_index);
                                        }
                                    }
                             
                            }
                        }

                    }

                     

                    if ((elevators[elevator_index].current_floor>=elevators[elevator_index].final_destination)&&(elevators[elevator_index].current_floor!=LOBBY))
                        {   if (elevators[elevator_index].num_people>0)
                                {printf("Num people: %d \t", elevators[elevator_index].num_people);
                                printf("Floor: %d \t", elevators[elevator_index].current_floor);
                                printf("Direction: %d \t", (elevator_head[elevator_index]->Person).direction);
                                printf("Direction: %d \t", (elevator_head[elevator_index]->Person).direction);
                                printf("Current floor: %d \t", (elevator_head[elevator_index]->Person).current_floor);
                                printf("To floor: %d \n", (elevator_head[elevator_index]->Person).to_floor);
                                }
                            elevator_direction=DOWN;
                            elevators[elevator_index].direction=elevator_direction;
                            elevators[elevator_index].final_destination=LOBBY;
                            while ((queue_size[elevator_direction][elevators[elevator_index].current_floor]>0) && (elevators[elevator_index].num_people<ELEVATOR_CAPACITY)) // while elevator is going down and in this level find people in the queue and has capacity take the people from the queue
                            {
                                next_in_Line = hall_head[elevator_direction][elevators[elevator_index].current_floor]->Person; // take the person from the wait list
                                people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time=people[next_in_Line.person_type][next_in_Line.index].elevator_wait_time+tnow- people[next_in_Line.person_type][next_in_Line.index].line_up_time+DOOR_TIME; // calculating wait time in DOWN elevator_direction.  //hp: Amir, this was causing an error. The very last one had a + rather than -, so it was adding the appointment time. Also, I added that maximization.  
                                Load_Event_Elevator(&elevator_head[elevator_index],tnow, next_in_Line.person_type, next_in_Line.index,people[next_in_Line.person_type][next_in_Line.index].to_floor,elevator_direction); // add people to elevator's list
                                people[next_in_Line.person_type][next_in_Line.index].elevator_start_travel_time=tnow+DOOR_TIME;
                                elevators[elevator_index].num_people ++; // update people's count in the elevators
                                Remove_Event_Person(&hall_head[elevator_direction][elevators[elevator_index].current_floor]);
                                queue_size[elevator_direction][elevators[elevator_index].current_floor] -- ; // decrease number of people in the queue of each floor.
                                door_open=YES; 
                            }
                        }
                }     

                //// Elevator has arrived a floor and it is full.    
                /// This should be after elevator changes its elevator_direction.
                
                
                if ((elevators[elevator_index].num_people==ELEVATOR_CAPACITY) && (queue_size[elevator_direction][elevators[elevator_index].current_floor]>0)&&(elevators[elevator_index].current_floor!=LOBBY)) // if there are people waiting in the queue in this floor, but elevator had NO capacity to take those people. 
                {   
                    door_open=YES;
                    elevators[elevator_index].elevator_time[elevator_direction]=2*DOOR_TIME+elevators[elevator_index].elevator_time[elevator_direction];
                    //printf("%d/t",queue_size[elevator_direction][elevators[elevator_index].current_floor]);
                
                    next_in_Line = hall_head[elevator_direction][elevators[elevator_index].current_floor]->Person;
        
                    Load_Event(&event_head, tnow +  2*DOOR_TIME+EPSILON, ELEVATOR_REQUEST, next_in_Line.person_type, next_in_Line.index); // Note that "+EPSILON" part makes sure that elevator will leave the floor and we do not assign the same elevator.      
                }
                

                if ((elevators[elevator_index].current_floor == LOBBY )&&(elevators[elevator_index].num_people==0 )&&(elevators[elevator_index].final_destination==LOBBY))
                        {elevators[elevator_index].idle = YES;}


                if (elevators[elevator_index].idle==NO)
                { 
                    if (door_open==NO)
                    {   //elevators[elevator_index].current_floor=elevators[elevator_index].next_floor;
                        if (elevators[elevator_index].direction == UP)
                        {
                            elevators[elevator_index].current_floor=elevators[elevator_index].current_floor+1;
                            
                        }
                        if (elevators[elevator_index].direction == DOWN)
                        {
                            elevators[elevator_index].current_floor=elevators[elevator_index].current_floor-1;
                        }
                        elevators[elevator_index].elevator_time[elevator_direction]=elevators[elevator_index].elevator_time[elevator_direction]+time_per_floor;
                        Load_Event(&event_head,tnow+time_per_floor, ELEVATOR_ARRIVAL , ELEVATOR, elevator_index);
                        
                    }
                    if (door_open==YES)
                    {   
                        
                        elevators[elevator_index].elevator_time[elevator_direction]=elevators[elevator_index].elevator_time[elevator_direction]+DOOR_TIME;
                        Load_Event(&event_head,tnow+DOOR_TIME, SEND_ELEVATOR , ELEVATOR, elevator_index);
                        door_open=NO;
                    }
                }
                
            } 

             //Ends part 3.

/*444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
Part 4: This part takes care of the events when a person leaves the clinic and would like to go to lobby.
444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444*/

            if (next_event.event_type == CLINIC_DEPARTURE)

            {   //printf("CLINIC_DEPARTURE\t");
                person_out_clinic++; // This paremeter counts the total " CLINIC_DEPARTURE" events.
                tnow=next_event.time;
                person_current_floor=people[next_event.entity_type][next_event.entity_index].to_floor;
                people[next_event.entity_type][next_event.entity_index].current_floor=person_current_floor;
                
                //sms: We can change the following line in order to enable travel between floors. E.g., someone can go to an upper floor to a labratory. 
                //sms: For now I am working with Lobby, but will generalize this. The current code can handle travel to other floors than lobby.
                person_to_floor=LOBBY;
                people[next_event.entity_type][next_event.entity_index].to_floor=person_to_floor;
                person_direction=(person_to_floor<=person_current_floor);
                people[next_event.entity_type][next_event.entity_index].direction=person_direction;// If the statement is true, the direction is DOWN, and otherwise it is up.
                

                 
                elevator_avail = Elevator_Available(elevators, person_current_floor,person_direction ); // when the patient is comming back to the LOBBY, if there is an elevator available in his level
                if (elevator_avail) //this means someone arrives to an available elevator at their floor.
                {   //printf("An elevator was available right away at floors \n "); // I would skip reading this if, as at the moment it does not happen. I think this will play a role when we define pur idling policy.
                    elevator_index = elevator_avail - 1;
                    //printf("%d \n",elevator_index );
                    Load_Event_Elevator(&elevator_head[elevator_index],tnow,next_event.entity_type,next_event.entity_index,person_to_floor,person_direction); // add people to elevator's list
                    elevators[elevator_index].num_people++;
                    if (elevators[elevator_index].num_people>ELEVATOR_CAPACITY)
                        {printf("Elevator capacity error \n");} 
                    people[next_event.entity_type][next_event.entity_index].elevator_start_travel_time=tnow;
                    
                    if (elevators[elevator_index].idle == YES)
                    {   printf("An idle elevator was available at upper floors \n");
                        elevators[elevator_index].direction = person_direction;  
                        elevators[elevator_index].final_destination = person_to_floor; //hp: Is this ok in down direction too?
                        elevators[elevator_index].elevator_time[person_direction]=DOOR_TIME+elevators[elevator_index].elevator_time[person_direction];
                        Load_Event(&event_head, tnow+DOOR_TIME, SEND_ELEVATOR, ELEVATOR, elevator_index);
                        elevators[elevator_index].idle = NO;
                    }
                    
                }
                
                else // if there is no elevator in this level 
                {   
                    Load_Event_Person(&hall_head[person_direction][person_current_floor],tnow, next_event.entity_type, next_event.entity_index); // add this person to wait list in that level
                    queue_size[person_direction][person_current_floor]++; // this iparameter "queue_size" is capturing length of queue at each level
                    people[next_event.entity_type][next_event.entity_index].line_up_time=tnow;
                    if (queue_size[person_direction][person_current_floor] ==1) 
                        {Load_Event(&event_head, tnow, ELEVATOR_REQUEST, next_event.entity_type,next_event.entity_index); // Note that "+EPSILON" part makes sure that elevator will leave the floor and we do not assign the same elevator.      
                        }
                }
            } //end part 4.

/*555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555
Part 5: This part takes care of the events when a person leaves the clinic and would like to go to another floor.
555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555*/

            if (next_event.event_type == ELEVATOR_REQUEST)
            {
                //printf("ELEVATOR_REQUEST\t");
                elevator_call++;
                tnow=next_event.time;
                
                person_direction=people[next_event.entity_type][next_event.entity_index].direction;
                person_to_floor=people[next_event.entity_type][next_event.entity_index].to_floor;
                person_current_floor=people[next_event.entity_type][next_event.entity_index].current_floor;

                
                
                
                for (i = 0; i < NUM_ELEVATORS; i++)
                {   
                    wait_time = 999; // initialize wait_time to something large                        
                    if (person_direction==DOWN)
                    { 
                        if (elevators[i].direction==person_direction)
                        {   
                            
                            if (elevators[i].current_floor>=person_current_floor)
                            {
                                distance=elevators[i].current_floor-person_current_floor;
                                if (distance<0)
                                    {printf("Distance claculation error \n");}
                                //sms: In the following line, we may refine this estimation later by considering door_times.
                                elevators[i].time_to_reach=time_per_floor*distance;
                            }
                            else
                            {
                                elevators[i].time_to_reach=999;
                            } 
                        }

                        if (elevators[i].direction==1-person_direction)
                        {
                            //for (j = 0; j < ELEVATOR_CAPACITY; j++)
                            //{
                            if (elevators[i].num_people>0)
                                {elevators[i].final_destination=max( elevators[i].final_destination,((elevator_head[i]->Person).to_floor)  );}   
                            //}                            
                            distance=elevators[i].final_destination-elevators[i].current_floor+abs((elevators[i].final_destination-person_current_floor));
                            elevators[i].time_to_reach=time_per_floor*distance;
                            if (distance<0)
                                    {printf("Distance claculation error \t");
                                    }
                            

                        }
                    }   

                    if (person_direction==UP)
                    { 
                            

                        if (elevators[i].direction==person_direction)
                        {
                            if (elevators[i].next_floor<=person_current_floor)
                            {
                                distance=person_current_floor-elevators[i].next_floor;
                                elevators[i].time_to_reach=time_per_floor*distance;
                            }
                            else
                            {
                                elevators[i].time_to_reach=999;
                            } 
                        }

                        if (elevators[i].direction==1-person_direction)
                        {                      
                            distance=elevators[i].current_floor+person_current_floor;
                            elevators[i].time_to_reach=time_per_floor*distance;
                        }
                    }

                        if (elevators[i].idle==YES)
                        {
                            distance=abs((elevators[i].current_floor-person_current_floor));
                            elevators[i].time_to_reach=distance*time_per_floor;
                            
                        }
                        wait_time = min(wait_time,elevators[i].time_to_reach);
                }

                for (k=0;k<NUM_ELEVATORS;k++)
                {
                    if (wait_time == elevators[k].time_to_reach) // find the elevator k that reaches to this person the soonest.
                        {   // If elevator k is in the higher floor compared to the person, we dont do anything because we know that elevator will go down and take the person on its way down. This is consistent with part 3 of the code, where we define the actions followed after "ELEVATOR_ARRIVAL" event.
                            // However, if elevator k is in the lower floor compared to the person, then we need to assign that floor to the elevator by parameter "FINAL_DESTINATION".
                            if (elevators[k].direction==1-person_direction)
                            {
                                elevators[k].final_destination=max(elevators[k].final_destination,person_current_floor);
                            }       
                                
                            if (elevators[k].idle==YES) // if the elevator is idle and in a lower level, we need to send it to the person right away.
                            {   
                                //printf("okok \t");
                                indicator=(elevators[k].current_floor>person_current_floor);
                                elevators[k].direction=indicator;
                                elevators[k].final_destination=person_current_floor;
                                Load_Event(&event_head,tnow, SEND_ELEVATOR , ELEVATOR, k);  // add ELVEVATOR_ARRIVAL event into the list. this means that elevator is in the level of the person and ready to move in DOWN direction and take that person
                                //hp: check following two lines to make sure you need them.
                                people[next_event.entity_type][next_event.entity_index].elevator_ind = k;
                                elevators[k].idle=NO;          
                            }  
                            break;  
                        }
                }                    
                
            }


/**************************************************************************************************************************
Part 2: Summary statistics go down here.
*************************************************************************************************************************/


            if (num_events_on_calendar == 0) //this indicates this simulation replication is over
                keepgoing = 0;

        } //end while keepgoing
         // records all stats for each replication 

        
        //record KPIs from this replication

    }  //end for each rep
    //print stats results
    
    // Calculte the total wait tiem for all peopel
    double total_wait=0.0;
    for (my_type=0;my_type<3;my_type++)
    {
        for (my_index=0;my_index<1000;my_index++)
        {
            total_wait=total_wait+people[my_type][my_index].elevator_wait_time;
        }
    }

    Print_Calendar();
    fclose(output_file);
}//end for main()

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
            people[PATIENT][total_pats].direction = UP;
            people[DOCTOR][total_docs].to_floor = clinics[clinic_index].floor;
            people[DOCTOR][total_docs].start_time = clinics[clinic_index].doc_start_times[i];
            people[DOCTOR][total_docs].end_time = clinics[clinic_index].doc_end_times[i]; 
            people[DOCTOR][total_docs].appointment_duration = clinics[clinic_index].doc_end_times[i]; // add them to the list when the want to come back 
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
    int storey=20 ;
    int counter=0;
    int i, j;

    tnow = 0; //sms: tnow (meant to indicate "time now") is the variable I use in simulation models to represent the "clock time since the replication start"
    //sms: I think you are using elevator_clock to get at this idea, but there should just be the one "clock" variable (not one for each elevator)
    //sms: basically, tnow gets updated each time we take the next soonest event off the event calendar.

    num_events_on_calendar = 0; // @ t=0 no one wait for elevator in any direction in any floor

    for (i = 0; i < NUM_DIRECTIONS; i++)
        for (j = 0; j < NUM_FLOORS; j++)
            people_waiting_elevator[i][j] = 0;

    for (i=0;i<NUM_FLOORS;i++)
        {queue_size[UP][i]=0;
        queue_size[DOWN][i]=0;
        }
    for (i = 0; i < NUM_ELEVATORS; i++) // @ t=0 all elevators are at their idle situations and their t=0 floor idle
    {   
        elevators[i].num_people = 0;
        elevators[i].idle = 1; // idle can get binary values , 1 == True
        elevators[i].current_floor = elevators[i].floor_idle[0]; //set the current floor of elevators as the idle floor assigned at time 0.
        elevators[i].final_destination = 0;
        //elevators[i].counter = 0; 

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
            people[STAFF][total_staff].direction = UP;
            people[STAFF][total_staff].to_floor = clinic_floor;
            people[STAFF][total_staff].start_time = STAFF_START_TIME;
            people[STAFF][total_staff].end_time = STAFF_END_TIME;
            people[STAFF][total_staff].appointment_duration = STAFF_END_TIME; //  add to the list when they are done as departure clinic event
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
                people[PATIENT][total_pats].direction = UP;
                people[PATIENT][total_pats].to_floor = clinic_floor;   //when entering lobby, the to floor is the clinic they are going to
                people[PATIENT][total_pats].start_time = clinics[i].doc_start_times[j] + k * appointment_duration;
                people[PATIENT][total_pats].appointment_duration = appointment_duration; // assume uniform duration time for each patient
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
int Elevator_Available(elevator elevs[], int floor, int direction)
{
    int i, num_avail = 0;
    double unif, equal_probs;

    for (i = 0; i < NUM_ELEVATORS; i++)
    {
        //if ((elevs[i].idle == 1) && (elevs[i].current_floor == floor))
        // hp: I have removed the condition (elevs[i].idle == 1) (by commenting above line) and added (elevs[i].num_people < ELEVATOR_CAPACITY) instead. 
        //hp: This will take care of the case where before elevator door gets closed, someone arrives.
        if ((elevs[i].num_people < ELEVATOR_CAPACITY) && (elevs[i].current_floor == floor))
            {
                if ((elevs[i].direction== direction)||(elevs[i].idle== YES))
                    {num_avail++;}
            }
    }

    if (num_avail == 0) // no  elevator available in the same floor 
        return 0;
    else //pick one of the available ones with equal prob
    {
        equal_probs = 1.0 / num_avail;
        unif = Unif();

        num_avail = 0;
        for (i = 0; i < NUM_ELEVATORS; i++)
        {
            //if ((elevs[i].idle == 1) && (elevs[i].current_floor == floor))
            if ((elevs[i].num_people < ELEVATOR_CAPACITY) && (elevs[i].current_floor == floor))
            {
                if ((elevs[i].direction== direction)||(elevs[i].idle== YES))
                    {num_avail++;}
                if (unif < num_avail * equal_probs)
                    return i + 1;  //the +1 makes it so that a "true" value (i.e., > 0) is returned even if the elevator with index 0 is chosen.
            }
        }
    }
return 0;  //should never get here
} 

//sms: eventually tidy up code to have one Load_Event function call that handles load_event, load_event_person, and load_event_elevator...there is a lot of duplication and copy/paste
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
void Load_Event_Elevator(struct person_node** head_ref, double time,int person_type, int index, int floor_to, int direction)
{
    struct person_node* current;
    struct person_node* person_ptr;

    person_ptr = (struct person_node*)malloc(sizeof(struct person_node));
    person_ptr->Person.time = time;
    person_ptr->Person.person_type = person_type;
    person_ptr->Person.index = index;
    person_ptr->Person.to_floor = floor_to;

    if (direction==UP)
    {
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
    }

    if (direction==DOWN)
    {
        /* Special case for inserting at the head  */
        if (*head_ref == NULL || (*head_ref)->Person.to_floor <= person_ptr->Person.to_floor)
        {
            person_ptr->next = *head_ref;
            *head_ref = person_ptr;
        }
        else
        {
            /* Locate the node before the point of insertion */  // here we want to locate them based on their floor 
            current = *head_ref;
            while (current->next != NULL && current->next->Person.to_floor > person_ptr->Person.to_floor)
                current = current->next;

            person_ptr->next = current->next;
            current->next = person_ptr;
        }
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
        printf("Elevator: head_ref should never be NULL when calling Remove_Event\n");
        exit(0);
    }

    temp = *head_ref;
    *head_ref = temp->next;
    free(temp);
    num_events_on_elevator--;
}





