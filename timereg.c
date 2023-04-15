#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

#define MAX 100
#define CLEAN 1
#define N_CLEAN 0

typedef struct {
    char name[MAX];
    double seconds;
} Category;

Category * library[MAX];
int count = 0;
int current_day;

int menu(){
    int choice;
    
    printf("\nPlease insert your option\n\n");
    printf("5 - Show current day\n");
    printf("4 - Close day\n");
    printf("3 - Show data\n");
    printf("2 - Reset\n");
    printf("1 - Add time\n");
    printf("0 - Timer\n\n");
    printf("[-1 for shut down]\n\n$ ");
    
    scanf("%d", &choice);
    getchar();

    return choice;
}

void list_cat(){
    for (int i = count - 1; i >= 0; i--)
        printf("%d - %s\n", i, library[i]->name);
    printf("\n");
}

void calculate_time(double seconds_spent, int * hours, int * minutes, int * seconds){

    *hours = seconds_spent/3600; 
	
	*minutes = (seconds_spent -(3600*(*hours)))/60;
	
	*seconds = (seconds_spent -(3600*(*hours))-((*minutes)*60));

}

void show_data(){
    int hours, minutes, seconds;
    printf("\n");
    for (int i = count - 1; i >= 0; i--){
        calculate_time(library[i]->seconds, &hours, &minutes, &seconds);
        printf("%-20s %dh %dm %ds\n", library[i]->name, hours, minutes, seconds);
    }
}

void timer(){
    int choice = 0;
    struct timeval start, end;
    int hours, minutes, seconds;

    
    printf("\nWelcome to TIMER mode.\nWhich category do you want to time?\n\n");
    list_cat();

    printf("$ ");
    scanf("%d", &choice);
    getchar();  

    printf("\nStarting the timer for %s...\n", library[choice]->name);
    sleep(3);
    printf("\nThe timer for %s has begun!\n[Press ENTER for stopping timer]", library[choice]->name);

    gettimeofday(&start, NULL);

    getchar();

    gettimeofday(&end, NULL);

    double seconds_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 0.000001;

    calculate_time(seconds_spent, &hours, &minutes, &seconds);

    printf("\nTime spent in %s: %dh %dm %ds\n\n", library[choice]->name, hours, minutes, seconds);

    library[choice]->seconds += seconds_spent;

    
}

void add(){
    int choice;
    double hours, minutes, seconds;

    printf("\nWelcome to ADD mode.\nWhich category do you want to add time?\n\n");
    list_cat();

    printf("$ ");
    scanf("%d", &choice);
    getchar();

    printf("\nInsert the number of:\n\n> Hours: ");
    scanf("%lf", &hours);
    getchar();

    printf("> Minutes: ");
    scanf("%lf", &minutes);
    getchar();

    printf("> Seconds: ");
    scanf("%lf", &seconds);
    getchar();

    library[choice]->seconds += hours*60*60 + minutes*60 + seconds;

    printf("\nTime successfully added!\n");

}

void reset(){
    for (int i = count - 1; i >= 0; i--)
        library[i]->seconds = 0;

    printf("\nAll registers have been resetted!\n");
}

void save(char * file_name, int clean){
    //save file
    FILE * fp = fopen(file_name, "w");

    for (int i = count -1; i >= 0; i--){
        fprintf(fp, "%s %f\n", library[i]->name, library[i]->seconds);
        if (clean)
            free(library[i]);
    }

    fclose(fp);
}

void close_day(){
    char name[MAX];
    sprintf(name, "Records/Day%d.txt", current_day);

    current_day++;

    FILE * day = fopen("count.txt", "w");

    fprintf(day, "%d", current_day);

    fclose(day);

    save(name, N_CLEAN);

    reset();

    save("registos.txt", CLEAN);

    printf("\nThe day %d has been successfully closed!\n", current_day-1);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

    FILE * reg = fopen(argv[1], "r");
    FILE * day = fopen("count.txt", "r");
    
    // test for file not existing. 
    if ( reg == NULL || day == NULL) {   
        printf("Error! Could not open file\n"); 
        exit(-1); 
    } 

    char name[MAX];
    double time;
    
    fscanf(day, "%d", &current_day);

    // read file
    while (fscanf(reg, "%s %lf", name, &time) == 2){
        Category * new = (Category*)malloc(sizeof(Category));

        strcpy(new->name, name);
        new->seconds = time;

        library[count++] = new;
    }

    fclose(reg);

    int c = menu();

    while (c != -1){
        
        switch (c) {
            case 0:
                timer();
                break;
            case 1:
                add();
                break;
            case 2:
                reset();
                break;
            case 3:
                show_data();
                break;
            case 4:
                close_day();
                break;
            case 5:
                printf("\nCurrently at day %d.\n", current_day);
                break;
            default:
                printf("No valid option was selected.\n");
                break;
        }
    
        printf("\n[Press ENTER to continue]");
        getchar();
    
        c = menu();
    } 

    //save file
    save("registos.txt", CLEAN);
    
    return 0;
}
