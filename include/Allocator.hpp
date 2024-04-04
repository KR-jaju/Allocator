
#pragma once

#include <cstdint>
#include <cstddef>

template <std::size_t l, std::size_t u>
class Allocator {
public:
	Allocator(void *pool): pool((unsigned char *)pool), free_tree(2) {}
	void	*allocate(std::size_t exp) {
		return (fetchBlock(1, exp));
	}
	void	free(void *ptr);
private:
	unsigned char	*pool;
	unsigned int	free_tree;

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
};

/*
leaf인 경우
1. 크기가 맞음 -> return (addr)
2. 크기가 안 맞음 -> 분할하고 재귀
leaf아닌 경우
1. 크기가 맞음 -> return null
2. 크기가 안 맞음 -> 재귀

1, 0
0, 0

*/
