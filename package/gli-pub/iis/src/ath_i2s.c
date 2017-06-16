//#define here()        do { printk("%s:%d\n", __func__, __LINE__); } while(0)
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <asm/io.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/interrupt.h>

#include "934x.h"
#include "atheros.h"
#include "ath_i2s.h"

#undef AOW 
#undef USE_MEMCPY
#define MAX_I2S_WRITE_RETRIES 2

int ath_i2s_major = 253;
int ath_i2s_minor = 0;

static int i2s_start;

module_param(ath_i2s_major,     int, S_IRUGO);
module_param(ath_i2s_minor,     int, S_IRUGO);

MODULE_AUTHOR("Mughilan@Atheros");
MODULE_LICENSE("Dual BSD/GPL");

void ath_i2s_init_reg(int master);
void ath_i2s_request_dma_channel(int);
void ath_i2s_dma_desc(unsigned long, int);
void ath_i2s_dma_start(int);
void ath_i2s_dma_pause(int);
void ath_i2s_dma_resume(int);
void ath_i2s_clk(unsigned long, unsigned long);
void ath_i2s_dpll();
int  ath_ex_i2s_open(void); 
void ath_ex_i2s_close(void);
irqreturn_t ath_i2s_intr(int irq, void *dev_id, struct pt_regs *regs);

void i2s_get_stats(i2s_stats_t *p)
{
    memcpy(p, &stats, sizeof(struct i2s_stats));

}EXPORT_SYMBOL(i2s_get_stats);    

void i2s_clear_stats()
{
    stats.write_fail = 0; 
    stats.rx_underflow = 0;
}EXPORT_SYMBOL(i2s_clear_stats);


int ath_i2s_desc_busy(struct file *filp)
{
    int mode;
    int own;
    int ret = 0;
    int j = 0;

    ath_i2s_softc_t *sc = &sc_buf_var;
    ath_mbox_dma_desc *desc;
    i2s_dma_buf_t *dmabuf;

    if (!filp)
        mode = 0;
    else
        mode = filp->f_mode;

    if (mode & FMODE_READ) {
        dmabuf = &sc->sc_rbuf;
        own = sc->rpause;
    } else {
        dmabuf = &sc->sc_pbuf;
        own = sc->ppause;
    }        

    desc = dmabuf->db_desc;

    for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
        if (desc[j].OWN) {
            desc[j].OWN = 0;
        }
        ath_i2s_dma_resume(0);
    }        

    return ret;
} EXPORT_SYMBOL(ath_i2s_desc_busy);

int ath_i2s_init_desc(struct file *filp)
{
    ath_i2s_softc_t *sc = &sc_buf_var;
    i2s_dma_buf_t *dmabuf;
    i2s_buf_t *scbuf;
    uint8_t *bufp = NULL;
    int j, k, byte_cnt, tail = 0, mode = 1;
    ath_mbox_dma_desc *desc;
    unsigned long desc_p;

    if (!filp) {
        mode = FMODE_WRITE;
    } else {
        mode = filp->f_mode;
    }

    if (mode & FMODE_READ) {
#ifndef AOW
        init_waitqueue_head(&sc->wq_tx);
#endif
        dmabuf = &sc->sc_rbuf;
        sc->ropened = 1;
        sc->rpause = 0;
    } else {
#ifndef AOW
        init_waitqueue_head(&sc->wq_rx);
#endif
        dmabuf = &sc->sc_pbuf;
        sc->popened = 1;
        sc->ppause = 0;
    }

    dmabuf->db_desc = (ath_mbox_dma_desc *)
                        dma_alloc_coherent(NULL,
                        ATH_I2S_NUM_DESC *
                        sizeof(ath_mbox_dma_desc),
                        &dmabuf->db_desc_p, GFP_DMA);

    if (dmabuf->db_desc == NULL) {
        printk(KERN_CRIT "DMA desc alloc failed for %d\n", mode);
        return ENOMEM;
    }

    for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
        dmabuf->db_desc[j].OWN = 0;
#ifdef SPDIF
        for (k = 0; k < 6; k++) {
            dmabuf->db_desc[j].Va[k] = 0;
            dmabuf->db_desc[j].Ua[k] = 0;
            dmabuf->db_desc[j].Ca[k] = 0;
            dmabuf->db_desc[j].Vb[k] = 0;
            dmabuf->db_desc[j].Ub[k] = 0;
            dmabuf->db_desc[j].Cb[k] = 0;
        }
#if 0
        /* 16 Bit, 44.1 KHz */
        dmabuf->db_desc[j].Ca[0] = 0x00100000;
        dmabuf->db_desc[j].Ca[1] = 0x000000f2;
        dmabuf->db_desc[j].Cb[0] = 0x00200000;
        dmabuf->db_desc[j].Cb[1] = 0x000000f2;
        /* 16 Bit, 48 KHz */
        dmabuf->db_desc[j].Ca[0] = 0x02100000;
        dmabuf->db_desc[j].Ca[1] = 0x000000d2;
        dmabuf->db_desc[j].Cb[0] = 0x02200000;
        dmabuf->db_desc[j].Cb[1] = 0x000000d2;
#endif
        /* For Dynamic Conf */
        dmabuf->db_desc[j].Ca[0] |= SPDIF_CONFIG_CHANNEL(SPDIF_MODE_LEFT);
        dmabuf->db_desc[j].Cb[0] |= SPDIF_CONFIG_CHANNEL(SPDIF_MODE_RIGHT);
#ifdef SPDIFIOCTL
        dmabuf->db_desc[j].Ca[0] = 0x00100000;
        dmabuf->db_desc[j].Ca[1] = 0x02100000;
        dmabuf->db_desc[j].Cb[0] = 0x00200000;
        dmabuf->db_desc[j].Cb[1] = 0x02100000;
#endif
#endif
    }

    /* Allocate data buffers */
    scbuf = dmabuf->db_buf;
    
    if (!(bufp = kmalloc(ATH_I2S_NUM_DESC * ATH_I2S_BUFF_SIZE, GFP_KERNEL))) {
        printk(KERN_CRIT "Buffer allocation failed for \n");
        goto fail3;
    }
    
    if (mode & FMODE_READ) {
        sc->sc_rmall_buf = bufp;
    } else {
        sc->sc_pmall_buf = bufp;
    }
    
    for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
        scbuf[j].bf_vaddr = &bufp[j * ATH_I2S_BUFF_SIZE];
        scbuf[j].bf_paddr = dma_map_single(NULL, scbuf[j].bf_vaddr,
                                            ATH_I2S_BUFF_SIZE,
                                            DMA_BIDIRECTIONAL);
    }
    dmabuf->tail = 0;
    
    // Initialize desc
    desc = dmabuf->db_desc;
    desc_p = (unsigned long) dmabuf->db_desc_p;
    byte_cnt = ATH_I2S_NUM_DESC * ATH_I2S_BUFF_SIZE;
    tail = dmabuf->tail;
    
    while (byte_cnt && (tail < ATH_I2S_NUM_DESC)) {
        desc[tail].rsvd1 = 0;
        desc[tail].size = ATH_I2S_BUFF_SIZE;
        if (byte_cnt > ATH_I2S_BUFF_SIZE) {
            desc[tail].length = ATH_I2S_BUFF_SIZE;
            byte_cnt -= ATH_I2S_BUFF_SIZE;
            desc[tail].EOM = 0;
        } else {
            desc[tail].length = byte_cnt;
            byte_cnt = 0;
            desc[tail].EOM = 0;
        }
        desc[tail].rsvd2 = 0;
        desc[tail].rsvd3 = 0;
        desc[tail].BufPtr = (unsigned int) scbuf[tail].bf_paddr;
        desc[tail].NextPtr = (desc_p +
                              ((tail +
                              1) *
                              (sizeof(ath_mbox_dma_desc))));
        if (mode & FMODE_READ) {
            desc[tail].OWN = 1;
        } else {
            desc[tail].OWN = 0;
        }
        tail++;
    }
    tail--;
    desc[tail].NextPtr = desc_p;
    
    dmabuf->tail = 0;
    
    return 0;

fail3:
    if (mode & FMODE_READ) {
        dmabuf = &sc->sc_rbuf;
    } else {
        dmabuf = &sc->sc_pbuf;
    }
    dma_free_coherent(NULL,
                      ATH_I2S_NUM_DESC * sizeof(ath_mbox_dma_desc),
                      dmabuf->db_desc, dmabuf->db_desc_p);
    if (mode & FMODE_READ) {
        if (sc->sc_rmall_buf) {
            kfree(sc->sc_rmall_buf);
        }
    } else {
        if (sc->sc_pmall_buf) {
            kfree(sc->sc_pmall_buf);
        }
    }
    
    return -ENOMEM;
}

int ath_ex_i2s_open()
{
    ath_i2s_softc_t *sc = &sc_buf_var;
    int opened = 0, mode = MASTER;

    i2s_start = 1;

    ath_i2s_init_reg(1);

    if (sc->popened) {
        printk("%s, %d I2S speaker busy\n", __func__, __LINE__);
        return -EBUSY;
    }

    opened = (sc->ropened | sc->popened);

    /* Reset MBOX FIFO's */
    if (!opened) {
        ath_reg_wr(ATH_MBOX_FIFO_RESET, 0xff); // virian
        udelay(500);
    }

    /* Allocate and initialize descriptors */
    if (ath_i2s_init_desc(NULL) == ENOMEM) {
        return -ENOMEM;
    }

    if (!opened) {
        ath_i2s_request_dma_channel(mode);
    }

    return (0);
}
EXPORT_SYMBOL(ath_ex_i2s_open);

int ath_i2s_open(struct inode *inode, struct file *filp)
{
    ath_i2s_softc_t *sc = &sc_buf_var;
    int opened = 0, mode = MASTER;

    if ((filp->f_mode & FMODE_READ) && (sc->ropened)) {
        printk("%s, %d I2S mic busy\n", __func__, __LINE__);
        return -EBUSY;
    }
    if ((filp->f_mode & FMODE_WRITE) && (sc->popened)) {
        printk("%s, %d I2S speaker busy\n", __func__, __LINE__);
        return -EBUSY;
    }

    opened = (sc->ropened | sc->popened);

    /* Reset MBOX FIFO's */
    if (!opened) {
        ath_reg_wr(ATH_MBOX_FIFO_RESET, 0xff); // virian
        udelay(500);
    }

    /* Allocate and initialize descriptors */
    if (ath_i2s_init_desc(filp) == ENOMEM) {
        return -ENOMEM;
    }

    if (!opened) {
        ath_i2s_request_dma_channel(mode);
    }

    return (0);
}

ssize_t ath_i2s_read(struct file * filp, char __user * buf,
                     size_t count, loff_t * f_pos)
{
#define prev_tail(t) ({ (t == 0) ? (ATH_I2S_NUM_DESC - 1) : (t - 1); })
#define next_tail(t) ({ (t == (ATH_I2S_NUM_DESC - 1)) ? 0 : (t + 1); })

    uint8_t *data;
    ssize_t retval;
    struct ath_i2s_softc *sc = &sc_buf_var;
    i2s_dma_buf_t *dmabuf = &sc->sc_rbuf;
    i2s_buf_t *scbuf;
    ath_mbox_dma_desc *desc;
    unsigned int byte_cnt, mode = 1, offset = 0, tail = dmabuf->tail;
    unsigned long desc_p;
    int need_start = 0;
    
    byte_cnt = count;
    
    if (sc->ropened < 2) {
        ath_reg_rmw_set(ATH_MBOX_INT_ENABLE, ATH_MBOX_TX_DMA_COMPLETE);
        need_start = 1;
    }
    
    sc->ropened = 2;
    
    scbuf = dmabuf->db_buf;
    desc = dmabuf->db_desc;
    desc_p = (unsigned long) dmabuf->db_desc_p;
    data = scbuf[0].bf_vaddr;
    
    desc_p += tail * sizeof(ath_mbox_dma_desc);
    
#ifndef AOW
    if (!need_start) {
        wait_event_interruptible(sc->wq_tx, desc[tail].OWN != 1);
    }
#endif

    while (byte_cnt && !desc[tail].OWN) {
        if (byte_cnt >= ATH_I2S_BUFF_SIZE) {
            desc[tail].length = ATH_I2S_BUFF_SIZE;
            byte_cnt -= ATH_I2S_BUFF_SIZE;
        } else {
            desc[tail].length = byte_cnt;
            byte_cnt = 0;
        }
        dma_cache_sync(NULL, scbuf[tail].bf_vaddr, desc[tail].length, DMA_FROM_DEVICE);
        retval = copy_to_user(buf + offset, scbuf[tail].bf_vaddr, 
                              ATH_I2S_BUFF_SIZE);
        if (retval)
            return retval;
        desc[tail].BufPtr = (unsigned int) scbuf[tail].bf_paddr;
        desc[tail].OWN = 1;
        
        tail = next_tail(tail);
        offset += ATH_I2S_BUFF_SIZE;
    }
    
    dmabuf->tail = tail;
    
    if (need_start) {
        ath_i2s_dma_desc((unsigned long) desc_p, mode);
        if (filp) {
            ath_i2s_dma_start(mode);
        }
    } else if (!sc->rpause) {
        ath_i2s_dma_resume(mode);
    }
    
    return offset;
}



ssize_t ath_i2s_wr(struct file * filp, const char __user * buf,
			 size_t count, loff_t * f_pos, int resume)
{
#define prev_tail(t) ({ (t == 0) ? (ATH_I2S_NUM_DESC - 1) : (t - 1); })
#define next_tail(t) ({ (t == (ATH_I2S_NUM_DESC - 1)) ? 0 : (t + 1); })

    uint8_t *data;
    ssize_t retval;
    int byte_cnt, offset, need_start = 0;
    int mode = 0;
    struct ath_i2s_softc *sc = &sc_buf_var;
    i2s_dma_buf_t *dmabuf = &sc->sc_pbuf;
    i2s_buf_t *scbuf;
    ath_mbox_dma_desc *desc;
    int tail = dmabuf->tail;
    unsigned long desc_p;
    
    byte_cnt = count;
    
    if (sc->popened < 2) {
        ath_reg_rmw_set(ATH_MBOX_INT_ENABLE, ATH_MBOX_RX_DMA_COMPLETE);
        need_start = 1;
    }
    
    sc->popened = 2;
    
    scbuf = dmabuf->db_buf;
    desc = dmabuf->db_desc;
    desc_p = (unsigned long) dmabuf->db_desc_p;
    offset = 0;
    data = scbuf[0].bf_vaddr;
    
    desc_p += tail * sizeof(ath_mbox_dma_desc);

#ifndef AOW
    if (!need_start) {
        retval = wait_event_interruptible(sc->wq_rx, desc[tail].OWN != 1);
        if (retval == -ERESTARTSYS) {
            return -ERESTART;
        }
    }
#endif

    while (byte_cnt && !desc[tail].OWN) {
        if (byte_cnt >= ATH_I2S_BUFF_SIZE) {
            desc[tail].length = ATH_I2S_BUFF_SIZE;
            byte_cnt -= ATH_I2S_BUFF_SIZE;
        } else {
            desc[tail].length = byte_cnt;
            byte_cnt = 0;
        }
#ifdef USE_MEMCPY        
        memcpy(scbuf[tail].bf_vaddr, buf + offset, ATH_I2S_BUFF_SIZE);
#else        
        retval = copy_from_user(scbuf[tail].bf_vaddr, buf + offset,
                                ATH_I2S_BUFF_SIZE);
        if (retval)
            return retval;
#endif            
        dma_cache_sync(NULL, scbuf[tail].bf_vaddr, desc[tail].length, DMA_TO_DEVICE);
        desc[tail].BufPtr = (unsigned int) scbuf[tail].bf_paddr;
        desc[tail].OWN = 1;
        
        tail = next_tail(tail);
        offset += ATH_I2S_BUFF_SIZE;
    }
    
    dmabuf->tail = tail;
    
    if (need_start) {
        ath_i2s_dma_desc((unsigned long) desc_p, mode);
        ath_i2s_dma_start(mode);
    } else if (!sc->ppause) {
        ath_i2s_dma_resume(mode);
    }
    
    return count - byte_cnt;
}


void ath_ex_i2s_write(size_t count, const char * buf, int resume)
{
    int tmpcount, ret = 0;
    int cnt = 0;
    char *data;
#ifdef I2S_DEBUG
    static int myVectCnt = 0; 
    u_int16_t *locBuf = (u_int16_t *)buf;
    for( tmpcount = 0; tmpcount < (count>>1); tmpcount++) {
        *locBuf++ = myTestVect[myVectCnt++];        
        if( myVectCnt >= (myTestVectSz *2) ) myVectCnt = 0;
    }
#endif
eagain:
    tmpcount = count;
    data = (char *) buf;
    ret = 0;

    do {
        ret = ath_i2s_wr(NULL, data, tmpcount, NULL, 1);
        cnt++;
        if (ret == -ERESTART) {
#ifdef I2S_DEBUG
            printk("%s:%d %d\n", __func__, __LINE__, ret);
#endif
            return;
        }
        if (ret == EAGAIN) {
#ifdef I2S_DEBUG
            printk("%s:%d %d\n", __func__, __LINE__, ret);
#endif
            goto eagain;
        }

        tmpcount = tmpcount - ret;
        data += ret;
#ifdef AOW
        if (cnt > MAX_I2S_WRITE_RETRIES) {
            if (i2s_start) {
                //ath_i2s_dma_start(0);
                i2s_start = 0;
            }
            stats.write_fail++;
            printk("... %s, %d: stats %d....\n", __func__, __LINE__, stats.write_fail);
            return;
        }            
#endif
    } while(tmpcount);
}
EXPORT_SYMBOL(ath_ex_i2s_write);


ssize_t ath_i2s_write(struct file * filp, const char __user * buf,
			 size_t count, loff_t * f_pos)
{
    int tmpcount, ret = 0;
    int cnt = 0;
    char *data;
#ifdef I2S_DEBUG
    static int myVectCnt = 0;
    u_int16_t *locBuf = (u_int16_t *)buf;
    for( tmpcount = 0; tmpcount < (count>>1); tmpcount++) {
        *locBuf++ = myTestVect[myVectCnt++];
        if( myVectCnt >= (myTestVectSz *2) ) myVectCnt = 0;
    }
#endif
eagain:
    tmpcount = count;
    data = (char *) buf;
    ret = 0;

    do {
        ret = ath_i2s_wr(NULL, data, tmpcount, NULL, 1);
        cnt++;
        if (ret == -ERESTART) {
            return ret;
        }
        if (ret == EAGAIN) {
            printk("%s:%d %d\n", __func__, __LINE__, ret);
            goto eagain;
        }


        tmpcount = tmpcount - ret;
        data += ret;
#ifdef I2S_DEBUG
        if (cnt > MAX_I2S_WRITE_RETRIES) {
            if (i2s_start) {
                //ath_i2s_dma_start(0);
                i2s_start = 0;
            }
            stats.write_fail++;
            printk("... %s, %d: stats %d....\n", __func__, __LINE__, stats.write_fail);
            return 0;
        }
#endif
    } while(tmpcount);

    return 0;
}


int ath_i2s_close(struct inode *inode, struct file *filp)
{
    int j, own, mode;
    ath_i2s_softc_t *sc = &sc_buf_var;
    i2s_dma_buf_t *dmabuf;
    ath_mbox_dma_desc *desc;
    
    if (!filp) {
        mode  = 0;
    } else {
        mode = filp->f_mode;
    }
    
    if (mode & FMODE_READ) {
        dmabuf = &sc->sc_rbuf;
        own = sc->rpause;
    } else {
        dmabuf = &sc->sc_pbuf;
        own = sc->ppause;
    }
    
    desc = dmabuf->db_desc;
    if (own) {
        for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
            desc[j].OWN = 0;
        }
        ath_i2s_dma_resume(mode);
    } else { 
        for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
            while (desc[j].OWN) {
                schedule_timeout_interruptible(HZ);
            }
        }
    }
    
    for (j = 0; j < ATH_I2S_NUM_DESC; j++) {
        dma_unmap_single(NULL, dmabuf->db_buf[j].bf_paddr,
                         ATH_I2S_BUFF_SIZE, DMA_BIDIRECTIONAL);
    }
    
    if (mode & FMODE_READ) {
        kfree(sc->sc_rmall_buf);
    } else {
        kfree(sc->sc_pmall_buf);
    }
    dma_free_coherent(NULL,
                      ATH_I2S_NUM_DESC * sizeof(ath_mbox_dma_desc),
                      dmabuf->db_desc, dmabuf->db_desc_p);
    
    if (mode & FMODE_READ) {
        sc->ropened = 0;
        sc->rpause = 0;
    } else {
        sc->popened = 0;
        sc->ppause = 0;
    }
    
    return (0);
}

void ath_ex_i2s_close()          
{                                
    ath_i2s_close(NULL, NULL);
}
EXPORT_SYMBOL(ath_ex_i2s_close); 

int ath_i2s_release(struct inode *inode, struct file *filp)
{
    printk(KERN_CRIT "release\n");
    return 0;
}

int ath_i2s_ioctl(struct inode *inode, struct file *filp,
		     unsigned int cmd, unsigned long arg)
{
    #define ATH_STEREO_CONFIG_DEFAULT ( \
                ATH_STEREO_CONFIG_SPDIF_ENABLE | \
                ATH_STEREO_CONFIG_ENABLE | \
                ATH_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE | \
                ATH_STEREO_CONFIG_MASTER | \
                ATH_STEREO_CONFIG_PSEDGE(2))

	int data, mask = 0, st_cfg = 0;
	struct ath_i2s_softc *sc = &sc_buf_var;

	switch (cmd) {
    case I2S_PAUSE:
        data = arg;
	ath_i2s_dma_pause(data);
	if (data) {
		sc->rpause = 1;
	} else {
		sc->ppause = 1;
	}
        return 0;
    case I2S_RESUME:
        data = arg;
	ath_i2s_dma_resume(data);
	if (data) {
		sc->rpause = 0;
	} else {
		sc->ppause = 0;
	}
        return 0;
	case I2S_VOLUME:
		data = arg;
		if (data < 15) {
			if (data < 0) {
				mask = 0xf;
			} else {
				mask = (~data) & 0xf;
				mask = mask | 0x10;
			}
		} else {
			if (data <= 22) {
				if (data == 15) {
					data = 0;
				} else {
					mask = data - 15;
				}
			} else {
				mask = 7;
			}
		}
		data = mask | (mask << 8);
		ath_reg_wr(ATH_STEREO_VOLUME, data);
		return 0;

	case I2S_FREQ:		/* Frequency settings */
		data = arg;
        switch (data) {
            case 44100:
                if (ath_ref_freq == (25 * 1000000)) {
                    ath_i2s_clk(0x15a2102a, 0x61a1);
                } else {
                    ath_i2s_dpll();
                    ath_i2s_clk(0x30a9036, 0x61a2);
                }
                break;
            case 48000:
                if (ath_ref_freq == (25 * 1000000)) {
                    ath_i2s_clk(0x127bb02e, 0x61a1);
                } else {
                    ath_i2s_dpll();
                    ath_i2s_clk(0xfb7e83a, 0x61a2);
                }
                break;
            default:
                printk(KERN_CRIT "Freq %d not supported \n",
                   data);
                return -ENOTSUPP;
        }
		return 0;

	case I2S_FINE:
		data = arg;
		return 0;

	case I2S_DSIZE:
		data = arg;
		switch (data) {
            case 8:
                //ath_reg_wr(ATH_STEREO_CONFIG, 0xa00302);
                st_cfg = (ATH_STEREO_CONFIG_DEFAULT |
                            ATH_STEREO_CONFIG_DATA_WORD_SIZE(ATH_STEREO_WS_8B));
                break;
            case 16:
                //ath_reg_wr(ATH_STEREO_CONFIG, 0xa21302);
                st_cfg = (ATH_STEREO_CONFIG_DEFAULT |
                            ATH_STEREO_CONFIG_PCM_SWAP |
                            ATH_STEREO_CONFIG_DATA_WORD_SIZE(ATH_STEREO_WS_16B));
                break;
            case 24:
                //ath_reg_wr(ATH_STEREO_CONFIG, 0xa22b02);
                st_cfg = (ATH_STEREO_CONFIG_DEFAULT |
                            ATH_STEREO_CONFIG_PCM_SWAP |
                            ATH_STEREO_CONFIG_DATA_WORD_SIZE(ATH_STEREO_WS_24B) |
                            ATH_STEREO_CONFIG_I2S_32B_WORD);
                break;
            case 32:
                //ath_reg_wr(ATH_STEREO_CONFIG, 0xa23b02);
                st_cfg = (ATH_STEREO_CONFIG_DEFAULT |
                            ATH_STEREO_CONFIG_PCM_SWAP |
                            ATH_STEREO_CONFIG_DATA_WORD_SIZE(ATH_STEREO_WS_32B) |
                            ATH_STEREO_CONFIG_I2S_32B_WORD);
                break;
		    default:
			    printk(KERN_CRIT "Data size %d not supported \n", data);
			return -ENOTSUPP;
		}
        ath_reg_wr(ATH_STEREO_CONFIG, (st_cfg | ATH_STEREO_CONFIG_RESET));
        udelay(100);
        ath_reg_rmw_clear(ATH_STEREO_CONFIG, ATH_STEREO_CONFIG_RESET);
	return 0;

	case I2S_MODE:		/* mono or stereo */
		data = arg;
	    /* For MONO */
		if (data != 2) {
	        ath_reg_rmw_set(ATH_STEREO_CONFIG, MONO);      
        } else {
	        ath_reg_rmw_clear(ATH_STEREO_CONFIG, MONO);      
        }
		/*Otherwise default settings. Nothing to do */
		return 0;
	case I2S_COUNT:
		data = arg;
		return 0;

	default:
		return -ENOTSUPP;
	}


}

irqreturn_t ath_i2s_intr(int irq, void *dev_id, struct pt_regs *regs)
{
    uint32_t r;
    ath_i2s_softc_t *sc = &sc_buf_var;

    r = ath_reg_rd(ATH_MBOX_INT_STATUS); 

#ifndef AOW
    if (r & ATH_MBOX_RX_DMA_COMPLETE) {
        wake_up_interruptible(&sc->wq_rx);
    }
    if (r & ATH_MBOX_TX_DMA_COMPLETE) {
        wake_up_interruptible(&sc->wq_tx);
    }
#endif

    /* Ack the interrupts */
    ath_reg_wr(ATH_MBOX_INT_STATUS, r);

    return IRQ_HANDLED;
}

/* The I2S initialization fn carries hard-coded values.
 * Register descriptions are to be defined and updated.
 * TODO list
 */
void ath_i2s_init_reg(int master)
{
    #define ATH_STEREO_CONFIG_DEFAULT ( \
                ATH_STEREO_CONFIG_SPDIF_ENABLE | \
                ATH_STEREO_CONFIG_ENABLE | \
                ATH_STEREO_CONFIG_PCM_SWAP | \
                ATH_STEREO_CONFIG_DATA_WORD_SIZE(ATH_STEREO_WS_16B) | \
                ATH_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE | \
                ATH_STEREO_CONFIG_MASTER | \
                ATH_STEREO_CONFIG_PSEDGE(2))

    unsigned int rddata;

    /* Reset I2S Controller */
    ath_reset(ATH_RESET_I2S);
    udelay(1000);

    rddata = ath_reg_rd(ATH_GPIO_IN_ENABLE1);
    rddata = rddata | 0x0e00;
    ath_reg_wr(ATH_GPIO_IN_ENABLE1, rddata);
    
    rddata = ath_reg_rd(ATH_GPIO_OUT_FUNCTION2);
    rddata = rddata & 0x00ffffff;
    rddata = rddata | ATH_GPIO_OUT_FUNCTION2_ENABLE_GPIO_11(0x0e);
    ath_reg_wr(ATH_GPIO_OUT_FUNCTION2, rddata);
    
    rddata = ath_reg_rd(ATH_GPIO_OUT_FUNCTION3);
    rddata = rddata | (ATH_GPIO_OUT_FUNCTION3_ENABLE_GPIO_12(0x0d) |
                       ATH_GPIO_OUT_FUNCTION3_ENABLE_GPIO_13(0x0c) |
                       ATH_GPIO_OUT_FUNCTION3_ENABLE_GPIO_15(0x19));
    ath_reg_wr(ATH_GPIO_OUT_FUNCTION3, rddata);
    
    rddata = ath_reg_rd(ATH_GPIO_OUT_FUNCTION1);
    rddata = rddata & 0xffffff00;
    rddata = rddata | ATH_GPIO_OUT_FUNCTION1_ENABLE_GPIO_4(0x0f);
    ath_reg_wr(ATH_GPIO_OUT_FUNCTION1, rddata);
    
    rddata = rddata & 0xffffff00;
    ath_reg_wr(ATH_GPIO_OUT_FUNCTION4, rddata);
    
    rddata = ath_reg_rd(ATH_GPIO_OE);
    rddata = rddata | ATH_GPIO_OE_EN(0x4000);
    rddata = rddata & 0xffff47ef;
    ath_reg_wr(ATH_GPIO_OE, rddata);

    // Program Stereo Config Register.
    ath_reg_wr(ATH_STEREO_CONFIG,
                    (ATH_STEREO_CONFIG_DEFAULT |
                     ATH_STEREO_CONFIG_RESET));
    udelay(100);
    ath_reg_rmw_clear(ATH_STEREO_CONFIG,
                            ATH_STEREO_CONFIG_RESET);

}

void ath_i2s_request_dma_channel(int mode)
{
    ath_reg_wr(ATH_MBOX_DMA_POLICY, 
                    (ATH_MBOX_DMA_POLICY_RX_QUANTUM |
                     ATH_MBOX_DMA_POLICY_RX_QUANTUM |
                     ATH_MBOX_DMA_POLICY_TX_FIFO_THRESH(6)));
}

void ath_i2s_dma_desc(unsigned long desc_buf_p, int mode)
{
    /*
     * Program the device to generate interrupts
     * RX_DMA_COMPLETE for mbox 0
     */
    if (mode) {
        ath_reg_wr(ATH_MBOX_DMA_TX_DESCRIPTOR_BASE0, desc_buf_p);
    } else {
        ath_reg_wr(ATH_MBOX_DMA_RX_DESCRIPTOR_BASE0, desc_buf_p);
    }
}

void ath_i2s_dma_start(int mode)
{
    /*
     * Start
     */
    if (mode) {
        ath_reg_wr(ATH_MBOX_DMA_TX_CONTROL0, ATH_MBOX_DMA_START);
    } else {
        ath_reg_wr(ATH_MBOX_DMA_RX_CONTROL0, ATH_MBOX_DMA_START);
    }
}
EXPORT_SYMBOL(ath_i2s_dma_start);

void ath_i2s_dma_pause(int mode)
{
    /*
     * Pause
     */
    if (mode) {
        ath_reg_wr(ATH_MBOX_DMA_TX_CONTROL0, ATH_MBOX_DMA_PAUSE);
    } else {
        ath_reg_wr(ATH_MBOX_DMA_RX_CONTROL0, ATH_MBOX_DMA_PAUSE);
    }
}
EXPORT_SYMBOL(ath_i2s_dma_pause);

void ath_i2s_dma_resume(int mode)
{
    /*
     * Resume
     */
    if (mode) {
        ath_reg_wr(ATH_MBOX_DMA_TX_CONTROL0, ATH_MBOX_DMA_RESUME);
    } else {
        ath_reg_wr(ATH_MBOX_DMA_RX_CONTROL0, ATH_MBOX_DMA_RESUME);
    }
}
EXPORT_SYMBOL(ath_i2s_dma_resume);

void ath_i2s_dpll()
{
    /*
     * Configure AUDIO DPLL
     */
    ath_reg_rmw_clear(ATH_AUDIO_DPLL2, (ATH_AUDIO_DPLL2_PATH_GAIN(0xf) | 
                                  ATH_AUDIO_DPLL2_PROP_GAIN(0x7f)));
    ath_reg_rmw_set(ATH_AUDIO_DPLL2, (ATH_AUDIO_DPLL2_PATH_GAIN(0x4) | 
                                  ATH_AUDIO_DPLL2_PROP_GAIN(0x32)));
    ath_reg_rmw_clear(ATH_AUDIO_DPLL3, ATH_AUDIO_DPLL3_PHASE_SHIFT(0x7f));
    ath_reg_rmw_set(ATH_AUDIO_DPLL3, ATH_AUDIO_DPLL3_PHASE_SHIFT(0x6));
    if (!is_ar934x_10()) {
        ath_reg_rmw_clear(ATH_AUDIO_DPLL2, ATH_AUDIO_DPLL2_RANGE);
        ath_reg_rmw_set(ATH_AUDIO_DPLL2, ATH_AUDIO_DPLL2_RANGE);
    }
}
EXPORT_SYMBOL(ath_i2s_dpll);

void ath_i2s_clk(unsigned long frac, unsigned long pll)
{
    /*
     * Tick...Tick...Tick
     */
    ath_reg_wr(ATH_FRAC_FREQ_CONFIG, frac);
    ath_reg_wr(ATH_AUDIO_PLL_CONFIG, pll);
    ath_reg_wr(ATH_AUDIO_PLL_CONFIG, (pll & ~ATH_AUDIO_PLL_CFG_PWR_DWN));
}
EXPORT_SYMBOL(ath_i2s_clk);

loff_t ath_i2s_llseek(struct file *filp, loff_t off, int whence)
{
    printk(KERN_CRIT "llseek\n");
    return off;
}


struct file_operations ath_i2s_fops = {
    .owner   = THIS_MODULE,
    .llseek  = ath_i2s_llseek,
    .read    = ath_i2s_read,
    .write   = ath_i2s_write,
    .ioctl   = ath_i2s_ioctl,
    .open    = ath_i2s_open,
    .release = ath_i2s_close,
};

void ath_i2s_cleanup_module(void)
{
    ath_i2s_softc_t *sc = &sc_buf_var;
    
    printk(KERN_CRIT "unregister\n");
    
    free_irq(sc->sc_irq, NULL);
    unregister_chrdev(ath_i2s_major, "ath_i2s");
}

int ath_i2s_init_module(void)
{
    ath_i2s_softc_t *sc = &sc_buf_var;
    int result = -1;
    int master = 1;
    
    /*
     * Get a range of minor numbers to work with, asking for a dynamic
     * major unless directed otherwise at load time.
     */
    if (ath_i2s_major) {
        result = register_chrdev(ath_i2s_major, "ath_i2s",
                                 &ath_i2s_fops);
    }
    if (result < 0) {
        printk(KERN_WARNING "ath_i2s: can't get major %d\n",
               ath_i2s_major);
        return result;
    }
    
    sc->sc_irq = ATH_MISC_IRQ_DMA;
    
    /* Establish ISR would take care of enabling the interrupt */
    result = request_irq(sc->sc_irq, ath_i2s_intr, IRQF_DISABLED,
                         "ath_i2s", NULL);
    if (result) {
        printk(KERN_INFO
               "i2s: can't get assigned irq %d returns %d\n",
               sc->sc_irq, result);
    }
    
    ath_i2s_init_reg(master);
    
    I2S_LOCK_INIT(&sc_buf_var);
    
    return 0;		/* succeed */
}

module_init(ath_i2s_init_module);
module_exit(ath_i2s_cleanup_module);
