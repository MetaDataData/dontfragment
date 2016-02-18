#include <linux/module.h>
#include <linux/kernel.h>
#include <net/ip.h>

#include <linux/netfilter_ipv4/ip_tables.h>

#include "ipt_DF.h"

MODULE_AUTHOR("Semyon Verchenko");
MODULE_DESCRIPTION("Netfilter module to set/reset DF flag");
MODULE_LICENSE("Dual BSD/GPL");

static int df_tg_check(const struct xt_tgchk_param *param)
{
	if (strcmp(param->table, "mangle"))
	{
		printk(KERN_WARNING "df is called from table %s; call df only from mangle table\n", param->table);
		return -EINVAL;
	}
	return 0;
}

static unsigned int df_tg(struct sk_buff *skb, const struct xt_action_param *param)
{
	struct iphdr *iph = ip_hdr(skb);
	df_mode mode = ((struct xt_df_tginfo *)(param->targinfo))->mode;

	if (!skb_make_writable(skb, skb->len)){
		printk(KERN_ERR "DF: Error making skb writable\n");
		return NF_DROP;
	}

	if ((mode == IPT_DF_SET   &&  (iph->frag_off & htons(IP_DF))) ||
			(mode == IPT_DF_RESET && !(iph->frag_off & htons(IP_DF))))
		return XT_CONTINUE;

	if (mode == IPT_DF_SET)
		iph->frag_off |= htons(IP_DF);
	else if (mode == IPT_DF_RESET)
		iph->frag_off &= ~htons(IP_DF);
	else {
		/* printk(KERN_WARNING "unknown DF mode %u; doing nothing\n", (int)mode); */
		return XT_CONTINUE;
	}

	ip_send_check(iph);

	return XT_CONTINUE;
}

static struct xt_target ipt_df = {
	.name = "DF",
	.target = df_tg,
	.table = "mangle",
	.targetsize = sizeof(struct xt_df_tginfo),
	.checkentry = df_tg_check,
	.me = THIS_MODULE,
};

static int __init df_tg_init(void)
{
	printk(KERN_INFO "DF loading\n");
	return xt_register_target(&ipt_df);
}

void __exit df_tg_exit(void)
{
	printk(KERN_INFO "DF unloading\n");
	xt_unregister_target(&ipt_df);
}

module_init(df_tg_init);
module_exit(df_tg_exit);
