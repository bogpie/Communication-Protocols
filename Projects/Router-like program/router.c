#include "queue.h"
#include "skel.h"
#include <netinet/if_ether.h>

int main(int argc, char *argv[])
{
	//setvbuf(stdout, NULL, _IONBF, 0);

	packet m;
	int rc;
	init(argc - 2, argv + 2);

	RouteTableEntry *routeTable = malloc(sizeof(RouteTableEntry) * 100000);
	ArpTableEntry *arpTable = malloc(sizeof(ArpTableEntry) * 100000);
	int arpTableSize = 0;

	DIE(routeTable == NULL, "Route table memory allocation error\n");
	DIE(arpTable == NULL, "Arp table memory allocation error\n");

	unsigned int routeTableSize = 0;

	parse_route_table(routeTable, &routeTableSize, argv);

	queue packets;
	packets = queue_create();
	DIE(packets == NULL, "Packets queue memory allocation error\n");

	while (1)
	{
		rc = get_packet(&m);

		DIE(rc < 0, "get_message\n");

		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + OFFSET_IP);
		struct icmphdr *icmp_hdr = parse_icmp(m.payload);

		/*
			2. If received IP packet for router and it is an ECHO request packet, answer.
			Discard original packet.
		*/

		if (ip_hdr->daddr == inet_addr(get_interface_ip(m.interface)))
		{
			if (icmp_hdr->type == ICMP_ECHO)
			{
				int id = icmp_hdr->un.echo.id;
				int seq = icmp_hdr->un.echo.sequence;
				send_icmp(ip_hdr->saddr, ip_hdr->daddr, eth_hdr->ether_dhost, eth_hdr->ether_shost,
						  ICMP_ECHOREPLY, 0, m.interface, id, seq);
			}
			continue;
		}

		if (eth_hdr->ether_type == htons(ETHERTYPE_ARP))
		{
			struct arp_header *arpHdr = parse_arp(m.payload);
			DIE(arpHdr == NULL, "arp_hdr wasn't extracted successfully\n");

			/*
				3.	If received ARP Request, then send ARP Reply
			*/
			if (arpHdr->op == htons(ARPOP_REQUEST))
			{
				printf("Received ARP request, sending ARP reply\n");
				memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, sizeof(eth_hdr->ether_dhost));
				get_interface_mac(m.interface, eth_hdr->ether_shost);
				uint32_t interfaceIP = inet_addr(get_interface_ip(m.interface));
				send_arp(arpHdr->spa, interfaceIP, eth_hdr, m.interface, htons(ARPOP_REPLY));
			}

			/*
				4.	If received ARP Reply, update ARP table.
				Send top packet from queue to that router. 
			*/
			else if (arpHdr->op == htons(ARPOP_REPLY))
			{
				if (!queue_empty(packets))
				{
					packet *top = queue_deq(packets);
					struct ether_header *eth_hdr_top = (struct ether_header *)top->payload;

					memcpy(eth_hdr_top->ether_dhost, eth_hdr_top->ether_shost, sizeof(eth_hdr_top->ether_dhost));
					get_interface_mac(m.interface, eth_hdr_top->ether_shost);

					send_packet(top->interface, top);
				}

				struct ArpTableEntry arpTableEntry;

				arpTableEntry.ip = arpHdr->spa;
				memcpy(arpTableEntry.mac, arpHdr->sha, sizeof(arpTableEntry.mac));

				arpTable[arpTableSize++] = arpTableEntry;
			}
		}

		/*
			5.	ICMP_TIME_EXCEEDED case
		*/
		if (ip_hdr->ttl <= 1)
		{
			uint32_t interfaceIP = inet_addr(get_interface_ip(m.interface));
			send_icmp_error(ip_hdr->saddr, interfaceIP, eth_hdr->ether_dhost,
							eth_hdr->ether_shost, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, m.interface);
			continue;
		}

		/*
			6. Wrong checksum case
		*/
		if (ip_checksum(ip_hdr, sizeof(struct iphdr)) != 0)
		{
			continue;
		}

		/* 
			7. Decrement TTL, update checksum
		*/
		--ip_hdr->ttl;
		ip_hdr->check = 0;
		ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));

		/*
			8. Look for most specific route table entry; find its next hop. In case of 
			error, send ICMP Destination unreachable
		*/

		RouteTableEntry *bestRoute = get_best_route(ip_hdr->daddr, routeTable, routeTableSize);
		if (bestRoute == NULL) // can't found in route table
		{
			uint32_t interfaceIP = inet_addr(get_interface_ip(m.interface));
			send_icmp_error(ip_hdr->saddr, interfaceIP, eth_hdr->ether_dhost,
							eth_hdr->ether_shost, ICMP_DEST_UNREACH, 0, m.interface);
			continue;
		}
		else // can reach destination
		{
			ArpTableEntry *arpTableEntry = get_arp_entry(bestRoute->nextHop, arpTable, arpTableSize);

			struct ether_header *eth_hdr_new = malloc(sizeof(struct ether_header));

			if (arpTableEntry == NULL)
			{
				memset(eth_hdr_new->ether_dhost, 0xff, 6);
				eth_hdr_new->ether_type = htons(ETHERTYPE_ARP);
				get_interface_mac(bestRoute->interface, eth_hdr_new->ether_shost);
				queue_enq(packets, &m);
				send_arp(ip_hdr->daddr, inet_addr(get_interface_ip(bestRoute->interface)), eth_hdr_new, bestRoute->interface, htons(ARPOP_REQUEST));
			}
			else // forwarding process
			{
				ArpTableEntry *arpTableEntry = get_arp_entry(bestRoute->nextHop, arpTable, arpTableSize);
				eth_hdr_new->ether_type = eth_hdr->ether_type;
				memcpy(eth_hdr_new->ether_dhost, arpTableEntry->mac, sizeof(eth_hdr_new->ether_dhost));
				get_interface_mac(bestRoute->interface, eth_hdr_new->ether_shost);
				send_packet(bestRoute->interface, &m);
			}
		}
	}
}
