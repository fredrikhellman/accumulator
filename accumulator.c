/* This module was written based on the two following sources:
 *  * https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234
 *  * https://linux-kernel-labs.github.io/master/labs/device_drivers.html
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fredrik");
MODULE_DESCRIPTION("Accumulates numbers");
MODULE_VERSION("1.0");

#define MY_MAJOR 123
#define DEVICE_NAME "accumulator"
#define NUMBER_LEN 32

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset);
static ssize_t device_write(struct file *flip, const char __user *buffer, size_t len, loff_t *offset);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);

static struct file_operations file_ops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

struct my_device_data {
  struct cdev cdev;
  char number[NUMBER_LEN+1];
} dev;

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset) {
  struct my_device_data *my_data;
  size_t count;
  
  if (*offset > 0) {
    return 0;
  }

  my_data = flip->private_data;

  count = len < sizeof(my_data->number) ? len : sizeof(my_data->number);
  
  if (copy_to_user(buffer, my_data->number, count))
    return -EFAULT;
  
  *offset += count;
  
  return count;
}

static ssize_t device_write(struct file *flip, const char __user *buffer, size_t len, loff_t *offset) {
  struct my_device_data *my_data;
  size_t count;
  char workspace[NUMBER_LEN+1];

  long input;
  long number;
  
  if (*offset > 0) {
    return -EINVAL;
  }

  my_data = flip->private_data;

  count = len < sizeof(workspace) ? len : sizeof(workspace);
  
  memset(workspace, 0, sizeof(workspace));
  if (copy_from_user(workspace, buffer, count)) {
    return -EFAULT;
  }

  if (kstrtol(workspace, 10, &input)) {
    return -EFAULT;
  }

  if (kstrtol(my_data->number, 10, &number)) {
    return -EFAULT;
  }

  number += input;

  snprintf(my_data->number, NUMBER_LEN+1, "%ld", number);  
  
  *offset += count;
  
  return count;
}

static int device_open(struct inode *inode, struct file *file) {
  struct my_device_data *my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
  file->private_data = my_data;
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
 return 0;
}

static int __init accumulator_init(void) {
  int err;
  
  err = register_chrdev_region(MKDEV(MY_MAJOR, 0), 1, DEVICE_NAME);
  if (err != 0) {
    printk(KERN_ALERT "Failed to register character device region: %d\n", err);
    return err;
  }

  cdev_init(&dev.cdev, &file_ops);
  cdev_add(&dev.cdev, MKDEV(MY_MAJOR, 0), 1);

  dev.number[0] = '0';

  printk(KERN_INFO "accumulator loaded with device major number %d\n", MY_MAJOR);
  return 0;
}

static void __exit accumulator_exit(void) {
  cdev_del(&dev.cdev);
  unregister_chrdev_region(MKDEV(MY_MAJOR, 0), 1);
  printk(KERN_INFO "accumulator unloaded\n");
}

module_init(accumulator_init);
module_exit(accumulator_exit);
