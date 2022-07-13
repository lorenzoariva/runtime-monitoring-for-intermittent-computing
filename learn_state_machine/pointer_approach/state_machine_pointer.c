#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state_machine_pointer.h"

/* state machine pointer approach functions */
void StateMachine_Init(stateMachine_t * stateMachine) {
    printf("Initialising state machine.\r\n");
    stateMachine->currState = ST_IDLE;
}

void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event) {

    // Iterate through the state transition matrix, checking if there is both a match with the current state
    // and the event
    for(int i = 0; i < sizeof(stateTransMatrix)/sizeof(stateTransMatrix[0]); i++) {
        if(stateTransMatrix[i].currState == stateMachine->currState) {
            if(stateTransMatrix[i].event == event || stateTransMatrix[i].event == EV_ANY) {
                // Transition to the next state
                stateMachine->currState =  stateTransMatrix[i].nextState;

                // Call the function associated with transition
                (stateFunctionA[stateMachine->currState].func)();
                break;
            }
        }
    }
}

const char * StateMachine_GetStateName(state_t state) {
    return stateFunctionA[state].name;
}

/* states functions */
/* monitor function to print in output the status of the current state */
void monitor(int state, int statusNumber, int P){
    if(P==-1){
        printf("[%d]  %s\n",state, status[statusNumber]);
    } else {
        printf("[%d]  %s  P:%d\n",state, status[statusNumber],P);
    }
}

/* Either in case of error or success this function reset to 0 the count value in var.txt */
void clearUp(){
    FILE *file = fopen(FILEPATH, "w");
    fprintf(file,"0\n");
}

/*  
    [STATE 2] If the state [1] try more than 3 times to end up the for cycle,
    state [2] is called. This state clearUp the var.txt and print out its current status. 
*/
void noProgress(){
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));
    monitor(2,2,-1);
    clearUp();
    monitor(2,3,-1);
}

/* [STATE 3] in case of success, state 3 is called, clearUp var.txt, print the number of trials (count) and its status */
void endTasks(){
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));
    monitor(3,2,-1);
    printf("COUNT: %d\n",count);
    clearUp();
    monitor(3,3,-1);
}

/* 
    [STATE 1] it's the "loop state", it tries to complete a for cycle based on wait 2 seconds every iteration.
    In case of success state [3] is called, in case of error (to many trials) state [2] is called.
*/
void loopState(){
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));
    monitor(1,2,-1);
    count++;
    FILE *file = fopen(FILEPATH, "w");
    fprintf(file,"%d\n",count);
    fclose(file);

    if((count)<4){
        
        for(int i=0; i<5; i++){
            monitor(1,0,i);
            sleep(2);
        }
        //Success case, pass to state [3]
        monitor(1,3,-1);
        // Success
        printf("Success.\r\n");
        StateMachine_RunIteration(&stateMachine, EV_SUCCESS);
    } else {
        //Error case, pass to state [2]
        monitor(1,1,-1);
        // Fail
        printf("Fail.\r\n");
        StateMachine_RunIteration(&stateMachine, EV_FAIL);
    }
    

}

/*
    [STATE 0] is the inital state.
    Check if the file var.txt already exists:
        YES ->  open the file and read the value of the counts.
        NO  ->  create a new file and set the value of the counts to 0.
    Print its status.
    Call the loopState (state [1]).
*/

void startState(){
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));
    monitor(0,2,-1);
    char reading[2];
    FILE *vfile;
    
    if(fopen(FILEPATH, "r")){
        vfile = fopen(FILEPATH, "r");
        fgets(reading, 2, vfile);
    } else {
        vfile = fopen(FILEPATH, "w");
        fprintf(vfile,"0\n");
    }
    
    
    count = atoi(reading);
    fclose(vfile);

    //Pass to monitor and then to [1]
    monitor(0,0,-1);
    monitor(0,3,-1);

    // Start loop
    printf("Enter loop.\r\n");
    StateMachine_RunIteration(&stateMachine, EV_LOOP);

}


int main(){
    char asw[2];
    int correct = FALSE;

    while(!correct){
        printf("Do you want to start the program? [y/n]\n");
        scanf("%s",asw);
        if(asw[0]=='y'){

            StateMachine_Init(&stateMachine);
            printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));

            // Event Start
            printf("Start event.\r\n");
            StateMachine_RunIteration(&stateMachine, EV_START);

            correct = TRUE;
        } else if(asw[0]=='n') {
            correct = TRUE;
        }
    }
    
    return 0;

}