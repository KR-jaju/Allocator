
#pragma once

#include <cstdint>
#include <cstddef>
#include <bitset>
#include <cmath>

template <std::size_t l, std::size_t u>
class Allocator {
public:
	Allocator(void *pool): pool(static_cast<unsigned char *>(pool)), free_tree(2), allocated(0) {}
	void	*allocate(std::size_t size) {
		std::size_t	exp = 0;
		size -= 1;
		while (size) {
			size >>= 1;
			++exp;
		}
		if (exp > u)
			return (nullptr);
		return (fetchBlock(1, std::max(exp, l)));
	}
	void	free(void *ptr) {
		std::size_t	idx = this->fetchAllocatedBlock(static_cast<unsigned char *>(ptr));
		std::size_t	parent;
		std::size_t	buddy;

		if (idx == 0)
			throw "Unable to free given pointer";
		this->allocated &= ~(1u << idx); // 할당된 블럭 해제
		this->free_tree |= (1u << idx); // 할당될 수 있는 자원으로 변경
		while (idx > 1) {
			parent = idx >> 1;
			buddy = idx ^ 1u;
			if (!(this->free_tree & (1u << buddy))) // 버디랑 합칠 수 없음
				break ;
			this->free_tree |= (1u << parent); // 부모가 할당가능해지고
			this->free_tree &= (~(1u << idx)) & (~(1u << buddy)); // 나와 buddy는 불가능해짐
			std::cout << idx << ", " << buddy << " merged!" << std::endl;
			idx = parent;
		}
	}
private:
	unsigned char	*pool;
	unsigned int	free_tree;
	unsigned int	allocated;

	void	*fetchBlock(std::size_t idx, std::size_t exp) {
		int const	leaf_flag = this->free_tree & (1 << idx);
		std::size_t const	begin = 1 << (u - exp);
		std::size_t const	end = 2 << (u - exp);
		void	*ret;

		if (idx >= end)
			return (nullptr);
		if (begin <= idx) { // 레벨이 맞음
			if (!leaf_flag)
				return (nullptr);
			this->free_tree &= ~leaf_flag; // 할당했으므로 free_tree에서 제거
			this->allocated |= leaf_flag; // 할당했으므로 allocated에 추가
			return (this->pool + (1 << exp) * (idx - begin));
		}
		this->free_tree &= ~leaf_flag; // 현재 블럭이 free하지 않음
		this->free_tree |= (leaf_flag << idx) | (leaf_flag << idx << 1); // 현재 블럭이 free했다면 자식이 free하게 분할됨
		if (leaf_flag)
			std::cout << idx << ": split!\n";
		if ((ret = fetchBlock(idx * 2, exp)) != nullptr) //오른쪽 도전
			return (ret);
		return (ret = fetchBlock(idx * 2 + 1, exp)); // 왼쪽 도전
	}
	std::size_t	fetchAllocatedBlock(unsigned char *ptr) {
		std::size_t	end = 2 << (u - l); // 마지막
		std::size_t	idx = 1;
		unsigned char	*base = this->pool;
		std::size_t		half = (1 << u >> 1);

		while (idx < end) {
			if (this->allocated & (1 << idx)) // 할당됨
				return (idx);
			if (ptr < base + half) { // 오른쪽
				idx = idx * 2;
			} else {
				idx = idx * 2 + 1;
				base += half;
			}
			half >>= 1;
		}
		return (0);
	}
};
