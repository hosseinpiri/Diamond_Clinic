#pragma once

//INTEGER constants
#define EPSILON 0.000001 // for event list
#define NUM_FLOORS 9
#define NUM_CLINICS 56  //VCH clinics...still need to get non-VCH clinics using DC
#define NUM_ELEVATORS 5 //
#define MAX_STAFF_PER_CLINIC 5
#define MAX_DOCS_PER_CLINIC 5
#define MAX_PATS_PER_DOC 8
#define MAX_PEOPLE  (NUM_CLINICS*MAX_DOCS_PER_CLINIC*MAX_PATS_PER_DOC) //2240; the system will have more possible patients than docs or staff
#define ELEVATOR_CAPACITY 4 //made up
#define BUILDING_HOURS 12	
#define BOGUS_VALUE -999

#define EVENT_ENTRIES (MAX_PEOPLE*3)  //the *3 is just to more than cover need. MAX_APPTS*NUM_CLINICS is the max # of patients we are scheduling with arrival times on the event calendar.  will eventually add their departure times to event calendar.  other events like end of simulation.  prob 2* is enough. 
#define LOBBY 0

#define NUM_DIRECTIONS 2 //one for up, one for down...obvious that there are two, this is just to make the code more readable in places instead of using [2] we'll use [NUM_DIRECTIONS] for array declarations, so that we don't have to think what the 2 represents. 
#define UP 0
#define DOWN 1
#define NO_DIRECTION 3

//EVENT types
#define PERSON_ARRIVES_LOBBY 1
#define ELEVATOR_ARRIVAL 2
#define CLINIC_DEPARTURE 3
#define WAITING 4
#define PERSON_READY_LOBBY 5
//PERSON types
#define NUM_PEOPLE_TYPES 3
#define PATIENT 0
#define STAFF 1
#define DOCTOR 2

//FLOATING POINT constants
#define ELEVATOR_VELOCITY 500.0  //feet per minute
#define	FLOOR_DISTANCE 14.0 //distance betweeen consecutive floors, in feet
#define ELEVATOR_DOOR_OPEN_TIME 5.0/60 //in minutes...I'm assuming 5 seconds...we'll use this for door close time too 
#define PERSON_LOAD_TIME 5.0/60 //in minutes...I'm assuming 5 seconds
#define HOURS_OPEN 12.0
#define STAFF_START_TIME 0  //for now i'm just assuming staff start at start of day (minute 0) and finish 8 hours later
#define STAFF_END_TIME 480
#define AVG_MINS_BEFORE_APPT 15.0
#define SD_MINS_BEFORE_APPT 0  //0 to start with a determnistic model to make debugging easier
#define FACILITY_END_TIME (HOURS_OPEN*60.0)
#define NO_SHOW_PROB 0  //0 to start with deterministic model

//SIMULATION RUN parameters
#define REPS 1  //1 for now for debugging...crank this up later

//STRUCTURES
typedef struct
{
	int person_type; //PATIENT, STAFF, OR DOCTOR
	int index; //index of patient array this patient will occupy
	int clinic;
	int to_floor;
	double start_time;  //for patients, this is the scheduled appointment start time; for staff/docs, this is their working start time
	double arrive_to_elevator_time;
	double elevator_wait_time;
	double elevator_start_travel_time;
	double elevator_travel_time;
	double total_time_to_get_clinic ; 
	int elevator_ind; // elevator index that a person move with
	double time; // gonna define

	//these are only relevant if person_type is PATIENT
	int num_visitors; //indicates how many other people (e.g., family) are with this patient.
	int no_show; // = 1 indicates patient doesn't show up for appointment. 
	double appointment_wait_time;
	double appointment; // appointment duration
	//this is only relevant if type is STAFF or DOC
	double end_time; //their planned minutes from 0 to end their shift
} person;

typedef struct 
{
	int current_floor;
	int next_floor;
	int num_people;
	double elevator_time[2]; // 0: elevator going up , 1 : elevator going down , 2 : elevator in idle condition
	double elevator_tot_time;
	int direction;
	int idle;  //binary; 1 = true
	int floor_idle[BUILDING_HOURS]; //indicates which floor this elevator should idel at by hour of day. 
	int floor_to[NUM_FLOORS];  //1 if a button is pressed inside to stop at that floor or elevator is assigned to stop at that floor for a hall pickup
	int person_type[ELEVATOR_CAPACITY]; //indicates whether each person on the elevator is a patient, doctor, or staff
	int person_index[ELEVATOR_CAPACITY];  // indicates person index of that type 
} elevator;

typedef struct
{
	int index;
	int floor;  //i'm assuming lobby is entered as floor 0, and that one story above ground is called floor 1, etc.  
	int num_staff;
	int num_docs;
	int num_pats[MAX_DOCS_PER_CLINIC];
	double doc_start_times[MAX_DOCS_PER_CLINIC];
	double doc_end_times[MAX_DOCS_PER_CLINIC];
} clinic;

typedef struct 
{
	double time;		//minutesp
	int event_type;		//LOBBY ARRIVAL, ELEVATOR ARRIVAL, ETC. 
	int entity_type;	//PATIENT, STAFF, DOCTOR, ELEVATOR
	int entity_index;	//if type is pat, staff, or doc, then this will be the index of that array that holds their info
	int elevator_num;   // used for ELEVATOR ARRIVAL event				
						//if type is elevator, then this is the elevator index
} event;


struct event_node
{
	event Event;		
	struct event_node* next;
} ;  //for creating event calendar as a linked list 

struct person_node
{
	person Person;
	struct person_node* next;
};  //for creating event calendar as a linked list 

//sms: put these into a statistics structure
double elevator_wait_time[NUM_DIRECTIONS];  //we will record this for both patients and staff, where the index 2 is for the UP vs. DOWN direction
double elevator_travel_time[NUM_DIRECTIONS]; // AP: why we need to record which direction elevators are moving? STATS!

//GLOBAL variables/arrays
int people_waiting_elevator[NUM_DIRECTIONS][NUM_FLOORS];
int total_pats=0, total_staff=0, total_docs=0;  //these are totals for the day, for knowing the actual max used index of the structure arrays below
int num_events_on_calendar, dummy;
double tnow;
double elevator_travel_time_per_floor = FLOOR_DISTANCE / ELEVATOR_VELOCITY; //minutes

struct person_node* person_ptr; 
struct event_node* event_head; //starts the linked list for event calendar
struct person_node* elevator_head[NUM_ELEVATORS]; //a linked list of people for each elevator
struct person_node* hall_head[NUM_DIRECTIONS][NUM_FLOORS+1]; //1(lobby)+NUM_FLOORS a linked list of people waiting for elevators in each direction

clinic clinics[NUM_CLINICS];
person people[NUM_PEOPLE_TYPES][MAX_PEOPLE]; 
elevator elevators[NUM_ELEVATORS];

//HEADERS for functions
void Open_and_Read_Files();
void Load_Event(struct event_node**, double, int, int, int);
void Load_Event_Person(struct person_node**, double,int, int);
void Load_Event_Elevator(struct person_node**, double ,int , int , int );
void Remove_Event_Elevator(struct person_node**);
void Remove_Event(struct event_node**);
void Remove_Event_Person(struct person_node**);
void Load_Lobby_Arrivals();
void Initialize_Rep();
void Print_Calendar();
void Send_Elevator(int index , event next_event) ;
int Elevator_Available(elevator elevs[], int floor);

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(x, y) (((x) < (y)) ? (x) : (y))

double wait_time;
int num_events_on_headhall ;
int num_events_on_elevator;

person next_in_Line;
int people_queue_lobby; 
int k ; 