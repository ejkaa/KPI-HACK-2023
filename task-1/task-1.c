#include "superkarel.h"
#define SPEED 50

void turn_right(){
    set_step_delay(0);
    turn_left();
    turn_left();
    turn_left();
    set_step_delay(SPEED);
}

void turn_back(){
    set_step_delay(0);
    turn_left();
    turn_left();
    set_step_delay(SPEED);
}

void step_back(){
    set_step_delay(0);
    turn_back();
    step();
    turn_back();
    set_step_delay(SPEED);
}

int big_conditions_and_turns(){ // ked najde okno, pripadne slepu ulicku
    if(left_is_blocked() && right_is_blocked() && no_beepers_present()) { // ak tam vobec nejaka chodba je
        while (left_is_blocked() && right_is_blocked() && front_is_clear()) step(); // chod na koniec chodby
            turn_back();
            if(left_is_blocked() && right_is_blocked()){ // znamena ze tam je slepa ulicka
                while (left_is_blocked() && right_is_blocked()) step();
                turn_left();
                return 0; // pokracuj
            }
            step(); // tu je okno, zastav
    }
    else { // ak nie tak nepreskumavaj, proste je tam okno
        turn_back();
        step();
        turn_left();
    }
    return 1; // zastav
}

void find_window() {
    if (front_is_clear()) step();
    else turn_right();

    while (left_is_blocked() && front_is_clear()) { // popri stene
        if (right_is_blocked()) { // ked okno je na zaciatku steny
            while (right_is_blocked() && left_is_blocked()) step(); // vyjde von oknom
            turn_back();
            step();
            return;
        }
        step();
    } // dosla mu stena abo narazil

    if (front_is_clear()) {
        if(beepers_present()){ // namiesto !beepers_present()
            pick_beeper();
            if(beepers_present()) return;
            else put_beeper();
        }

        step(); // ak mu dosla stena
        if (left_is_blocked()) { // nasiel okno
            step_back();
            turn_left();
            step();
            if (big_conditions_and_turns()) return;
        } else { // je tam schod iba
            step_back();
            turn_left();
        }
    } else { // ak narazil
        if (left_is_clear()) { // je tam okno?
            turn_left();
            step();
            if (big_conditions_and_turns()) return;
        }
        turn_right(); // ak tam proste nie je okno
    }
    find_window();
}

int main(){
    set_step_delay(SPEED);
    turn_on("task-1.kw");

    step();
    while(left_is_blocked() && right_is_blocked()){
        put_beeper();
        step();
    }
    put_beeper();
    turn_left();

    do{
        put_beeper();
        find_window();
    }while(!beepers_present());

    turn_left();
    while(beepers_present()) {
        while(beepers_present()) pick_beeper();
        step();
    }
    step_back();

    turn_off();
    return 0;
}
