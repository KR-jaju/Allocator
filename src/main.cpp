
#include <iostream>
#include <Allocator.hpp>

int	main(void) {
	char	pool[120]; // 30 * 2^2 = 120
	Allocator<30, 2>	a(pool);

	a.dump();
	void	*ptr1 = a.allocate(30);
	a.dump();
	void	*ptr2 = a.allocate(30);
	a.dump();
	a.free(ptr1);
	a.dump();
	a.free(ptr2);
	a.dump();
	void	*ptr3 = a.allocate(64);
	a.dump();

	std::cout << ptr1 << std::endl;
	std::cout << ptr2 << std::endl;
	std::cout << ptr3 << std::endl;
}