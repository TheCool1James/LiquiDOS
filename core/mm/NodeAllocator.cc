/**
 * Copyright (c) 2019 The cxkernel Authors. All rights reserved.
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 * 
 * @file   pmm_node.cc
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * @date   Created on June 06 2019, 11:49 AM
 */

#define __MODULE__ "NDALC"

#include <stdlib.h>
#include <linked_list.h>
#include <panic.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "arch/x86/arch_utils.h"
#include "system.h"
#include "core/vm.h"
#include "arch/interface.h"
#include "core/pmm_node.h"

#define PAGE_IS_IN_ARENA(p, a) \
        (((virt_t)(p) >= (virt_t)(a)->pages) && (virt_t)(p) < (virt_t)(a)->pages+(a)->size/ARCH_PAGE_SIZE*sizeof(page_t))
#define ADDRESS_FROM_ARENA(p, a) \
        ((a)->base + (phys_t)(((virt_t)(p) - (virt_t)((a)->pages)) / sizeof(page_t)) * (ARCH_PAGE_SIZE))
#define ADDRESS_IS_IN_ARENA(p, a) \
        (((phys_t)(p) >= (a)->base) && ((phys_t)(p) < (a)->base+(a)->size*(ARCH_PAGE_SIZE)))
#define INDEX_FROM_ADDRESS(p, a) (((phys_t)(p)-(a)-> base) / (ARCH_PAGE_SIZE))
#define ADDRESS_FROM_INDEX(i, a) ((phys_t)((i)*(ARCH_PAGE_SIZE)+(a)->base))

namespace pmm
{
    static PmmNode __internal_PmmNodeAllocator;

    phys_t PmmNode::PageToPhysical(uintptr_t page)
    {
        pmm_arena_t* arena;
        foreach_llist_entry(arena, node, arena_list.next)
        {
            if(PAGE_IS_IN_ARENA(page, arena))
                return ADDRESS_FROM_ARENA(page, arena);
        }
        return 0;
    }

    void PmmNode::AddArena(pmm_arena_t* arena, bitmap_t* bt)
    {
        OS_LOG("Add arena from 0x%08lX to 0x%08lX\n", arena->base, (arena->base+arena->size*ARCH_PAGE_SIZE));
        INIT_LLIST(&arena->node);
        INIT_LLIST(&arena->free_list);
        arena->free = 0;
        pmm_arena_t* an = NULL;
        if(arena_list.next != NULL)
        {
            foreach_llist_entry(an, node, arena_list.next)
            {
                if(an->priority > arena->priority)
                    break;
            }
        }
        if(an == NULL)
            list_append(list_end(&arena_list), &arena->node);
        else
            list_insert(&an->node, &arena->node);
        // Prepare page array
        memset(arena->pages, 0, arena->size*sizeof(page_t));
        list_node_t* prev = &arena->free_list;
        for(size_t i = 0; i < arena->size; i++)
        {
            INIT_LLIST(&arena->pages[i].node);
            list_append(prev, &arena->pages[i].node);
            prev = &arena->pages[i].node;
            arena->free++;
        }
        // Go through bitmap
        if(bt != NULL)
        {
            for(size_t i = arena -> base / ARCH_PAGE_SIZE; i < arena -> size; i++)
            {
                if(bitmap_tstbit(bt -> bitmap, i))
                    list_remove(&arena->pages[i].node);
            }
        }
        OS_PRN("Loaded 0x%X free page_t entries\n", list_count(arena->free_list.next));
    }

    size_t PmmNode::Allocate(size_t cnt, uintptr_t p)
    {
        list_node_t* pages = (list_node_t*) p;
        size_t ret = 0;
        if(cnt == 0) return 0;
        pmm_arena_t* arena;
        foreach_llist_entry(arena, node, arena_list.next)
        {
            while(ret < cnt)
            {
                page_t* pg = LIST_ENTRY(
                    list_remove(list_end(&arena->free_list)),
                    page_t, node
                );
                if(!pg) return ret;
                pg->flags |= CX_PAGE_OCCUPIED;
                arena->free--;
                list_append(list_end(pages), &pg->node);
                ret++;
            }
        }
        return ret;
    }

    size_t PmmNode::AllocateSingle(uintptr_t p)
    {
        return Allocate(1, p);
    }

    size_t PmmNode::AllocateContiguous(size_t cnt, uintptr_t p)
    {
        list_node_t* pages = (list_node_t*) p;
        size_t ret = 0;
        if(cnt == 0) return 0;

        // Search all arenas...
        size_t r1 = 0;
        int idx = 0;
        pmm_arena_t* farena = NULL;

        // Start brute-force search...
        pmm_arena_t* arena;
        foreach_llist_entry(arena, node, arena_list.next)
        {
            size_t r2 = 0;
            for(int i = 0; i < (int) arena -> size; i++)
            {
                if(CHECK_MFLG(arena -> pages[i].flags, CX_PAGE_OCCUPIED))
                    r2++;
                else
                {
                    if(r2 <= r1 && r2 >= cnt)
                        idx = i, r1 = r2, farena = arena;
                    if(r2 == cnt)
                        goto done;
                    r2 = 0;
                }
            }
        }

done:
        // Allocate the run!
        if(r1 != cnt || !farena) return 0;
        for(int i = idx; i < (int) r1; i++)
        {
            page_t* pg = &farena -> pages[i];
            list_remove(&pg->node);
            pg->flags |= CX_PAGE_OCCUPIED;
            farena->free--;
            list_append(list_end(pages), &pg->node);
            ret++;
        }

        OS_DBG("Contiguous allocation at 0x%lX for %d pages\n", PmmNode::PageToPhysical((uintptr_t)(&farena -> pages[idx])), ret);

        return ret;
    }

    size_t PmmNode::Free(uintptr_t p)
    {
        list_node_t* pages = (list_node_t*) p;
        size_t ret = 0;
        while(pages->next != NULL)
        {
            page_t* pg = LIST_ENTRY(list_remove(list_end(pages)), TYPEOF(page_t), node);
            pmm_arena_t* arena;
            foreach_llist_entry(arena, node, arena_list.next)
            {
                if(PAGE_IS_IN_ARENA(pg, arena))
                {
                    pg->flags &= ~CX_PAGE_OCCUPIED;
                    list_append(&arena->free_list, &pg->node);
                    arena->free++;
                    ret++;
                    break;
                }
            }
        }
        return ret;
    }

    PhysicalAllocator* GetPmmNodeAllocator()
    {
        return static_cast<PhysicalAllocator*>(&__internal_PmmNodeAllocator);
    }
} // namespace
