/*
 * Copyright © 2015, Avinaba Dalal "corphish" <d97.avinaba@gmail.com>
 *
 * Charger Control driver for smb1360-charger-fg
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>

#include "zapdos_charger_control.h"

#define DRIVER_MAJOR_VERSION 		1
#define DRIVER_MINOR_VERSION 		0

#define MAX_CUSTOM_CURRENT			1000
#define MIN_CUSTOM_CURRENT			100		//TBD 

#define DEFAULT_SWITCH 				1		//Master Switch enabled by default
#define DEFAULT_CURRENT				1000			

//Tuneables
int master_switch = DEFAULT_SWITCH;		//disabled by default
int custom_current = DEFAULT_CURRENT;		

int check_switch_validity (int input_switch) {

	/* Checks whether value of switch is proper or not.
	   Return 0 if proper else return 1.*/

	if (input_switch == 0 || input_switch == 1)
		return 0;
	else
		return 1;
}

int check_current_limits (int input_current) {

	/*
		Checks whether user input current is within safe limits or not.
		Return 0 if within safe limit, else return 1
	*/

	if (input_current >= MIN_CUSTOM_CURRENT && input_current <= MAX_CUSTOM_CURRENT)
		return 0;
	else
		return 1;
}

//sys fs stuff
static ssize_t master_switch_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", master_switch);
}

static ssize_t master_switch_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val, rc;
	sscanf(buf, "%d", &val);
	rc = check_switch_validity(val);
	if(rc) {
		master_switch = DEFAULT_SWITCH;
		pr_info ("zapdos_charger_control: Invalid switch selection. Using default value %d\n", master_switch);
	} else {
		master_switch = val;
		pr_info ("zapdos_charger_control: Switch - %d\n", master_switch);
	}

	return count;
}

static ssize_t custom_current_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", custom_current);
}

static ssize_t custom_current_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int new_current, rc;
	sscanf(buf, "%d", &new_current);
	rc = check_current_limits(new_current);
	if (rc) {
		custom_current = DEFAULT_CURRENT;
		pr_info ("zapdos_charger_control: Unsafe current input, switching to defalt value of %d\n", custom_current);
	} else {
		custom_current = new_current;
	}
	return count;
}

static ssize_t charger_version_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "Charger Control %u.%u", DRIVER_MAJOR_VERSION, DRIVER_MINOR_VERSION);
} 

static struct kobj_attribute master_switch_attribute =
	__ATTR(enabled,
		0666,
		master_switch_show,
		master_switch_store);
		
static struct kobj_attribute charger_ctrl_ver_attribute =
	__ATTR(version,
		0444,
		charger_version_show, NULL);

static struct kobj_attribute custom_current_attribute =
	__ATTR(custom_current,
		0666,
		custom_current_show,
		custom_current_store);

static struct attribute *charger_control_attrs[] =
	{
		&custom_current_attribute.attr,
		&master_switch_attribute.attr,
		&charger_ctrl_ver_attribute.attr,
		NULL,
	};

static struct attribute_group charger_control_attr_group =
	{
		.attrs = charger_control_attrs,
	};

static struct kobject *charger_control_kobj;

static int charger_control_probe(void)
{
	int sysfs_result;
	printk(KERN_DEBUG "[%s]\n",__func__);

	charger_control_kobj = kobject_create_and_add("zapdos_charger_control", kernel_kobj);

	if (!charger_control_kobj) {
		pr_err("%s Interface create failed!\n",
			__FUNCTION__);
		return -ENOMEM;
        }

	sysfs_result = sysfs_create_group(charger_control_kobj,
			&charger_control_attr_group);

	if (sysfs_result) {
		pr_info("%s sysfs create failed!\n", __FUNCTION__);
		kobject_put(charger_control_kobj);
	}
	return sysfs_result;
}

static void charger_control_remove(void)
{
	if (charger_control_kobj != NULL)
		kobject_put(charger_control_kobj);
}

module_init(charger_control_probe);
module_exit(charger_control_remove);
MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Avinaba Dalal <d97.avinaba@gmail.com>");
MODULE_DESCRIPTION("SMB1360 Charger control driver");

