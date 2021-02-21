#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct minirq_dev {
    struct gpio_desc *gpiod;
    struct work_struct work;
    int irq;
};

void irq_bottom_half(struct work_struct *work)
{
    pr_info("Rising edge detected!\n");
    return;
}

static irqreturn_t irq_top_half(int irq, void *p)
{
    struct platform_device *pdev = p;
    struct minirq_dev *minirq;

    minirq = platform_get_drvdata(pdev);
    schedule_work(&(minirq -> work));
    return IRQ_HANDLED;
}

static int minirq_probe(struct platform_device *pdev)
{
    struct device *dev = &(pdev-> dev);
    struct minirq_dev *minirq;
    int ret = 0;

    minirq = devm_kzalloc(dev, sizeof(struct minirq_dev), GFP_KERNEL);
    if (!minirq) {
        dev_err(dev, "Failed to allocate memory.\n");
	return -ENOMEM;
    }

    minirq->gpiod = devm_gpiod_get_index(dev, "minirq", 0, GPIOD_IN);
    if (IS_ERR(minirq->gpiod)) {
        dev_err(dev, "Failed to get gpio descriptor.\n");
        return PTR_ERR(minirq -> gpiod);
    }
    
    ret = gpiod_to_irq(minirq->gpiod);
    if (ret < 0) {
        dev_err(dev, "Failed to get irq from gpiod.\n");
        return ret;
    }
    minirq->irq = ret;

    INIT_WORK(&minirq->work, irq_bottom_half);

    ret = devm_request_irq(dev, minirq->irq, irq_top_half, 
            IRQF_TRIGGER_RISING, "minirq", pdev);

    if (ret < 0) {
        dev_err(dev, "Failed to request IRQ.\n");
        return ret;
    }

    platform_set_drvdata(pdev, minirq);
    return 0;
}

static const struct of_device_id minirq_ids[] = {
    {.compatible = "minirq",},
    {}
};

static struct platform_driver minirq_driver = {
    .driver = {
        .name = "minirq",
        .of_match_table = minirq_ids,
    },
    .probe = minirq_probe
};
MODULE_LICENSE("GPL");
module_platform_driver(minirq_driver);
