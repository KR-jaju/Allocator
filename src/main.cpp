
#include <iostream>
#include <Allocator.hpp>

int	main(void) {
	char	pool[256];
	Allocator<2, 8>	a(pool);

	void	*ptr1 = a.allocate(30);
	void	*ptr2 = a.allocate(30);
	a.free(ptr1);
	a.free(ptr2);
	void	*ptr3 = a.allocate(64);

	std::cout << ptr1 << std::endl;
	std::cout << ptr2 << std::endl;
	std::cout << ptr3 << std::endl;
}