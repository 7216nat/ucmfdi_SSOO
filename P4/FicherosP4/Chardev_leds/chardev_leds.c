#include <linux/module.h>
#include <asm-generic/errno.h>
#include <linux/init.h>
#include <linux/tty.h>      /* For fg_console */
#include <linux/kd.h>       /* For KDSETLED */
#include <linux/vt_kern.h>
#include <linux/version.h> /* For LINUX_VERSION_CODE */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>	/* for copy_to_user */
#include <linux/cdev.h>

#define ALL_LEDS_ON 0x7
#define ALL_LEDS_OFF 0
#define SCROLL_LED_ON 0x1
#define NUM_LED_ON 0x4
#define CAPS_LED_ON 0x2

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Chardev_leds");

struct tty_driver* kbd_driver= NULL;

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static inline int set_leds(struct tty_driver* handler, unsigned int mask);

#define SUCCESS 0
#define DEVICE_NAME "chardev_leds"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */

dev_t start;
struct cdev* chardev_leds=NULL;
static int Device_Open = 0;	/* Is device open?
				 * Used to prevent multiple access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;		/* This will be initialized every time the
				   device is opened successfully */
static int default_leds = '\0';		/* Tracks the number of times the character
				 * device has been opened */

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* Get driver handler */
struct tty_driver* get_kbd_driver_handler(void)
{
    printk(KERN_INFO "chardev_leds: loading\n");
    printk(KERN_INFO "chardev_leds: fgconsole is %x\n", fg_console);
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return vc_cons[fg_console].d->port.tty->driver;
#else
    return vc_cons[fg_console].d->vc_tty->driver;
#endif
}

/* Set led state to that specified by mask */
static inline int set_leds(struct tty_driver* handler, unsigned int mask)
{
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return (handler->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,mask);
#else
    return (handler->ops->ioctl) (vc_cons[fg_console].d->vc_tty, NULL, KDSETLED, mask);
#endif
}


/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
    int major;		/* Major number assigned to our device driver */
    int minor;		/* Minor number assigned to the associated character device */
    int ret;

    /* Get available (major,minor) range */
    if ((ret=alloc_chrdev_region (&start, 0, 1,DEVICE_NAME))) {
        printk(KERN_INFO "Can't allocate chrdev_region()");
        return ret;
    }

    /* Create associated cdev */
    if ((chardev_leds=cdev_alloc())==NULL) {
        printk(KERN_INFO "cdev_alloc() failed ");
        unregister_chrdev_region(start, 1);
        return -ENOMEM;
    }

    cdev_init(chardev_leds,&fops);

    if ((ret=cdev_add(chardev_leds,start,1))) {
        printk(KERN_INFO "cdev_add() failed ");
        kobject_put(&chardev_leds->kobj);
        unregister_chrdev_region(start, 1);
        return ret;
    }

    major=MAJOR(start);
    minor=MINOR(start);

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'sudo mknod -m 666 /dev/%s c %d %d'.\n", DEVICE_NAME, major,minor);
    printk(KERN_INFO "Try to cat and echo to the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
    /* Destroy chardev */
    if (chardev_leds)
        cdev_del(chardev_leds);

    /*
     * Unregister the device
     */
    unregister_chrdev_region(start, 1);
}

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/chardev_leds"
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++;

    /* Initialize msg */
    sprintf(msg, "%d\n", default_leds);

    /* Initially, this points to the beginning of the message */
    msg_Ptr = msg;

    /* Increase the module's reference counter */
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;		/* We're now ready for our next caller */

    /*
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module.
     */
    module_put(THIS_MODULE);

    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
                           char *buffer,	/* buffer to fill with data */
                           size_t length,	/* length of the buffer     */
                           loff_t * offset)
{
    /*
     * Number of bytes actually written to the buffer
     */
    int bytes_to_read = length;

    /*
     * If we're at the end of the message,
     * return 0 -> end of file
     */
    if (*msg_Ptr == 0)
        return 0;

    /* Make sure we don't read more chars than
     * those remaining to read
    	 */
    if (bytes_to_read > strlen(msg_Ptr))
        bytes_to_read=strlen(msg_Ptr);

    /*
     * Actually transfer the data onto the userspace buffer.
     * For this task we use copy_to_user() due to security issues
     */
    if (copy_to_user(buffer,msg_Ptr,bytes_to_read))
        return -EFAULT;

    /* Update the pointer for the next read operation */
    msg_Ptr+=bytes_to_read;

    /*
     * The read operation returns the actual number of bytes
     * we copied  in the user's buffer
     */
    return bytes_to_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/chardev_leds
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    int bytes_to_write = len;
    int j;
    kbd_driver= get_kbd_driver_handler();
    char myBuff[BUF_LEN];
    short leds = 0x0;
    unsigned int led_num = 0, led_caps = 0, led_scroll = 0;
    
    if (copy_from_user(myBuff, buff, bytes_to_write))
	return -EFAULT;
    for (j = 0; j < len; j++){
	if(myBuff[j] == '3' && !led_scroll){
	    led_scroll = 1;
	    leds |= SCROLL_LED_ON;        
	}
	if(myBuff[j] == '2' && !led_caps){
            led_caps = 1;
            leds |= CAPS_LED_ON;
	}
	if(myBuff[j] == '1' && !led_num){
	    led_num = 1;
	    leds |= NUM_LED_ON;
	}
    }
    
    set_leds(kbd_driver,leds);

    return bytes_to_write;
}
