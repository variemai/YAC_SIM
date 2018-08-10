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
#include <locale>
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
	unsigned short valid[16]; /*TODO use dynamic tables */
	unsigned long tag[16];
    unsigned short LRU[16];
}entry;

typedef struct characteristics_of_cache{
    unsigned long tmp_tag; 
    unsigned long tmp;
    unsigned block_offset;
    unsigned tag_shift;
    unsigned tag_size;
    unsigned index_size;
    unsigned no_set;
    unsigned asso = 1;
}cache_char;

/*************************Functions Declarations*******************************/

void
set_cache_specs(unsigned long,unsigned long,unsigned long,unsigned,cache_char*);

void
print_bin_index(unsigned long, unsigned);

unsigned long 
return_word(unsigned long , unsigned, unsigned long, unsigned );

unsigned
powerof2(bitset<32>);

unsigned
pow2(unsigned );

/*************************Functions Definitions********************************/

void
set_cache_specs(unsigned long cache_size, unsigned long block_size, 
        unsigned long memory_size,unsigned asso, cache_char* characteristics){
    
    unsigned i, j;
    unsigned long no_blocks;
	bitset<32> tag_mask;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> bitset0{ memory_size };
    bitset<32> bitset1{ cache_size };
	bitset<32> bitset2{ block_size };
    unsigned pow_of_asso = pow2(asso);
	characteristics->tag_size = powerof2(bitset0);
	i = powerof2(bitset1);
	characteristics->index_size = i - pow_of_asso;
	index_mask = index_mask << characteristics->index_size;
	tag_mask = index_mask;
	index_mask = ~index_mask;
    characteristics->asso = asso;
    characteristics->block_offset = powerof2(bitset2);
    characteristics->index_size = characteristics->index_size -
        characteristics->block_offset;
    for(j=0; j<characteristics->block_offset; j++){
        index_mask[j]=0;
    }
    /*The number of bits needed for RIGHT shift for tag*/
    characteristics->tag_shift = i-pow_of_asso;
    no_blocks = cache_size / block_size;
    characteristics->no_set=no_blocks/characteristics->asso;
    characteristics->tag_size = characteristics->tag_size - i + pow_of_asso;
    characteristics->tmp = index_mask.to_ulong();
    characteristics->tmp_tag = tag_mask.to_ulong();
}

void print_specs(cache_char* specs){
    cout<<"Tag Size: "<<specs->tag_size<<"\nIndex size: "<<specs->index_size
        <<endl<<"Tmp: "<<specs->tmp<<"\nTmp_tag: "<<specs->tmp_tag<<endl;
    cout << "Tag Shift: " << specs->tag_shift<<endl;
    cout << "Number of Sets: "<< specs->no_set << endl;
}

void
cache_access(vector<entry> &cache, unsigned long address, 
       cache_char* specs, cache_prof* prof_info){
	
    unsigned long index, tag, old_address;
    unsigned short done, min_lru, min_index;
	cout << "ADDR : " << address << " ";
	index = address & specs->tmp;
	index = index >> specs->block_offset;
	tag = address & specs->tmp_tag;
	tag = tag >> specs->tag_shift;
    /*cout << "TAG: " << tag <<endl;*/
	prof_info->check++;
    done = 0;
    min_lru = 0;
    min_index = 0;
    for(unsigned i=0; i<specs->asso; i++){
	    if (cache[index].valid[i] == 1 && cache[index].tag[i] == tag) {
		    prof_info->hit++;
            /*Update the LRU info*/
            cache[index].LRU[i]++;
		    cout << "HIT with index: ";
		    print_bin_index(index,specs->index_size);
            if(specs->asso > 1) {
                cout << " WAY: " << i;
            }
            done = 1;
	    }
    }
    if(!done){
        prof_info->miss++;
        /*Miss-first search for an invalid position in the way*/
        for(unsigned i=0; i<specs->asso; i++){
            if(cache[index].valid[i] == 0){
                cache[index].tag[i] = tag;
                cache[index].valid[i] = 1;
                cache[index].LRU[i] = 1;
                cout << "MISS, with index: ";
                print_bin_index(index,specs->index_size);
                cout << " WAY: " << i;
                cout << " LRU: " << cache[index].LRU[i] ;
                done = 1;
                break;
            }
        }
        if(!done && specs->asso > 1){
            /*LRU replacement algorithm*/
            min_lru = cache[index].LRU[0];
            for(unsigned i=0; i<specs->asso; i++){
                if(cache[index].LRU[i] < min_lru ){
                    min_lru = cache[index].LRU[i];
                    min_index = i;
                    cout << " MIN_LRU =  " << min_lru << " ";
                }
            }
            old_address=return_word(cache[index].tag[min_index],specs->tag_shift,index,specs->block_offset);
			cout <<"MISS, Replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
            cout << " WAY: " << min_index;
            cout << " LRU: " << cache[index].LRU[min_index] ;
            cache[index].tag[min_index] = tag;
            cache[index].LRU[min_index] = 1;
        }
        else if(!done && specs->asso == 1){
			old_address = return_word(cache[index].tag[0],specs->tag_shift,index,specs->block_offset);
			cout <<"miss, replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
		    cache[index].tag[0] = tag;
        }
    }
    /*
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
	}*/
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
        for(unsigned j=0; j<specs->asso; j++){
		bitset<32> bits = {cache[i].tag[j]};
		cout << "Index: ";
		print_bin_index(i,specs->index_size);
        cout << " Way: " << j;
		cout << " Valid: " << cache[i].valid[j] << ", Tag: ";
		for(unsigned k=specs->tag_size-1; k>=0; k--){
			cout << bits[k] ;
			if (k==0) break;
		}
        cout << " LRU: " << cache[i].LRU[j];
		cout << endl;
	}
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
    unsigned asso;
	unsigned long cache_size, block_size, memory_size, address;
	string str;
	ifstream infile;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
    cache_char cache_specs;
    int alnum_flag;
    string::size_type i;
    locale loc;
/*********************************** Initialization ***************************/
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
    i = 0;
	cout << "Enter the memory size: ";
	cin >> memory_size;
	cout << "Enter the cache size: ";
	cin >> cache_size;
	cout << "Enter the size of the Word: ";
	cin >> word_size;
	cout << "Enter the block size: ";
	cin >> block_size;
	cout << "Enter associativity (1,2,4,8 or 16): ";
	cin >> asso;
    set_cache_specs(cache_size,block_size,memory_size,asso, &cache_specs);
    for(unsigned i=0; i<asso; i++){
        init_entry.tag[i] = 0;
        init_entry.valid[i] = 0;
        init_entry.LRU[i] = 0;
    }
	for(unsigned i=0; i<cache_specs.no_set; i++){
		cache.push_back(init_entry);
	}
    print_specs(&cache_specs);

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
		/*Input from file*/
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
        /*Display contents of cache*/
		if(str.compare("display") == 0 ){
            display_contents(cache,&cache_specs);
			continue;
		}
		/*Input single address*/
        alnum_flag = 0;
        while(i<str.length()){
            if(isdigit(str[i])==false){
                alnum_flag = 1;
                break;
            } 
            i++;
        }
        i = 0;
        if(alnum_flag == 1){
            cout << "Address not alphanumeric"<< endl;
            continue;
        }
		address = stoi(str);
        cache_access(cache, address, &cache_specs,&prof_info);
	}
	return 0;
}
