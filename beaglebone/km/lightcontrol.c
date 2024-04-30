#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system_misc.h> /* cli(), *_flags */
#include <linux/uaccess.h>
#include <asm/uaccess.h> /* copy_from/to_user */

//For timers and interrupts
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("Dual BSD/GPL");

#define RED_GPIO 67
#define YELLOW_GPIO 68
#define GREEN_GPIO 44

/*Declaration of necessary driver functions*/
static int lightcontrol_init(void);
static void lightcontrol_exit(void);
static int lightcontrol_open(struct inode *, struct file *);
static int lightcontrol_release(struct inode *, struct file *);
static ssize_t lightcontrol_write(struct file *, const char *, size_t, loff_t *);


/* The file operations structure */
struct file_operations lightcontrol_fops = {
	write: lightcontrol_write,
	open: lightcontrol_open,
	release: lightcontrol_release,
};


/* Set required global variables */
static int lightcontrol_major = 61;
static char *lightcontrol_buffer;
static int lightcontrol_len;


/* Setting the init and exit functions */
module_init(lightcontrol_init);
module_exit(lightcontrol_exit);

static unsigned capacity = 256;
static unsigned bite = 128;
module_param(capacity, uint, S_IRUGO);
module_param(bite, uint, S_IRUGO);


//create an enum of the different traffic modes
enum ight_mode{
    GREEN,
    RED,
    YELLOW
};

static int lightcontrol_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int lightcontrol_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static int lightcontrol_init(void)
{
    int ret;
    ret = register_chrdev(lightcontrol_major, "lightcontrol", &lightcontrol_fops);
    if (ret < 0)
    {
            printk(KERN_ALERT
                    "lightcontrol: cannot obtain major number %d\n", lightcontrol_major);
            return ret;
    }
    lightcontrol_buffer = kmalloc(capacity, GFP_KERNEL);
    if (!lightcontrol_buffer)
    {
            printk(KERN_ALERT "Insufficient kernel memory\n");
            ret = -ENOMEM;
            goto fail;
    }
    memset(lightcontrol_buffer, 0, capacity);
    ret = gpio_request(RED_GPIO, "red_led");
    if (ret) {
        printk(KERN_ERR "Failed to request access to RED GPIO pin\n");
        return ret;
    }

    ret = gpio_request(YELLOW_GPIO, "yellow_led");
    if (ret) {
        printk(KERN_ERR "Failed to request access to YELLOW GPIO pin\n");
        gpio_free(RED_GPIO);
        return ret;
    }

    ret = gpio_request(GREEN_GPIO, "green_led");
    if (ret) {
        printk(KERN_ERR "Failed to request access to GREEN GPIO pin\n");
        gpio_free(RED_GPIO);
        gpio_free(YELLOW_GPIO);
        return ret;
    }
    printk(KERN_INFO "Successfully requested access to GPIO pins\n");

    gpio_direction_output(YELLOW_GPIO, 0);
    gpio_direction_output(RED_GPIO, 0);
    gpio_direction_output(GREEN_GPIO, 0);

    gpio_set_value(GREEN_GPIO, 1);


    printk(KERN_INFO "Light control module loaded\n");
    return 0;
fail:
	lightcontrol_exit();
	return ret;
}

static void lightcontrol_exit(void)
{

    // printk(KERN_INFO "GPIO pins released.\n");
    gpio_set_value(RED_GPIO, 0);
    gpio_set_value(YELLOW_GPIO, 0);
    gpio_set_value(GREEN_GPIO, 0);
    gpio_free(RED_GPIO);
    gpio_free(YELLOW_GPIO);
    gpio_free(GREEN_GPIO);

    /* Freeing the major number */
    unregister_chrdev(lightcontrol_major, "lightcontrol");
    /* Freeing buffer memory */
    if (lightcontrol_buffer)
    {
        kfree(lightcontrol_buffer);
    }
    printk(KERN_INFO "Light control module unloaded\n");
}

static ssize_t lightcontrol_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    int user_input;
    if (*f_pos >= capacity){
        printk(KERN_INFO
            "write called: process id %d, command %s, count %d, buffer full\n",
            current->pid, current->comm, count);
        return -ENOSPC;
    }
    /* do not eat more than a bite */
    if (count > bite) count = bite;
    /* do not go over the end */
    if (count > capacity - *f_pos) count = capacity - *f_pos;
    if (copy_from_user(lightcontrol_buffer + *f_pos, buf, count)){
        return -EFAULT;
    }
    // Input always in form of a number
    if(sscanf(lightcontrol_buffer+*f_pos,"%d", &user_input) == 1){
    }
    else{
        printk(KERN_ERR "Invalid format received\n");
        return -EINVAL;
    }
    // Control the traffic lights based on the user input
    if (user_input == 1)
    {
        //If the light is red then it stays red
        if(gpio_get_value(RED_GPIO) == 1){
            gpio_set_value(RED_GPIO,1);
            gpio_set_value(YELLOW_GPIO, 0);
            gpio_set_value(GREEN_GPIO, 0);
        }
        else if(gpio_get_value(YELLOW_GPIO) == 1){
            gpio_set_value(RED_GPIO,1);
            gpio_set_value(YELLOW_GPIO, 0);
            gpio_set_value(GREEN_GPIO, 0);
        }
        else if(gpio_get_value(GREEN_GPIO) == 1){
            gpio_set_value(RED_GPIO,0);
            gpio_set_value(YELLOW_GPIO, 1);
            gpio_set_value(GREEN_GPIO, 0);
        }
    }

    else if(user_input == 0){
        if (gpio_get_value(RED_GPIO) == 1)
        {
            gpio_set_value(RED_GPIO, 0);
            gpio_set_value(YELLOW_GPIO, 1);
            gpio_set_value(GREEN_GPIO, 0);
        }
        // Turn off yellow and turn on green if yellow is on
        else if (gpio_get_value(YELLOW_GPIO) == 1)
        {
            gpio_set_value(RED_GPIO, 0);
            gpio_set_value(YELLOW_GPIO, 0);
            gpio_set_value(GREEN_GPIO, 1);
        }
        // Keep green on if green is on
        else if (gpio_get_value(GREEN_GPIO) == 1)
        {
            gpio_set_value(GREEN_GPIO, 1);
        }
        else
        {
            return -EINVAL; // Return error if the traffic lights are in an invalid state
        }
    }
    else{
        return -EINVAL;
    }
    *f_pos += count;
    lightcontrol_len = *f_pos;
    return count;
}
