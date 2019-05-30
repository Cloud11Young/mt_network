#ifndef EVMAP_H_INCLUDED_
#define EVMAP_H_INCLUDED_

struct event_map_entry
{
/*	struct {                                      
		struct event_map_entry *hte_next;                      
	}map_node;
*/
	HT_ENTRY(event_map_entry) map_node;
	
	evutil_socket_t fd;
	union { /* This is a union in case we need to make more things that can
			   be in the hashtable. */
		struct evmap_io evmap_io;
	} ent;
};

struct event_io_map
{
	 /* The hash table itself. */                                        
    struct event_map_entry **hth_table;                                            
    /* How long is the hash table? */                                   
    unsigned hth_table_length;                                          
    /* How many elements does the table contain? */                     
    unsigned hth_n_entries;                                             
    /* How many elements will we allow in the table before resizing it? */ 
    unsigned hth_load_limit;                                            
    /* Position of hth_table_length in the primes table. */             
    int hth_prime_idx;    
}

#define HT_SET_HASH_(elm, field, hashfn)	\
	((void)0)

#define HT_BUCKET_(head, field, elm, hashfn)				\
	((head)->hth_table[HT_ELT_HASH_(elm,field,hashfn) % head->hth_table_length])

 static inline struct event_map_entry **                                          
  event_io_map_HT_FIND_P_(struct event_io_map *head, struct event_map_entry *elm)		
  {                                                                     
    struct event_map_entry **p;                                                    
    if (!head->hth_table)                                               
      return NULL;                                                      
//    p = &HT_BUCKET_(head, field, elm, hashfn);	
	p = &((head)->hth_table[HT_ELT_HASH_(elm,map_node,hashfn) % head->hth_table_length])
	
    while (*p) {                                                        
      if (eqfn(*p, elm))                                                
        return p;                                                       
      p = &(*p)->map_node.hte_next;                                        
    }                                                                   
    return p;                                                           
  }  

int event_io_map_HT_GROW(struct event_io_map *head, unsigned size)                      
  {                                                                     
    unsigned new_len, new_load_limit;                                   
    int prime_idx;                                                      
    struct type **new_table;                                            
    if (head->hth_prime_idx == (int)event_io_map_N_PRIMES - 1)                
      return 0;                                                         
    if (head->hth_load_limit > size)                                    
      return 0;                                                         
    prime_idx = head->hth_prime_idx;                                    
    do {                                                                
      new_len = event_io_map_PRIMES[++prime_idx];                             
      new_load_limit = (unsigned)(load*new_len);                        
    } while (new_load_limit <= size &&                                  
             prime_idx < (int)event_io_map_N_PRIMES);                         
    if ((new_table = mallocfn(new_len*sizeof(struct type*)))) {         
      unsigned b;                                                       
      memset(new_table, 0, new_len*sizeof(struct type*));               
      for (b = 0; b < head->hth_table_length; ++b) {                    
        struct type *elm, *next;                                        
        unsigned b2;                                                    
        elm = head->hth_table[b];                                       
        while (elm) {                                                   
          next = elm->field.hte_next;                                   
          b2 = HT_ELT_HASH_(elm, field, hashfn) % new_len;              
          elm->field.hte_next = new_table[b2];                          
          new_table[b2] = elm;                                          
          elm = next;                                                   
        }                                                               
      }                                                                 
      if (head->hth_table)                                              
        freefn(head->hth_table);   
	
      head->hth_table = new_table;                                      
    } else {                                                            
      unsigned b, b2;                                                   
      new_table = reallocfn(head->hth_table, new_len*sizeof(struct type*)); 
      if (!new_table) return -1;                                        
      memset(new_table + head->hth_table_length, 0,                     
             (new_len - head->hth_table_length)*sizeof(struct type*));  
      for (b=0; b < head->hth_table_length; ++b) {                      
        struct type *e, **pE;                                           
        for (pE = &new_table[b], e = *pE; e != NULL; e = *pE) {         
          b2 = HT_ELT_HASH_(e, field, hashfn) % new_len;                
          if (b2 == b) {                                                
            pE = &e->field.hte_next;                                    
          } else {                                                      
            *pE = e->field.hte_next;                                    
            e->field.hte_next = new_table[b2];                          
            new_table[b2] = e;                                          
          }                                                             
        }                                                               
      }                                                                 
      head->hth_table = new_table;                                      
    }                                                                   
    head->hth_table_length = new_len;                                   
    head->hth_prime_idx = prime_idx;                                    
    head->hth_load_limit = new_load_limit;                              
    return 0;                                                           
  }                                    
  
#define HT_FOI_INSERT_(field, head, elm, newent, var)       \
  {                                                         \
    HT_SET_HASHVAL_(newent, field, (elm)->field.hte_hash);  \
    newent->field.hte_next = NULL;                          \
    *var = newent;                                          \
    ++((head)->hth_n_entries);                              \
  }
  
#define HT_FIND_OR_INSERT_(event_io_map, map_node, hashsocket, io, event_map_entry, elm, ptr, y, n) 
  {                                                                     
    struct event_io_map *ptr_head_ = io;                                    
    struct event_map_entry **ptr;  
	
    if (!ptr_head_->hth_table || ptr_head_->hth_n_entries >= ptr_head_->hth_load_limit)      
      event_io_map_HT_GROW(ptr_head_, ptr_head_->hth_n_entries+1);        
 //   HT_SET_HASH_((elm), map_node, hashsocket);  
 
    ptr = event_io_map_HT_FIND_P_(ptr_head_, (&key_));  
	
    if (*ptr) {                                                         							
		ent_ = *ptr;				                                                                
    } else {                                                            
        ent_ = mm_calloc(1,sizeof(struct event_map_entry)+fdinfo_len); 
		if (EVUTIL_UNLIKELY(ent_ == NULL))		
		    return (-1);			
		ent_->fd = fd;				
		(evmap_io_init)(&ent_->ent.evmap_io);			
		HT_FOI_INSERT_(map_node, io, &key_, ent_, ptr);                                              
    }                                                                   
  }
  
#define GET_IO_SLOT_AND_CTOR(ctx, io, fd, evmap_io, evmap_io_init, fdinfo_len)	\
	do {								\
		struct event_map_entry key_, *ent_;			\
		key_.fd = fd;						\
		HT_FIND_OR_INSERT_(event_io_map, map_node, hashsocket, io, \
		    event_map_entry, &key_, ptr,			\
		    {							\
			    ent_ = *ptr;				\
		    },							\
		    {							\
			    ent_ = mm_calloc(1,sizeof(struct event_map_entry)+fdinfo_len); \
			    if (EVUTIL_UNLIKELY(ent_ == NULL))		\
				    return (-1);			\
			    ent_->fd = fd;				\
			    (evmap_io_init)(&ent_->ent.evmap_io);			\
			    HT_FOI_INSERT_(map_node, map, &key_, ent_, ptr) \
				});					\
		(ctx) = &ent_->ent.evmap_io;					\
	} while (0)

#endif