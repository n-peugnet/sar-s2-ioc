#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/delay.h>
#include <asm/io.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nicolas & Nour, 2020");
MODULE_DESCRIPTION("Module, pour controller un écran lcd.");

/////////////////////////////////// GPIO ///////////////////////////////////

/*******************************************************************************
 * GPIO Pins
 ******************************************************************************/
#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

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

void gpio_write(int gpio, int value)
{
    int regnum = gpio / 32;
    int offset = gpio % 32;
    if (value&1)
        gpio_regs->gpset[regnum] = (0x1 << offset);
    else
        gpio_regs->gpclr[regnum] = (0x1 << offset);
}


/*******************************************************************************
 * LCD's Instructions ( source = doc )
 * Ces constantes sont utilisées pour former les mots de commandes
 * par exemple dans la fonction lcd_init()
 ******************************************************************************/

/* commands */
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

/* Global vars */

// colum
int X;

// line
int Y;

/* generate E signal */
void lcd_strobe(void)
{
    gpio_write(E, 1);
    udelay(100);
    gpio_write(E, 0);
}

/* send 4bits to LCD : valable pour les commande et les data */

void lcd_write4bits(int data)
{
    /* first 4 bits */
    gpio_write(D7, data>>7);
    gpio_write(D6, data>>6);
    gpio_write(D5, data>>5);
    gpio_write(D4, data>>4);
    lcd_strobe();

    /* second 4 bits */
    gpio_write(D7, data>>3);
    gpio_write(D6, data>>2);
    gpio_write(D5, data>>1);
    gpio_write(D4, data>>0);
    lcd_strobe();
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd);
    udelay(2000);               // certaines commandes sont lentes
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character);
    udelay(100);
}

void lcd_set_cursor(int x, int y)
{
    X = x;
    Y = y;
}

/* initialization : pour comprendre la séquence, il faut regarder le cours */
// Q4: Expliquer le rôle des masques : LCD_FUNCTIONSET, LCD_FS_4BITMODE, etc.
void lcd_init(void)
{
    gpio_write(E, 0);
    lcd_command(0b00110011);    /* initialization */
    lcd_command(0b00110010);    /* initialization */
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF);
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);
}

void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);
}

// Q5: Expliquez comment fonctionne cette fonction
void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20;
    int i = 0;

    for (; (Y < 4) && (i < strlen(txt)); Y++, X=0) {
        lcd_command(LCD_SETDDRAMADDR + a[Y] + X);
        for (; (X < len) && (i < (Y + 1) * len) && (i < strlen(txt)); i++, X++) {
            lcd_data(txt[i]);
        }
    }
}

//////////////////////////////// fops //////////////////////////////////////

static int major;

static int 
open_lcd(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "MPlcd open()\n");

    /* Setting up GPIOs to output */
    gpio_fsel(RS, GPIO_FSEL_OUTPUT);
    gpio_fsel(E,  GPIO_FSEL_OUTPUT);
    gpio_fsel(D4, GPIO_FSEL_OUTPUT);
    gpio_fsel(D5, GPIO_FSEL_OUTPUT);
    gpio_fsel(D6, GPIO_FSEL_OUTPUT);
    gpio_fsel(D7, GPIO_FSEL_OUTPUT);

    /* initialization */
    lcd_init();
    lcd_clear();
    return 0;
}

static ssize_t 
write_lcd(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "MPlcd write()\n");
    lcd_message(buf);
    return count;
}

static int 
release_lcd(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "MPlcd close()\n");
    return 0;
}

struct file_operations fops_lcd =
{
    .open       = open_lcd,
    .write      = write_lcd,
    .release    = release_lcd 
};

/////////////////////////////// __init /////////////////////////////////////

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "MPlcd Hello World !\n");
    major = register_chrdev(0, "MPlcd", &fops_lcd); // 0 est le numéro majeur qu'on laisse choisir par linux
    return 0;
}

/////////////////////////////// __exit /////////////////////////////////////

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "MPlcd Goodbye World!\n");
   unregister_chrdev(major, "MPlcd");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);