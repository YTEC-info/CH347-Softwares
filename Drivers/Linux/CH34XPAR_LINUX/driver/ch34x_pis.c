/*
 * CH341/CH347 USB to multiple interfaces driver
 *
 * Copyright (C) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Web:      http://wch.cn
 * Author:   WCH <tech@wch.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Update Log:
 * V1.0 - initial version
 * V1.1 - fix ioctl bugs when copy data from user space
 * V1.2 - fix write & read & ioctl bugs
 * V1.3 - add support of ch347 chip
 */

#define DEBUG
#define VERBOSE_DEBUG

#undef DEBUG
#undef VERBOSE_DEBUG

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/version.h>

#define DRIVER_AUTHOR "WCH"
#define DRIVER_DESC   "USB to multiple interface driver for ch341/ch347, etc."
#define VERSION_DESC  "V1.3 On 2023.02"

#define CH34x_MINOR_BASE    200
#define CH341_PACKET_LENGTH 32
#define CH347_PACKET_LENGTH 512
#define MAX_BUFFER_LENGTH   0x1000

#define VENDOR_WRITE_TYPE 0x40
#define VENDOR_READ_TYPE  0XC0

#define CH34x_PARA_INIT 0xB1
#define VENDOR_VERSION	0x5F

#define CH34x_PARA_CMD_R0 0xAC /* read data0 from parport */
#define CH34x_PARA_CMD_R1 0xAD /* read data1 from parport */
#define CH34x_PARA_CMD_W0 0xA6 /* write data0 to parport */
#define CH34x_PARA_CMD_W1 0xA7 /* write data1 to parport */

#define CH34x_EPP_IO_MAX (CH341_PACKET_LENGTH - 1)
#define CH341_EPP_IO_MAX 0xFF

#define CH34x_DEBUG_READ  0x95
#define CH34x_DEBUG_WRITE 0x9A

/* ioctl commands for interaction between driver and application */
#define IOCTL_MAGIC 'W'

#define CH34x_GET_DRV_VERSION	    _IOR(IOCTL_MAGIC, 0x80, u16)
#define CH34x_CHIP_VERSION	    _IOR(IOCTL_MAGIC, 0x81, u16)
#define CH34x_CHIP_TYPE		    _IOR(IOCTL_MAGIC, 0x82, u16)
#define CH34x_CHIP_ID		    _IOR(IOCTL_MAGIC, 0x83, u16)
#define CH34x_FUNCTION_SETPARA_MODE _IOW(IOCTL_MAGIC, 0x90, u16)
#define CH34x_FUNCTION_READ_MODE    _IOW(IOCTL_MAGIC, 0x91, u16)
#define CH34x_FUNCTION_WRITE_MODE   _IOW(IOCTL_MAGIC, 0x92, u16)
#define CH34x_SET_TIMEOUT	    _IOW(IOCTL_MAGIC, 0x93, u16)
#define CH34x_PIPE_DATA_READ	    _IOWR(IOCTL_MAGIC, 0xb0, u16)
#define CH34x_PIPE_DATA_WRITE	    _IOWR(IOCTL_MAGIC, 0xb1, u16)
#define CH34x_PIPE_WRITE_READ	    _IOWR(IOCTL_MAGIC, 0xb2, u16)
#define CH34x_PIPE_DEVICE_CTRL	    _IOW(IOCTL_MAGIC, 0xb3, u16)

#define DEFAULT_TIMEOUT 1000

/* Define these values to match your devices */

/* table of devices that work with this driver */
static const struct usb_device_id ch34x_usb_ids[] = {
	{ USB_DEVICE(0x1a86, 0x5512) },
	{ USB_DEVICE_INTERFACE_NUMBER(0x1a86, 0x55db, 0x02) }, /* CH347 Mode1 SPI+IIC+UART */
	{ USB_DEVICE_INTERFACE_NUMBER(0x1a86, 0x55dd, 0x02) }, /* CH347 Mode3 JTAG+UART */
	{}						       /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, ch34x_usb_ids);

typedef enum _CHIP_TYPE {
	CHIP_CH341 = 0,
	CHIP_CH347
} CHIP_TYPE;

#define WRITES_IN_FLIGHT 8

struct ch34x_pis {
	struct usb_device *udev;	 /*the usb device for this device*/
	struct usb_interface *interface; /*the interface for this device*/
	struct usb_endpoint_descriptor *interrupt_in_endpoint;
	u16 ch34x_id[2]; /* device vid and pid */

	struct semaphore limit_sem;  /* limiting the number of writes in progress */
	struct usb_anchor submitted; /* in case we need to retract our submissions */

	size_t interrupt_in_size;	    /*the size of rec data (interrupt)*/
	unsigned char *interrupt_in_buffer; /*the buffer of rec data (interface)*/
	struct urb *interrupt_in_urb;

	size_t bulk_in_size;	       /*the size of rec data (bulk)*/
	unsigned char *bulk_in_buffer; /*the buffer of rec data (bulk)*/
	struct urb *read_urb;	       /*the urb of bulk_in*/
	u8 bulk_in_endpointAddr;       /*bulk input endpoint*/
	u8 bulk_out_endpointAddr;      /*bulk output endpoint*/
	unsigned char *bulk_out_buffer;

	u8 para_rmode;
	u8 para_wmode;
	int readtimeout;
	int writetimeout;

	struct mutex io_mutex; /* synchronize I/O with disconnect */
	CHIP_TYPE chiptype;
	u8 chipver;
	int errors;
	spinlock_t err_lock;
	struct kref kref;
};

static struct usb_driver ch34x_pis_driver;
static void ch34x_delete(struct kref *kref);
static void ch34x_draw_down(struct ch34x_pis *ch34x_dev);

static int ch34x_fops_release(struct inode *inode, struct file *file)
{
	struct ch34x_pis *ch34x_dev;

	ch34x_dev = (struct ch34x_pis *)file->private_data;
	if (ch34x_dev == NULL)
		return -ENODEV;

	mutex_lock(&ch34x_dev->io_mutex);

	if (ch34x_dev->interface)
		usb_autopm_put_interface(ch34x_dev->interface);
	mutex_unlock(&ch34x_dev->io_mutex);

	/* decrement the count on our device */
	kref_put(&ch34x_dev->kref, ch34x_delete);
	return 0;
}

/* USB control transfer in */
static int ch34x_control_transfer_in(u8 request, u16 value, u16 index, struct ch34x_pis *ch34x_dev, unsigned char *buf,
				     u16 len)
{
	int retval;

	retval = usb_control_msg(ch34x_dev->udev, usb_rcvctrlpipe(ch34x_dev->udev, 0), request, VENDOR_READ_TYPE, value,
				 index, buf, len, ch34x_dev->readtimeout);

	return retval;
}

/* USB control transfer out */
static int ch34x_control_transfer_out(u8 request, u16 value, u16 index, struct ch34x_pis *ch34x_dev, unsigned char *buf,
				      u16 len)
{
	int retval;

	retval = usb_control_msg(ch34x_dev->udev, usb_sndctrlpipe(ch34x_dev->udev, 0), request, VENDOR_WRITE_TYPE,
				 value, index, buf, len, ch34x_dev->writetimeout);

	return retval;
}

/*
 * Initial operation for parallel port working mode.
 * @mode: 0x00/0x01->EPP mode, 0x02->MEM mode
 */
static int ch34x_parallel_init(unsigned char mode, struct ch34x_pis *ch34x_dev)
{
	int retval;
	u8 requesttype = VENDOR_WRITE_TYPE;
	u8 request = CH34x_PARA_INIT;
	u16 value = (mode << 8) | (mode < 0x00000100 ? 0x02 : 0x00);
	u16 index = 0;
	u16 len = 0;
	retval = usb_control_msg(ch34x_dev->udev, usb_sndctrlpipe(ch34x_dev->udev, 0), request, requesttype, value,
				 index, NULL, len, ch34x_dev->writetimeout);

	return retval;
}

/*
 * Read operation for parallel port read in EPP/MEM mode.
 */
ssize_t ch34x_fops_read(struct file *file, char __user *to_user, size_t count, loff_t *file_pos)
{
	struct ch34x_pis *ch34x_dev;
	unsigned char buffer[4], *ibuf;
	int retval, i;
	unsigned long bytes_per_read, times;
	int actual_len;
	unsigned long bytes_to_read, totallen = 0;

	ibuf = kmalloc(2, GFP_KERNEL);
	if (!ibuf)
		return -ENOMEM;

	ch34x_dev = (struct ch34x_pis *)file->private_data;
	if (count == 0 || count > MAX_BUFFER_LENGTH) {
		return -EINVAL;
	}
	bytes_per_read = (ch34x_dev->chipver >= 0x20) ?
				 (CH341_EPP_IO_MAX - (CH341_EPP_IO_MAX & (CH341_PACKET_LENGTH - 1))) :
				 CH34x_EPP_IO_MAX;

	times = count / bytes_per_read;
	ibuf[0] = buffer[0] = buffer[2] = ch34x_dev->para_rmode;
	buffer[1] = (unsigned char)bytes_per_read;
	buffer[3] = (unsigned char)(count - times * bytes_per_read);

	if (buffer[3])
		times++;

	mutex_lock(&ch34x_dev->io_mutex);
	if (!ch34x_dev->interface) {
		retval = -ENODEV;
		mutex_unlock(&ch34x_dev->io_mutex);
		goto exit;
	}
	mutex_unlock(&ch34x_dev->io_mutex);

	for (i = 0; i < times; i++) {
		if ((i + 1) == times && buffer[3]) {
			ibuf[1] = buffer[3];
			bytes_to_read = buffer[3];
		} else {
			ibuf[1] = buffer[1];
			bytes_to_read = bytes_per_read;
		}

		ch34x_dev->bulk_in_buffer = kmalloc(sizeof(unsigned char) * bytes_to_read, GFP_KERNEL);
		if (ch34x_dev->bulk_in_buffer == NULL) {
			retval = -ENOMEM;
			goto exit;
		}

		mutex_lock(&ch34x_dev->io_mutex);
		retval = usb_bulk_msg(ch34x_dev->udev,
				      usb_sndbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_out_endpointAddr), ibuf, 0x02,
				      NULL, ch34x_dev->writetimeout);
		if (retval) {
			mutex_unlock(&ch34x_dev->io_mutex);
			goto exit1;
		}
		retval = usb_bulk_msg(ch34x_dev->udev,
				      usb_rcvbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_in_endpointAddr),
				      ch34x_dev->bulk_in_buffer, bytes_to_read, &actual_len, ch34x_dev->readtimeout);

		if (retval) {
			mutex_unlock(&ch34x_dev->io_mutex);
			goto exit1;
		}
		mutex_unlock(&ch34x_dev->io_mutex);
		retval = copy_to_user(to_user + totallen, ch34x_dev->bulk_in_buffer, actual_len);
		if (retval != actual_len)
			goto exit1;
		totallen += actual_len;
	}
exit1:
	kfree(ch34x_dev->bulk_in_buffer);
exit:
	kfree(ibuf);
	return retval == 0 ? totallen : retval;
}

static void ch34x_write_bulk_callback(struct urb *urb)
{
	struct ch34x_pis *ch34x_dev;

	ch34x_dev = urb->context;

	/* sync/async unlink faults aren't errors */
	if (urb->status) {
		if (!(urb->status == -ENOENT || urb->status == -ECONNRESET || urb->status == -ESHUTDOWN))
			dev_err(&ch34x_dev->interface->dev, "%s - nonzero write bulk status received: %d\n", __func__,
				urb->status);

		spin_lock(&ch34x_dev->err_lock);
		ch34x_dev->errors = urb->status;
		spin_unlock(&ch34x_dev->err_lock);
	}

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length, urb->transfer_buffer, urb->transfer_dma);
	up(&ch34x_dev->limit_sem);
}

/*
 * Write operation for parallel port read in EPP/MEM mode.
 */
ssize_t ch34x_fops_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *file_pos)
{
	struct ch34x_pis *ch34x_dev;
	int retval = 0;
	char *ibuf;
	char *buf = NULL;
	struct urb *urb = NULL;
	unsigned int i;
	unsigned int mlen, mnewlen, totallen;
	int times;

	ch34x_dev = (struct ch34x_pis *)file->private_data;
	if (count > MAX_BUFFER_LENGTH || count <= 0) {
		retval = -EINVAL;
		goto exit;
	}

	/*
	 * limit the number of URBs in flight to stop a user from using up all
	 * RAM
	 */
	if (down_interruptible(&ch34x_dev->limit_sem)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	spin_lock_irq(&ch34x_dev->err_lock);
	retval = ch34x_dev->errors;
	if (retval < 0) {
		ch34x_dev->errors = 0;
		retval = (retval == -EPIPE) ? retval : -EIO;
	}
	spin_unlock_irq(&ch34x_dev->err_lock);
	if (retval < 0)
		goto exit;

	times = count / CH34x_EPP_IO_MAX;
	mlen = count - times * CH34x_EPP_IO_MAX;
	mnewlen = times * CH341_PACKET_LENGTH;

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = (char *)kmalloc(sizeof(unsigned char) * MAX_BUFFER_LENGTH, GFP_KERNEL);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}

	for (i = 0; i < mnewlen; i += CH341_PACKET_LENGTH) {
		buf[i] = ch34x_dev->para_wmode;
		retval = copy_from_user(buf + i + 1, user_buffer + totallen, CH34x_EPP_IO_MAX);
		if (retval) {
			goto error;
		}
		totallen += CH341_PACKET_LENGTH;
	}
	if (mlen) {
		buf[i] = ch34x_dev->para_wmode;
		retval = copy_from_user(buf + i + 1, user_buffer + totallen, mlen);
		if (retval) {
			goto error;
		}
		mnewlen += mlen + 1;
	}

	ibuf = usb_alloc_coherent(ch34x_dev->udev, mnewlen, GFP_KERNEL, &urb->transfer_dma);
	if (!ibuf) {
		retval = -ENOMEM;
		goto error;
	}

	memcpy(ibuf, buf, mnewlen);

	mutex_lock(&ch34x_dev->io_mutex);
	if (!ch34x_dev->interface) {
		mutex_unlock(&ch34x_dev->io_mutex);
		retval = -ENODEV;
		goto error;
	}

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, ch34x_dev->udev, usb_sndbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_out_endpointAddr),
			  ibuf, mnewlen, ch34x_write_bulk_callback, ch34x_dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &ch34x_dev->submitted);

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	mutex_unlock(&ch34x_dev->io_mutex);
	if (retval) {
		dev_err(&ch34x_dev->interface->dev, "%s - failed submitting write urb, error %d\n", __func__, retval);
		goto error_unanchor;
	}

	/*
	 * release our reference to this urb, the USB core will eventually free
	 * it entirely
	 */
	usb_free_urb(urb);

	return count;

error_unanchor:
	usb_unanchor_urb(urb);
error:
	if (buf)
		kfree(buf);
	if (urb) {
		usb_free_coherent(ch34x_dev->udev, count, ibuf, urb->transfer_dma);
		usb_free_urb(urb);
	}
	up(&ch34x_dev->limit_sem);
exit:
	return retval;
}

static int ch34x_flush(struct file *file, fl_owner_t id)
{
	struct ch34x_pis *ch34x_dev;
	int res;

	ch34x_dev = file->private_data;
	if (ch34x_dev == NULL)
		return -ENODEV;

	/* wait for io to stop */
	mutex_lock(&ch34x_dev->io_mutex);
	ch34x_draw_down(ch34x_dev);

	/* read out errors, leave subsequent opens a clean slate */
	spin_lock_irq(&ch34x_dev->err_lock);
	res = ch34x_dev->errors ? (ch34x_dev->errors == -EPIPE ? -EPIPE : -EIO) : 0;
	ch34x_dev->errors = 0;
	spin_unlock_irq(&ch34x_dev->err_lock);

	mutex_unlock(&ch34x_dev->io_mutex);

	return res;
}

/*
 * Read operation for I2C/SPI interface.
 */
static int ch34x_data_read(struct ch34x_pis *ch34x_dev, void *obuffer, u32 bytes_to_read)
{
	int bytes_read;
	unsigned char *obuf;
	int retval = 0;

	if ((bytes_to_read > MAX_BUFFER_LENGTH) || (bytes_to_read == 0)) {
		retval = -EINVAL;
		goto exit;
	}

	spin_lock_irq(&ch34x_dev->err_lock);
	retval = ch34x_dev->errors;
	if (retval < 0) {
		ch34x_dev->errors = 0;
		retval = (retval == -EPIPE) ? retval : -EIO;
	}
	spin_unlock_irq(&ch34x_dev->err_lock);
	if (retval < 0)
		goto exit;

	obuf = (char *)kmalloc(sizeof(unsigned char) * bytes_to_read, GFP_KERNEL);
	if (!obuf) {
		retval = -ENOMEM;
		goto error;
	}

	mutex_lock(&ch34x_dev->io_mutex);
	retval = usb_bulk_msg(ch34x_dev->udev, usb_rcvbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_in_endpointAddr), obuf,
			      bytes_to_read, &bytes_read, ch34x_dev->readtimeout);

	if (retval) {
		mutex_unlock(&ch34x_dev->io_mutex);
		goto error;
	}
	mutex_unlock(&ch34x_dev->io_mutex);

	retval = copy_to_user((char __user *)obuffer, obuf, bytes_read);
	if (retval)
		retval = -ENOMEM;

error:
	kfree(obuf);
exit:
	return retval == 0 ? bytes_read : retval;
}

/*
 * Write operation for I2C/SPI interface.
 */
static int ch34x_data_write(struct ch34x_pis *ch34x_dev, void *ibuffer, u32 count)
{
	unsigned char *ibuf;
	int retval = 0;
	struct urb *urb = NULL;

	if (count > MAX_BUFFER_LENGTH || count <= 0) {
		retval = -EINVAL;
		goto exit;
	}

	/*
	 * limit the number of URBs in flight to stop a user from using up all
	 * RAM
	 */
	if (down_interruptible(&ch34x_dev->limit_sem)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	spin_lock_irq(&ch34x_dev->err_lock);
	retval = ch34x_dev->errors;
	if (retval < 0) {
		ch34x_dev->errors = 0;
		retval = (retval == -EPIPE) ? retval : -EIO;
	}
	spin_unlock_irq(&ch34x_dev->err_lock);
	if (retval < 0)
		goto exit;

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	ibuf = usb_alloc_coherent(ch34x_dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!ibuf) {
		retval = -ENOMEM;
		goto error;
	}

	retval = copy_from_user(ibuf, (char __user *)ibuffer, count);
	if (retval)
		goto error;

	mutex_lock(&ch34x_dev->io_mutex);
	if (!ch34x_dev->interface) {
		mutex_unlock(&ch34x_dev->io_mutex);
		retval = -ENODEV;
		goto error;
	}

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, ch34x_dev->udev, usb_sndbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_out_endpointAddr),
			  ibuf, count, ch34x_write_bulk_callback, ch34x_dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &ch34x_dev->submitted);

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	mutex_unlock(&ch34x_dev->io_mutex);
	if (retval) {
		dev_err(&ch34x_dev->interface->dev, "%s - failed submitting write urb, error %d\n", __func__, retval);
		goto error_unanchor;
	}

	/*
	 * release our reference to this urb, the USB core will eventually free
	 * it entirely
	 */
	usb_free_urb(urb);

	return count;

error_unanchor:
	usb_unanchor_urb(urb);
error:
	if (urb) {
		usb_free_coherent(ch34x_dev->udev, count, ibuf, urb->transfer_dma);
		usb_free_urb(urb);
	}
	up(&ch34x_dev->limit_sem);
exit:
	return retval;
}

/*
 * Write then Read operation for I2C/SPI interface.
 */
static int ch34x_data_write_read(struct ch34x_pis *ch34x_dev, void *ibuffer, void *obuffer, u32 readstep, u32 readtime,
				 u32 count)
{
	int bytes_read;
	unsigned char *ibuf;
	unsigned char *obuf = NULL;
	int retval = 0;
	struct urb *urb = NULL;
	int bytes_to_read;
	int totallen;
	int i;

	bytes_to_read = readstep * readtime;
	if (count > MAX_BUFFER_LENGTH || bytes_to_read > MAX_BUFFER_LENGTH) {
		retval = -EINVAL;
		goto exit;
	}
	/*
	 * limit the number of URBs in flight to stop a user from using up all
	 * RAM
	 */
	if (down_interruptible(&ch34x_dev->limit_sem)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	spin_lock_irq(&ch34x_dev->err_lock);
	retval = ch34x_dev->errors;
	if (retval < 0) {
		ch34x_dev->errors = 0;
		retval = (retval == -EPIPE) ? retval : -EIO;
	}
	spin_unlock_irq(&ch34x_dev->err_lock);
	if (retval < 0)
		goto exit;

	obuf = (char *)kmalloc(sizeof(unsigned char) * bytes_to_read, GFP_KERNEL);
	if (!obuf) {
		retval = -ENOMEM;
		goto error;
	}

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	ibuf = usb_alloc_coherent(ch34x_dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!ibuf) {
		retval = -ENOMEM;
		goto error;
	}

	retval = copy_from_user(ibuf, (char __user *)ibuffer, count);
	if (retval)
		goto error;

	mutex_lock(&ch34x_dev->io_mutex);
	if (!ch34x_dev->interface) {
		mutex_unlock(&ch34x_dev->io_mutex);
		retval = -ENODEV;
		goto error;
	}

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, ch34x_dev->udev, usb_sndbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_out_endpointAddr),
			  ibuf, count, ch34x_write_bulk_callback, ch34x_dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &ch34x_dev->submitted);

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);

	mutex_unlock(&ch34x_dev->io_mutex);

	if (retval) {
		dev_err(&ch34x_dev->interface->dev, "%s - failed submitting write urb, error %d\n", __func__, retval);
		goto error_unanchor;
	}

	for (i = 0; i < readtime; i++) {
		mutex_lock(&ch34x_dev->io_mutex);
		retval = usb_bulk_msg(ch34x_dev->udev,
				      usb_rcvbulkpipe(ch34x_dev->udev, ch34x_dev->bulk_in_endpointAddr),
				      obuf + totallen, CH341_PACKET_LENGTH, &bytes_read, ch34x_dev->readtimeout);

		if (retval) {
			mutex_unlock(&ch34x_dev->io_mutex);
			goto error1;
		}
		totallen += bytes_read;
		mutex_unlock(&ch34x_dev->io_mutex);
	}

	retval = copy_to_user((char __user *)obuffer, obuf, totallen);
	if (retval) {
		retval = -ENOMEM;
		goto error1;
	}

	/*
	 * release our reference to this urb, the USB core will eventually free
	 * it entirely
	 */
	usb_free_urb(urb);

	return totallen;

error1:
	kfree(obuf);
	return retval;
error_unanchor:
	usb_unanchor_urb(urb);
error:
	if (obuf)
		kfree(obuf);
	if (urb) {
		usb_free_coherent(ch34x_dev->udev, count, ibuf, urb->transfer_dma);
		usb_free_urb(urb);
	}
	up(&ch34x_dev->limit_sem);
exit:
	return retval;
}

static void ch34x_delete(struct kref *kref)
{
	struct ch34x_pis *ch34x_dev = container_of(kref, struct ch34x_pis, kref);
	usb_put_dev(ch34x_dev->udev);

	kfree(ch34x_dev);
}

static int ch34x_fops_open(struct inode *inode, struct file *file)
{
	struct ch34x_pis *ch34x_dev;
	struct usb_interface *interface;
	int retval = 0;
	unsigned int subminor;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
	subminor = iminor(inode);
#else
	subminor = iminor(file->f_path.dentry->d_inode);
#endif

	interface = usb_find_interface(&ch34x_pis_driver, subminor);
	if (!interface) {
		retval = -ENODEV;
		goto exit;
	}

	ch34x_dev = usb_get_intfdata(interface);
	if (!ch34x_dev) {
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&ch34x_dev->kref);

	file->private_data = ch34x_dev;

exit:
	return retval;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
static int ch34x_fops_ioctl(struct inode *inode, struct file *file, unsigned int ch34x_cmd, unsigned long ch34x_arg)
#else
static long ch34x_fops_ioctl(struct file *file, unsigned int ch34x_cmd, unsigned long ch34x_arg)
#endif
{
	int retval = 0;
	char *buf;
	int readtimeout = 0;
	int writetimeout = 0;
	u32 bytes_to_read;
	u32 bytes_write;
	u32 readstep;
	u32 readtime;
	u8 mode;
	char *drv_version = VERSION_DESC;
	struct ch34x_pis *ch34x_dev;
	unsigned long arg1, arg2, arg3;

	ch34x_dev = (struct ch34x_pis *)file->private_data;
	if (ch34x_dev == NULL) {
		return -ENODEV;
	}

	buf = kmalloc(2, GFP_KERNEL);
	if (!buf)
		return -EFAULT;

	switch (ch34x_cmd) {
	case CH34x_GET_DRV_VERSION:
		retval = copy_to_user((char __user *)ch34x_arg, (char *)drv_version, strlen(VERSION_DESC));
		break;
	case CH34x_CHIP_VERSION:
		retval = ch34x_control_transfer_in(VENDOR_VERSION, 0x0000, 0x0000, ch34x_dev, buf, 0x02);
		if (retval != 0x02)
			break;
		ch34x_dev->chipver = *(buf + 1) << 8 | *buf;
		retval = put_user(*buf, (u8 __user *)ch34x_arg);
		break;
	case CH34x_CHIP_TYPE:
		retval = put_user(ch34x_dev->chiptype, (u8 __user *)ch34x_arg);
		break;
	case CH34x_CHIP_ID:
		retval = copy_to_user((u16 __user *)ch34x_arg, ch34x_dev->ch34x_id, sizeof(u16) * 2);
		break;
	case CH34x_FUNCTION_SETPARA_MODE:
		retval = get_user(mode, (u8 __user *)ch34x_arg);
		if (retval)
			goto exit;
		retval = ch34x_control_transfer_out(CH34x_DEBUG_WRITE, 0x2525, (unsigned short)(mode << 8 | mode),
						    ch34x_dev, NULL, 0x00);
		break;
	case CH34x_FUNCTION_READ_MODE:
		retval = get_user(mode, (u8 __user *)ch34x_arg);
		if (retval)
			goto exit;
		if (mode)
			ch34x_dev->para_rmode = CH34x_PARA_CMD_R1;
		else
			ch34x_dev->para_rmode = CH34x_PARA_CMD_R0;
		break;
	case CH34x_FUNCTION_WRITE_MODE:
		retval = get_user(mode, (u8 __user *)ch34x_arg);
		if (retval)
			goto exit;
		if (mode)
			ch34x_dev->para_wmode = CH34x_PARA_CMD_W1;
		else
			ch34x_dev->para_wmode = CH34x_PARA_CMD_W0;
		break;
	case CH34x_SET_TIMEOUT:
		retval = get_user(readtimeout, (u32 __user *)ch34x_arg);
		if (retval)
			goto exit;
		retval = get_user(writetimeout, ((u32 __user *)ch34x_arg + 1));
		if (retval)
			goto exit;
		ch34x_dev->readtimeout = readtimeout;
		ch34x_dev->writetimeout = writetimeout;
		break;
	case CH34x_PIPE_DATA_READ:
		retval = get_user(bytes_to_read, (u32 __user *)ch34x_arg);
		if (retval)
			goto exit;
		arg1 = (unsigned long)((u32 __user *)ch34x_arg + 1);
		retval = ch34x_data_read(ch34x_dev, (void *)arg1, bytes_to_read);
		if (retval <= 0) {
			retval = -EFAULT;
			goto exit;
		}
		retval = put_user(retval, (u32 __user *)ch34x_arg);
		break;
	case CH34x_PIPE_DATA_WRITE:
		retval = get_user(bytes_write, (u32 __user *)ch34x_arg);
		if (retval)
			goto exit;
		arg1 = (unsigned long)((u32 __user *)ch34x_arg + 1);
		retval = ch34x_data_write(ch34x_dev, (void *)arg1, bytes_write);
		break;
	case CH34x_PIPE_WRITE_READ:
		retval = get_user(bytes_write, (u32 __user *)ch34x_arg);
		if (retval)
			goto exit;
		retval = get_user(readstep, (u32 __user *)ch34x_arg + 2);
		if (retval)
			goto exit;
		retval = get_user(readtime, (u32 __user *)ch34x_arg + 3);
		if (retval)
			goto exit;

		arg1 = (unsigned long)((u32 __user *)ch34x_arg + 1);
		arg2 = (unsigned long)((u8 __user *)ch34x_arg + 16);
		arg3 = (unsigned long)((u8 __user *)ch34x_arg + 16 + MAX_BUFFER_LENGTH);
		retval = ch34x_data_write_read(ch34x_dev, (void *)arg2, (void *)arg3, readstep, readtime, bytes_write);
		if (retval <= 0) {
			retval = -EFAULT;
			goto exit;
		}
		retval = put_user(retval, (u32 __user *)arg1);
		break;
	case CH34x_PIPE_DEVICE_CTRL:
		retval = get_user(mode, (u8 __user *)ch34x_arg);
		if (retval)
			goto exit;
		retval = ch34x_parallel_init(mode, ch34x_dev);
		break;
	default:
		retval = -ENOTTY;
		break;
	}

exit:
	kfree(buf);
	return retval;
}

static const struct file_operations ch34x_fops_driver = {
	.owner = THIS_MODULE,
	.open = ch34x_fops_open,
	.release = ch34x_fops_release,
	.read = ch34x_fops_read,
	.write = ch34x_fops_write,
	.flush = ch34x_flush,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
	.ioctl = ch34x_fops_ioctl,
#else
	.unlocked_ioctl = ch34x_fops_ioctl,
#endif
};

/*
 * usb class driver info in order to get a minor number from the usb core
 * and to have the device registered with the driver core
 */
static struct usb_class_driver ch34x_class = {
	.name = "ch34x_pis%d",
	.fops = &ch34x_fops_driver,
	.minor_base = CH34x_MINOR_BASE,
};

static int ch34x_pis_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	struct ch34x_pis *ch34x_dev;

	size_t buffer_size;
	int retval = -ENOMEM;
	int i;

	/* allocate memory for our device state and initialize it */
	ch34x_dev = kzalloc(sizeof(struct ch34x_pis), GFP_KERNEL);
	if (!ch34x_dev) {
		goto error;
	}

	/* init */
	kref_init(&ch34x_dev->kref);
	sema_init(&ch34x_dev->limit_sem, WRITES_IN_FLIGHT);
	spin_lock_init(&ch34x_dev->err_lock);
	init_usb_anchor(&ch34x_dev->submitted);

	ch34x_dev->udev = usb_get_dev(interface_to_usbdev(intf));
	ch34x_dev->ch34x_id[0] = ch34x_dev->udev->descriptor.idVendor;
	ch34x_dev->ch34x_id[1] = ch34x_dev->udev->descriptor.idProduct;
	ch34x_dev->interface = intf;

	iface_desc = intf->cur_altsetting;

	if (iface_desc->desc.bNumEndpoints < 1)
		return -ENODEV;

	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if ((endpoint->bEndpointAddress & USB_DIR_IN) && (endpoint->bmAttributes & 3) == 0x02) {
			buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
			ch34x_dev->bulk_in_size = buffer_size;
			ch34x_dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
		}

		if (((endpoint->bEndpointAddress & USB_DIR_IN) == 0x00) && (endpoint->bmAttributes & 3) == 0x02) {
			ch34x_dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}

		if ((endpoint->bEndpointAddress & USB_DIR_IN) && (endpoint->bmAttributes & 3) == 0x03) {
			ch34x_dev->interrupt_in_endpoint = endpoint;
		}
	}

	ch34x_dev->readtimeout = DEFAULT_TIMEOUT;
	ch34x_dev->writetimeout = DEFAULT_TIMEOUT;

	mutex_init(&ch34x_dev->io_mutex);

	/* save our data point in this interface device */
	usb_set_intfdata(intf, ch34x_dev);

	retval = usb_register_dev(intf, &ch34x_class);
	if (retval) {
		/* something prevented us from registering this driver */
		dev_err(&intf->dev, "Not able to get a minor for this device.\n");
		usb_set_intfdata(intf, NULL);
		goto error;
	}

	if (id->idProduct == 0x5512)
		ch34x_dev->chiptype = CHIP_CH341;
	else
		ch34x_dev->chiptype = CHIP_CH347;

	/* let the user know what node this device is now attached to */
	dev_info(&intf->dev, "USB device ch34x_pis #%d now attached", intf->minor);

	return 0;

error:
	if (ch34x_dev)
		kref_put(&ch34x_dev->kref, ch34x_delete);

	return retval;
}

static void ch34x_draw_down(struct ch34x_pis *ch34x_dev)
{
	int time;

	time = usb_wait_anchor_empty_timeout(&ch34x_dev->submitted, 1000);
	if (!time)
		usb_kill_anchored_urbs(&ch34x_dev->submitted);
}

static int ch34x_pis_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct ch34x_pis *ch34x_dev = usb_get_intfdata(intf);

	if (!ch34x_dev)
		return 0;
	ch34x_draw_down(ch34x_dev);
	return 0;
}

static int ch34x_pis_resume(struct usb_interface *intf)
{
	return 0;
}

static void ch34x_pis_disconnect(struct usb_interface *intf)
{
	struct ch34x_pis *ch34x_dev;
	int minor = intf->minor;

	ch34x_dev = usb_get_intfdata(intf);
	usb_set_intfdata(intf, NULL);

	/* give back our minor */
	usb_deregister_dev(intf, &ch34x_class);

	mutex_lock(&ch34x_dev->io_mutex);
	ch34x_dev->interface = NULL;
	mutex_unlock(&ch34x_dev->io_mutex);

	usb_kill_anchored_urbs(&ch34x_dev->submitted);

	/* decrement our usage count*/
	kref_put(&ch34x_dev->kref, ch34x_delete);

	pr_info("CH34x_pis-%d now disconnected.\n", minor);
}

static int ch34x_pre_reset(struct usb_interface *intf)
{
	struct ch34x_pis *ch34x_dev = usb_get_intfdata(intf);

	mutex_lock(&ch34x_dev->io_mutex);
	ch34x_draw_down(ch34x_dev);

	return 0;
}

static int ch34x_post_reset(struct usb_interface *intf)
{
	struct ch34x_pis *ch34x_dev = usb_get_intfdata(intf);

	ch34x_dev->errors = -EPIPE;
	mutex_unlock(&ch34x_dev->io_mutex);

	return 0;
}

/* usb driver Interface */
static struct usb_driver ch34x_pis_driver = {
	.name = "ch34x_pis",
	.probe = ch34x_pis_probe,
	.disconnect = ch34x_pis_disconnect,
	.suspend = ch34x_pis_suspend,
	.resume = ch34x_pis_resume,
	.pre_reset = ch34x_pre_reset,
	.post_reset = ch34x_post_reset,
	.id_table = ch34x_usb_ids,
	.supports_autosuspend = 1,
};

static int __init ch34x_pis_init(void)
{
	int retval;

	printk(KERN_INFO KBUILD_MODNAME ": " DRIVER_DESC "\n");
	printk(KERN_INFO KBUILD_MODNAME ": " VERSION_DESC "\n");
	retval = usb_register(&ch34x_pis_driver);
	if (retval)
		printk(KERN_INFO "CH34x Device Register Failed.\n");
	return retval;
}

static void __exit ch34x_pis_exit(void)
{
	printk(KERN_INFO KBUILD_MODNAME ": "
					"ch34x driver exit.\n");
	usb_deregister(&ch34x_pis_driver);
}

module_init(ch34x_pis_init);
module_exit(ch34x_pis_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
