#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#define MAX_CONVERSATION_LENGTH 100
#define MAX_MESSAGE_LENGTH 512
#define PREFIX_LENGTH 10
sem_t person1_turn;
sem_t person2_turn;
char conversation[MAX_CONVERSATION_LENGTH][MAX_MESSAGE_LENGTH];
int conversation_index = 0;
int correction_count = 0;
int iced_tea_count = 0;
int conversation_running = 1;
void replace_iced_tea(char *message) {
    char buffer[MAX_MESSAGE_LENGTH];
    char *insert_point = &buffer[0];
    const char *temp = message;
    const char *match;
    while ((match = strstr(temp, "iced tea")) != NULL) {
        size_t len = match - temp;
        strncpy(insert_point, temp, len);
        insert_point += len;
        strcpy(insert_point, "iced coffee");
        insert_point += strlen("iced coffee");
        temp = match + strlen("iced tea");
        iced_tea_count++;
    }
    strcpy(insert_point, temp);
    strcpy(message, buffer);
}
void *person1(void *arg) {
    while (conversation_running) {
        sem_wait(&person1_turn);
        char message[MAX_MESSAGE_LENGTH];
        printf("Person 1, your turn (type 'Bye.' to end): ");
        fgets(message, MAX_MESSAGE_LENGTH, stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "Bye.") == 0) {
            conversation_running = 0;
            sem_post(&person2_turn);
            break;
        }
        if (strstr(message, "iced tea") != NULL) {
            printf("Correction detected: Changing 'iced tea' to 'iced coffee'\n");
            replace_iced_tea(message);
            correction_count++;
        }
        char corrected_message[MAX_MESSAGE_LENGTH];
        snprintf(corrected_message, sizeof(corrected_message), "Person 1: %.501s", message);
        strncpy(conversation[conversation_index++], corrected_message, MAX_MESSAGE_LENGTH - 1);
        printf("Person 1 said: %s\n", corrected_message);
        printf("Number of occurrences: %d\n", iced_tea_count);
        sem_post(&person2_turn);
    }
    return NULL;
}
void *person2(void *arg) {
    while (conversation_running) {
        sem_wait(&person2_turn);
        char message[MAX_MESSAGE_LENGTH];
        printf("Person 2, your turn (type 'Bye.' to end): ");
        fgets(message, MAX_MESSAGE_LENGTH, stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "Bye.") == 0) {
            conversation_running = 0;
            sem_post(&person1_turn);
            break;
        }
        if (strstr(message, "iced tea") != NULL) {
            printf("Correction detected: Changing 'iced tea' to 'iced coffee'\n");
            replace_iced_tea(message);
            correction_count++;
        }
        char corrected_message[MAX_MESSAGE_LENGTH];
        snprintf(corrected_message, sizeof(corrected_message), "Person 2: %.501s", message);
        strncpy(conversation[conversation_index++], corrected_message, MAX_MESSAGE_LENGTH - 1);
        printf("Person 2 said: %s\n", corrected_message);
        printf("Number of occurrences: %d\n", iced_tea_count);
        sem_post(&person1_turn);
    }
    return NULL;
}
int main() {
    pthread_t t1, t2;
    sem_init(&person1_turn, 0, 1);
    sem_init(&person2_turn, 0, 0);
    pthread_create(&t1, NULL, person1, NULL);
    pthread_create(&t2, NULL, person2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&person1_turn);
    sem_destroy(&person2_turn);
    printf("\nCorrected Conversation:\n");
    for (int i = 0; i < conversation_index; i++) {
        printf("%s\n", conversation[i]);
    }
    printf("\n Total number of iced tea detected and corrected  'iced tea': %d\n", iced_tea_count);
    return 0;
}



	

