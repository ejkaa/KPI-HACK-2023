#include "superkarel.h"
#define SPEED 1

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

int count_beepers(){ // pozbiera, spocita
    int count = 0;
    while(beepers_present()){
        pick_beeper();
        count++;
    }
    for (int i = 0; i < count; ++i) // vrati ich naspat
        put_beeper();
    return count; // vrati pocet beeperov pod sebou
}

int find_frame(int frame_num){
    while (not_facing_west()) turn_left();
    while (front_is_clear()) step();
    turn_left();
    while (front_is_clear()) step(); // ide na start
    turn_back();

    while (front_is_clear()) {
        while (front_is_clear()){ // ide hore, hlada targetnuty frame
            step();
            if(count_beepers() == frame_num) return 1;
        } // ak ho nenasiel
        turn_back();
        while (front_is_clear()) step(); // ide dole
        turn_left();
        if(front_is_clear()){ // ak sa da, ide do dalsieho stlpca
            step();
            turn_left();
        }
    }
    return 0; // ak ho nenasiel vobec
}

int find_xy(int index){ // zisti vysku y / sirku x
    int xy = 0;
    int step_counter = 0;

    while (front_is_clear()){
        step();
        step_counter ++; // tolko presiel, o tolko sa vrati
        if(count_beepers() == index) xy = step_counter; // zapise najvacsiu vzdialenost dvoch rovnakych policok
    }
    turn_back();
    for (int i = 0; i < step_counter; ++i) step(); // vrati sa
    return xy;
}

int inside(int number, int len, int array[len]){ // vrati 1, ak je cislo v poli
    for (int i = 0; i < len; ++i)
        if (array[i] == number) return 1;
    return 0;
}

void scan_frame(int frame, int higher_frames[5], int x, int y){ // zisti ktore framey su nad nim
    int index = 0;
    for (int side = 0; side < 4; ++side) { // prejde kazdu hranu
        int len = y;
        if(side%2 == 0) len = x;

        for (int i = 0; i < len; ++i) {
            step();
            int count = count_beepers();

            if (count != frame) { // ak najde iny frame
                if(!inside(count, 5, higher_frames)){
                    higher_frames[index] = count; // zapise ho
                    index ++;
                }
            }
        }
        turn_left();
    }
}

void pick_all_beepers(){
    while (not_facing_east()) turn_left();
    while (front_is_clear()) step();
    turn_left();
    while (front_is_clear()) step(); // najde pravy horny roh
    turn_left();

    int check = 1;
    while(check){
        if(right_is_blocked()) turn_left();
        else if(left_is_blocked()) turn_right(); // otoci sa smerom do pola
        while (front_is_clear()){
            while (beepers_present()) pick_beeper(); // pozbiera beepre
            step();
        }
        if((facing_north() && left_is_blocked()) || (facing_south() && right_is_blocked())) check = 0; // check ci nie je na konci
        while (beepers_present()) pick_beeper(); // posledne policko
        while (not_facing_west()) turn_left();
        if(front_is_clear()) step();
    }
}

void print_results(int result[5]){
    while (not_facing_south()) turn_left();
    while(front_is_clear()) step(); // pride na start
    turn_left();
    for (int i = 0; i < 5; ++i) { // prejde piatimi polickami
        for (int j = 0; j < result[i]; ++j) put_beeper(); // zapise result
        if(front_is_clear()) step();
        else break; // ak by bolo pole mensie ako 5
    }
    while(front_is_clear()) step(); // dojde tam kde ma byt na konci
}

int count_not_zero(int array[5]) { // zisti kolko nenulovych prvkov je v poli
    int count = 0;
    for (int i = 0; i < 5; ++i)
        if (array[i] != 0)
            count++;
    return count;
}

void find_max_rows(int max, int max_rows[5], int array_2d[5][5], int frames_count){
    int index = 0;
    for (int i = 0; i < frames_count; ++i) {
        if(count_not_zero(array_2d[i]) == max){
            max_rows[index] = i+1;
            index ++;
        }
    }
}

void write_two(int max_rows[5], int array_2d[5][5], int result_index, int result[5], int x, int y){
    int check = inside(max_rows[x], 5, array_2d[max_rows[y]-1]);
    result[result_index] = check ? max_rows[y] : max_rows[x];
    result[result_index + 1] = check ? max_rows[x] : max_rows[y];

    for (int j = 0; j < 2; ++j) { // premaze obidva riadky
        for (int i = 0; i < 5; ++i)
            array_2d[max_rows[j]-1][i] = 0;
    }
}

int remove_row(int max_rows[5], int array_2d[5][5]){
    if(!inside(max_rows[0], 5, array_2d[max_rows[1]-1]) && !inside(max_rows[0], 5, array_2d[max_rows[2]-1])) return 0;
    if(!inside(max_rows[1], 5, array_2d[max_rows[2]-1]) && !inside(max_rows[1], 5, array_2d[max_rows[0]-1])) return 1;
    if(!inside(max_rows[2], 5, array_2d[max_rows[0]-1]) && !inside(max_rows[2], 5, array_2d[max_rows[1]-1])) return 2;
    return 0;
}

void sort_final_result(int array_2d[5][5] , int result[5] ) {
    int frames_count = 1;
    for (int i = 0; i < 5; ++i)
        if (array_2d[i][0] != 0) frames_count++; // spocita kolko je vo svete framov

    int result_index = 0;
    for (int i = 0; i < frames_count; ++i) { // ide zapisovat do resultu, ale len tolkokrat kolko je framov

        int max = 0;
        for (int j = 0; j < frames_count; ++j) {
            if (count_not_zero(array_2d[j]) > max) {
                max = count_not_zero(array_2d[j]); // najde kolko je maximum nenulovych hodnot
            }
        }
        if (max == 0) break; // ked prejde uz vsetky nenulove riadky

        int max_rows[5] = {0, 0, 0, 0, 0};
        find_max_rows(max, max_rows, array_2d, frames_count);

        if (count_not_zero(max_rows) == 1) { // ak je v max_rows len jedno cislo
            result[result_index] = max_rows[0]; //zapise ho
            result_index++;
            for (int j = 0; j < 5; ++j) // premaze ten riadok
                array_2d[max_rows[0] - 1][j] = 0;

        } else if (count_not_zero(max_rows) == 2) { // ak su tam 2 cisla
            write_two(max_rows, array_2d, result_index, result, 0, 1);
            result_index += 2;

        } else if (count_not_zero(max_rows) == 3) {
            int removed_row = remove_row(max_rows, array_2d);

            result[result_index] = max_rows[removed_row];
            result_index ++;
            for (int j = 0; j < 5; ++j) // premaze ten riadok
                array_2d[max_rows[removed_row] - 1][j] = 0;

            if (removed_row == 0)
                write_two(max_rows, array_2d, result_index, result, 1, 2);
            if (removed_row == 1)
                write_two(max_rows, array_2d, result_index, result, 0, 2);
            if (removed_row == 2)
                write_two(max_rows, array_2d, result_index, result, 0, 1);
            result_index += 2;
        }
    }
    if(result_index >= frames_count) result_index--;
    for (int j = 0; j <= frames_count; ++j)
        if (!inside(j, 5, result))
            result[result_index] = j;
}

int main(){
    set_step_delay(SPEED);
    turn_on("task-2.kw");

    int result[] = {0,0,0,0,0}; // pole ktore vypise na prvy riadok
    int array_2d[5][5]= { // pole, ktore budem vyhodnocovat na konci
            {0,0,0,0,0},
            {0,0,0,0,0},
            {0,0,0,0,0},
            {0,0,0,0,0},
            {0,0,0,0,0},
    };

    turn_left();
    for (int i = 1; i <= 5 ; ++i) {
        if(find_frame(i) == 0) break; // ak nenajde frame, je ich menej ako 5

        int y = find_xy(i); turn_left();
        int x = find_xy(i); turn_back();

        int higher_frames[5] = {0,0,0,0,0};
        scan_frame(i, higher_frames, x, y); // zisti ktore framey su nad nim

        for (int j = 0; j < 5; ++j) // a zapise to
            array_2d[i-1][j] = higher_frames[j];
    }

    pick_all_beepers();
    sort_final_result(array_2d, result);
    print_results(result);

    turn_off();
    return 0;
}
