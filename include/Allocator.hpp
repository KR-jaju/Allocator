
#pragma once

#include <cstdint>
#include <cstddef>

template <std::size_t l, std::size_t u>
class Allocator {
public:
	Allocator(void *pool) {
		free_tree = 2;
	}
	void	*allocate(std::size_t size) {
		return (fetchBlock(1, size));
	}
	void	free(void *ptr);
private:
	unsigned char	*pool;
	unsigned int	free_tree;

	// void	*fetchBlock(std::size_t idx, std::size_t exp) {
	// 	int const	leaf_flag = this->free_tree & (1 << idx);
	// 	void	*ret;

	// 	if (exp < l) // 이제는 더 이상 물러날 곳이 없다
	// 		return (nullptr);
	// 	if (1 << (exp - l) <= idx && idx < (2 << (exp - l))) // 레벨이 맞음
	// 		return (leaf_flag ? this->pool + (1 << exp) * (idx - (1 << (exp - l))) : nullptr); // 할당 불가능하면 nullptr
	// 	this->free_tree &= ~leaf_flag; // 현재 블럭이 free하지 않음
	// 	this->free_tree |= (leaf_flag << idx) | (leaf_flag << idx << 1); // 현재 블럭이 free했다면 자식이 free하게 분할됨
	// 	if (ret = fetchBlock(idx * 2, exp) != nullptr) //오른쪽 도전
	// 		return (ret);
	// 	return (ret = fetchBlock(idx * 2 + 1, exp)); // 왼쪽 도전
	// }
	void	*fetchBlock(std::size_t idx, std::size_t size) {
		int const	leaf_flag = this->free_tree & (1 << idx);
		void	*ret;

		if (size >> l == 0) // 이제는 더 이상 물러날 곳이 없다
			return (nullptr);
		if ((size >> l) <= idx && idx < (size >> l << 1)) { // 레벨이 맞음
			if (!leaf_flag)
				return (nullptr);
			this->free_tree &= leaf_flag;
			return (this->pool + size * (idx - (size >> l)));
		}
		this->free_tree &= ~leaf_flag; // 현재 블럭이 free하지 않음
		this->free_tree |= (leaf_flag << idx) | (leaf_flag << idx << 1); // 현재 블럭이 free했다면 자식이 free하게 분할됨
		if ((ret = fetchBlock(idx * 2, size)) != nullptr) //오른쪽 도전
			return (ret);
		return (fetchBlock(idx * 2 + 1, size)); // 왼쪽 도전
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
