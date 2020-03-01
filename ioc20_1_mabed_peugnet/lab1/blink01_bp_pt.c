//------------------------------------------------------------------------------
// Headers that are required for printf and mmap
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BP     18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

int BP_ON = 0;   // mis à 1 si le bouton a été appuyé, mis à 0 quand la tâche qui attend l'appui a vu l'appui
int BP_OFF = 0;  // mis à 1 si le bouton a été relâché, mis à 0 quand la tâche qui attend le relâchement a vu le relâchement


struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
};

struct gpio_s *gpio_regs_virt; 


static void 
gpio_fsel(uint32_t pin, uint32_t fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);
}

static int
gpio_read (uint32_t pin)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    uint32_t mask = 1;
    uint32_t val = gpio_regs_virt->gplev[reg] >> bit & mask;
    return val;

}

//------------------------------------------------------------------------------
// Access to memory-mapped I/O
//------------------------------------------------------------------------------

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

static int mmap_fd;

static int
gpio_mmap ( uint32_t volatile ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = ( uint32_t volatile * ) mmap_result;

    return 0;
}

void
gpio_munmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

void * read_BP()
{
    unsigned int prev_val=1, new_val=1;
    printf("test");

    while (1)
    {
        delay(20);
        new_val=gpio_read(GPIO_BP);
        printf("%d\n", new_val);
        if (prev_val!=new_val)             // changement d'état
        {
            prev_val=new_val;
            if (new_val==0) { // appui détecté
                BP_ON= 1;
                printf("appui detecte");
            } else { // relâchement détecté
                BP_OFF = 1;
                printf("relachement detecte");
            }
            prev_val=new_val;
        }
        if (BP_ON == 1) {
            BP_ON = 0;
            gpio_write ( GPIO_LED0, 1 );
        }
        if (BP_OFF == 1) {
            BP_OFF = 0;
            gpio_write ( GPIO_LED0, 0 );
        }
    }
}
//------------------------------------------------------------------------------
// Main Programm
//------------------------------------------------------------------------------

struct thread_args{
    int half_period;
    uint32_t led;
};

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

void * blink(void * args) {
    printf ( "-- info: start blinking.\n" );
    struct thread_args * data = (struct thread_args *) args;
    uint32_t val = 0;

    while (1) {
        gpio_write ( data->led, val );
        delay ( data->half_period );
        val = 1 - val;
    }
}


int
main ( int argc, char **argv )
{
    // Get args
    // ---------------------------------------------

    printf("test1 ouessheu \n");
    int period, half_period;

    period = 1000; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;
    uint32_t volatile * gpio_base = 0;

    // map GPIO registers
    // ---------------------------------------------

    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    // Create thread
    // ---------------------------------------------

    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    // while(1) {
    //     printf("%d\n", gpio_read ( GPIO_BP ));
    // }


    struct thread_args args;
    args.half_period = half_period;
    args.led = GPIO_LED1;

    pthread_t thread1;
    pthread_t thread2;
    

    printf("test2");
    if (pthread_create(&thread1, NULL, blink, &args)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    if (pthread_create(&thread2, NULL, read_BP, NULL)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    printf("test452 ouessheu \n");


    if (pthread_join(thread1, NULL)) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }
    if (pthread_join(thread2, NULL)) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }


    return 0;
}
