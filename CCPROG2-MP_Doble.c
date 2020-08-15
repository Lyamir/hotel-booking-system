#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX25 26
#define MAX40 41

struct roomdata{ 		//This structure stores all data used by each unique room code
	char code; 			// This variable stores the unique room code
	int capacity; 		// This variable stores how any guests may stay inside the type of room
	int limit[8]; 		// This variable stores the number of rooms available for the room code and is separated per day (index 0-6) 
						//and index 7 stores the original number of rooms available per day to be used in generateReport() 
	float weekdayrate;	//This variable stores the weekday rate of the room in PHP
	float weekendrate;	//This variable stores the weekend rate of the room in PHP
	float perguestrate;	//This variable stores the additional rate of the room per additional guest in PHP
};

struct personalinfo{		//This structure stores the personalinfo of the person who booked the room
	char lastname[MAX25];	//This variable stores the last name of the person who booked the room
	char firstname[MAX25];	//This variable stores the first name of the person who booked the room
	char email[MAX40];		//This variable stores the email of the person who booked the room
};

struct bookingreq{				//This structure stores a booking request
	struct personalinfo info;	//This structure uses struct personalinfo to store the personal info of the person who issued a booking request
	char code;					//This variable stores the room code of the desired room to be booked
	int guests;					//This variable stores how many additional guests there are in the booking request
	int dow;					//This variable stores the starting day of the room booking
	int days;					//This variable stores the number of days where in the person who issued the booking will be staying
	
};

void readtext(struct roomdata arr[], int *nlines){
	/*. 	Function Name: 	readtext
			Purpose: 		This function reads the rooms.txt file and puts the data read into arr[]. 
							The function also increments *nlines per line of data read from rooms.txt
			Parameters: 	struct roomdata arr[] is an array of roomdata from main. The data from rooms.txt will be stored into the array
							and will be used by the other functions.
							int *nlines is the number of lines in the rooms.txt file which is also the number of unique room codes which will
							be used by the other functions
	*/
		int i, j; //for loop counters
		int totalrooms = 0; //counts the total number of rooms available in one day
		struct roomdata room; //temporary structure for the scanned data in rooms.txt 
		
		printf("Loading hotel information from rooms.txt: ");
		FILE *fptr; //file pointer
		
		fptr =fopen("rooms.txt", "r");

		if (fptr == NULL) {
			printf("Failed!\n");
			printf("The rooms.txt file is not found or missing.\n");
			printf("Exiting the program. Bye!");
			exit(1);
		}
		
		else{
			
			while(fscanf(fptr, "%c %d %d %f %f %f\n", &room.code, &room.capacity, &room.limit[0], &room.weekdayrate, &room.weekendrate, &room.perguestrate) == 6){
				
				arr[*nlines] = room;
				
				for(j = 1; j < 8; j++){
					arr[*nlines].limit[j] = room.limit[0];
				}
				*nlines = *nlines + 1;
			}	
		}
		
		fclose(fptr);
				
		for (j = 0; j < *nlines; j++) {
			totalrooms += arr[j].limit[0];
		}
		
		for(j = 0; j < *nlines; j++){
			if(arr[j].code < 65 || arr[j].code > 90){
				printf("Failed!\n");
				printf("Found an invalid \"room code\" at line %d\n", j+1);
				printf("Exiting the program. Bye!");
				exit(1);
			}
		}

		if (totalrooms == 0) {
			printf("Success!\n");
			printf("The hotel has 0 rooms available.\n");
			printf("No eligible rooms were found in the input file exiting the program. Bye!");
			exit(1);
		}

		for (i = 0; i < *nlines; i++) {
			for (j = 0; j < *nlines; j++)
				if (i != j)
					if (arr[i].code == arr[j].code) {
						printf("Success!\n");
						printf("The hotel \"room codes\" are not unique.\n");
						printf("Exiting the program. Bye!");
						exit(1);
					}
		}
		
		printf("Success!\n");
		printf("There are a total of %d room/s available\n\n", totalrooms);

}

int bookingChecker(struct roomdata D[], struct bookingreq temp, int *nlines){
	
	int i, index;
	int found = 0;
		for(i = 0; i < *nlines; i++){
			if(temp.code == D[i].code){
				index = i;
				found = 1;
				break;
			}
		}
			if(!found){
				printf("\nSorry I am unable to process this request due to invalid room code\n\n");
				return 0;
			}
			
				for(i = temp.dow; i < (temp.days + temp.dow); i++){
				 if (D[index].limit[i] == 0){
						printf("\nSorry I am unable to process this request due to room availability\n\n");
						return 0;
					}				
				}
						
			if (temp.guests > D[index].capacity || temp.guests < 1){
				printf("\nSorry, I am unable to satisfy this request due to room capacity\n\n");
				return 0;
					}
				
			else if (temp.dow > 6 || temp.dow < 0){
				printf("\nSorry, I am unable to satisfy this request due to invalid starting day\n\n");
				return 0;
					}
				
			else if (temp.days  > (7 - temp.dow) || temp.days < 0){
				printf("\nSorry, I am unable to satisfy this request do to invalid # of days to stay\n\n");
				return 0;
			}				
		
		return 1;
			
}

void dayConverter(int day){
	switch(day){
		case 0: printf("Monday\n");break;
		case 1: printf("Tuesday\n");break;
		case 2: printf("Wednesday\n");break;
		case 3: printf("Thursday\n");break;
		case 4: printf("Friday\n");break;
		case 5: printf("Saturday\n");break;
		case 6: printf("Sunday\n");break;
	}
}

float getAmount(struct bookingreq req, struct roomdata room[], int *nlines){
	int i;
	int code;
	float amount = 0;
	int weekend = 0;
	int weekday = 0;
	
	for(i = 0; i < *nlines; i++){
		if(room[i].code == req.code){
			code = i;
			break;
		}		
	}
	
	for(i = req.dow; i <= req.days + req.dow - 1; i++){
		if (i == 5 || i == 6)
			weekend++;
		else
			weekday++;
	}
	if (weekend != 0){
		amount = weekend * (room[code].weekendrate + (req.guests-1) * room[code].perguestrate);
	}
	
	if (weekday != 0){
		amount = amount + weekday * (room[code].weekdayrate + (req.guests-1) * room[code].perguestrate);
	}
	return amount;
}

void upperCase(char lastname[MAX25]){
	int i;
	for(i = 0; i < strlen(lastname); i++)
		if(lastname[i] >= 97 && lastname[i] <= 122)
			lastname[i] = lastname[i] - 32;
}

void generateEmail(struct bookingreq req, struct roomdata room, int bookingnum, float total){
	FILE *fptr;
	
	char filename[MAX25];
	char num[3];
	char name[MAX25];
	int weekdays = 0;
	int weekends = 0;
	int i;
	
	system("mkdir emails\\");
	
	strcpy(name, req.info.lastname);
	upperCase(name);
	strcpy(filename, "emails/");
	strcat(filename, name);
	itoa(bookingnum, num, 10);
	strcat(filename, "-");
	strcat(filename, num);
	strcat(filename, ".txt");
	
	fptr = fopen(filename, "w");
	if(fptr != NULL){
		fprintf(fptr, "Dear %s %s,\n\n", name, req.info.firstname);
		
		fprintf(fptr, "Thank you for booking your stay with us.\n");
		fprintf(fptr, "Your booking reference number is %d.\n", bookingnum);
		fprintf(fptr, "The e-mail address used for this booking is %s.\n\n", req.info.email);
		
		fprintf(fptr, "Booking Details: \n");
		fprintf(fptr, "\tRoom Code: %c\n", req.code);
		fprintf(fptr, "\tNumber of Guests: %d\n", req.guests);
		fprintf(fptr, "\tFirst day of booking: ");
		switch(req.dow){
			case 0: fprintf(fptr, "Monday\n");break;
			case 1: fprintf(fptr, "Tuesday\n");break;
			case 2: fprintf(fptr, "Wednesday\n");break;
			case 3: fprintf(fptr, "Thursday\n");break;
			case 4: fprintf(fptr, "Friday\n");break;
			case 5: fprintf(fptr, "Saturday\n");break;
			case 6: fprintf(fptr, "Sunday\n");break;
		}
		fprintf(fptr, "\tNumber of days: %d\n", req.days);
		
		for(i = req.dow; i < req.dow + req.days; i++){
			if(i == 5 || i == 4)
				weekdays++;
			else
				weekends++;
		}
		fprintf(fptr, "\tNumber of weekdays: %d\n", weekdays);
		fprintf(fptr, "\tNumber of weekends: %d\n\n", weekends);
		
		fprintf(fptr, "Fees:\n");
		fprintf(fptr, "\tWeekday room rate: %.2f\n", room.weekdayrate);
		fprintf(fptr, "\tWeekend room rate: %.2f\n", room.weekendrate);
		fprintf(fptr, "\tTotal cost for weekday stay: %.2f\n", room.weekdayrate * weekdays);
		fprintf(fptr, "\tTotal cost for weekend stay: %.2f\n", room.weekendrate * weekends);
		fprintf(fptr, "\tNumber of guests above the first: %d\n", req.guests - 1);
		fprintf(fptr, "\tTotal cost per guest: %.2f\n", room.perguestrate * req.days);
		fprintf(fptr, "\tTotal guest cost: %.2f\n\n", room.perguestrate * req.days * (req.guests - 1));
		
		fprintf(fptr, "\tGrand Total: %.2f\n\n", total);
		
		fprintf(fptr, "Thank you for choosing Junjun's Hotel. We hope you have a pleasant stay.");
		
		fclose(fptr);
	}
}

void generateReport(struct bookingreq req[], struct roomdata room[], int *nlines){
	FILE *fptr;
	int i, j;
	int roomweekday, bookedweekday, roomweekend, bookedweekend ;
	float occupancy;
	fptr = fopen("report.txt", "w");
	
	if (fptr != NULL){
		fprintf(fptr,"Room Reservation Report\n\n");
		for(i = 0; i < *nlines; i++){
			roomweekday = (room[i].limit[7]) * 5;
			bookedweekday = 0;
			bookedweekend = 0;
			roomweekend = room[i].limit[7] * 2;
			for(j = 0; j < 7; j++){
				if(j >= 0 && j <= 4){
					if(room[i].limit[j] != room[i].limit[7])
						bookedweekday = bookedweekday + (room[i].limit[7] - room[i].limit[j]);
				}
				else{
					if(room[i].limit[j] != room[i].limit[7])
						bookedweekend = bookedweekend + (room[i].limit[7] - room[i].limit[j]);
				}		
			}
			occupancy = (((bookedweekday + bookedweekend) * 1.0) / ((roomweekday + roomweekend) * 1.0)) * 100;
			fprintf(fptr,"%d, %c, %d, %d, %d, %d, %.2f%%\n", i+1, room[i].code, roomweekday, bookedweekday, roomweekend, bookedweekend, occupancy);
		}
	}
}

void bookingReq(struct roomdata arr[], struct bookingreq req[], int *reqCtr, int *nlines){
	int bookingnum = *reqCtr + 1;
	int i, j, num, dts;
	int sbookingreq = 1;
	struct bookingreq temp;
	float amount;
	
		printf("\nEnter Booking Request #%d\n", bookingnum);
		
		fprintf(stdout, "last name: ");
		gets(temp.info.lastname);
		
		while(temp.info.lastname[0] != '\0'){
			fprintf(stdout, "first name: ");
			gets(temp.info.firstname);
			
			fprintf(stdout, "email address: ");
			gets(temp.info.email);
			
			fprintf(stdout, "room code: ");
			fscanf(stdin, "%c", &temp.code);
			
			for (i = 0; i < *nlines; i++){
				if (arr[i].code == temp.code)
					num = i;
			}
			
			fprintf(stdout, "total # of guest (max %d): ", arr[num].capacity);
			fscanf(stdin, "%d", &temp.guests);
		
			fprintf(stdout, "starting day (0 - Monday | 1 - Tuesday | 2 - Wednesday...): ");
			fscanf(stdin, "%d", &temp.dow);
			
			dts = 7 - temp.dow;
		
			fprintf(stdout, "# of days to stay (max %d): ", dts);
			fscanf(stdin, "%d", &temp.days);
		
			if(bookingChecker(arr, temp, nlines)){
					strcpy(req[*reqCtr].info.lastname, temp.info.lastname);
					strcpy(req[*reqCtr].info.firstname, temp.info.firstname);
					strcpy(req[*reqCtr].info.email, temp.info.email);
					req[*reqCtr].code = temp.code;
					req[*reqCtr].guests = temp.guests;
					req[*reqCtr].dow = temp.dow;
					req[*reqCtr].days = temp.days;
					for(i = 0; i < *nlines; i++){
						if(arr[i].code == temp.code){
							for(j = 0; j < 7; j++){
								if(j >= temp.dow && j < (temp.days + temp.dow)){
									arr[i].limit[j] = arr[i].limit[j] - 1;
								}
							}
							break;
						} 		
					}
					
					amount = getAmount(req[*reqCtr], arr, nlines);
					
					printf("\nSuccessfully booked a room for %d guest/s for %d day/s of stay starting ", req[*reqCtr].guests, req[*reqCtr].days);
					dayConverter(req[*reqCtr].dow);
					
					printf("Total amount for this booking in PHP: %.2f\n", amount);
					
					generateEmail(req[*reqCtr], arr[i], sbookingreq, amount);
					
					sbookingreq++;
					*reqCtr = *reqCtr + 1;
					
			}
		
			printf("%s", req[*reqCtr].info.firstname);
		
			bookingnum++;
			printf("\nEnter Booking Request #%d\n", bookingnum);
		
			fprintf(stdout, "last name: ");
			gets(temp.info.lastname);
			gets(temp.info.lastname);
			
		}
		
		
		printf("Processed %d successful requests out of %d submitted requests\n", sbookingreq-1, bookingnum-1);
		printf("Generating management report\n");
		generateReport(req, arr, nlines);
		printf("Thank you for using the hotel reservation system\n");
		printf("Bye!");
		
}


int main(){
		struct bookingreq br[70];
		struct roomdata arr[5];
		int bookingCtr = 0;
		int nlines = 0;
		readtext(arr, &nlines);

		bookingReq(arr, br, &bookingCtr, &nlines);

		return 0;
	}



