
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SLAB_H_INCLUDED_
#define _NGX_SLAB_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_slab_page_s  ngx_slab_page_t;

struct ngx_slab_page_s {
    uintptr_t         slab; // SMALL：分配块的大小所对应位移  EXACT：存储在该页中的块的分配信息 BIG：存储在该页中的块的分配信息 PAGE：
    ngx_slab_page_t  *next; // 下一页
    uintptr_t         prev; // 上一页
};


typedef struct {
    ngx_uint_t        total; // 当前 slot 的总内存大小
    ngx_uint_t        used;  // 当前 slot 的使用次数

    ngx_uint_t        reqs;  // 当前 slot 的请求次数
    ngx_uint_t        fails; // 当前 slot 的失败次数
} ngx_slab_stat_t; // slot 的状态


typedef struct {   // slab 结构
    ngx_shmtx_sh_t    lock;       // 共享内存锁

    size_t            min_size;   // 分配空间的最小值
    size_t            min_shift;  // 该最小值对应的位移数

    ngx_slab_page_t  *pages;       // 该 slab 中的所有页的数组
    ngx_slab_page_t  *last;        // 指向最后一页
    ngx_slab_page_t   free;        // 空闲页链表

    ngx_slab_stat_t  *stats;       // 每个 slot 的状态
    ngx_uint_t        pfree;       // 空闲页的数量

    u_char           *start;       // 分配地址开始地址
    u_char           *end;         // 可分配内存的最后字节

    ngx_shmtx_t       mutex;       // 互斥锁

    u_char           *log_ctx;     // 日志信息
    u_char            zero; 

    unsigned          log_nomem:1;

    void             *data;
    void             *addr;
} ngx_slab_pool_t;


void ngx_slab_init(ngx_slab_pool_t *pool);
void *ngx_slab_alloc(ngx_slab_pool_t *pool, size_t size);
void *ngx_slab_alloc_locked(ngx_slab_pool_t *pool, size_t size);
void *ngx_slab_calloc(ngx_slab_pool_t *pool, size_t size);
void *ngx_slab_calloc_locked(ngx_slab_pool_t *pool, size_t size);
void ngx_slab_free(ngx_slab_pool_t *pool, void *p);
void ngx_slab_free_locked(ngx_slab_pool_t *pool, void *p);


#endif /* _NGX_SLAB_H_INCLUDED_ */
