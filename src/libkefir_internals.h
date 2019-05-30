/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019 Netronome Systems, Inc. */

#ifndef LIBKEFIR_INTERNALS_H
#define LIBKEFIR_INTERNALS_H

#include <stdint.h>

#include <linux/bpf.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#include "libkefir.h"
#include "libkefir_error.h"
#include "list.h"

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef array_size
#define array_size(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef offset_of
#define offset_of(type, member) ((size_t)&((type *)0)->member)
#endif

#ifndef container_of
#define container_of(pointer, type, member)	\
	((type *)(pointer) - offset_of(type, member))
#endif

#ifndef sizeof_member
#define sizeof_member(type, member) sizeof(*(&((type *)0)->member))
#endif

#ifndef __printf
#define __printf(a, b) __attribute__((format(printf, a, b)))
#endif

#define KEFIR_MAX_MATCH_PER_RULE	5
#define KEFIR_CPROG_INIT_BUFLEN		8192

enum comp_operator {
	OPER_EQUAL,
	OPER_LT,
	OPER_LEQ,
	OPER_GT,
	OPER_GEQ,
};

enum action_code {
	ACTION_CODE_DROP,
	ACTION_CODE_PASS,
};

#define KEFIR_MATCH_FLAG_IPV4	bit(0)
#define KEFIR_MATCH_FLAG_IPV6	bit(1)

enum match_type {
	KEFIR_MATCH_TYPE_UNSPEC = 0,

	KEFIR_MATCH_TYPE_ETHER_SRC,
	KEFIR_MATCH_TYPE_ETHER_DST,
	KEFIR_MATCH_TYPE_ETHER_ANY,	/* Either source or destination */
	KEFIR_MATCH_TYPE_ETHER_PROTO,

	KEFIR_MATCH_TYPE_IP_4_SRC,
	KEFIR_MATCH_TYPE_IP_4_DST,
	KEFIR_MATCH_TYPE_IP_4_ANY,
	KEFIR_MATCH_TYPE_IP_4_TOS,
	KEFIR_MATCH_TYPE_IP_4_TTL,
	KEFIR_MATCH_TYPE_IP_4_FLAGS,
	KEFIR_MATCH_TYPE_IP_4_L4PROTO,
	KEFIR_MATCH_TYPE_IP_4_L4DATA,
	KEFIR_MATCH_TYPE_IP_4_L4PORT_SRC,
	KEFIR_MATCH_TYPE_IP_4_L4PORT_DST,
	KEFIR_MATCH_TYPE_IP_4_L4PORT_ANY,
	KEFIR_MATCH_TYPE_IP_4_SPI,
	KEFIR_MATCH_TYPE_IP_4_TCP_FLAGS,

	KEFIR_MATCH_TYPE_IP_6_SRC,
	KEFIR_MATCH_TYPE_IP_6_DST,
	KEFIR_MATCH_TYPE_IP_6_ANY,
	KEFIR_MATCH_TYPE_IP_6_TOS,	/* Actually TCLASS, traffic class */
	KEFIR_MATCH_TYPE_IP_6_TTL,
	KEFIR_MATCH_TYPE_IP_6_FLAGS,
	KEFIR_MATCH_TYPE_IP_6_L4PROTO,
	KEFIR_MATCH_TYPE_IP_6_L4DATA,
	KEFIR_MATCH_TYPE_IP_6_L4PORT_SRC,
	KEFIR_MATCH_TYPE_IP_6_L4PORT_DST,
	KEFIR_MATCH_TYPE_IP_6_L4PORT_ANY,
	KEFIR_MATCH_TYPE_IP_6_SPI,
	KEFIR_MATCH_TYPE_IP_6_TCP_FLAGS,

	KEFIR_MATCH_TYPE_IP_ANY_SRC,
	KEFIR_MATCH_TYPE_IP_ANY_DST,
	KEFIR_MATCH_TYPE_IP_ANY_ANY,
	KEFIR_MATCH_TYPE_IP_ANY_TOS,
	KEFIR_MATCH_TYPE_IP_ANY_TTL,
	KEFIR_MATCH_TYPE_IP_ANY_FLAGS,
	KEFIR_MATCH_TYPE_IP_ANY_L4PROTO,
	KEFIR_MATCH_TYPE_IP_ANY_L4DATA,
	KEFIR_MATCH_TYPE_IP_ANY_L4PORT_SRC,
	KEFIR_MATCH_TYPE_IP_ANY_L4PORT_DST,
	KEFIR_MATCH_TYPE_IP_ANY_L4PORT_ANY,
	KEFIR_MATCH_TYPE_IP_ANY_SPI,
	KEFIR_MATCH_TYPE_IP_ANY_TCP_FLAGS,

	KEFIR_MATCH_TYPE_VLAN_ID,
	KEFIR_MATCH_TYPE_VLAN_PRIO,
	KEFIR_MATCH_TYPE_VLAN_ETHERTYPE,

	KEFIR_MATCH_TYPE_CVLAN_ID,
	KEFIR_MATCH_TYPE_CVLAN_PRIO,
	KEFIR_MATCH_TYPE_CVLAN_ETHERTYPE,

	KEFIR_MATCH_TYPE_MPLS_LABEL,
	KEFIR_MATCH_TYPE_MPLS_TC,
	KEFIR_MATCH_TYPE_MPLS_BOS,
	KEFIR_MATCH_TYPE_MPLS_TTL,

	KEFIR_MATCH_TYPE_ICMP_TYPE,
	KEFIR_MATCH_TYPE_ICMP_CODE,

	KEFIR_MATCH_TYPE_ARP_TIP,
	KEFIR_MATCH_TYPE_ARP_SIP,
	KEFIR_MATCH_TYPE_ARP_OP,
	KEFIR_MATCH_TYPE_ARP_THA,
	KEFIR_MATCH_TYPE_ARP_SHA,

	KEFIR_MATCH_TYPE_ENC_KEY_ID,
	KEFIR_MATCH_TYPE_ENC_DST_ID,
	KEFIR_MATCH_TYPE_ENC_SRC_ID,
	KEFIR_MATCH_TYPE_ENC_DST_PORT,
	KEFIR_MATCH_TYPE_ENC_TOS,
	KEFIR_MATCH_TYPE_ENC_TTL,

	KEFIR_MATCH_TYPE_GENEVE_OPTIONS,

	__KEFIR_MAX_MATCH_TYPE
};

enum value_format {
	KEFIR_VAL_FMT_BIT,	/* MPLS BoS */
	KEFIR_VAL_FMT_UINT3,	/* VLAN prio, MPLS TC */
	KEFIR_VAL_FMT_UINT6,	/* IPv4 ToS */
	KEFIR_VAL_FMT_UINT8,
	KEFIR_VAL_FMT_UINT12,	/* VLAN ID, TCP flags */
	KEFIR_VAL_FMT_UINT16,
	KEFIR_VAL_FMT_UINT20,	/* MPLS label */
	KEFIR_VAL_FMT_UINT32,
	KEFIR_VAL_FMT_MAC_ADDR,
	KEFIR_VAL_FMT_IPV4_ADDR,
	KEFIR_VAL_FMT_IPV6_ADDR,
};

struct kefir_value {
	union {
		struct ether_addr	eth;
		struct in6_addr		ipv6;
		struct in_addr		ipv4;
		uint32_t		u32;
		uint16_t		u16;
		uint8_t			u8;
		uint8_t			raw[sizeof(struct in6_addr)];
	}			data;
	enum value_format	format;
};

#define MATCH_FLAGS_USE_MASK	bit(0)
#define MATCH_FLAGS_USE_RANGE	bit(1)

/*
 * - A type for the match, indicating the semantics of the data to match
 *   (semantics needed for optimizations).
 * - An operator to indicate what type of comparison should be performed
 *   (equality, or other arithmetic or logic operator).
 * - A value to match. If matching against a range of values, this should be
 *   the minimum value of the range.
 * - A maximum value to match, for ranges.
 * - One mask to apply to the field.
 * - Option flags, indicating for example that the match is against a range of
 *   values instead of a single value.
 */
struct kefir_match {
	enum match_type		match_type;
	enum comp_operator	comp_operator;
	struct kefir_value	value;
	uint8_t			max_value[16];
	uint8_t			mask[16];
	uint64_t		flags;
};

struct kefir_rule {
	struct kefir_match matches[KEFIR_MAX_MATCH_PER_RULE];
	enum action_code action;
};

struct kefir_filter {
	struct list *rules;
};

/*
 * kefir_cprog
 */

#define OPT_FLAGS_NEED_ETHER	bit(0)
#define OPT_FLAGS_NEED_IPV4	bit(1)
#define OPT_FLAGS_NEED_IPV6	bit(2)
#define OPT_FLAGS_NEED_UDP	bit(3)
#define OPT_FLAGS_NEED_TCP	bit(4)
#define OPT_FLAGS_NEED_SCTP	bit(5)
#define OPT_FLAGS_NEED_L4	\
	(OPT_FLAGS_NEED_UDP + OPT_FLAGS_NEED_TCP + OPT_FLAGS_NEED_SCTP)
#define OPT_FLAGS_USE_MASKS	bit(6)
#define OPT_FLAGS_INLINE_FUNC	bit(7)
#define OPT_FLAGS_CLONE_FILTER	bit(8)
#define OPT_FLAGS_NO_VLAN	bit(9)
#define OPT_FLAGS_USE_PRINTK	bit(10)

struct kefir_cprog_options {
	uint64_t		flags;
	unsigned int		nb_matches;
	enum kefir_cprog_target	target;
	uint8_t	req_helpers[__BPF_FUNC_MAX_ID / 8 + 1];
};

struct kefir_cprog {
	const kefir_filter		*filter;
	struct kefir_cprog_options	options;
};

/*
 * Shared functions
 */

int kefir_add_rule_to_filter(kefir_filter *filter, struct kefir_rule *rule,
			     ssize_t index);

#endif /* LIBKEFIR_INTERNALS_H */
