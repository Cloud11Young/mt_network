#ifndef BUFFEREVENT_SOCKET_H_INCLUDED_
#define BUFFEREVENT_SOCKET_H_INCLUDED_

class bufferevent_private : public bufferevent
{
public:
	virtual int (*enable)(struct bufferevent *, short);
	virtual int (*disable)(struct bufferevent *, short);
	virtual void (*unlink)(struct bufferevent *);
	virtual void (*destruct)(struct bufferevent *);
	virtual int (*adj_timeouts)(struct bufferevent *);
	virtual int (*flush)(struct bufferevent *, short, enum bufferevent_flush_mode);
	virtual int (*ctrl)(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

	struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options);
	int bufferevent_socket_connect(struct bufferevent *, const struct sockaddr *, int);
	int bufferevent_socket_connect_hostname(struct bufferevent *, struct evdns_base *, int, const char *, int);
	int bufferevent_socket_get_dns_error(struct bufferevent *bev);

	int bufferevent_priority_set(struct bufferevent *bufev, int pri);
	int bufferevent_get_priority(const struct bufferevent *bufev);
	
	void bufferevent_lock(struct bufferevent *bufev);
	void bufferevent_unlock(struct bufferevent *bufev);

	void bufferevent_incref(struct bufferevent *bufev);
	int bufferevent_decref(struct bufferevent *bufev);

	int bufferevent_flush(struct bufferevent *bufev, short iotype, enum bufferevent_flush_mode mode);
	void bufferevent_trigger(struct bufferevent *bufev, short iotype, int options);
	void bufferevent_trigger_event(struct bufferevent *bufev, short what, int options);
	
	int bufferevent_set_max_single_read(struct bufferevent *bev, size_t size);
	int bufferevent_set_max_single_write(struct bufferevent *bev, size_t size);
	ev_ssize_t bufferevent_get_max_single_read(struct bufferevent *bev);
	ev_ssize_t bufferevent_get_max_single_write(struct bufferevent *bev);
	ev_ssize_t bufferevent_get_max_to_read(struct bufferevent *bev);
	ev_ssize_t bufferevent_get_max_to_write(struct bufferevent *bev);
	
	
public:
	int bufferevent_init_common_(struct bufferevent_private *, struct event_base *, const struct bufferevent_ops *, enum bufferevent_options options);
	
	void bufferevent_suspend_read_(struct bufferevent *bufev, bufferevent_suspend_flags what);
	void bufferevent_unsuspend_read_(struct bufferevent *bufev, bufferevent_suspend_flags what);
	void bufferevent_suspend_write_(struct bufferevent *bufev, bufferevent_suspend_flags what);
	void bufferevent_unsuspend_write_(struct bufferevent *bufev, bufferevent_suspend_flags what);
	
	int bufferevent_disable_hard_(struct bufferevent *bufev, short event);
	
	void bufferevent_incref_and_lock_(struct bufferevent *bufev);
	int bufferevent_decref_and_unlock_(struct bufferevent *bufev);
	
	void bufferevent_run_readcb_(struct bufferevent *bufev, int options);
	void bufferevent_run_writecb_(struct bufferevent *bufev, int options);
	void bufferevent_run_eventcb_(struct bufferevent *bufev, short what, int options);

	enum bufferevent_options bufferevent_get_options_(struct bufferevent *bev);
	const struct sockaddr* bufferevent_socket_get_conn_address_(struct bufferevent *bev);
	void bufferevent_socket_set_conn_address_fd_(struct bufferevent *bev, int fd);
	void bufferevent_socket_set_conn_address_(struct bufferevent *bev, struct sockaddr *addr, size_t addrlen);

private:
	static inline void bufferevent_trigger_nolock_(struct bufferevent *bufev, short iotype, int options);

public:
	struct evbuffer_cb_entry* read_watermarks_cb;
	unsigned own_lock;
	unsigned readcb_pending;
	unsigned writecb_pending;
	unsigned connecting;
	unsigned connection_refused;
	short eventcb_pending;
	bufferevent_suspend_flags read_suspended;
	bufferevent_suspend_flags write_suspended;
	
	int errno_pending;
	int dns_error;
	struct event_callback deferred;
	bufferevent_options options;
	
	int refcnt;
	void* lock;
	
	ev_ssize_t max_single_read;
	ev_ssize_t max_single_write;
	
	struct bufferevent_rate_limit *rate_limiting;
	union {
		struct sockaddr_in6 in6;
		struct sockaddr_in in;
	} conn_address;
	struct evdns_getaddrinfo_request *dns_request;
};

#endif