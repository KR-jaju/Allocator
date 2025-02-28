
#pragma once

#include <cstdint>
#include <cstddef>
#include <bitset>
#include <cmath>
#include <algorithm>

template <std::size_t leaf_size, std::size_t height>
class Allocator
{
public:
	Allocator(void *pool);
	void*	allocate(std::size_t size);
	void	free(void *ptr);
	void	dump() const;
private:
	using byte = unsigned char;
	using BlockID = int;
	struct Block
	{
		Block*	prev;
		Block*	next;
		std::size_t level;
	};
	byte*		pool;
	Block		blocks[1 << height];
	Block*		free_list[height + 1]; // per level free-list

	bool	isAllocated(BlockID block_id, std::size_t level) const;
	BlockID	getBlockID(Block const* block) const;
	BlockID	getBuddyID(Block const* block) const;
	Block*	getBlock(void* ptr);
	Block*	getBlock(BlockID block_id);
	Block const*	getBlock(BlockID block_id) const;
	BlockID	allocateBlock(std::size_t level);
};

template <std::size_t leaf_size, std::size_t height>
Allocator<leaf_size, height>::Allocator(void *pool)
	: pool(static_cast<byte*>(pool)), blocks(), free_list()
{
	Block*& free_list = this->free_list[0];

	free_list = this->getBlock(0); // 최상위 블럭만 할당 가능함.
	free_list->prev = free_list;
	free_list->next = free_list;
	free_list->level = 0;
}

template <std::size_t leaf_size, std::size_t height>
void	*Allocator<leaf_size, height>::allocate(std::size_t size) // size보다 큰 leaf_size * 2^n 중 가장 작은 n을 고름.
{
	std::size_t block_size = leaf_size;
	std::size_t	level = height;

	while (block_size < size)
	{
		block_size *= 2;
		level -= 1;
	}
	if (level < 0)
		return (nullptr);
	BlockID const block_id = this->allocateBlock(level);

	if (block_id == -1)
		return (nullptr);
	return (this->pool + block_id * leaf_size);
}

template <std::size_t leaf_size, std::size_t height>
void	Allocator<leaf_size, height>::free(void *ptr)
{
	Block*	block = this->getBlock(ptr);
	BlockID block_id = this->getBlockID(block);

	assert(block != nullptr); // block_id is valid (invalid pointer)
	assert(block->prev == nullptr && block->next == nullptr); // block is not in free_list (double free)
	while (block->level > 0)
	{
		Block*&	free_list = this->free_list[block->level];
		BlockID buddy_id = this->getBuddyID(block);
		Block*	buddy = this->getBlock(buddy_id);

		/*
		To merge,
			buddy must be the same level
			buddy must be in the free_list
		When not merge,
			add block in free_list
		When merge,
			pop buddy from free_list,
			try merge on parent level
		*/
		if (!(buddy->level == block->level && buddy->next != nullptr && buddy->prev != nullptr))
			break;
		if (buddy->next == buddy) // buddy -> buddy -> buddy .....
		{
			buddy->prev = nullptr;
			buddy->next = nullptr;
			free_list = nullptr;
		}
		else
		{
			Block* prev = buddy->prev;
			Block* next = buddy->next;

			buddy->prev = nullptr;
			buddy->next = nullptr;
			prev->next = next;
			next->prev = prev;
			if (free_list == buddy)
				free_list = next;
		}
		std::cout << block_id << ", " << buddy_id << " merged!" << std::endl;

		if (buddy_id < block_id)
		{
			std::swap(buddy_id, block_id);
			std::swap(buddy, block);
		}
		block->level -= 1; // parent block = (block or buddy);
		buddy->level = height + 1; // invalidate buddy(owned by block)
	}
	Block*&	free_list = this->free_list[block->level];

	if (free_list == nullptr) // block -> block -> block .....
	{
		block->prev = block;
		block->next = block;
		free_list = block;
	}
	else // (free_list->prev) -> block -> free_list
	{
		Block* prev = free_list->prev;

		prev->next = block;
		block->prev = prev;
		block->next = free_list;
		free_list->prev = block;
	}
}

template <std::size_t leaf_size, std::size_t height>
void	Allocator<leaf_size, height>::dump() const
{
	std::string result;
	std::size_t	width = (1 << height);
	std::function<void(std::size_t, std::size_t)> print;

	result.resize((width + 1) * (height + 1));

	print = [&](std::size_t level, BlockID block_id)
	{
		if (level > height)
			return;
		std::size_t block_width = width >> level;
		
		if (this->isAllocated(block_id, level))
		{
			for (int j = 0; j < block_width; ++j)
				result.at(level * (width + 1) + block_id + j) = 'A';
		}
		else
		{
			for (int j = 0; j < block_width; ++j)
				result.at(level * (width + 1) + block_id + j) = 'E';
		}
		print(level + 1, block_id);
		print(level + 1, block_id + (block_width / 2));
	};
	for (int i = 0; i <= height; ++i)
		result.at(i * (width + 1) + width) = '\n';
	print(0, 0);
	std::cout << result << std::endl;
}

template <std::size_t leaf_size, std::size_t height>
bool	Allocator<leaf_size, height>::isAllocated(BlockID block_id, std::size_t level) const
{
	Block const* block = this->getBlock(block_id);

	return (block->level == level && block->prev == nullptr && block->next == nullptr);
}

template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::BlockID	Allocator<leaf_size, height>::getBuddyID(Block const* block) const
{
	BlockID	block_id = this->getBlockID(block);
	std::size_t width = 1 << (height - block->level);

	return (block_id ^ width);
}

template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::BlockID	Allocator<leaf_size, height>::getBlockID(Block const* block) const
{
	return (std::distance(this->blocks, block));
}
template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::Block*	Allocator<leaf_size, height>::getBlock(void* ptr)
{
	std::ptrdiff_t const offset = static_cast<byte*>(ptr) - this->pool;

	if (offset % leaf_size != 0)
		return (nullptr); // invalid pointer
	BlockID const block_id = offset / leaf_size;

	if (block_id < 0 || (1 << height) <= block_id)
		return (nullptr);
	return (this->getBlock(block_id));
}

template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::Block const*	Allocator<leaf_size, height>::getBlock(BlockID block_id) const
{
	return (&this->blocks[block_id]);
}

template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::Block*	Allocator<leaf_size, height>::getBlock(BlockID block_id)
{
	return (&this->blocks[block_id]);
}

template <std::size_t leaf_size, std::size_t height>
typename Allocator<leaf_size, height>::BlockID	Allocator<leaf_size, height>::allocateBlock(std::size_t level)
{
	if (level > height)
		return (-1);
	Block*& free_list = this->free_list[level];

	if (free_list == nullptr) // 현재 레벨에 할당 가능한 블럭이 없음
	{
		assert(level != 0);
		BlockID	parent_id = this->allocateBlock(level - 1);
	
		if (parent_id == -1)
			return (-1);
		Block*	block = this->getBlock(parent_id);
		block->level += 1;
		BlockID buddy_id = this->getBuddyID(block);
		Block*	buddy = this->getBlock(buddy_id);

		buddy->level = block->level;
		buddy->prev = buddy;
		buddy->next = buddy;
		free_list = buddy;
		return (parent_id);
	}
	BlockID block_id = this->getBlockID(free_list);

	if (free_list == free_list->next)
	{
		free_list->prev = nullptr;
		free_list->next = nullptr;
		free_list = nullptr;
		return (block_id);
	}
	Block* prev = free_list->prev;
	Block* next = free_list->next;

	prev->next = next;
	next->prev = prev;
	free_list->prev = nullptr;
	free_list->next = nullptr;
	free_list = next;
	return (block_id);
}
