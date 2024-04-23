////////////////////////////////////////////////////////////////////////////////
//
// Path Planning
//
// Description here
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "adc.h"
#include "ir.h"
#include "motors.h"
#include "path_planning.h"

#define GOAL_X_CM 0
#define GOAL_Y_CM 305   // Goal is 10 feet in front of the rover when landed.
#define WOMBAT_STARTING_X_CM 0
#define WOMBAT_STARTING_Y_CM 0


////////////////////////// Public Variables ////////////////////////////////////

struct vector Goal;
struct vector Wombat;

///////////////////// Public Function Prototypes ///////////////////////////////

// Discription here
void path_plan_initialize(void) {
    Goal.X = {GOAL_X_CM, GOAL_Y_CM};

    Wombat.X = {WOMBAT_STARTING_X_CM, WOMBAT_STARTING_Y_CM};
}

vector update_path(vector current) {

    struct vector direction;
    
    if (current == Goal) {
        // Arrived at goal, end program
        direction = {0, 0};
    }
    else {
        direction = {Goal.X-current.X, Goal.Y-current.Y};
    }

    return 
}