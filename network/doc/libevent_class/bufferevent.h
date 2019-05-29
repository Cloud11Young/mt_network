#ifndef BUFFEREVENT_H_INCLUDED_
#define BUFFEREVENT_H_INCLUDED_

enum bufferevent_options
{
	BEV_OPT_CLOSE_ON_FREE = (1<<0),
	BEV_OPT_THREADSAFE=(1<<1),
	BEV_OPT_DEFER_CALLBACKS=(1<<2),
	BEV_OPT_UNLOCKS_CALLBACKS = (1<<3)
};

struct event_watermark
{
	size_t low;
	size_t high;
};

class bufferevent
{
public:
	virtual int (*enable)(struct bufferevent *, short);
	virtual int (*disable)(struct bufferevent *, short);
	virtual void (*unlink)(struct bufferevent *);
	virtual void (*destruct)(struct bufferevent *);
	virtual int (*adj_timeouts)(struct bufferevent *);
	virtual int (*flush)(struct bufferevent *, short, enum bufferevent_flush_mode);
	virtual int (*ctrl)(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);
	virtual void bufferevent_free(struct bufferevent *bufev);
	
	void (*bufferevent_data_cb_read)(struct bufferevent *bev, void *ctx);
	void (*bufferevent_data_cb_write)(struct bufferevent *bev, void *ctx);
    void (*bufferevent_event_cb_error)(struct bufferevent *bev, short what, void *ctx);
	
	int bufferevent_base_set(struct event_base *base, struct bufferevent *bufev);
	struct event_base *bufferevent_get_base(struct bufferevent *bev);

	void bufferevent_setcb(struct bufferevent *bufev, bufferevent_data_cb readcb, bufferevent_data_cb writecb, bufferevent_event_cb eventcb, void *cbarg);
	void bufferevent_getcb(struct bufferevent *bufev, bufferevent_data_cb *readcb_ptr, bufferevent_data_cb *writecb_ptr, bufferevent_event_cb *eventcb_ptr, void **cbarg_ptr);

	int bufferevent_setfd(struct bufferevent *bufev, evutil_socket_t fd);
	evutil_socket_t bufferevent_getfd(struct bufferevent *bufev);

	struct bufferevent *bufferevent_get_underlying(struct bufferevent *bufev);

	int bufferevent_write(struct bufferevent *bufev, const void *data, size_t size);
	int bufferevent_write_buffer(struct bufferevent *bufev, struct evbuffer *buf);
	size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size);
	int bufferevent_read_buffer(struct bufferevent *bufev, struct evbuffer *buf);

	struct evbuffer *bufferevent_get_input(struct bufferevent *bufev);
	struct evbuffer *bufferevent_get_output(struct bufferevent *bufev);
	
	int bufferevent_enable(struct bufferevent *bufev, short event);
	int bufferevent_disable(struct bufferevent *bufev, short event);
	short bufferevent_get_enabled(struct bufferevent *bufev);
	int bufferevent_set_timeouts(struct bufferevent *bufev, const struct timeval *timeout_read, const struct timeval *timeout_write);
	
	void bufferevent_setwatermark(struct bufferevent *bufev, short events, size_t lowmark, size_t highmark);
	int bufferevent_getwatermark(struct bufferevent *bufev, short events, size_t *lowmark, size_t *highmark);

public:
	int bufferevent_enable_locking_(struct bufferevent *bufev, void *lock);
	int bufferevent_add_event_(struct event *ev, const struct timeval *tv);
	
	void bufferevent_init_generic_timeout_cbs_(struct bufferevent *bev);
	int bufferevent_generic_adj_timeouts_(struct bufferevent *bev);
	int bufferevent_generic_adj_existing_timeouts_(struct bufferevent *bev);

public: 
	struct event_base* ev_base;
	struct event ev_read;
	struct event ev_write;
	struct evbuffer* input;
	struct evbuffer* output;
	struct event_watermark wm_read;
	struct event_watermark wm_write;
	struct timeval timeout_read;
	struct timeval timeout_write;
	short enabled;
};
#endif