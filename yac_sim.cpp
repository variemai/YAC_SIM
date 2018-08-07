/*****************************************************
 * University of Crete, Computer Science Department  *
 * cache simulator for the purposes of CS-225 ex 11  *
 * Author: Vardas Ioannis 04/02/2018                 *
 *****************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>
#include <vector>
#include <stdint.h>
#include <bitset>
#define MAX_FILENAME 256
using namespace std;

unsigned word_size;

typedef struct profile_info{
	int check;
	int hit;
	int miss;
}cache_prof;

/*Create entry struct for cache*/
typedef struct cache_entry{
	unsigned short valid;
	unsigned long tag;
}entry;

typedef struct characteristics_of_cache{
	unsigned long cache_size;
    unsigned long block_size;
    unsigned long tmp_tag; 
    unsigned long tmp;
    //unsigned long memory_size;
    unsigned no_blocks;
    unsigned block_offset;
    unsigned tag_shift;
    unsigned tag_size;
    unsigned index_size;
    unsigned no_set;
    unsigned asso = 1;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> tag_mask;
}cache_char;

/*************************Functions Declarations*******************************/

void
set_cache_specs(unsigned long, unsigned long, unsigned long, cache_char*);

void
print_bin_index(unsigned long, unsigned);

unsigned long 
return_word(unsigned long , unsigned, unsigned long, unsigned );

unsigned
powerof2(bitset<32>);

/*************************Functions Definitions********************************/

void
set_cache_specs(unsigned long cache_size, unsigned long block_size, 
        unsigned long memory_size, cache_char* characteristics){
    
    unsigned i, j;
	bitset<32> bitset0{ memory_size };
    bitset<32> bitset1{ cache_size };
	bitset<32> bitset2{ block_size };
    characteristics->cache_size = cache_size;
    characteristics->block_size = block_size;
	characteristics->tag_size = powerof2(bitset0);
	i = powerof2(bitset1);
	characteristics->index_size = i;
	characteristics->index_mask = characteristics->index_mask << i;
	characteristics->tag_mask = characteristics->index_mask;
	characteristics->index_mask = ~characteristics->index_mask;
    characteristics->block_offset = powerof2(bitset2);
    characteristics->index_size = characteristics->index_size -
        characteristics->block_offset;
    for(j=0; j<characteristics->block_offset; j++){
        characteristics->index_mask[j]=0;
    }
    characteristics->tag_shift = i;
    characteristics->no_blocks = characteristics->cache_size / block_size;
    characteristics->no_set=characteristics->no_blocks/characteristics->asso;
    characteristics->tag_size = characteristics->tag_size - i;
    characteristics->tmp = characteristics->index_mask.to_ulong();
    characteristics->tmp_tag = characteristics->tag_mask.to_ulong();
}


void
cache_access(vector<entry> &cache, unsigned long address, 
       cache_char* specs, cache_prof* prof_info){
	
    unsigned long index, tag, old_address;
	cout << "ADDR : " << address << " ";
	index = address & specs->tmp;
	index = index >> specs->block_offset;
	tag = address & specs->tmp_tag;
	tag = tag >> specs->tag_shift;
	prof_info->check++;
	if (cache[index].valid == 1 && cache[index].tag == tag) {
		prof_info->hit++;
		cout << "HIT with index: ";
		print_bin_index(index,specs->index_size);
	}
	else {
		prof_info->miss++;
		if(cache[index].valid==1){
			old_address = return_word(cache[index].tag,specs->tag_shift,index,specs->block_offset);
			cout <<"miss, replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
		}
		else{
			cout << "MISS, with index: ";
			print_bin_index(index,specs->index_size);
			cache[index].valid = 1;
		}
		cache[index].tag = tag;
	}
	cout << endl;
}


unsigned
powerof2(bitset<32> bitset1) {
	
    unsigned i;
	for (i = 0; i < bitset1.size(); i++)
	{
		if (bitset1[i] == 1) break;
	}
	return i;
}

unsigned
pow2(unsigned num){
	
    unsigned i = 0;
	while(num != 0 && num!=1 ){
		num = num / 2;
		i++;
	}
	return i;
}

void
print_bin_index(unsigned long index, unsigned size){
	
    bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		cout << bits[i];
		if(i==0) break;
	}
}


void
display_contents(vector<entry> &cache, cache_char* specs){
	
    for(unsigned i=0; i<specs->no_set; i++) {
		bitset<32> bits = {cache[i].tag};
		cout << "Index: ";
		print_bin_index(i,specs->index_size);
		cout << " Valid: " << cache[i].valid << ", Tag: ";
		for(unsigned j=specs->tag_size-1; j>=0; j--){
			cout << bits[j] ;
			if (j==0) break;
		}
		cout << endl;
	}
}

unsigned long
return_word(unsigned long tag, unsigned tag_shift, unsigned long index,
	
        unsigned block_offset){
	unsigned long retval;
	retval = tag << tag_shift;
	index = index << block_offset;
	retval = retval | index;
	return retval;
}

void
print_results(cache_prof *prof_info){
	
    if(prof_info->check == 0 ) return ;
	float hitrate = 0.0;
	cout << "************* Cache Simulation Results ************"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",prof_info->check);
	printf ("*             Number of HITS : %18d *\n",prof_info->hit);
	printf ("*             Number of MISSES : %16d *\n",prof_info->miss);
	hitrate =100* (float(prof_info->hit) / float(prof_info->check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	cout << "***************************************************"<< endl;
}


int
main(void){
/******************************** Variables declarations **********************/
	string filename;
	unsigned long cache_size, block_size, memory_size, address;
	string str;
	ifstream infile;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
    cache_char cache_specs;
/*********************************** Initialization ***************************/
	init_entry.valid=0;
	init_entry.tag=0;
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
	cout << "Enter the memory size: ";
	cin >> memory_size;
	cout << "Enter the cache size: ";
	cin >> cache_size;
	cout << "Enter the size of the Word: ";
	cin >> word_size;
	cout << "Enter the block size: ";
	cin >> block_size;
    set_cache_specs(cache_size,block_size,memory_size,&cache_specs);
	for(unsigned i=0; i<cache_specs.no_set; i++){
		cache.push_back(init_entry);
	}

/***************************Simulation Starting********************************/
	printf("Insert an address or a valid command\n");
	printf("For a list of the available commands type \"cmd\"\n");
	while (cin >> str){
		if(str.compare("cmd") == 0){
			printf("\texit:\t\t\t\tPrints the results and exits YAC SIM\n");
			printf("\tsource:\t\t\t\tRead addresses from a file\n");
			printf("\tdisplay_contents:\t\tPrints the contents of the Cache\n");
			continue;
		}
		if(str.compare("exit") == 0 ) {
			print_results(&prof_info);
			break;
		}
		//Input from file
		if(str.compare("source") == 0 ){
			cout << "Enter trace filename : ";
			cin >> filename;
			if(filename.size() > MAX_FILENAME){
				cout << "Filename exceeded max size\nExiting..." << endl;
				exit(-1);
			}
			cout << "Filename given: " << filename << endl;
			infile.open(filename, ios::in);
			if (!infile){
				cout << "Error! File not found...\n";
				exit(-1);
			}
			while (getline(infile, str)){
				address = stoi(str);
                cache_access(cache, address, &cache_specs,&prof_info);
			}
			str.clear();
			infile.close();
			continue;
		}
        //Display contents of cache
		if(str.compare("display_contents") == 0 ){
			continue;
		}
		//Input single address
		address = stoi(str);
        cache_access(cache, address, &cache_specs,&prof_info);
	}
	return 0;
}
