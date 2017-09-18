#include <linux/netfilter.h>  
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/netfilter_ipv4.h>  
#include <linux/ip.h>  
#include <linux/inet.h>
#include<linux/types.h> 
#include <linux/kernel.h>

/** 
 * 钩子函数实体 
 * 获取目的IP地址,并且根据NAT协议进行IP地址修改
 * 这里实现的是无端口概念的NAT 
 * 这里涉及到源NAT和目的NAT
 */  
unsigned int my_hookfn1(unsigned int hooknum,  
    struct sk_buff *skb,  
    const struct net_device *in,  
    const struct net_device *out,  
    int (*okfn)(struct sk_buff *))  
{  
    struct iphdr *iph;  
    iph = ip_hdr(skb);  
    
    if(iph->daddr == in_aton("10.1.1.10"))
    {
    iph->daddr = in_aton("192.168.1.10");  
    /* 修改ip层校验和 */
    iph->check = 0; /* 将检验和先置为0 */
    iph->check = ip_fast_csum (( char * )iph,iph->ihl); /*修改IP Stack校验和*/
    /* 修改SKB校验和 unknown command */
    skb->ip_summed = CHECKSUM_NONE;
    } 
    else if(iph->daddr == in_aton("10.2.2.10"))
    { 
    iph->daddr = in_aton("172.16.1.10");  
    /* 修改ip层校验和 */
    iph->check = 0; /* 将检验和先置为0 */
    iph->check = ip_fast_csum (( char * )iph,iph->ihl); /*修改校验和*/
    /* 修改SKB校验和 */
    skb->ip_summed = CHECKSUM_NONE;
    }
    else{
	
	}
    return NF_ACCEPT;  
}  
  
/* 实例化一个钩子函数 */  
static struct nf_hook_ops nfho1 = {  
    .hook = my_hookfn1,  
    .pf = PF_INET,  
    .hooknum = NF_INET_PRE_ROUTING,  
    .priority = NF_IP_PRI_FIRST,  
};  
  
/* POSTROUTING HOOK */
unsigned int my_hookfn2(unsigned int hooknum,  
    struct sk_buff *skb,  
    const struct net_device *in,  
    const struct net_device *out,  
    int (*okfn)(struct sk_buff *))  
{  
    struct iphdr *iph;  
    iph = ip_hdr(skb);
  
    if(iph->saddr == in_aton("172.16.1.10")){
       iph->saddr = in_aton("10.2.2.10");  
       /* 修改ip层校验和 */
       iph->check = 0; /* 将检验和先置为0 */
       iph->check = ip_fast_csum (( char * )iph,iph->ihl); /*修改校验和*/

       /* 修改SKB校验和 */
       skb->ip_summed = CHECKSUM_NONE;
    }

    else if(iph->saddr == in_aton("192.168.1.10")) 
    {  
    iph->saddr = in_aton("10.1.1.10");  
    /* 修改ip层校验和 */
    iph->check = 0; /* 将检验和先置为0 */
    iph->check = ip_fast_csum (( char * )iph,iph->ihl); /*修改校验和*/

    /* 修改SKB校验和 */
    skb->ip_summed = CHECKSUM_NONE;
    }
    return NF_ACCEPT;  
}  
  
/* 实例化一个钩子函数 */  
static struct nf_hook_ops nfho2 = {  
    .hook = my_hookfn2,  
    .pf = PF_INET,  
    .hooknum = NF_INET_POST_ROUTING,  
    .priority = NF_IP_PRI_FIRST,  
};  

/* 模块加载初始化 */
static int __init sknf_init(void)  
{  
    if (nf_register_hook(&nfho1) || nf_register_hook(&nfho2)) {  
        printk(KERN_ERR "nf_register_hook() failed\n");  
        return -1;  
    }  
    return 0;  
}  

/* 卸载一个模块 */
static void __exit sknf_exit(void)  
{  
    nf_unregister_hook(&nfho1);
    nf_unregister_hook(&nfho2);  
}  
  
module_init(sknf_init);  
module_exit(sknf_exit);  
MODULE_AUTHOR("RTOS");  
MODULE_LICENSE("GPL"); 
