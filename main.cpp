#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <csignal>

using namespace std;

const int PARKING_SIZE = 20;
const int CAR_NUMBER = 30;

vector<int> parking(PARKING_SIZE);
vector<pthread_mutex_t> lockers(PARKING_SIZE);

void print_error(string err_message){
    perror(err_message.c_str());
    exit(EXIT_FAILURE);
}

void *take_place(void *arg) {
    pthread_t current_thread = pthread_self();

    while(true) {
        int place_number = -1;
        //cout << "Car with id " << current_thread << " starts searching parking lot" << endl;

        for (int i = 0; i < PARKING_SIZE && place_number == -1; i++) {
            pthread_mutex_lock(&lockers[i]);
            if (parking[i] == 0) {
                parking[i] = 1;
                place_number = i;
               // cout << "Place " << i << " occupied by " << current_thread << endl;
            }

            pthread_mutex_unlock(&lockers[i]);
            sleep(1);
        }

        if (place_number != -1) {
            pthread_mutex_lock(&lockers[place_number]);

           // cout << "Place " << place_number << " is now free" << endl;
            parking[place_number] = 0;

            pthread_mutex_unlock(&lockers[place_number]);
        }
    }
}

int main() {
    int err;
    vector<pthread_t> cars(CAR_NUMBER);

    for (int i = 0; i < PARKING_SIZE; i++) {
        parking[i] = 0;
    }

    for(int i = 0; i < PARKING_SIZE; i++) {
        if (pthread_mutex_init(&lockers[i], 0)) {
            print_error("Can`t init mutex");
        }
    }

    for (int i = 0; i < CAR_NUMBER; i++) {
        pthread_t thread;
        err = pthread_create(&thread, NULL, take_place, NULL);
        cars[i] = thread;
        if (err != 0) {
            print_error("Can`t create thread");
        }
    }

    while (true) {
        for (int i = 0; i < PARKING_SIZE; i++) {
            cout << parking[i] << " ";
        }
        cout << endl;
        sleep(1);
    }

    for(int i = 0; i < CAR_NUMBER; i++){
        err = pthread_join(cars[i], NULL);
        if(err != 0){
            print_error("Can`t close thread");
        }
    }

    for(int i = 0; i < PARKING_SIZE; i++) {
        if(pthread_mutex_destroy(&lockers[i])){
            print_error("Can`t destroy mutex");
        }
    }

}