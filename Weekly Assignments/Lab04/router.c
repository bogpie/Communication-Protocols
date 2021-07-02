#include "skel.h"

int interfaces[ROUTER_NUM_INTERFACES];
struct route_table_entry *rtable;
int rtable_size;

struct arp_entry *arp_table;
int arp_table_len;

/*
 Returns a pointer (eg. &rtable[i]) to the best matching route
 for the given dest_ip. Or NULL if there is no matching route.
*/

struct route_table_entry *get_best_route(__u32 dest_ip)
{
	/* TODO 1: Implement the function */
	uint32_t bestMask = -1;
	int bestEntryIndex = -1;

	for (int i = 0; i < rtable_size; ++i)
	{
		struct route_table_entry entry = rtable[i];
		//and intre adresa ip a destinatiei si masca ==? prefix
		if ((dest_ip & entry.mask) == entry.prefix)
		{
			if (entry.mask > bestMask)
			{
				bestMask = entry.mask;
				bestEntryIndex = i;
			}
		}
	}

	if (bestMask != -1)
	{
		return &rtable[bestEntryIndex];
	}

	return NULL;
}

/*
 Returns a pointer (eg. &arp_table[i]) to the best matching ARP entry.
 for the given dest_ip or NULL if there is no matching entry.
*/
struct arp_entry *get_arp_entry(__u32 ip)
{
	for (int i = 0; i < arp_table_len; i++)
	{
		if (ip == arp_table[i].ip)
		{
			return &arp_table[i];
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	msg m;
	int rc;

	init();
	rtable = malloc(sizeof(struct route_table_entry) * 100);
	arp_table = malloc(sizeof(struct arp_entry) * 100);
	DIE(rtable == NULL, "memory");
	rtable_size = read_rtable(rtable);

	parse_arp_table();
	/* Students will write code here */

	while (1)
	{
		rc = get_packet(&m);

		DIE(rc < 0, "get_message");
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));

		/* TODO 3: Check the checksum */
		//uint16_t check1 = ip_hdr->check;
		uint16_t checkResult = ip_checksum(ip_hdr, sizeof(struct iphdr));
		if (checkResult != 0)
		{
			continue;
		}

		/* TODO 4: Check TTL >= 1 */
		if (ip_hdr->ttl < 1)
		{
			continue;
		}

		/* TODO 5: Find best matching route (using the function you wrote at TODO 1) */
		__u32 destination = ip_hdr->daddr;
		struct route_table_entry *bestRoute = get_best_route(destination);
		if (bestRoute == NULL)
		{
			continue;
		}

		/* TODO 6: Update TTL and recalculate the checksum */
		ip_hdr->ttl--;
		ip_hdr->check = 0;
		ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));

		/* TODO 7: Find matching ARP entry and update Ethernet addresses */
		struct arp_entry *arpEntry = get_arp_entry(destination);
		if (arpEntry == NULL)
		{
			continue;
		}
		//eth_hdr->ether_dhost = arpEntry->mac;
		memcpy(&eth_hdr->ether_dhost,&arpEntry->mac,sizeof(eth_hdr->ether_dhost));
		
		//eth_hdr->ether_shost = get_interface_mac(bestRoute->interface, eth_hdr->ether_shost);
		uint8_t newInterfaceMac = get_interface_mac(bestRoute->interface, eth_hdr->ether_shost);
		memcpy(&eth_hdr->ether_shost,&newInterfaceMac,sizeof(eth_hdr->ether_shost));


		/* TODO 8: Forward the packet to best_route->interface */
		send_packet(bestRoute->interface, &m);
	}

	return 0;
}
