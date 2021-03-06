/**
 * @file plic.c
 * @note 访问 MMIO 内存必须使用 volatile 关键字，详情见 **Effectice Modern C++**
 *       Item 40: Use std::atomic for concurrency, volatile for special memory
 */
#include <plic.h>
#include <mm.h>
void plic_init()
{
    plic_set_threshold(0);
    plic_enable_interuupt(0xa);
    plic_set_priority(0xa, 1);
}

void plic_enable_interuupt(uint32_t id)
{
    volatile uint32_t *plic_enable_address = (volatile uint32_t *)PLIC_ENABLE_ADDRESS;
    *plic_enable_address |= (1 << id);
}

void plic_set_priority(uint32_t id, uint8_t priority)
{
    ((volatile uint32_t*)PLIC_PRIORITY_ADDRESS)[id] = priority & 7;
}

void plic_set_threshold(uint8_t threshold)
{
    *(volatile uint32_t*)PLIC_THRESHOLD_ADDRESS = threshold & 7;
}

/**
 * @brief 获取外中断号
 *
 * @return 最高优先级的待处理中断号，不存在则返回 0
 */
uint32_t plic_claim()
{
    return *(volatile uint32_t*)PLIC_CLAIM_ADDRESS;
}

void plic_complete(uint32_t id)
{
    *(volatile uint32_t*)PLIC_COMPLETE_ADDRESS = id;
}

/*
 *
 * @brief 返回待处理终端向量
 *
 * @note 可以先 enable 再 claim 来清空 pending
 */
uint64_t plic_pending()
{
    return *(volatile uint64_t*)PLIC_PENDING_ADDRESS;
}

/**
 * @brief 判断某终端是否带处理
 */
int plic_is_pending(uint32_t id)
{
    uint64_t pending = plic_pending();
    volatile uint32_t *p = (volatile uint32_t*)&pending;
    return p[id/32] & (id % 32);
}
