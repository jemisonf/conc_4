#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>


struct thread_args {
  sem_t * sem_1;
  sem_t * sem_2;
  char ingredient_name_1[256];
  char ingredient_name_2[256];
  char name[256];
};

struct pusher_args {
  sem_t * watched_ingredient;
  sem_t * other_ingredient_1;
  sem_t * other_ingredient_2;
  int * bool_1;
  int * bool_2;
  int * watched_bool;
};

sem_t agent_sem;
sem_t tobacco_sem;
sem_t paper_sem;
sem_t match_sem;
sem_t tobacco;
sem_t paper;
sem_t match;
sem_t mutex;
int is_tobacco = 0, is_paper = 0, is_match = 0;

void * agent_thread(void * args) {
  struct thread_args * agent_args = (struct thread_args *) args;
  while(1) {
    sem_wait(&agent_sem);
    printf("Agent %s is putting out %s\n", agent_args->name, agent_args->ingredient_name_1);
    fflush(stdout);
    sem_post(agent_args->sem_1);
    printf("Agent %s is putting out %s\n", agent_args->name, agent_args->ingredient_name_2);
    fflush(stdout);
    sem_post(agent_args->sem_2);
    sleep(5);
  }
}

void * smoker_thread(void * args) {
  struct thread_args * smoker_args = (struct thread_args *) args;
  while (1) {
    printf("The smoker with %s is waiting for ingredients\n", smoker_args->name);
    sem_wait(smoker_args->sem_1); 
    printf("The smoker with %s has picked up both other ingredients and is making a cigarette\n", smoker_args->name);
    fflush(stdout);
    sem_post(&agent_sem);
    sleep(5);
  }
}

void * pusher_thread(void * raw_args) {
  struct pusher_args * args = (struct pusher_args *) raw_args;
  while(1) {
     sem_wait(args->watched_ingredient);
     printf("Pusher encountered ingredient\n");
     sem_wait(&mutex);

     if (*(args->bool_1)) {
       *(args->bool_1) = 0;
       printf("Posting ingredient 1\n");
       sem_post(args->other_ingredient_1);
     } else if (*(args->bool_2)) { 
       *(args->bool_2) = 0;
       printf("Posting ingredient 2\n");
       sem_post(args->other_ingredient_2);
     } else {
       printf("Posting watched ingredient\n");
       *(args->watched_bool) = 1; 
     }

     sem_post(&mutex);
  }
}

int main(int argc, char* argv[]) {
  pthread_t agent_a; 
  pthread_t agent_b; 
  pthread_t agent_c; 
  pthread_t smoker_tobacco;
  pthread_t smoker_match;
  pthread_t smoker_paper;
  pthread_t pusher_tobacco;
  pthread_t pusher_paper;
  pthread_t pusher_match;
  sem_init(&agent_sem, 0, 1);
  sem_init(&tobacco_sem, 0, 0);
  sem_init(&match_sem, 0, 0);
  sem_init(&paper_sem, 0, 0);
  sem_init(&tobacco, 0, 0);
  sem_init(&match, 0, 0);
  sem_init(&paper, 0, 0);
  sem_init(&mutex, 0, 1);
  struct thread_args agent_a_args = {
                                      sem_1: &tobacco,
                                      sem_2: &paper,
                                      ingredient_name_1: "Tobacco",
                                      ingredient_name_2: "Paper",
                                      name: "A"
                                    };

  struct thread_args agent_b_args = {
                                      sem_1: &paper,
                                      sem_2: &match,
                                      ingredient_name_1: "Paper",
                                      ingredient_name_2: "Match",
                                      name: "B"
                                    };
  struct thread_args agent_c_args = {
                                      sem_1: &tobacco,
                                      sem_2: &match,
                                      ingredient_name_1: "Tobacco",
                                      ingredient_name_2: "Match",
                                      name: "C"
                                    };
  struct thread_args smoker_tobacco_args = {
                                             sem_1: &tobacco_sem,
                                             name: "Tobacco"
                                           };
  struct thread_args smoker_match_args = {
                                             sem_1: &match_sem,
                                             name: "Match"
                                         };

  struct thread_args smoker_paper_args = {
                                             sem_1: &paper_sem,
                                             name: "Paper"
                                         };

  struct pusher_args pusher_tobacco_args = {
                                             watched_ingredient: &tobacco,
                                             other_ingredient_1: &paper_sem,
                                             other_ingredient_2: &match_sem,
                                             bool_1: &is_paper,
                                             bool_2: &is_match,
                                             watched_bool: &is_tobacco
                                           };
  struct pusher_args pusher_match_args = {
                                             watched_ingredient: &match,
                                             other_ingredient_1: &paper_sem,
                                             other_ingredient_2: &tobacco_sem,
                                             bool_1: &is_paper,
                                             bool_2: &is_tobacco,
                                             watched_bool: &is_match
                                           };
  struct pusher_args pusher_paper_args = {
                                             watched_ingredient: &paper,
                                             other_ingredient_1: &match_sem,
                                             other_ingredient_2: &tobacco_sem,
                                             bool_1: &is_match,
                                             bool_2: &is_tobacco,
                                             watched_bool: &is_paper
                                           };
  pthread_create(&smoker_paper, NULL, smoker_thread, &smoker_paper_args);
  pthread_create(&smoker_match, NULL, smoker_thread, &smoker_match_args);
  pthread_create(&smoker_tobacco, NULL, smoker_thread, &smoker_tobacco_args);
  pthread_create(&pusher_tobacco, NULL, pusher_thread, &pusher_tobacco_args);
  pthread_create(&pusher_paper, NULL, pusher_thread, &pusher_paper_args);
  pthread_create(&pusher_match, NULL, pusher_thread, &pusher_match_args);
  pthread_create(&agent_a, NULL, agent_thread, &agent_a_args);
  pthread_create(&agent_b, NULL, agent_thread, &agent_b_args);
  pthread_create(&agent_c, NULL, agent_thread, &agent_c_args);
  while (1) {};
}
