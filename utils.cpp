#include "utils.h"


bool isPowerOfTwo(unsigned long n)
{
    return (ceil(log2(n)) == floor(log2(n)));
}


/*Returns the index of the first bit that is equal to 1 in the bitset*/
unsigned powerof2(std::bitset<32> bitset1)
{
    unsigned i;
	for (i = 0; i < bitset1.size(); i++)
	{
		if (bitset1[i] == 1) break;
	}
	return i;
}

/*Returns the exponent of a number that is power of 2*/
unsigned pow2(unsigned long num)
{
    unsigned i = 0;
	while(num != 0 && num!=1 ){
		num = num / 2;
		i++;
	}
	return i;
}

unsigned long get_sizeof_memory(void)
{
    unsigned long memory_size;
    std::cout << "Enter the Main Memory size in Bytes: ";
    std::cin >> memory_size;
    if(std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bad Input: Memory size is not a number" << std::endl;
        return 0;
    }
    return (isPowerOfTwo(memory_size) == 0) ? 0 : memory_size;
}

unsigned long get_sizeof_word(void)
{
    unsigned long sizeof_word;
    std::cout << "Enter the size of the Word in Bytes: ";
    std::cin >> sizeof_word;
    if(std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bad Input: Word size is not a number" << std::endl;
        return 0;
    }
    return (isPowerOfTwo(sizeof_word) == 0 ) ? 0 : sizeof_word;
}

unsigned long  get_sizeof_cache(void){
    unsigned long sizeof_cache;
    std::cout << "Enter the Cache size in Bytes: ";
    std::cin >> sizeof_cache;
    if(std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bad Input: Cache size is not a number" << std::endl;
        return 0;
    }
    return (isPowerOfTwo(sizeof_cache) == 0 ) ? 0 : sizeof_cache;
}

unsigned long get_sizeof_cacheline(void)
{
    unsigned long block_size;
    std::cout << "Enter the Cache Block(Line) size in Bytes: ";
    std::cin >> block_size;
    if(std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bad Input: Block size is not a number" << std::endl;
    }
    return (isPowerOfTwo(block_size) == 0 ) ? 0 : block_size;

}

unsigned long get_associativity(void)
{
    unsigned long asso;
    std::cout << "Enter associativity (1,2,4,8 or 16): ";
    std::cin >> asso;
    if(std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bad Input: Associativity is not a number" << std::endl;
        return 0;
    }
    if(asso == 1) return asso;
    return (isPowerOfTwo(asso) == 0 ) ? 0 : asso;
}
