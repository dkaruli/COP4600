/**
 *Assignment 5
 *COP 4600
 *Team Members: Deni Karuli, Justin Gentry, Miles Friedman
 *Group #23
 *File Name: char-device.c
 *Due Date: April 25th, 2016
 *Citations:
 *	1-http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
 *	2-http://www.tldp.org/LDP/lkmpg/2.6/html/index.html
*/

//Required Libraries
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

//Declaring Globals
#define DEVICE_N "char-device"
#define CLASS_N  "char"
#define BUFFERMAX 1000

//Defining License
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deni Karuli, Justin Gentry, Miles Friedman");
MODULE_DESCRIPTION("Assignment 5 COP4600");
MODULE_VERSION("1.0");

//Common variables
static int    major_number;
static char   response[BUFFERMAX];
static short  response_size=0;
static int    access_counter = 0;
static struct class* char_Class = NULL;
static struct device* char_Device = NULL;

//Required Functions
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

//Operations Structure
static struct file_operations fops =
{
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};

static int __init chardevice_init(void){
 printk(KERN_INFO "Device driver starting initialization\n");
 
 //Create Unique Major Number for this session
 major_number = register_chrdev(0, DEVICE_N, &fops);
  
 //Error Check for New Session
 if(major_number < 0){
 printk(KERN_ALERT "FAILED TO CREATE NEW SESSION\n");
 return major_number;
 }
 printk(KERN_INFO "New Session Created: #%d\n",major_number);
 
 //Populate Class
 char_Class = class_create(THIS_MODULE, CLASS_N);
 
 //Error Check Class Creation
 if(IS_ERR(char_Class)){
 unregister_chrdev(major_number, DEVICE_N);
 printk(KERN_ALERT "FAILED TO CREATE CLASS\n");
 return PTR_ERR(char_Class);
 }
 printk(KERN_INFO "SUCCESSFULL CLASS CREATION\n");
 
 //Populate Driver
 char_Device = device_create(char_Class, NULL, MKDEV(major_number, 0), NULL, DEVICE_N);
 
 //Error Check Driver Creation
 if(IS_ERR(char_Device)){
 class_destroy(char_Class);
 unregister_chrdev(major_number, DEVICE_N);
 printk(KERN_ALERT "FAILED TO CREATE DRIVER\n");
 return PTR_ERR(char_Device);
 }
 printk(KERN_INFO "SUCCESSFULL DEVICE CREATION\n");
 return 0;
}

//Exit Method
static void __exit chardevice_exit(void){
 device_destroy(char_Class, MKDEV(major_number, 0));
 class_unregister(char_Class);
 class_destroy(char_Class);
 unregister_chrdev(major_number, DEVICE_N);
 printk(KERN_INFO "DEVICE DRIVER TERMINATED\n");
}

//Define functions open, read, write, release
static int dev_open(struct inode *inodep, struct file *filep){
 access_counter++;
 printk(KERN_INFO "DEVICE OPENED! ACCESS COUNTER: %d\n", access_counter);
 return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
 int errorCounter = 0;
 int k;
 if(response_size>len){
 int new_size = (response_size - len);
 errorCounter = copy_to_user(buffer, response, len);
 //Success Message
 if(errorCounter == 0){
 printk(KERN_INFO "USER HAS OBTAINED %d CHARS FROM SYSTEM\n", len);
 response_size = new_size;
 for(k = 0; k<BUFFERMAX; k++)
 {
 if(k<(BUFFERMAX-len)){
 response[k]=response[k+len];
 }
 else{
 response[k]= '\0';
 }
 }
 return 0;
 }
 //Error Message
 else{
 printk(KERN_INFO "USER HAS FAILED TO OBTAIN %d CHARS FROM SYSTEM\n", errorCounter);
 return -EFAULT;
 }
 }
 else{
 errorCounter = copy_to_user(buffer, response, response_size);
 //Success Message
 if(errorCounter == 0){
 printk(KERN_INFO "USER HAS OBTAINED %d CHARS FROM SYSTEM\n", response_size);
 for(k = 0; k<BUFFERMAX; k++){
 response[k]= '\0';
 }
 return (response_size = 0);
 }
 //Error Message
 else{
 printk(KERN_INFO "USER HAS FAILED TO OBTAIN %d CHARS FROM SYSTEM\n", errorCounter);
 return -EFAULT;
 }
 } 
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
 int i = 0;
 if((response_size+len)>BUFFERMAX){
 for(i = 0; i<(BUFFERMAX - response_size); i++){
 response[i+(response_size)] = buffer[i];
 }
 printk(KERN_INFO "MAXIMUM BUFFER SIZE REACHED, ONLY %d BYTES STORED\n", i);
 response_size = strlen(response);
 }
 else{
 if(strlen(response)<1){
 sprintf(response, "%s", buffer);
 }
 else{
 sprintf(response, "%s%s", response, buffer);
 }
 response_size = strlen(response);
 printk(KERN_INFO "SYSTEM OBTAINED %d CHARS FROM USER, %d BYTES LEFT\n", len,(BUFFERMAX -response_size));
 i = len;
 }
 return i;
}

static int dev_release(struct inode *inodep, struct file *filep){
printk(KERN_INFO "DEVICE SUCCESSFULLY TERMINATED!\n");
return 0;
}

module_init(chardevice_init);
module_exit(chardevice_exit);
