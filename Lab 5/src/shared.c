#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N (4)
#define ALICE_DEPOSIT_AMMOUNT ((ssize_t)100000)
#define BOB_WITHDRAW_AMMOUNT ((ssize_t)500)

typedef struct {
    ssize_t balance;
    ssize_t n_operations;
} account_t;

typedef struct {
    account_t *account;
    ssize_t n_deposits;
} alice_args_t;

typedef struct {
    account_t *account;
    ssize_t n_withdrawals;
} bob_args_t;

void account_init(account_t *account) {
    account->balance = 0;
    account->n_operations = 0;
}

pthread_mutex_t trinco = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwl;

void account_deposit(account_t *account, size_t ammount) {  // every time the deposit and withdraw are called they are critical sections

    pthread_rwlock_wrlock(&rwl);
    account->balance += ammount;
    account->n_operations += 1;
    pthread_rwlock_unlock(&rwl);
}

ssize_t account_withdraw(account_t *account, size_t ammount) {
        pthread_rwlock_wrlock(&rwl);
    if (account->balance >= ammount) {
        account->balance -= ammount;
        account->n_operations += 1;
        pthread_rwlock_unlock(&rwl);
        return ammount;
    }

    return -1;
}

void *account_print_info(void *arg) {
    account_t const *account = (account_t const *)arg;

    pthread_rwlock_rdlock(&rwl);
    sleep(1);
    printf("account info: balance= %zd$, #operations = %zd\n", account->balance,   //because this func is only called in main that doesnt have thread competition, it doesnt need locks
           account->n_operations);
    pthread_rwlock_unlock(&rwl);
    return NULL;
}

void *alice_thread_fn(void *arg) {
    alice_args_t const *args = (alice_args_t const *)arg;

    size_t total_deposited = 0;
    for (size_t i = 0; i < args->n_deposits; i++) {
        account_deposit(args->account, ALICE_DEPOSIT_AMMOUNT);
        total_deposited += ALICE_DEPOSIT_AMMOUNT;
    }

    pthread_rwlock_rdlock(&rwl);
    sleep(1);
    printf("alice deposited a total of: %zd$\n", total_deposited);
    pthread_rwlock_unlock(&rwl);
    return NULL;
}

void *bob_thread_fn(void *arg) {
    bob_args_t const *args = (bob_args_t const *)arg;

    size_t total_withdrawn = 0;
    size_t failed_withdrawals = 0;
    for (size_t i = 0; i < args->n_withdrawals; i++) {
        ssize_t ret = account_withdraw(args->account, BOB_WITHDRAW_AMMOUNT);
        if (ret != -1) {
            total_withdrawn += ret;
        } else {
            failed_withdrawals += 1;
        }
    }
    pthread_rwlock_rdlock(&rwl);
    sleep(1);
    printf("bob withdrew a total of %zd$, failing to withdraw %zu times\n",
           total_withdrawn, failed_withdrawals);
    pthread_rwlock_unlock(&rwl);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t tid[6];

    ssize_t total_ops = 5;
    if (argc > 1) {
        total_ops = atoll(argv[1]);
    }

    account_t account;
    account_init(&account);

    pthread_rwlock_init(&rwl, NULL);

    alice_args_t alice_args = {.n_deposits = total_ops, .account = &account};

    bob_args_t bob_args = {.n_withdrawals = total_ops, .account = &account};


    if (pthread_create(&tid[0], NULL, alice_thread_fn, (void *)&alice_args) != 0) {
        fprintf(stderr, "failed to create alice thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&tid[1], NULL, bob_thread_fn, (void *)&bob_args) != 0) {
        fprintf(stderr, "failed to create bob thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);


    for (int i = 0; i < 4; i++)
        if (pthread_create(&tid[1+i], NULL, account_print_info, (void *)&account) != 0) {
            fprintf(stderr, "failed to create print thread: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
            printf("%d", i);
        }


    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
    pthread_join(tid[4], NULL);
    pthread_join(tid[5], NULL);


    printf("end of finantial history\n");
    account_print_info(&account);

    return 0;
}
