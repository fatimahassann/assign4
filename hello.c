#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h> 
#include <linux/netlink.h>
#include <linux/spinlock.h>
#include <net/sock.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");


struct myfile 
{
	struct file *f;
	mm_segment_t fs;
	loff_t pos;
};

struct myfile* open_file_for_read(char* filename)
{	
	struct myfile * mf=kmalloc(sizeof(struct myfile),GFP_KERNEL);	

	mf->f = filp_open(filename,0,0);
	mf->pos=0;


	if(IS_ERR(mf->f))
	{	
		printk(KERN_ALERT "error");
		mf->f=NULL;
	}

	return mf;
}

volatile int read_from_file_until (struct myfile *mf, char *buf, unsigned long vlen)
{
    
	int read;

	mf->fs=get_fs();
	set_fs(KERNEL_DS);

	read= vfs_read(mf->f, buf, vlen, &mf->pos);

	set_fs(mf->fs);
	

	int counter=0;
	int i=0;
	while(counter!=3)
	{
		if(buf[i]==' ')
			counter++;
		 /*printk(KERN_ALERT "%s", buf[i]);*/
		i++;

	}

	char buf2[i];
	strncpy(buf2,buf,i);
	buf2[i]=NULL;
	printk(KERN_ALERT "%s\n", buf2);
	

	return read;
}

void close_file(struct myfile *mf)
{
	if(mf)
		filp_close(mf->f,NULL);

	kfree(mf);
}


static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello World CSCE-3402 :) \n");
	
	struct myfile * mf=kmalloc(sizeof(struct myfile), GFP_KERNEL);
        mf = open_file_for_read("/proc/version");
	

	int r;
	char *buf=kmalloc(1000,GFP_KERNEL);
	r=read_from_file_until(mf,buf,1000);
	
	close_file(mf);	
	kfree(buf);
	return 0;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_ALERT "Bye Bye CSCE-3402 :) \n");
}

module_init(hello_init);
module_exit(hello_cleanup);

