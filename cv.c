//Shahaddin Gafarov 

//This code is about solving the producer cosnsumer problem by the use of mutex and threads.
//The max size for the input is 50, and buffer is 15.
//Producer thread reads the charactters and places them into the buffer
//Consumer thread retrieves these characters from the buffer and prints them until all buffer is cleared out.
//By using theads the threads run in paralell and not sequential,
//for example if the consumer is running and buffer is empty it waits for producer to put more elements
//and when the buffer is full and producer is running, it waits the consumer thread to empty the buffer
//Program keeps asking new inputs until user puts the command "exit"


//Main Source I have used for overall code:
//https://www.tutorialspoint.com/producer-consumer-problem-in-c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

//Buffer size can have 15 positions at max
char buffer[15];
//The input will have 50 characters at max(+ the \0), also anything bigegr than that it will be discarded
char input[51];

//we need head and tail for circular buffer
//https://embedjournal.com/implementing-circular-buffer-embedded-c/
int head = 0;
int tail = 0;

//count is the number of characters "waiting"
//as we get more in producer it increases
//wehereas the consumer "consumes" the input, which decreases thecount
int count = 0;

//creating the producer and the consumer thread
pthread_t producer_thread;
pthread_t consumer_thread;

//mutex is a lock that is for preventing simultaneous access at the same time
pthread_mutex_t mutex;

//signals for when the buffer is full and empty
pthread_cond_t full;
pthread_cond_t empty;

//how the producer thread works is defined here
void *producer_function(void *arg)
{
    //character index is different from count
    //it strictly is used for getting the characters from the input
    //into the tail
    int character_index = 0;
    //get the input in main() to the local input
    char *input = (char *)arg;

    //do the process until all the letters from the input is retrieved
    while(input[character_index] != '\0')
    {
        //we need to lock the mutex to make the access exclusive
        pthread_mutex_lock(&mutex);

        //if the buffer is full we will wait for other thread(consumer)
        //to make some space
        if(count == 15)
        {
            while(count == 15)
            {
                pthread_cond_wait(&full, &mutex);
            }
        }

        //update the tail of the buffer to be character-indexth letter of the input
        buffer[tail] = input[character_index];
        //go to next character
        character_index = character_index + 1;

        //go to the next slot to be able to put the next input to tail
        tail = (tail + 1);

        //since we have 15 spaces at max
        //anything bigger or equal to 15, we go back to the start 
        //(thus a circular pattern)
        if(tail >= 15)
        {
            tail = tail % 15;
        }
        //increase the count as we put a new character to buffer
        count = count + 1;
        
        //this if else case is for tail==0 specifically
        //when tail is 0, decrementing it should lead to the end of the buffer
        
        //this is what we see in years or weeks
        //when we go one day before monday, or one month behind january
        //we find ourselves at Sunday(7th day of the weel), or December(12th month of the year) 
        if(tail == 0)
        {
            //preventing buffer[-1]
            //print the character
            printf("Produced: %c", buffer[tail - 1 + 15]);
            printf("\n");
        }

        else
        {
            //we can normally print the chacarter if tail is > 0
            printf("Produced: %c", buffer[tail - 1]);
            printf("\n");
        }   
        //signal the buffer is not empty
        pthread_cond_signal(&empty);

        //once the character is processed, we can unlock the mutex 
        pthread_mutex_unlock(&mutex);
    }

    ///we need to lock the mutex to make the access exclusive
    pthread_mutex_lock(&mutex);
    //put a null pointer at the end of the tail to show an end
    buffer[tail] = '\0';

    //update the tail and the counter one last time
    tail = (tail + 1);
    if(tail >= 15)
    {
        tail = tail % 15;
    }
    count = count + 1;

    //signal the buffer is not empty
    pthread_cond_signal(&empty);
    //we can unlock the mutex 
    pthread_mutex_unlock(&mutex);

    //print the final line for producer
    printf("Producer: done");
    printf("\n");
    //pthread_exit(NULL);

    //exit the thread and return null
    //it is not mandatory fopr code's success, but is for preventing warnings
    pthread_exit(NULL);
    return NULL;
}

//how the consumer thread works is defined here
void *consumer_function()
{
    //the character that will be printed from the buffer
    char character = '\0';

    //forever going loop until a specific desired event happens
    int xxxx = 1;
    while(xxxx==1)
    {
        //again we need to lock the mutex
        pthread_mutex_lock(&mutex);

        //this time around instead of count=15,
        //we look for count=0
        if(count == 0)
        {
            //and if there are no elements in the buffer, we simply
            //wait for new characters to come
            while(count == 0)
            {
                pthread_cond_wait(&empty, &mutex);
            }
        }

        //same logic with producer but in reverse:
        //we get the character from buffer
        character = buffer[head];
        //and the head of the buffer goes to the next one
        head = head + 1;
        //and checks for overflow, to restart at the startt
        if(head >= 15)
        {
            head = head % 15;
        }
        //this time around, we decrease the count instead
        count = count - 1;

        //this can happen two ways
        //1st e reach the end of the buffer which has a '\0'
        //or the '\0' we have defined at the start caused this
        //as there were no elements in the buffer and it stayed null throughout the first iteration of the while loop
        if(character == '\0')
        {
            xxxx = 0;
            break;
        }

        //if everything goes sommothly, we simply print the character 
        else
        {
            printf("Consumed: %c", character);
            printf("\n");
        }

        //same logic with producer, but instead not full
        pthread_cond_signal(&full);
        //and we unlock the mutex as we have finished the iteration
        pthread_mutex_unlock(&mutex);
    }
    
    //final print statement
    printf("Consumer: done");
    printf("\n");
    //exit the thread and return null
    //it is not mandatory for code's success, but is for preventing warnings
    pthread_exit(NULL);
    return NULL;
}

//https://stackoverflow.com/a/7898516//I had an issue with characters after 50 getting also printed in my program
//I used this algorithm as a solution
//What this does is basically flushing/clearing the input buffer
void clear_input_buffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF){}
}

//main function
int main()
{
    //keeping a forever going loop, to get inputs until the word exit is inputted by the user
    int xxx = 1;
    while(xxx == 1)
    {
        //calling the mutex and  the confdition variables
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&full, NULL);
        pthread_cond_init(&empty, NULL);


        //Asking the user for input, and only saving 50 characters at most
        printf("Enter input (type 'exit' to quit): ");

        fgets(input, 51, stdin);
        //printf(input);
        input[50] = '\0';


        // //input[strcspn(input, "\n")] = '\0';
        // //deleting newline characters from the code
        // //maybe except the last one becasue fgets itself adds a \n to the input
        for(int i=0; i< strlen(input); i++)
        {
            //replace newline with null character
            if(input[i] == '\n')
            {
                input[i] = '\0';
            }
        }

        //I had ian issue that when I had given input >50, the code would
        //reinput the whole thing in the while loop as an input from me:
        //if there was 112 characters I would get the result as 50-50-12 package
        //rather than simply forgetting the rest
        if (strlen(input) >= 50) 
        {
            clear_input_buffer();
        }



        //if the input is exit, that is not for getting processed but for finishing the task
        if(strcmp(input, "exit") ==0)
        {
            //stop the loop(while break replaces the job of xxx=0, 
            //I still keep it as a more clear sign of what this if statement does for visability purposes)
            xxx = 0;
            break;
        }

        //Ffor seeing whether the input is read properly
        printf("Input: %s", input);
        printf("\n");

        //For seeing whether the input is with correct amount of characters
        int characters = strlen(input);
        printf("Count: %d characters", characters);
        printf("\n");

        //creating the consumer and producer threads declared above using their respective functions
        //one difference from the source code is that the producer thread doesn't tkae NULL
        //but it rather needs the input top operate
        pthread_create(&producer_thread, NULL, producer_function, input);
        pthread_create(&consumer_thread, NULL, consumer_function, NULL);

        //joining the results of the threads
        //also makes sure the result is parallel, and not sequential like on the test cases
        pthread_join(producer_thread, NULL);
        pthread_join(consumer_thread, NULL);

        //a more fun way of resetting them all to zero
        head = 0;
        count = head;
        tail = count;
    }

    //similar to free(), but this is for threads as cleanup process
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    //After all threads have been processed ---> Parent:Done 
    printf("Parent: done");
    printf("\n");

}


//SELF NOTE for running WSL on my Windows:
//wsl --install
//sudo apt install gcc
//cd /mnt/c/Users/sahed/OneDrive/Desktop/Circular_Buffer_Pthread

