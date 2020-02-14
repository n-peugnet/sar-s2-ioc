#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/io.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nicolas, 2020");
MODULE_DESCRIPTION("Module, pour controller une led");

////////////////////////////// parameters //////////////////////////////////

static int btn;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

/////////////////////////////////// GPIO ///////////////////////////////////

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

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
}
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void gpio_write(int pin, int val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

//////////////////////////////// fops //////////////////////////////////////

static int major;

static int 
open_ledbp(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "MPopen()\n");
    return 0;
}

static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "MPread()\n");
    return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "MPwrite()\n");
    size_t i;
    for (i = 0; i < count -1; i++)
    {
        printk(KERN_DEBUG "MP led %d = %c\n", i, buf[i]);
        gpio_write(leds[i], (buf[i] - '0') & 1);
    }
    return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "MPclose()\n");
    return 0;
}

struct file_operations fops_ledbp =
{
    .open       = open_ledbp,
    .read       = read_ledbp,
    .write      = write_ledbp,
    .release    = release_ledbp 
};

/////////////////////////////// __init /////////////////////////////////////

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "MP Hello World !\n");
    printk(KERN_DEBUG "MP btn=%d !\n", btn);
    int i;
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "MP LED %d = %d\n", i, leds[i]);
    major = register_chrdev(0, "MPledbp", &fops_ledbp); // 0 est le numéro majeur qu'on laisse choisir par linux
    gpio_fsel(leds[0], GPIO_FSEL_OUTPUT);
    gpio_fsel(leds[1], GPIO_FSEL_OUTPUT);
    gpio_fsel(btn, GPIO_FSEL_INPUT);
    return 0;
}

/////////////////////////////// __exit /////////////////////////////////////

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "MP Goodbye World!\n");
   unregister_chrdev(major, "MPledbp");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);