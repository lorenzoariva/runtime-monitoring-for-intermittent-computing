#ifndef STATE_MACHINE_POINTER_H
#define STATE_MACHINE_POINTER_H

/* CONSTANT */

#define FILEPATH "var.txt"
#define READ_ONLY "r"
#define MAXSTATUSC 10
#define MAXSTATUS 6
#define TRUE 1
#define FALSE 0
char status[MAXSTATUS][MAXSTATUSC] = {"GOOD", "ERROR", "STARTING", "ENDED"};

int count = 0;


typedef enum {
    ST_IDLE,
    ST_START,
    ST_LOOP,
    ST_NOPROGRESS,
    ST_END
} state_t;

typedef struct {
    state_t currState;
} stateMachine_t;

stateMachine_t stateMachine;

/// \brief      All the possible events that can occur for this state machine.
/// \details    Unlike states_t, these do not need to be kept in a special order.
typedef enum {
    EV_ANY,
    EV_NONE,
    EV_START,
    EV_LOOP,
    EV_FAIL,
    EV_SUCCESS
} event_t;



//DECLARATION
void StateMachine_Init(stateMachine_t * stateMachine);
void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event);
const char * StateMachine_GetStateName(state_t state);

void monitor(int state, int statusNumber, int P);

void clearUp();

void noProgress();

void endTasks();

void loopState();

void startState();


typedef struct {
    const char * name;
    void (*func)(void);
} stateFunctionRow_t;

/// \brief  Maps a state to it's state transition function, which should be called
///         when the state transitions into this state.
/// \warning    This has to stay in sync with the state_t enum!
static stateFunctionRow_t stateFunctionA[] = {
      // NAME           // FUNC
    { "ST_IDLE",        NULL },
    { "ST_START",       &startState },
    { "ST_LOOP",        &loopState },      
    { "ST_NOPROGRESS",  &noProgress },
    { "ST_END",         &endTasks }
};

typedef struct {
    state_t currState;
    event_t event;
    state_t nextState;
} stateTransMatrixRow_t;

static stateTransMatrixRow_t stateTransMatrix[] = {
    // CURR STATE  // EVENT              // NEXT STATE
    { ST_IDLE,      EV_START,       ST_START  },
    { ST_START,     EV_LOOP,        ST_LOOP },
    { ST_LOOP,      EV_FAIL,        ST_NOPROGRESS    },
    { ST_LOOP,      EV_SUCCESS,     ST_END  }
};

#endif