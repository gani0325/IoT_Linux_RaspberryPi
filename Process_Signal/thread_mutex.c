// 뮤텍스로 동기화 이용하기
#include <stdio.h>
#include <pthread.h>

int g_var = 1;
pthread_mutex_t mid;

void *inc_function(void *);
void *dec_function(void *);

int main(int argc, char **argv)
{
    pthread_t ptInc, ptDec;

    pthread_mutex_init(&mid, NULL); 	/* 뮤텍스 초기화 */

    pthread_create(&ptInc, NULL, inc_function, NULL);

    pthread_create(&ptDec, NULL, dec_function, NULL);
    pthread_join(ptInc, NULL);
    pthread_join(ptDec, NULL);

    pthread_mutex_destroy(&mid); 	/* 뮤텍스 삭제 */

    return 0;
}

void *inc_function(void *arg)
{
    pthread_mutex_lock(&mid); 		/* 임계 구역 설정 */
    printf("Inc : %d < Before\n", g_var);
    g_var++;
    printf("Inc : %d > After\n", g_var);
    pthread_mutex_unlock(&mid); 	/* 임계 구역 해제 */

    return NULL;
}

void *dec_function(void *arg)
{
    pthread_mutex_lock(&mid); 		/* 임계 구역 설정 */
    printf("Dec : %d < Before\n", g_var);
    g_var--;
    printf("Dec : %d > After\n", g_var);
    pthread_mutex_unlock(&mid); 	/* 임계 구역 해제 */

    return NULL;
}

/*
뮤텍스를 2개의 스레드에서 모두 사용할 수 있도록 전역 변수로 선언
pthread_mutex_init() 함수를 통해서 초기화 후 사용이 긑나면
pthread_mutex_destory() 함수를 불러서 해제한다

뮤텍스의 초기화가 완료되면 2개의 스레드 함수에서 임계 구역 (값을 출력한 후 값을 변화시키고 다시 변경된 값을 출력) 옆에
pthread_mutex_lock() 함수를 추가하고 뒷부분에 pthread_mutex_unlock() 함수를 추가하여 보호하도록 한다

증가 부분의 코드가 완료되고 난 이후에 감소 부분이 실행되거나
반대로 감수 부분의 실행이 완료된 이후에 증가 부분이 실행되는 것을 확인한다

gani@gani:~/raspi/Process_Signal $ gcc -o thread_mutex thread_mutex.c -lpthread
gani@gani:~/raspi/Process_Signal $ ./thread_mutex 
*/