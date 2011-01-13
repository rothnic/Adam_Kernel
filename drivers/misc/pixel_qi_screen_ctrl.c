/*
 * Copyright (C) 2010 Malata, Corp. All Right Reserved.
 *
 * Athor: LiuZheng <xmlz@malata.com>
 */
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/pixel_qi_screen_ctrl.h>

#define TAG 			"pixelQi: "

static ssize_t
ps_ctrl_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
	int gpio, state;
	gpio = (int)dev_get_drvdata(dev);
	state = gpio_get_value(gpio);
	return sprintf(buffer, "%d", state);
}

static ssize_t
ps_ctrl_store(struct device *dev, struct device_attribuet *attr, const char *buffer, ssize_t count)
{
	int state, gpio;
	gpio = (int)dev_get_drvdata(dev);
	state = simple_strtol(buffer, NULL, 10);
	gpio_set_value(gpio, state ? 1 : 0);
	return count;
}

static DEVICE_ATTR(state, 0777, ps_ctrl_show, ps_ctrl_store);

static int ps_ctrl_probe(struct platform_device *pdev)
{
	int ret, gpio;
	gpio = (int)pdev->dev.platform_data;
	if (gpio <= 0) {
		pr_err(TAG "gpio <= 0\n");
		return -EFAULT;
	}
	ret = device_create_file(&pdev->dev, &dev_attr_state);
	if (ret < 0) {
		pr_err(TAG "device_create_file=NULL\n");
		return -EFAULT;
	}
	ret = gpio_request(gpio, PQS_CTRL_DRV_NAME);
	if (ret < 0) {
		pr_err(TAG "gpio_request=NULL");
		goto failed_request_gpio;
	}
	gpio_direction_output(gpio, 1);
	platform_set_drvdata(pdev, (void*)gpio);
	return 0;
failed_request_gpio:
	return -EFAULT;
}

static int ps_ctrl_remove(struct platform_device *pdev)
{
	device_remove_file(&pdev->dev, &dev_attr_state);
	return 0;
}

static struct platform_driver ps_ctrl_driver = {
	.probe = ps_ctrl_probe, 
	.remove = ps_ctrl_remove, 
	.driver = {
		.name = PQS_CTRL_DRV_NAME, 
		.owner = THIS_MODULE, 
	},
};

static int ps_ctrl_init()
{
	return platform_driver_register(&ps_ctrl_driver);
}

static void ps_ctrl_exit()
{
	platform_driver_unregister(&ps_ctrl_driver);
}

module_init(ps_ctrl_init);
module_exit(ps_ctrl_exit);

