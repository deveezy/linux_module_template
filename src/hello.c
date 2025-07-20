#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("noname");
MODULE_DESCRIPTION("USB connect/disconnect logger");
MODULE_VERSION("0.1");

static int usb_notifier(struct notifier_block *nb, unsigned long action,
                        void *data) {
  struct usb_device *udev = data;

  switch (action) {
  case USB_DEVICE_ADD:
    printk(KERN_INFO "[usb_monitor] USB device added: %04x:%04x\n",
           le16_to_cpu(udev->descriptor.idVendor),
           le16_to_cpu(udev->descriptor.idProduct));
    break;

  case USB_DEVICE_REMOVE:
    printk(KERN_INFO "[usb_monitor] USB device removed: %04x:%04x\n",
           le16_to_cpu(udev->descriptor.idVendor),
           le16_to_cpu(udev->descriptor.idProduct));
    break;
  }

  return NOTIFY_OK;
}

static struct notifier_block usb_nb = {
    .notifier_call = usb_notifier,
};

static int __init usb_logger_init(void) {
  printk(KERN_INFO "usb_monitor: loading...\n");
  usb_register_notify(&usb_nb);
  return 0;
}

static void __exit usb_logger_exit(void) {
  printk(KERN_INFO "usb_monitor: unloading...\n");
  usb_unregister_notify(&usb_nb);
}

module_init(usb_logger_init);
module_exit(usb_logger_exit);
