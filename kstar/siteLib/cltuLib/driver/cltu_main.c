/******************************************************************************
 *  Copyright (c) 2010 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE


2010. 3. 10
made by woong ryol, Lee

2013. 4. 4 
FPGA updated....


*/



/*#include <linux/config.h> */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/mm.h>

#include <linux/fs.h>
#include "linux/version.h"


#include <linux/interrupt.h>
#include <linux/wait.h>


#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include "asm/io.h"        /* ioremap, writel, readl, .. */
#include "asm/uaccess.h"   /* copy_from/to_user, .. */

#include "asm/pgtable.h" /* pgprot_noncached(prot) */

#include "asm/irq.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,1))
#include <linux/delay.h> /* for udelay */
#endif



#include "ioctl_cltu.h"
#include "cltu_main.h"
#include "cltu_debug.h"
#include "cltu_Def.h"

#if 1
#include "drv_xtu2.h"
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/version.h>
#endif



DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);


/*#define CLTU_DUAL_PORT_SIZE 0x8000 */
/*static u32 CLTU_DUAL_PORT_ADDR; */


/* for high kernel version(3.7.10-gentoo).  assisted by POSTECH  2013. 6. 27*/
#ifndef VM_RESERVED
#define VM_RESERVED  (VM_DONTEXPAND | VM_DONTDUMP )
#endif


#define MAXDEV 4

int cltu_use_interrupts = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
module_param(cltu_use_interrupts, int, 0644);
MODULE_PARM_DESC(cltu_use_interrupts, "use interrupts [0 = OFF]");

#endif


/* Globals */
/*
static int       cltu_major = 0;
cltu_device_t     cltu_device[MAX_CLTU_CARDS+1];
cltu_device_t*    cltu_device_irq_cmp[MAX_DEVICE_IRQ_CMP];
int           cltu_number_of_cards = 0;
*/
/* Forward declaration */
/*static void cltu_exit(void); */



int prev_msb=0, prev_nsb=0;


/** Globals .. keep to a minimum! */
char cltu_driver_name[] = "CLTU";
char cltu__driver_string[] = "NFRI timing synchronization device";
char cltu__copyright[] = "Copyright (c) 2010 NFRI, woong";
static CLTU_Device* devices[MAXDEV];
static int idx=0;

/*struct class* cltu_device_class; */
/*void *buff;*/

/*
void simple_vma_open(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "Simple VMA open, virt %lx, phys %lx\n",
	vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}
void simple_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "Simple VMA close.\n");
}

static struct vm_operations_struct simple_remap_vm_ops = {
.open = simple_vma_open,
.close = simple_vma_close,
};
*/


/* avoid GCC compile error!.  overflow the frame size which is limited in 2048 byte */
/* move here from function. STACK_CHECK_MAX_FRAME_SIZE probably 208 */
unsigned int logbuf[XTU_BMEM_SIZE]; 

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
CLTU_Device* cltu_lookupDevice(struct device *dev)
{
	int id;

	for (id = 0; id < idx; ++id){
		CLTU_Device* cltu_device = devices[id];
		if (dev == &devices[id]->pci_dev->dev){
			return cltu_device;
		}
	}
	
	return 0;
}
#else
CLTU_Device* cltu_lookupDevice(struct pci_dev *dev)
{
	int id;

	for (id = 0; id < idx; ++id){
		CLTU_Device* cltu_device = devices[id];
		if (dev == devices[id]->pci_dev){
			return cltu_device;
		}
	}
	return 0;
}
#endif

static CLTU_Device *cltu_lookup_device(struct inode* inode)
{
	int major = MAJOR(inode->i_rdev);
	int isearch;

	for (isearch = 0; isearch < idx; ++isearch){
		CLTU_Device* device = devices[isearch];

		assert(device != 0);

		if (device->ldev.major == major){
/*			info("\"%s\" major : %d", device->ldev.drv_name, major); */
			return device;
		}
	}

	assert(0);
	return 0;
}




/************************************************/
/* it happen after cltu_exit() or  ctrl+C pressed or remod...................       */

int cltu_release(struct inode *inode, struct file *filp)
{
/*	CLTU_Device  *device;
	int  idev;
*/
/*	info("called cltu_release" ); */
/*
	if( buff != NULL)
	{
		info(" free vmalloc buffer");
		vfree(buff);
	}
*/
#if 0
	
	for (idev = 0; idev <= idx; idev++) 
	{
		device = devices[idev];
		
		info ("CLTU: Device #%d: iounmap CLTU PCI control base", device->idx);
		/* Free interrupt handler */
/*		if (device->interrupt_line)
		{
			writel(0x0, device->virt_addr + 0x68);

			free_irq (device->interrupt_line, device);
			cltu_device_irq_cmp[device->interrupt_line] = 0;
			device->interrupt_line = 0;
			printk ("CLTU: Device #%d: Removing IRQ #%d\n",
				device->devno, device->pcidev->irq);
		}
*/
		/* Unmap CLTU control base */
		if (device->virt_addr) 
		{
			info ("CLTU: Device #%d: iounmap CLTU PCI control base", device->idx);
			iounmap (device->virt_addr);
			device->virt_addr = NULL;
		}

	}
#endif
	return 0;
}


int cltu_mmap(struct file *filp, struct vm_area_struct *vma)
{
	CLTU_Device *device;
	unsigned long virtaddr  = 0;
	unsigned long physaddr  = 0;
	unsigned long size      = 0;
/*	unsigned int pageNumber = 0;*/
	
#if 0
	info("vm_pgoff   0x%x", (unsigned int)vma->vm_pgoff << PAGE_SHIFT);
	info("vm_start   0x%x", (unsigned int)vma->vm_start );
	info("vm_end     0x%x", (unsigned int)vma->vm_end );
	info("vm_flags   0x%x", (unsigned int)vma->vm_flags );
#endif

	/* Get cltu data struct pionter */
	if (!filp->private_data) 
	{
		err ("cltu_mmap() : filp->private_data = NULL pointer\n");
		return -EAGAIN;
	}
	else
		device = (CLTU_Device*)filp->private_data;	

	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	
	virtaddr = vma->vm_start;
	size     = vma->vm_end - vma->vm_start;
/*
	info("vmalloc test.");
	buff = vmalloc(4*PAGE_SIZE);
	if( buff == NULL) info("vmalloc... failed!");
	else {
		*((volatile unsigned int*)buff) = 0xabcd1234;
		}
*/

	if( device->card_ver == VERSION_R1 ) {
		physaddr = device->phys_pci_bar2;
	}
	else if ( device->card_ver == VERSION_R2 ) {
/*		physaddr = device->phys_pci_bar0 ; */

/*		info("physaddr addr: 0x%x", (unsigned int)device->phys_addr_bar0 ); */
		physaddr = device->phys_addr_bar0; 
/*
		pageNumber = physaddr/PAGE_SIZE;
*/
/*		info("page>>PAGE_SHIFT: %d, pgN0: %d", physaddr>>PAGE_SHIFT, pageNumber);  same result */

/*
		physaddr = virt_to_phys(device->virt_addr);
		info("virtual addr: 0x%x", (unsigned int)device->virt_addr );
		info("physaddr addr: 0x%x", (unsigned int)physaddr);
*/
/*		physaddr = (unsigned long)buff ; */

	}
	else {
		err ("cltu_mmap() : Wrong version! (%d)", device->card_ver);
		return -EAGAIN;
	}

#if 0
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
	if( remap_pfn_range ( vma, vma->vm_start, physaddr>>PAGE_SHIFT, size, pgprot_noncached(vma->vm_page_prot) ) ) 
/*	if( remap_pfn_range ( vma, vma->vm_start, pageNumber, size, pgprot_noncached(vma->vm_page_prot) ) ) */
#else 
	if( remap_page_range( vma, vma->vm_start, physaddr, size, pgprot_noncached(vma->vm_page_prot) ) )
#endif
	{
		return -EAGAIN;
	}
#else


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
	if( REMAP_PAGE_RANGE(vma, virtaddr, physaddr, size, pgprot_noncached(vma->vm_page_prot) ) )
		return -EAGAIN;
#else
	if( REMAP_PAGE_RANGE(vma, virtaddr, physaddr, size, vma->vm_page_prot ) )
		return -EAGAIN;
#endif

#endif

/*
	vma->vm_ops = &simple_remap_vm_ops;
	simple_vma_open(vma);
*/
	return 0;

}
/*
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
int cltu_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
int cltu_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
int cltu_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
long cltu_ioctl( struct file *filp, unsigned int cmd, unsigned long arg )
#endif
{
	int ret=0;
	int test=0;
	unsigned int argVal;
	CLTU_Device *device;
/*	CLTU2_PORT_CONFIG  cfg; */
	ozXTU_DBGM_T xtu_dbgm; 
	ozPCONFIG_HLI_T myhli;
	ozPCONFIG_LLI_T mylli;
/*	ozPCONFIG_T   myPcfg; */
	ozTIME_T mltime;
	ozSHOT_TIME_T mShotTime;
	ozXTU_MODE_T clkCfg;
/*	unsigned int logbuf[XTU_BMEM_SIZE]; */
	
/*	unsigned char sect;
	unsigned int current_time, current_tick;
	unsigned int shot_start_time, shot_start_tick;
	unsigned int shot_end_time, shot_end_tick;  
*/
	/* Get cltu data struct pionter */
	if (!filp->private_data) 
	{
		err ("filp->private_data = NULL pointer\n");
		return -ENODEV;

	}
	else {
		device = (CLTU_Device*)filp->private_data;
		test	= down_interruptible(&device->sem);

		if (test)
		{
			/* We didn't get the semaphore. */
			err("We didn't get the semaphore.");
			ret	= -ERESTARTSYS;
			return ret;
		}
	}
	

	if( _IOC_TYPE(cmd) != IOCTL_XTU_MAGIC ){
		up(&device->sem);
		return -EINVAL; /* The IOCTL code isn't ours.*/
	}
	if( _IOC_NR(cmd) >= IOCTL_XTU_MAXNR ) {
		up(&device->sem);
		return -EINVAL; /* The IOCTL service is unrecognized. */
	}

	
#if 0
	size = _IOC_SIZE(cmd);
	if( size )
	{
		ret = 0;
		/* This function may sleep */
		if( _IOC_DIR(cmd) & _IOC_READ ) ret = access_ok(VERIFY_WRITE, (void *) arg, size);
		else if( _IOC_DIR(cmd) & _IOC_WRITE) ret = access_ok(VERIFY_READ, (void *) arg, size);

		if( ret)  {
			err (" access_ok ... error.");
			return ret;
		}
	}
	else ret = -EINVAL;
#endif

	

	switch(cmd) {
	case IOCTL_XTU_GETSTATE : break;
	case IOCTL_XTU_READ: 
		info("virt_addr0[0]:    0x%x, addr: %p", *((volatile unsigned int*)device->virt_addr), device->virt_addr );
		info("virt_addr0[c]:    0x%x", *((volatile unsigned int*)(device->virt_addr +0xc)) );
		argVal = *((volatile unsigned int*)device->virt_addr);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) {
			err("%s: IOCTL_XTU_READ, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		}
		break;
	case IOCTL_XTU_WRITE: 
		ret = copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) );
		if( ret > 0) {
			err("%s: IOCTL_XTU_WRITE, copy_from_user failed(%d)\n", device->ldev.drv_name, ret );
		}
		info("user write data: 0x%x", argVal);
		break;
	case IOCTL_XTU_INTERRUPT_ATTACH:
#if 0
		err =  copy_from_user(&IntCallback, (CLTU_INTERRUPT_CALLBACK*)arg, sizeof(CLTU_INTERRUPT_CALLBACK)) ;
		if (err != WR_OK) {
			printk("copy_from_user failed\n");
			return err;
		}
		
/*		err = copy_to_user ((void*)arg, (void*)&kio, sizeof(dpio2_kio_t)); */


/*			IntCallback = (CLTU_INTERRUPT_CALLBACK*) arg; */
			cltu_device.callback_sem = IntCallback.sem_cb;
/*			callback_sem =  (cltu_device )(arg); 
			printk(KERN_ALERT "callback_sem: %d\n", callback_sem); */
/*			pci_read_config_byte(device->pcidev, PCI_INTERRUPT_LINE, &device->interrupt_line); */
#endif
			break;
	case IOCTL_XTU_INTERRUPT_ENABLE:
		info("interrupt enable, base: 0x%x", *((unsigned int *)device->virt_addr));
		/* Copy user buffer */
		ret = copy_from_user (&argVal, (int *)arg, sizeof(unsigned int));
		if (ret != 0) 
		{
			err("copy_from_user failed");
			up(&device->sem);
			return ret;
		}
		info("arg : %d\n", argVal); 

		/* Write enable interrupt   */
		writel(0xffffffff, device->virt_addr + 0x68);
#if 0
		writel(0x0, device->virt_addr + 0x68);
#endif
		break;

	case IOCTL_XTU_INTERRUPT_DISABLE:
		/* Copy user buffer */
		ret = copy_from_user (&argVal, (int *)arg, sizeof(unsigned int));
		if (ret != 0) 
		{
			err("copy_from_user failed");
			up(&device->sem);
			return ret;
		}

		info("arg : %d", argVal); 

		/* Write disable interrupt   */
		writel(0x0, device->virt_addr + 0x68);
		break;

	case IOCTL_XTU_SET_ENDIAN:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_ENDIAN, copy_from_user failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetEndian(device);
		break;
	case IOCTL_XTU_SET_MODE:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_MODE failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
/*		device->u32Type = argVal;*/ /* mode */
		device->xtuMode.mode = argVal; /* mode */
		r2_SetMode(device, device->xtuMode.mode, device->node_id);
		break;
	case IOCTL_XTU_SET_DBG_MODE:
		if( copy_from_user( (void *)&xtu_dbgm, (void *)arg, sizeof(ozXTU_DBGM_T) ) > 0 )
		{
			err("%s: IOCTL_XTU_SET_DBG_MODE failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetDBGMode(device, &xtu_dbgm);
		break;
	case IOCTL_XTU_SET_LOCAL_TIME:
		if( copy_from_user( (void *)&mltime, (void *)arg, sizeof(ozTIME_T) ) > 0 )
		{
			err("%s: IOCTL_XTU_SET_LOCAL_TIME failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetLocalTime(device, &mltime);
		break;
	case IOCTL_XTU_SET_CTU_DEFAULT_DELAY:
		r2_SetCTUShotDelay(device);
		break;
	case IOCTL_XTU_SET_LTU_DELAY:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_LTU_DELAY failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		device->shot_delay_ns = argVal;
		r2_SetLTUShotDelay(device);
		break;
	case IOCTL_XTU_SET_PORT_ACTIVE_LEVEL:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_PORT_ACTIVE_LEVEL failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetPortActiveLevel(device, argVal);
		break;
	case IOCTL_XTU_SET_PORT_CTL:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_PORT_CTL failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		device->xtu_pcfg[device->curPort].port_en = argVal;
		r2_SetPortCtrl(device, device->curPort);
//		info("nhli_num:%d, port_en:%d", device->xtu_pcfg[device->curPort].nhli_num, device->xtu_pcfg[device->curPort].port_en );
		break;
	case IOCTL_XTU_SET_PORT_NHLI_NUM:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_PORT_NHLI_NUM failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		device->xtu_pcfg[device->curPort].nhli_num = argVal;
//		info("nhli_num:%d, port_en:%d", device->xtu_pcfg[device->curPort].nhli_num, device->xtu_pcfg[device->curPort].port_en );
		break;
	case IOCTL_XTU_SET_PORT_HLI:
		if( copy_from_user( (void *)&myhli, (void *)arg, sizeof(ozPCONFIG_HLI_T) ) > 0 )
		{
			err("%s: IOCTL_XTU_SET_PORT_HLI failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
#if PRINT_HLI_CONFIG
		info("curr port: %d, curr HLI %d", device->curPort, device->curHLI);
		info(" sec, ms, ns ( %d, %d, %d)", myhli.intv.sec, myhli.intv.ms, myhli.intv.ns);
		info(" nhli_num, nlli_num ( %d, %d)", myhli.nhli_num, myhli.nlli_num);
#endif
		r2_SetPortHLI(device, device->curPort, device->curHLI, &myhli);
		break;
	case IOCTL_XTU_SET_PORT_LLI:
		if( copy_from_user( (void *)&mylli, (void *)arg, sizeof(ozPCONFIG_LLI_T) ) > 0 )
		{
			err("%s: IOCTL_XTU_SET_PORT_LLI failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
#if PRINT_LLI_CONFIG
		info("curr port: %d, cur LLI %d", device->curPort, device->curLLI);
		info("  sec, ms, ns ( %d, %d, %d)", mylli.intv.sec, mylli.intv.ms, mylli.intv.ns);
		info("    clk_freq, nlli_num ( %d, %d), opt: 0x%x", mylli.clk_freq, mylli.nlli_num, mylli.option);
#endif
		r2_SetPortLLI(device, device->curPort, device->curLLI, &mylli);
		break;
	case IOCTL_XTU_SELECT_PORT:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SELECT_PORT failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		device->curPort = (unsigned char)argVal;
		break;
	case IOCTL_XTU_SELECT_HLI:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SELECT_HLI failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		if(argVal >= XTU_ADDR_PHLI_MAX_NUM)
		{
			err("%s: XTU_ADDR_PHLI_MAX_NUM limit error. (%d)\n", device->ldev.drv_name, argVal);
			up(&device->sem);
			return (-EFAULT );
		}
		device->curHLI=argVal;
		break;
	case IOCTL_XTU_SELECT_LLI:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SELECT_LLI failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		if(argVal >= XTU_ADDR_PLLI_MAX_NUM)
		{
			err("%s: XTU_ADDR_PLLI_MAX_NUM limit error. (%d)\n", device->ldev.drv_name, argVal);
			up(&device->sem);
			return (-EFAULT );
		}
		device->curLLI=argVal;
		break;
	case IOCTL_XTU_SET_SHOT_TIME:
		if( copy_from_user (&mShotTime, (unsigned int *)arg, sizeof(ozSHOT_TIME_T) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_SHOT_TIME failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetShotTime(device, &mShotTime);
		break;
	case IOCTL_XTU_SHOT_START:
		r2_StartShotNow(device);
		break;
	case IOCTL_XTU_SHOT_END:
		r2_StopShotNow(device);
		break;
	case IOCTL_XTU_SET_IRIGB_SRC:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_IRIGB_SRC failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetIRIG_B_Src(device, argVal );
		break;
	case IOCTL_XTU_SET_CLK_CONFIG:
		if( copy_from_user( (void *)&clkCfg, (void *)arg, sizeof(ozXTU_MODE_T) ) > 0 )
		{
			err("%s: IOCTL_XTU_SET_CLK_CONFIG failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetClkConfig(device, clkCfg.tlkRxClk_en, clkCfg.tlkTxClk_sel, clkCfg.mclk_sel);		
		break;
	case IOCTL_XTU_SET_PERMANENT_CLK:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SET_PERMANENT_CLK failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		r2_SetPermClk(device, argVal );
		break;

		
		
	case IOCTL_XTU_GET_ENDIAN_CHECK:
		r2_GET_ENDIAN_CHECK(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_ENDIAN_CHECK, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_CLK_CONFIG:
		r2_GET_CLK_CONFIG(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_CLK_CONFIG, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_DBG_CONFIG:
		r2_GET_DBG_CONFIG(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_DBG_CONFIG, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_CLK_STATUS:
		r2_GET_CLK_STATUS(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_CLK_STATUS, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_SEL_XTU_MODE:
		r2_GET_SEL_XTU_MODE(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_SEL_XTU_MODE, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_SEL_IRIGB:
		r2_GET_SEL_IRIGB(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_SEL_IRIGB, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_CURR_TIME:
		r2_GET_TX_CURR_TIME(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_CURR_TIME, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_CURR_TIME_4:
		r2_GET_TX_CURR_TIME_4(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_CURR_TIME_4, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_CURR_TIME_8:
		r2_GET_TX_CURR_TIME_8(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_CURR_TIME_8, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_COMP_DELAY:
		r2_GET_TX_COMP_DELAY(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_COMP_DELAY, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME:
		r2_GET_TX_SHOT_TIME(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME_4:
		r2_GET_TX_SHOT_TIME_4(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME_4, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME_8:
		r2_GET_TX_SHOT_TIME_8(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME_8, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME_16:
		r2_GET_TX_SHOT_TIME_16(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME_16, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME_20:
		r2_GET_TX_SHOT_TIME_20(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME_20, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_TX_SHOT_TIME_24:
		r2_GET_TX_SHOT_TIME_24(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_TX_SHOT_TIME_24, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_CURR_TIME:
		r2_GET_RX_CURR_TIME(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_CURR_TIME, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_CURR_TIME_4:
		r2_GET_RX_CURR_TIME_4(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_CURR_TIME_4, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_CURR_TIME_8:
		r2_GET_RX_CURR_TIME_8(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_CURR_TIME_8, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_COMP_DELAY:
		r2_GET_RX_COMP_DELAY(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_COMP_DELAY, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME:
		r2_GET_RX_SHOT_TIME(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME_4:
		r2_GET_RX_SHOT_TIME_4(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME_4, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME_8:
		r2_GET_RX_SHOT_TIME_8(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME_8, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME_16:
		r2_GET_RX_SHOT_TIME_16(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME_16, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME_20:
		r2_GET_RX_SHOT_TIME_20(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME_20, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_RX_SHOT_TIME_24:
		r2_GET_RX_SHOT_TIME_24(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_RX_SHOT_TIME_24, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_VENDOR_ID:
		r2_GET_VENDOR_ID(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_VENDOR_ID, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_DEVICE_ID:
		r2_GET_DEVICE_ID(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_DEVICE_ID, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_VERSION:
		r2_GET_VERSION(device, &argVal);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_VERSION, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_LOG_GSHOT:
		r2_GetGshotLog(device, logbuf);
		ret = copy_to_user ((void*)arg, (void*)&logbuf, sizeof(logbuf)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_LOG_GSHOT, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;
	case IOCTL_XTU_GET_LOG_PORT:
		r2_GetPortLog(device, device->curPort, logbuf);
		ret = copy_to_user ((void*)arg, (void*)&logbuf, sizeof(logbuf)); 
		if( ret > 0 ) err("%s: IOCTL_XTU_GET_LOG_PORT, copy_to_user failed(%d)\n", device->ldev.drv_name, ret );
		break;

	case IOCTL_XTU_SET_READ_OFFSET:
		if( copy_from_user (&argVal, (unsigned int *)arg, sizeof(unsigned int) ) > 0)
		{
			err("%s: IOCTL_XTU_SELECT_LLI failed\n", device->ldev.drv_name );
			up(&device->sem);
			return( -EFAULT );
		}
		device->read_offset = argVal;
//		info("read offset: 0x%x", device->read_offset);
		break;
		
		
	default:
		ret = -ENOTTY;
		break;
	}
/*
	if( ret != 0 ) err("ret: %d", ret);
*/	
	up(&device->sem);
	return ret;
}

ssize_t cltu_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	CLTU_Device* device;
	ssize_t			ret;
/*	unsigned int n, i;
	unsigned int *value;
*/
	/* Get cltu data struct pionter */
	if (!filp->private_data) 
	{
		err ("cltu_read() : filp->private_data = NULL pointer");
		return ( -EFAULT );
	}
	else
		device = (CLTU_Device*)filp->private_data;

	if (device == NULL)
	{
		/* The referenced device doesn't exist. */
		ret	= -ENODEV;
		return ret;
	}


//	if( device->block_flag == 1 )
//		return -1;
/*	n = count / 4;
	if( n == 0 ) n = 1;
	value = kmalloc( n*sizeof(unsigned int), GFP_KERNEL);
	for(i=0; i< n ; i++) {
		*value + i = READ32(device->virt_addr + device->read_offset +i );
		info("value[%d]: 0x%x", i, value[i] );
	}
*/

//	argVal = *((volatile unsigned int*)(device->virt_addr + device->read_offset));
//	ret = copy_to_user ((void*)buf, (void*)value, count); 
	ret = copy_to_user ((void*)buf, (void*)(device->virt_addr + device->read_offset ), count); 
	if( ret > 0 ) {
		err("%s: cltu_read, copy_to_user failed(%d)\n", device->ldev.drv_name, (int)ret );
	}

//	kfree(value);

	return count;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
irqreturn_t cltu_isr(int irq, void* dev_id)
{
	int t_msb, t_nsb;
	
	CLTU_Device* device = (CLTU_Device*)dev_id;

	/* Check for NULL pointer */
	if ( device->virt_addr == NULL) 
	{
		err("NULL pointer!, device->virt_addr");
		return IRQ_NONE;
	}


	t_msb = readl( device->virt_addr+ 11*sizeof(int) );
	t_nsb = readl( device->virt_addr+ 12*sizeof(int) );
/*	err("t_msb: 0x%x, t_nsb: 0x%x\n", t_msb, t_nsb);  */

	if( (t_msb == prev_msb) && (t_nsb == prev_nsb) ) {
		return IRQ_NONE;
	}

//	if( device->block_flag == 1 ) 
//	{
//		wake_up_interruptible( &WaitQueue_Read);
//		device->block_flag = 0 ;
//	}	

	prev_msb = t_msb;
	prev_nsb = t_nsb;

	device->interrupt_count[0]++;

	return IRQ_HANDLED;
}
#endif

int cltu_open(struct inode *inode, struct file *filp)
{
	int err = 0;
	CLTU_Device* device = cltu_lookup_device(inode);

	if (device == 0){
		return -ENODEV;
	}

	/* Set filp->private_data  */
	filp->private_data = device;

	return err;
}

static int cltu_map_pci_memory (CLTU_Device  *device)
{
	int erret=0;
	int i;
	u32 flag0, flag2;

	/* get Memory address and resigstration ...... */
	device->phys_pci_bar0 = pci_resource_start( device->pci_dev, 0);
	device->phys_pci_bar1 = pci_resource_start( device->pci_dev, 1);
	device->phys_pci_bar2 = pci_resource_start( device->pci_dev, 2);

	device->phys_pci_bar0_len= pci_resource_len( device->pci_dev, 0);
	device->phys_pci_bar2_len= pci_resource_len( device->pci_dev, 2);

	flag0 = pci_resource_flags(device->pci_dev, 0);
	flag2 = pci_resource_flags(device->pci_dev, 2);

	

/*	CLTU_DUAL_PORT_ADDR = device->phys_pci_bar2; */

	if( device->card_ver == VERSION_R2) { 
#if 0
		device->phys_pci_bar0 &= PCI_BASE_ADDRESS_MEM_MASK;
		device->phys_pci_bar0 &= PAGE_MASK;
#endif

		if (flag0  & IORESOURCE_IO)
		{ 
			err("BAR0 is I/O mapped and MUST be memory mapped\n");
			return -1;
		}
		i = check_mem_region(device->phys_pci_bar0, device->phys_pci_bar0_len);
		if (i != 0) {
			err("BAR0 check_mem_region().... failed!");
			return -1;
		}
		/*  skip request_mem_region */

		device->virt_addr = ioremap_nocache(device->phys_pci_bar0, device->phys_pci_bar0_len);
		if( device->virt_addr != NULL) {
/*			info("virt_addr0[0]:    0x%x, addr: 0x%x", *((volatile unsigned int*)device->virt_addr), (unsigned int)device->virt_addr );
			info("virt_addr0[c]:    0x%x", *((volatile unsigned int*)(device->virt_addr +0xc)) );
*/
			}
		else
			err("Get base0 address failed!\n" );
	}
	
	else if (device->card_ver == VERSION_R1) 
	{ 
		if (flag2  & IORESOURCE_IO)
		{ 
			err("BAR2 is I/O mapped and MUST be memory mapped\n");
			return -1;
		}
		i = check_mem_region(device->phys_pci_bar2, device->phys_pci_bar2_len);
		if (i != 0) {
			err("BAR2 check_mem_region().... failed!");
			return -1;
		}
		/*	skip request_mem_region */
				

		device->virt_addr= ioremap_nocache(device->phys_pci_bar2, device->phys_pci_bar2_len);

		if( device->virt_addr!= NULL)
/*			info("ctrl_bar2[0]:    0x%x", *((volatile unsigned int*)device->virt_addr) ); */
		;
		else
			err("Get base2 address failed!" );
	}

	return erret;

}


static void cltu_device_create(CLTU_Device* device, struct pci_dev *pci_dev)
{
	devices[idx] = device;
	device->pci_dev = pci_dev;
	device->idx = idx++;
	sprintf(device->ldev.drv_name, "cltu.%d", device->idx );

#if 0
	pci_set_master(pci_dev);
#endif

	sema_init(&device->sem, 1);


	cltu_map_pci_memory(device);

	info("job finished");

/*	return device; */
}

/*
static void cltu_set_dev_type(
	CLTU_Device* device, const struct pci_device_id *ent)
{
	if ( ent->vendor == CLTU_VENDOR_ID )
	{
		if( ent->device == CLTU_DEVICE_ID ) 
		{
			device->card_ver = VERSION_R1;
			sprintf(device->model_name, "%s", "cltu_R1");
			info("Old (first version)");
		} 
		else if( ent->device == CLTU_DEVICE_ID_2 ) 
		{
			device->card_ver = VERSION_R2;
			sprintf(device->model_name, "%s", "cltu_R2");
		}
		else
			err( "It's not correct device! 0x%x\n", ent->device);
	} 
	else if ( ent->vendor == CLTU_VENDOR_ID_2  &&  ent->device == CLTU_DEVICE_ID_2 )
	{
		device->card_ver = VERSION_R2;
		sprintf(device->model_name, "%s", "cltu_R2");
		info("New (second version).");
	} 
	else
	{
		err( "It's not correct vendor! 0x%x\n", ent->vendor);
	}

}
*/
static int 
cltu_device_init(CLTU_Device *device)
{
	
	int rc;
	u32 addr;         /* Temp for addr value                   */
	char result;

/* move to probe 
	rc = pci_enable_device(device->pci_dev);
	if (rc != 0){
		err("pci_enable_device FAILED returning %d", rc);
		return rc;
	}
*/
	rc = register_chrdev(device->ldev.major = 0, 
			     device->ldev.drv_name, 
			     &cltu_proc_ops);

	if (rc < 0){
		err( "can't get major %d", device->ldev.major );
		return rc;
	}else{
		device->ldev.major = rc;
/*		info( "device major set %d", device->ldev.major ); */
	}

/*
 * if interrupts are requested globally, AND the board is wanting an interrupt,
 * ask Linux for the use thereof
 */
 #if 0
	if (cltu_use_interrupts != 0 && device->pci_dev->irq != 0)
	{

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19))
		rc = request_irq(device->pci_dev->irq, (void *)cltu_isr, SA_SHIRQ, device->ldev.drv_name, device); 
#else
		rc = request_irq( 
			device->pci_dev->irq, 
			(void *)cltu_isr, 
			IRQF_SHARED, /* IRQF_DISABLED | IRQF_SHARED */
			device->ldev.drv_name, 
			device
		);
#endif

		if (rc != 0){
			err("request_irq %d FAILED", device->pci_dev->irq);
			return rc;
		}
	}
#endif
/*
	if (device->mbox_sync.using_interrupt){
		rc = acq200_sendCommand(device, 
			   BP_SET_FUNCODE(BP_FC_SET_INTERRUPTS)+BP_SET_A1(3));
		if (rc != 0){
			err("BP_FC_SET_INTERRUPTS failed %d", rc);
			return rc;
		}
	}

	if (device->completed_onetime_setups == 0){
		unsigned funcode = 
			BP_SET_FUNCODE(BP_FC_SET_HOST_DMABUF)|
			BP_SET_A1(NMAPPINGS);
		rc = acq200_sendCommandA3A4(device,
			funcode,
			(u32)device->dma_buf.pa, 
			dma_buf_len(&device->dma_buf));
		device->completed_onetime_setups = 1;

		if (rc != 0){	
			err("BP_FC_SET_HOST_DMABUF failed %d", rc);
			return rc;
		}

		rc = acq200_getAvailableChannels(device);
	}
*/
#if 0
	acq200_create_sysfs_device(&device->pci_dev->dev);

	device->class_dev = CLASS_DEVICE_CREATE(
		acq200_device_class,			/* cls */
		NULL,					/* cls_parent */
		device->idx,				/* "devt" */
		&device->pci_dev->dev,			/* device */
		"acq200.%d", device->idx);		 /* fmt, idx */

	acq200_create_sysfs_class(device->class_dev);
#endif



	result = pci_read_config_dword( device->pci_dev, PCI_BASE_ADDRESS_0, &addr );
	if( result != PCIBIOS_SUCCESSFUL )
	{
		err( "%s%d PCI Config Mem Address read error.\n",
			device->model_name, device->card_ver );
		return( result );
	}
	device->phys_addr_bar0 = addr & PCI_BASE_ADDRESS_MEM_MASK;

/*	
	info( "phys_pci_bar0:    0x%x", device->phys_pci_bar0);
	info( "phys_addr_bar0:    0x%x", device->phys_addr_bar0);
*/


	return rc;	
}



int xtu_my_probe( int maxCnt )
{
	int found = 0;           /* How many xtu devices found so far */
	struct pci_dev *dev_ptr = NULL;	   /* For current entry in list during traversal */
	CLTU_Device* device;
	int rc=0;
	
	while ( (dev_ptr = pci_get_device(CLTU_VENDOR_ID, CLTU_DEVICE_ID, dev_ptr)) )
	{
		/* Remember the pointer to the pci_dev structure*/
/*		device_list[found] = dev_ptr; */
		found++;
		if( found > maxCnt ) {
			err("xtu_my_probe() max Cnt error! %d", found);
			return( maxCnt );
		}


		device = kmalloc(sizeof(CLTU_Device), GFP_KERNEL);

		assert(device);
		memset(device, 0, sizeof(CLTU_Device));

		rc = pci_enable_device(dev_ptr);
		if (rc != 0){
			err("pci_enable_device FAILED returning %d", rc);
			return rc;
		}

//		cltu_set_dev_type(device, ent); 
		device->card_ver = VERSION_R1;
		sprintf(device->model_name, "%s", "cltu_R1");
		info("Old (first version)");

		cltu_device_create(device, dev_ptr);

		rc = cltu_device_init(device);
/*		if (rc != 0){
			err("cltu_device_init() FAILED returning %d", rc);
			return rc;
		} */
		
	}
	if( found == 0 ) {
		info("No XTU v1 device found!");
		rc = (-1);
	} else rc = 0;
	
	return (rc);

}

int xtu_my_probe_v2( int maxCnt )
{
	int found = 0;           /* How many xtu devices found so far */
	struct pci_dev *dev_ptr = NULL;	   /* For current entry in list during traversal */
	CLTU_Device* device;
	int rc=0;
	
	while ( (dev_ptr = pci_get_device(CLTU_VENDOR_ID_2, CLTU_DEVICE_ID_2, dev_ptr)) )
	{
		/* Remember the pointer to the pci_dev structure*/
/*		device_list[found] = dev_ptr; */
		found++;
		if( found > maxCnt ) {
			err("xtu_my_probe() max Cnt error! %d", found);
			return( maxCnt );
		}


		device = kmalloc(sizeof(CLTU_Device), GFP_KERNEL);

		assert(device);
		memset(device, 0, sizeof(CLTU_Device));

		rc = pci_enable_device(dev_ptr);
		if (rc != 0){
			err("pci_enable_device FAILED returning %d", rc);
			return rc;
		}

//		cltu_set_dev_type(device, ent); 
		device->card_ver = VERSION_R2;
		sprintf(device->model_name, "%s", "cltu_R2");
		info("New (second version).");

		cltu_device_create(device, dev_ptr);

		rc = cltu_device_init(device);
/*		if (rc != 0){
			err("cltu_device_init() FAILED returning %d", rc);
			return rc;
		} */
		
	}

	if( found == 0 ) {
		info("No XTU v2 device found!");
		rc = (-1);
	} else rc = 0;
	
	return (rc);

}

#if 0
static int __devinit
cltu_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	CLTU_Device* device;
	int rc;
/*	info("Found CLTU device!"); */

	device = kmalloc(sizeof(CLTU_Device), GFP_KERNEL);
	
	assert(device);
	memset(device, 0, sizeof(CLTU_Device));
	assert(idx < MAXDEV);

	rc = pci_enable_device(dev);
	if (rc != 0){
		err("pci_enable_device FAILED returning %d", rc);
		return rc;
	}

	cltu_set_dev_type(device, ent); 

	cltu_device_create(device, dev);

	return cltu_device_init(device);
}


static void cltu_remove (struct pci_dev *dev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
	CLTU_Device *device = cltu_lookupDevice(&dev->dev);
#else
	CLTU_Device *device = cltu_lookupDevice(dev);
#endif
	
	if (device != 0)
	{
/*		int used_interrupts = device->mbox_sync.using_interrupt != 0; */
		if( cltu_use_interrupts )
		{
			/* Write disable interrupt   */
			writel(0x0, device->virt_addr + 0x68);
			
			free_irq(device->pci_dev->irq, device);
/*			device->mbox_sync.using_interrupt = 0; */
		}

		unregister_chrdev(device->ldev.major, device->ldev.drv_name);

		if (device->virt_addr) 
		{
			info("virt_addr0[0]:    0x%x, addr: %p", *((volatile unsigned int*)device->virt_addr), device->virt_addr );
			info("virt_addr0[c]:    0x%x: good bye!", *((volatile unsigned int*)(device->virt_addr +0xc)) );
			iounmap (device->virt_addr);
			info ("\"%s\" iounmap ok!", device->ldev.drv_name);
			device->virt_addr = NULL;
		}

/*		dbg(1, "free_irq %s", used_interrupts? "YES": "NO"); */
/*
		acq200_remove_sysfs_class(device->class_dev);
		acq200_remove_sysfs_device(&device->pci_dev->dev);
*/

		if (device->pci_dev)
			pci_disable_device(device->pci_dev);


		kfree(device);

		info("device free OK.");
	}
	
}
#endif


/*
 *
 * { Vendor ID, Device ID, SubVendor ID, SubDevice ID,
 *   Class, Class Mask, String Index }
 */
static struct pci_device_id cltu_pci_tbl[] __devinitdata = {
	{ CLTU_VENDOR_ID, CLTU_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{ CLTU_VENDOR_ID_2, CLTU_DEVICE_ID_2, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{  }
};
MODULE_DEVICE_TABLE(pci, cltu_pci_tbl);

#if 0
static struct pci_driver cltu_pci_driver = {
	.name     = cltu_driver_name,
	.id_table = cltu_pci_tbl,
	.probe    = cltu_probe,
	.remove   = __devexit_p(cltu_remove)
/*	
#ifdef PGMCOMOUT
	// Power Managment Hooks 
#ifdef CONFIG_PM
	.suspend  = cltu_suspend,
	.resume   = cltu_resume
#endif
#endif
*/
};
#endif

/*static int __init cltu_init(void) */
static int cltu_init_module(void)
{
	int rc=0;

	printk(KI "\n");
	info("%s\n%s", 
	     cltu__driver_string, cltu__copyright);
	printk(KI "Driver built %s\n", __DATE__ );
	
/*	cltu_device_class = class_create(THIS_MODULE, "cltuR1"); */
/*	rc = pci_register_driver( &cltu_pci_driver); */

	rc = xtu_my_probe_v2(MAXDEV);
	if( rc == 0 ) {
		/* find xtu and registered */
		return rc;
	}
	rc = xtu_my_probe(MAXDEV);

	return rc;
}

void cltu_exit_module(void)
{
	int i;
	if (!idx) return;

/*	pci_unregister_driver(&cltu_pci_driver); */
/*	class_destroy(cltu_device_class); */

	for( i=0; i<idx; i++ )
	{
		CLTU_Device* device = devices[i];
		if( cltu_use_interrupts )
		{
			/* Write disable interrupt   */
			writel(0x0, device->virt_addr + 0x68);
			
			free_irq(device->pci_dev->irq, device);
/*			device->mbox_sync.using_interrupt = 0; */
		}

		unregister_chrdev(device->ldev.major, device->ldev.drv_name);

		if (device->virt_addr) 
		{
			info("virt_addr0[0]:    0x%x, addr: %p", *((volatile unsigned int*)device->virt_addr), device->virt_addr );
			info("virt_addr0[c]:    0x%x: good bye!", *((volatile unsigned int*)(device->virt_addr +0xc)) );
			iounmap (device->virt_addr);
			info ("\"%s\" iounmap ok!", device->ldev.drv_name);
			device->virt_addr = NULL;
		}

/*		dbg(1, "free_irq %s", used_interrupts? "YES": "NO"); */
/*
		acq200_remove_sysfs_class(device->class_dev);
		acq200_remove_sysfs_device(&device->pci_dev->dev);
*/

		if (device->pci_dev)
			pci_disable_device(device->pci_dev);


		kfree(device);

		info("device free OK.");
	}

	info("Driver unloaded!");
}


module_init(cltu_init_module);
module_exit(cltu_exit_module);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("WoongR.Lee, NFRI");
MODULE_DESCRIPTION("Driver for CTU and LTU");


