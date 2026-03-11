#include "bitmap.h"
#include "stdlib.h"
#include "stdio.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, const int length)
{
    this->bitmap = bitmap;
    this->length = length;

    int bytes = ceil(length, 8);

    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

// 全局变量，用于选择算法类型，默认为首次适应算法
AllocateAlgorithm allocAlgorithm = FIRST_FIT;

// 修改allocation方法以实现不同的分配算法
int BitMap::allocate(const int count)
{
    if (count == 0)
        return -1;

    int index, empty, start;
    
    // 首次适应算法（First-Fit）
    if (allocAlgorithm == FIRST_FIT) {
        index = 0;
        while (index < length)
        {
            // 越过已经分配的资源
            while (index < length && get(index))
                ++index;

            // 不存在连续的count个资源
            if (index == length)
                return -1;

            // 找到1个未分配的资源
            // 检查是否存在从index开始的连续count个资源
            empty = 0;
            start = index;
            while ((index < length) && (!get(index)) && (empty < count))
            {
                ++empty;
                ++index;
            }

            // 存在连续的count个资源
            if (empty == count)
            {
                for (int i = 0; i < count; ++i)
                {
                    set(start + i, true);
                }

                return start;
            }
        }
    }
    // 最佳适应算法（Best-Fit）
    else if (allocAlgorithm == BEST_FIT) {
        int bestStart = -1;  // 最佳起始位置
        int bestSize = length + 1;  // 最小空闲块大小，初始化为比可能的最大值还大
        
        index = 0;
        while (index < length) {
            // 越过已经分配的资源
            while (index < length && get(index))
                ++index;
                
            // 找到一个空闲区域的起始位置
            if (index < length) {
                start = index;
                empty = 0;
                
                // 计算当前空闲区域的大小
                while ((index < length) && (!get(index))) {
                    ++empty;
                    ++index;
                }
                
                // 如果空闲区域足够大且比当前最佳的更小，则更新最佳位置
                if (empty >= count && empty < bestSize) {
                    bestStart = start;
                    bestSize = empty;
                }
            }
        }
        
        // 如果找到了合适的空闲区域
        if (bestStart != -1) {
            for (int i = 0; i < count; ++i) {
                set(bestStart + i, true);
            }
            return bestStart;
        }
    }
    // 最坏适应算法（Worst-Fit）
    else if (allocAlgorithm == WORST_FIT) {
        int worstStart = -1;  // 最坏起始位置
        int worstSize = 0;    // 最大空闲块大小
        
        index = 0;
        while (index < length) {
            // 越过已经分配的资源
            while (index < length && get(index))
                ++index;
                
            // 找到一个空闲区域的起始位置
            if (index < length) {
                start = index;
                empty = 0;
                
                // 计算当前空闲区域的大小
                while ((index < length) && (!get(index))) {
                    ++empty;
                    ++index;
                }
                
                // 如果空闲区域足够大且比当前最大的还大，则更新最大位置
                if (empty >= count && empty > worstSize) {
                    worstStart = start;
                    worstSize = empty;
                }
            }
        }
        
        // 如果找到了合适的空闲区域
        if (worstStart != -1) {
            for (int i = 0; i < count; ++i) {
                set(worstStart + i, true);
            }
            return worstStart;
        }
    }

    return -1;  // 找不到合适的空闲区域
}

void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}

// 设置内存分配算法的方法
void setAllocateAlgorithm(AllocateAlgorithm alg) {
    allocAlgorithm = alg;
}