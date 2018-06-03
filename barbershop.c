#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#ifndef NUM_CHAIRS
#define NUM_CHAIRS 4
#endif

int customers = 0;
sem_t mutex;
sem_t customer;
sem_t barber;
sem_t customer_done;
sem_t barber_done;

void * customer_thread(void* args) {
  int id = rand();
  while(1) {
    sem_wait(&mutex);
    printf("Customer %d is entering the barbershop. There are %d customers already there.\n", id, customers);
    fflush(stdout);
    if (customers == NUM_CHAIRS) {
      printf("Customer %d is leaving the barbershop because it is full\n", id);
      fflush(stdout);
      sem_post(&mutex); 
    } else {
      printf("Customer %d is waiting for the barber\n", id);
      fflush(stdout);
      customers += 1;
      sem_post(&mutex);

      sem_post(&customer);
      sem_wait(&barber);

      printf("Customer %d is getting their hair cut\n", id);
      fflush(stdout);

      sem_post(&customer_done);
      sem_wait(&barber_done);

      sem_wait(&mutex);
      customers -= 1;
      sem_post(&mutex);
      printf("Customer %d is finished getting their hair cut\n", id);
      fflush(stdout);
    }
    sleep(25);
  }

}

void * barber_thread(void* args) {
  while (1) {
    sem_wait(&customer);
    sem_post(&barber);

    printf("The barber is cutting a customer's hair\n");
    fflush(stdout);
    sleep(5);
    printf("The barber is done cutting a customer's hair\n");
    fflush(stdout);

    sem_wait(&customer_done);
    sem_post(&barber_done);
  }
}

int main(int argc, char* argv[]) {
  pthread_t customers[NUM_CHAIRS * 2];
  srand(time(NULL));
  sem_init(&mutex, 0, 1);
  sem_init(&customer, 0, 0);
  sem_init(&barber, 0, 0);
  sem_init(&customer_done, 0, 0);
  sem_init(&barber_done, 0, 0);
  printf("Running with %d customers and %d chairs\n", NUM_CHAIRS * 2, NUM_CHAIRS);
  for (int i = 0; i < NUM_CHAIRS * 2; i++) {
    pthread_create(&(customers[i]), NULL, &customer_thread, NULL);
  }
  barber_thread(NULL);
  return 1;
}
