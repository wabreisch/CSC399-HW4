/*****
 * Christian Duncan
 *
 * Dining Philosophers
 *    This is a simple implementation of the dining philosopher's problem.
 *    Here we have N philosophers at a round table eating rice from chopsticks.
 *    But each one shares chopsticks with the neighbor - so N chopsticks.
 *    And we dine... 
 *    Philosophers are notorious contemplators and often ponder the wonders of the 
 *    universe inbetween bites.
 *
 *    Implemented using a forked process per philosopher with a pipe per chopstick.
 *       Grabbing a chopstick means reading from the pipe.
 *       Putting it down means writing back to it.
 *       Start with exactly one character in each pipe so if a chopstick is grabbed
 *       (character read) another grab will freeze until the other puts it back down.
 *       Other ways to implement but this was done to illustrate pipes and forking as well.
 ****/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

class ChopStick {
private:
  int fd[2];  // fd[0] - read and fd[1] - write
 
public:
  ChopStick() {
    // Create a chopstick
    if (pipe(fd) == -1) {
      // Error occurred.
      fprintf(stderr, "Error creating chopstick.  Aborting!\n");
      exit(1);
    }
    putDown();  // Make sure the chopstick is available for picking up...
  }

  ~ChopStick() {
    // Close the pipe
    //   Careful - if the pipe is shared this can be problematic I believe!
    close(fd[0]);
    close(fd[1]);
  }

  /***
   * pickUp:
   *    Pickup the chopstick
   *    The same as claiming a resource.
   *    Thread will lock until resource is available.
   ***/
  void pickUp() {
    char val;
    if (read(fd[0], &val, sizeof(val)) == 0) {
      // Error!  Should not be end of file unless the chopstick was closed!
      fprintf(stderr, "Error.  Chopstick no longer available.\n");
      exit(1);
    }
  }

  /***
   * putDown:
   *   Put Down the chopstick
   *   The same as releasing a resource.
   ***/
  void putDown() {
    char val = 'c';
    write(fd[1], &val, sizeof(val));  // Write the character to the fd
  }
};

ChopStick *stick;

void createPhilosopher(int p, ChopStick& left, ChopStick& right);
void runPhilosopher(int p, ChopStick& left, ChopStick& right);
void think(int p, int k, int l, double prob);
void eat(int p, int k, int l);
void speak(int p, const char *message);

int main(int argc, char **argv) {
  int N;  // The number of philosophers at the table
  if (argc < 2) {
    N = 5; // Default;
  } else {
    N = atoi(argv[1]);
  }

  // Create the N chopsticks
  //    These are pipes - read/write
  stick = new ChopStick[N];

  // Create the N philosophers
  int p;
  for (p = 0; p < N; p++) {
    createPhilosopher(p, stick[p], stick[(p+1)%N]);  // p, left, right chopsticks to use
  }

  wait(NULL);  // Since none of them will ever finish... just wait around.
}

/***
 * createPhilosopher:
 *    Create a philosopher - a child process created via fork.
 ***/
void createPhilosopher(int p, ChopStick& left, ChopStick& right) {
  if (fork() == 0) {
    // I am the child process - A new PHILOSOPHER
    runPhilosopher(p, left, right);  // Do my thing...
    exit(0);    // Finished
  }
}

/***
 * runPhilosopher:
 *   The philosopher spends time thinking and eating
 *   Thinks for a bit.
 *   Grabs a left chopstick...
 *   Thinks for a bit.
 *   Grabs a right chopstick...
 *   Eats for a bit.
 *   Puts left chopstick down...
 *   Thinks for a bit.
 *   Puts right chopstick down...
 *   Repeat...
 ***/
void runPhilosopher(int p, ChopStick& left, ChopStick& right) {
  srandom(time(NULL));
  while (1) {
    speak(p, "Getting left chopstick.");
    left.pickUp();
    think(p, 1, 3, 0.01);  // Prob = 0.01 - sometimes think heavily here (amazing thoughts can distract)
    speak(p, "Getting right chopstick.");
    right.pickUp();
    eat(p, 1, 4);         // Eat for a few seconds (1 to 3) seconds.
    speak(p, "Putting down left chopstick.");
    left.putDown();
    think(p, 1, 4, 0.01);  // Prob = 0.01 - sometimes think
    speak(p, "Putting down right chopstick.");
    right.putDown();
    think(p, 1, 4, 1.0);  // Prob=1.0 - always think after eating but not always as long.
  }
}

/***
 * think: 
 *    Philosopher p thinks for (k to l-1) seconds with probability prob.
 *      Thinking is implemented here by "sleeping"
 ***/
void think(int p, int k, int l, double prob) {
  if (((double) random()) / RAND_MAX <= prob) {
    // Think every so often, before eating.
    speak(p, "Hmmm.... interesting.");
    sleep(random()%(l-k)+k);  // k to l-1 seconds of thinking time.
    speak(p, "Aha... I see");
  }
}

/***
 * eat:
 *    Philosopher p eats for k to l-1 seconds.
 *    Implemented again via sleeping.
 ***/
void eat(int p, int k, int l) {
  speak(p, "Mmmmm...");
  sleep(random()%(l-k)+k);  // k to l-1 seconds of thinking time.
  speak(p, " ... rice.");
}

/***
 * speak:
 *    Philosopher p says something "out loud"
 *    Just prints out a message.
 ***/
void speak(int p, const char *message) {
  printf("%d: %s\n", p, message);
}
