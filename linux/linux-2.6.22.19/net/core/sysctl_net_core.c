/* -*- linux-c -*-
 * sysctl_net_core.c: sysctl interface to net core subsystem.
 *
 * Begun April 1, 1996, Mike Shaver.
 * Added /proc/sys/net/core directory entry (empty =) ). [MS]
 */

#include <linux/mm.h>
#include <linux/sysctl.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/init.h>
#include <net/sock.h>

extern int netdev_max_backlog;
extern int weight_p;

extern __u32 sysctl_wmem_max;
extern __u32 sysctl_rmem_max;

extern int sysctl_core_destroy_delay;

#ifdef CONFIG_XFRM
extern u32 sysctl_xfrm_aevent_etime;
extern u32 sysctl_xfrm_aevent_rseqth;
extern int sysctl_xfrm_larval_drop;
extern u32 sysctl_xfrm_acq_expires;
#endif

static int net_msg_warn;	/* Unused, but still a sysctl */

static struct ctl_table net_core_table[] = {
#ifdef CONFIG_NET
	{
		.ctl_name	= NET_CORE_WMEM_MAX,
		.procname	= "wmem_max",
		.data		= &sysctl_wmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_RMEM_MAX,
		.procname	= "rmem_max",
		.data		= &sysctl_rmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_WMEM_DEFAULT,
		.procname	= "wmem_default",
		.data		= &sysctl_wmem_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_RMEM_DEFAULT,
		.procname	= "rmem_default",
		.data		= &sysctl_rmem_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_DEV_WEIGHT,
		.procname	= "dev_weight",
		.data		= &weight_p,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_MAX_BACKLOG,
		.procname	= "netdev_max_backlog",
		.data		= &netdev_max_backlog,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_MSG_COST,
		.procname	= "message_cost",
		.data		= &net_msg_cost,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec_jiffies,
		.strategy	= &sysctl_jiffies,
	},
	{
		.ctl_name	= NET_CORE_MSG_BURST,
		.procname	= "message_burst",
		.data		= &net_msg_burst,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{
		.ctl_name	= NET_CORE_OPTMEM_MAX,
		.procname	= "optmem_max",
		.data		= &sysctl_optmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
#ifdef CONFIG_XFRM
	{
		.ctl_name	= NET_CORE_AEVENT_ETIME,
		.procname	= "xfrm_aevent_etime",
		.data		= &sysctl_xfrm_aevent_etime,
		.maxlen		= sizeof(u32),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_AEVENT_RSEQTH,
		.procname	= "xfrm_aevent_rseqth",
		.data		= &sysctl_xfrm_aevent_rseqth,
		.maxlen		= sizeof(u32),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "xfrm_larval_drop",
		.data		= &sysctl_xfrm_larval_drop,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "xfrm_acq_expires",
		.data		= &sysctl_xfrm_acq_expires,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
#endif /* CONFIG_XFRM */
#endif /* CONFIG_NET */
	{
		.ctl_name	= NET_CORE_SOMAXCONN,
		.procname	= "somaxconn",
		.data		= &sysctl_somaxconn,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_BUDGET,
		.procname	= "netdev_budget",
		.data		= &netdev_budget,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_WARNINGS,
		.procname	= "warnings",
		.data		= &net_msg_warn,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{ .ctl_name = 0 }
};

static __initdata struct ctl_path net_core_path[] = {
	{ .procname = "net", .ctl_name = CTL_NET, },
	{ .procname = "core", .ctl_name = NET_CORE, },
	{ },
};

static __init int sysctl_core_init(void)
{
	struct ctl_table_header *hdr;

	hdr = register_sysctl_paths(net_core_path, net_core_table);
	return hdr == NULL ? -ENOMEM : 0;
}

__initcall(sysctl_core_init);
