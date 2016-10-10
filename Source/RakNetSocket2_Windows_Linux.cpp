/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EmptyHeader.h"

#ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS

#ifndef RAKNETSOCKET2_WINDOWS_LINUX_CPP
#define RAKNETSOCKET2_WINDOWS_LINUX_CPP

#if !defined(WINDOWS_STORE_RT) && !defined(__native_client__)

#if RAKNET_SUPPORT_IPV6==1

void PrepareAddrInfoHints2(addrinfo *hints)
{
	memset(hints, 0, sizeof (addrinfo)); // make sure the struct is empty
	hints->ai_socktype = SOCK_DGRAM; // UDP sockets
	hints->ai_flags = AI_PASSIVE;     // fill in my IP for me
}

void GetMyIP_Windows_Linux_IPV4And6( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	int idx=0;
	char ac[ 80 ];
	int err = gethostname( ac, sizeof( ac ) );
	RakAssert(err != -1);
	
	struct addrinfo hints;
	struct addrinfo *servinfo=0, *aip;  // will point to the results
	PrepareAddrInfoHints2(&hints);
	getaddrinfo(ac, "", &hints, &servinfo);

	for (idx=0, aip = servinfo; aip != NULL && idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; aip = aip->ai_next, idx++)
	{
		if (aip->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv4,sizeof(sockaddr_in));
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv6,sizeof(sockaddr_in6));
		}

	}

	freeaddrinfo(servinfo); // free the linked-list
	
	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}
}

#else

#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32__)
#include <netdb.h>
#endif
void GetMyIP_Windows_Linux_IPV4( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	int idx = 0;

	int rns2Socket = socket__(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (rns2Socket >= 0)
	{
		// A connected socket is required to get the true ip address
		// otherwise we are likely to just get 127.0.0.1
		// Connect to the public google DNS server
		sockaddr_in saGoogle;
		memset(&saGoogle,0,sizeof(sockaddr_in));
		saGoogle.sin_family = AF_INET;
		saGoogle.sin_addr.s_addr=inet_addr__("8.8.8.8");
		saGoogle.sin_port = htons(53);
		connect__(rns2Socket, (const sockaddr*) &saGoogle, sizeof(saGoogle));

		sockaddr_in sa;
		memset(&sa,0,sizeof(sockaddr_in));
		socklen_t len = sizeof(sa);
		getsockname__(rns2Socket, (sockaddr*)&sa, &len);

		addresses[idx].address.addr4.sin_addr.s_addr=sa.sin_addr.s_addr;
		idx++;

		// Disconnect the socket after getting the address
		memset(&saGoogle,0,sizeof(sockaddr_in));
		connect__(rns2Socket, (const sockaddr*) &saGoogle, sizeof(saGoogle));
		closesocket__(rns2Socket);
	}

	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}
}

#endif // RAKNET_SUPPORT_IPV6==1


void GetMyIP_Windows_Linux( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	#if RAKNET_SUPPORT_IPV6==1
		GetMyIP_Windows_Linux_IPV4And6(addresses);
	#else
		GetMyIP_Windows_Linux_IPV4(addresses);
	#endif
}


#endif // Windows and Linux

#endif // file header

#endif // #ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS
