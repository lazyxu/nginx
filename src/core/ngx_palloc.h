
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s { // 内存池中的清理函数
    ngx_pool_cleanup_pt   handler; // 函数指针，指向一个函数，参数为 data
    void                 *data;
    ngx_pool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s { // 内存池中的大内存块
    ngx_pool_large_t     *next;
    void                 *alloc;
};


typedef struct {
    u_char               *last;   // 指向上次分配到的数据字节数，下次从这里开始分配
    u_char               *end;    // 指向该内存池节点的最后一个字节
    ngx_pool_t           *next;   // 只想下一个内存池节点
    ngx_uint_t            failed; // 当前内存池节点分配内存失败次数
} ngx_pool_data_t;  // 记录某个内存池节点的分配信息


struct ngx_pool_s { // 内存池结构，分为 头部信息 和 内存池节点的分配信息，头部信息只有第一个节点才有
    ngx_pool_data_t       d;       // 该内存池节点的分配信息
    size_t                max;     // 该内存池节点存储数据的最大字节数
    ngx_pool_t           *current; // 指向开始分配内存的内存池节点，加快内存分配速度
    ngx_chain_t          *chain;   // (?)该内存池节点的数据块，以链表的方式存储，创建该内存池时初始化
    ngx_pool_large_t     *large;   // 该内存池中的大内存块，需要用时再向操作系统申请
    ngx_pool_cleanup_t   *cleanup; // 用于清理一些保存在内存池节点中的特殊东西，比如临时文件，模块，环境变量，http连接，tcp连接等等
    ngx_log_t            *log;     // 日志信息
};


typedef struct {
    ngx_fd_t              fd;
    u_char               *name;
    ngx_log_t            *log;
} ngx_pool_cleanup_file_t;


void *ngx_alloc(size_t size, ngx_log_t *log);
void *ngx_calloc(size_t size, ngx_log_t *log);

ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);
void ngx_reset_pool(ngx_pool_t *pool);

void *ngx_palloc(ngx_pool_t *pool, size_t size);
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void *data);
void ngx_pool_delete_file(void *data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */
