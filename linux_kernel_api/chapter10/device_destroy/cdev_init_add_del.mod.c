#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4061a14b, "module_layout" },
	{ 0xcbfa2717, "device_create" },
	{ 0x1eb2af36, "__class_create" },
	{ 0x616d48f9, "cdev_add" },
	{ 0xf67ff7c6, "cdev_init" },
	{ 0xa1d8dff3, "cdev_alloc" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x999e8297, "vfree" },
	{ 0x9b6f4b74, "class_destroy" },
	{ 0xe7419807, "device_destroy" },
	{ 0xa3da29c5, "cdev_del" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0x59733206, "module_put" },
	{ 0x7ecb001b, "__per_cpu_offset" },
	{ 0x26fd80b9, "per_cpu__cpu_number" },
	{ 0xb72397d5, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "1602F0B15DBB5AF655FFF90");
