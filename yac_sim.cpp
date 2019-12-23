/*****************************************************************************\
* YAC Sim - Yet Another Cache Simulator, created for the purposes of CS-225
* course CSD, University of Crete, creates a cache with user defined attributes
* and simulates accesses from user input
* Copyright (C) 2018  Ioannis Vardas - vardas@ics.forth.gr
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>
*******************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>
#include <vector>
#include <stdint.h>
#include <bitset>
#include <locale>
#include <limits>
#include <cmath>
#include <string.h>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_draw.H>
#pragma warning(disable : 4996)

using namespace std;
#define MAX_FILENAME 256
unsigned long word_size;

const int MAX_COLS = 8;
const int MAX_ROWS = 64;

class MyTable : public Fl_Table {

	char data[MAX_ROWS][MAX_COLS];		// data array for cells

	// Draw the row/col headings
	//    Make this a dark thin upbox with the text inside.
	//
	void DrawHeader(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	// Draw the cell data
	//    Dark gray text on white background with subtle border
	//
	void DrawData(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		// Draw box border
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	// Handle drawing table's cells
	//     Fl_Table calls this function to draw each visible cell in the table.
	//     It's up to us to use FLTK's drawing functions to draw the cells the way we want.
	//
	void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		static char s[40];
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			if (COL % word_size == 0 && COL!=0) {
				sprintf(s, "TAGS");
			}
			else {
				sprintf(s, "WORD%d", COL);		// "A", "B", "C", etc.
			}     
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_ROW_HEADER:                  // Draw row headers
			sprintf(s, "%02x:", ROW);                 // "001:", "002:", etc
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			sprintf(s, "%c", data[ROW][COL]);
			DrawData(s, X, Y, W, H);
			return;
		default:
			return;
		}
	}
public:
	// Constructor
	//     Make our data array, and initialize the table options.
	//
	MyTable(int X, int Y, int W, int H, const char* L = 0) : Fl_Table(X, Y, W, H, L) {
		// Fill data array
		for (int r = 0; r < X; r++)
			for (int c = 0; c < Y; c++)
				data[r][c] = 'X';
		// Rows
		rows(X);             // how many rows
		row_header(1);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(0);              // disable row resizing
		row_header_width(40);
		// Cols
		cols(Y);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(80);          // default width of columns
		col_resize(1);              // enable column resizing
		end();			// end the Fl_Table group
	}
	~MyTable() { }
};

class Spreadsheet : public Fl_Table {
	Fl_Int_Input* input;					// single instance of Fl_Int_Input widget
	int values[MAX_ROWS][MAX_COLS];			// array of data for cells
	int row_edit, col_edit;				// row/col being modified

protected:
	void draw_cell(TableContext context, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0);
	void event_callback2();				// table's event callback (instance)
	static void event_callback(Fl_Widget*, void* v) {	// table's event callback (static)
		((Spreadsheet*)v)->event_callback2();
	}
	static void input_cb(Fl_Widget*, void* v) {		// input widget's callback
		((Spreadsheet*)v)->set_value_hide();
	}

public:
	Spreadsheet(int X, int Y, int W, int H, const char* L = 0) : Fl_Table(X, Y, W, H, L) {
		callback(&event_callback, (void*)this);
		when(FL_WHEN_NOT_CHANGED | when());
		// Create input widget that we'll use whenever user clicks on a cell
		input = new Fl_Int_Input(W / 2, H / 2, 0, 0);
		input->hide();
		input->callback(input_cb, (void*)this);
		input->when(FL_WHEN_ENTER_KEY_ALWAYS);		// callback triggered when user hits Enter
		input->maximum_size(5);
		input->color(FL_YELLOW);
		for (int c = 0; c < MAX_COLS; c++)
			for (int r = 0; r < MAX_ROWS; r++)
				values[r][c] = c + (r * MAX_COLS);		// initialize cells
		end();
		row_edit = col_edit = 0;
		set_selection(0, 0, 0, 0);
	}
	~Spreadsheet() { }

	// Apply value from input widget to values[row][col] array and hide (done editing)
	void set_value_hide() {
		values[row_edit][col_edit] = atoi(input->value());
		input->hide();
		window()->cursor(FL_CURSOR_DEFAULT);		// XXX: if we don't do this, cursor can disappear!
	}
	// Start editing a new cell: move the Fl_Int_Input widget to specified row/column
	//    Preload the widget with the cell's current value,
	//    and make the widget 'appear' at the cell's location.
	//
	void start_editing(int R, int C) {
		row_edit = R;					// Now editing this row/col
		col_edit = C;
		set_selection(R, C, R, C);				// Clear any previous multicell selection
		int X, Y, W, H;
		find_cell(CONTEXT_CELL, R, C, X, Y, W, H);		// Find X/Y/W/H of cell
		input->resize(X, Y, W, H);				// Move Fl_Input widget there
		char s[30]; sprintf(s, "%d", values[R][C]);		// Load input widget with cell's current value
		input->value(s);
		input->position(0, strlen(s));			// Select entire input field
		input->show();					// Show the input widget, now that we've positioned it
		input->take_focus();
	}
	// Tell the input widget it's done editing, and to 'hide'
	void done_editing() {
		if (input->visible()) {				// input widget visible, ie. edit in progress?
			set_value_hide();					// Transfer its current contents to cell and hide
		}
	}
	// Return the sum of all rows in this column
	int sum_rows(int C) {
		int sum = 0;
		for (int r = 0; r < rows() - 1; ++r)			// -1: don't include cell data in 'totals' column
			sum += values[r][C];
		return(sum);
	}
	// Return the sum of all cols in this row
	int sum_cols(int R) {
		int sum = 0;
		for (int c = 0; c < cols() - 1; ++c)			// -1: don't include cell data in 'totals' column
			sum += values[R][c];
		return(sum);
	}
	// Return the sum of all cells in table
	int sum_all() {
		int sum = 0;
		for (int c = 0; c < cols() - 1; ++c)			// -1: don't include cell data in 'totals' column
			for (int r = 0; r < rows() - 1; ++r)			// -1: ""
				sum += values[r][c];
		return(sum);
	}
};

// Handle drawing all cells in table
void Spreadsheet::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
	static char s[30];
	switch (context) {
	case CONTEXT_STARTPAGE:			// table about to redraw
		break;

	case CONTEXT_COL_HEADER:			// table wants us to draw a column heading (C is column)
		fl_font(FL_HELVETICA | FL_BOLD, 14);	// set font for heading to bold
		fl_push_clip(X, Y, W, H);			// clip region for text
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);
			if (C == cols() - 1) {			// Last column? show 'TOTAL'
				fl_draw("TOTAL", X, Y, W, H, FL_ALIGN_CENTER);
			}
			else {				// Not last column? show column letter
				sprintf(s, "%c", 'A' + C);
				fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:			// table wants us to draw a row heading (R is row)
		fl_font(FL_HELVETICA | FL_BOLD, 14);	// set font for row heading to bold
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);
			if (R == rows() - 1) {			// Last row? Show 'Total'
				fl_draw("TOTAL", X, Y, W, H, FL_ALIGN_CENTER);
			}
			else {				// Not last row? show row#
				sprintf(s, "%d", R + 1);
				fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL: {			// table wants us to draw a cell
		if (R == row_edit && C == col_edit && input->visible()) {
			return;					// dont draw for cell with input widget over it
		}
		// Background
		if (C < cols() - 1 && R < rows() - 1) {
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, is_selected(R, C) ? FL_YELLOW : FL_WHITE);
		}
		else {
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, is_selected(R, C) ? 0xddffdd00 : 0xbbddbb00);	// money green
		}
		// Text
		fl_push_clip(X + 3, Y + 3, W - 6, H - 6);
		{
			fl_color(FL_BLACK);
			if (C == cols() - 1 || R == rows() - 1) {	// Last row or col? Show total
				fl_font(FL_HELVETICA | FL_BOLD, 14);	// ..in bold font
				if (C == cols() - 1 && R == rows() - 1) {	// Last row+col? Total all cells
					sprintf(s, "%d", sum_all());
				}
				else if (C == cols() - 1) {		// Row subtotal
					sprintf(s, "%d", sum_cols(R));
				}
				else if (R == rows() - 1) {		// Col subtotal
					sprintf(s, "%d", sum_rows(C));
				}
				fl_draw(s, X + 3, Y + 3, W - 6, H - 6, FL_ALIGN_RIGHT);
			}
			else {				// Not last row or col? Show cell contents
				fl_font(FL_HELVETICA, 14);		// ..in regular font
				sprintf(s, "%d", values[R][C]);
				fl_draw(s, X + 3, Y + 3, W - 6, H - 6, FL_ALIGN_RIGHT);
			}
		}
		fl_pop_clip();
		return;
	}

	case CONTEXT_RC_RESIZE:			// table resizing rows or columns
		if (input->visible()) {
			find_cell(CONTEXT_TABLE, row_edit, col_edit, X, Y, W, H);
			input->resize(X, Y, W, H);
			init_sizes();
		}
		return;

	default:
		return;
	}
}

// Callback whenever someone clicks on different parts of the table
void Spreadsheet::event_callback2() {
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	switch (context) {
	case CONTEXT_CELL: {				// A table event occurred on a cell
		switch (Fl::event()) { 				// see what FLTK event caused it
		case FL_PUSH:					// mouse click?
			done_editing();				// finish editing previous
			if (R != rows() - 1 && C != cols() - 1)		// only edit cells not in total's columns
				start_editing(R, C);				// start new edit
			return;

		case FL_KEYBOARD:				// key press in table?
			if (Fl::event_key() == FL_Escape) exit(0);	// ESC closes app
			done_editing();				// finish any previous editing
			if (C == cols() - 1 || R == rows() - 1) return;	// no editing of totals column
			switch (Fl::e_text[0]) {
			case '0': case '1': case '2': case '3':	// any of these should start editing new cell
			case '4': case '5': case '6': case '7':
			case '8': case '9': case '+': case '-':
				start_editing(R, C);			// start new edit
				input->handle(Fl::event());		// pass typed char to input
				break;
			case '\r': case '\n':			// let enter key edit the cell
				start_editing(R, C);			// start new edit
				break;
			}
			return;
		}
		return;
	}

	case CONTEXT_TABLE:					// A table event occurred on dead zone in table
	case CONTEXT_ROW_HEADER:				// A table event occurred on row/column header
	case CONTEXT_COL_HEADER:
		done_editing();					// done editing, hide
		return;

	default:
		return;
	}
}


typedef struct profile_info{
	int check;
	int hit;
	int miss;
}cache_prof;

/*Cache entry layout*/
typedef struct cache_entry{
	unsigned short valid[16]; /*TODO use dynamic tables */
	unsigned long tag[16];
    unsigned short LRU[16];
}entry;

/*Cache characteristics and some useful masks for bit operations*/
typedef struct characteristics_of_cache{
    unsigned long tmp_tag; /*tag mask*/
    unsigned long tmp; /*index mask*/
    unsigned block_offset;
    unsigned tag_shift; /*how many bits are the tag bits shifted to the left*/
    unsigned tag_size; /*in bits*/
    unsigned index_size;
    unsigned no_set;
    unsigned long asso = 1;
}cache_char;

/*************************Function Declarations*******************************/
bool isPowerOfTwo(unsigned long);
unsigned LRU_policy(vector<entry> &cache,cache_char*);
void set_cache_specs(unsigned long,unsigned long,unsigned long,unsigned,
        cache_char*);
void print_bin_index(unsigned long, unsigned);
unsigned long return_word(unsigned long , unsigned, unsigned long, unsigned );
unsigned powerof2(bitset<32>);
unsigned pow2(unsigned long);
void clear_contents(vector<entry> &cache, cache_char*);
unsigned long get_sizeof_memory(void);
/*************************Function Definitions********************************/

bool isPowerOfTwo(unsigned long n)
{
    return (ceil(log2(n)) == floor(log2(n)));
}

unsigned long get_sizeof_memory(void)
{
    unsigned long memory_size;
    cout << "Enter the Main Memory size in Bytes: ";
    cin >> memory_size;
    if(cin.fail()){
        cin.clear();
        //cin.ignore(std::numeric_limits<std::streamsize>, '\n');
        cout << "Bad Input: Memory size is not a number" << endl;
        return 0;
    }
    return (isPowerOfTwo(memory_size) == 0) ? 0 : memory_size;
}

unsigned long get_sizeof_word(void)
{
    unsigned long sizeof_word;
    cout << "Enter the size of the Word in Bytes: ";
    cin >> sizeof_word;
    if(cin.fail()){
        cin.clear();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Word size is not a number" << endl;
        return 0;
    }
    return (isPowerOfTwo(sizeof_word) == 0 ) ? 0 : sizeof_word;
}

unsigned long  get_sizeof_cache(void){
    unsigned long sizeof_cache;
    cout << "Enter the Cache size in Bytes: ";
    cin >> sizeof_cache;
    if(cin.fail()){
        cin.clear();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Cache size is not a number" << endl;
        return 0;
    }
    return (isPowerOfTwo(sizeof_cache) == 0 ) ? 0 : sizeof_cache;
}

unsigned long get_sizeof_cacheline(void)
{
    unsigned long block_size;
    cout << "Enter the Cache Block(Line) size in Bytes: ";
    cin >> block_size;
    if(cin.fail()){
        cin.clear();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Block size is not a number" << endl;
    }
    return (isPowerOfTwo(block_size) == 0 ) ? 0 : block_size;

}

unsigned long get_associativity(void)
{
    unsigned long asso;
    cout << "Enter associativity (1,2,4,8 or 16): ";
    cin >> asso;
    if(cin.fail()){
        cin.clear();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Associativity is not a number" << endl;
        return 0;
    }
    if(asso == 1) return asso;
    return (isPowerOfTwo(asso) == 0 ) ? 0 : asso;
}


/*Not used as function call*/
unsigned LRU_policy(vector<entry> &cache,cache_char*specs, unsigned long index)
{
    unsigned short min_lru;
    unsigned way;
    way = 0 ;
    min_lru = cache[index].LRU[0];
    for(unsigned i=0; i<specs->asso; i++){
        if(cache[index].LRU[i] < min_lru ){
            min_lru = cache[index].LRU[i];
            way = i;
            cout << " MIN_LRU =  " << min_lru << " ";
        }
    }
    return way;
}

void set_cache_specs(unsigned long cache_size, unsigned long block_size,
        unsigned long memory_size,unsigned long asso, cache_char* characteristics)
{
    unsigned i, j;
    unsigned long no_blocks;
	bitset<32> tag_mask;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> bitset0{ memory_size };
    bitset<32> bitset1{ cache_size };
	bitset<32> bitset2{ block_size };
	characteristics->tag_size = powerof2(bitset0);
    unsigned pow_of_asso = pow2(asso);
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

void print_specs(cache_char* specs)
{
    cout<<"Tag Size: "<<specs->tag_size<<"\nIndex size: "<<specs->index_size
        <<endl<<"Tmp: "<<specs->tmp<<"\nTmp_tag: "<<specs->tmp_tag<<endl;
    cout << "Tag Shift: " << specs->tag_shift<<endl;
    cout << "Number of Sets: "<< specs->no_set << endl;
}

void cache_access(vector<entry> &cache, unsigned long address,
       cache_char* specs, cache_prof* prof_info)
{
    unsigned long index, tag, old_address;
    unsigned short done, min_lru, max_lru;
    unsigned way;
	cout << "ADDR: " << address << " ";
	index = address & specs->tmp;
	index = index >> specs->block_offset;
	tag = address & specs->tmp_tag;
	tag = tag >> specs->tag_shift;
	prof_info->check++;
    done = 0;
    min_lru = 0;
    way = 0;
    for(unsigned long i=0; i<specs->asso; i++){
	    if (cache[index].valid[i] == 1 && cache[index].tag[i] == tag) {
		    prof_info->hit++;
            /*Update the LRU info*/
            cache[index].LRU[i]++;
		    cout << "HIT with index: ";
		    print_bin_index(index,specs->index_size);
            if(specs->asso > 1) {
                cout << " WAY: " << i;
                cout << " LRU: " << cache[index].LRU[i] ;
            }
            done = 1;
	    }
    }
    if(!done){
        prof_info->miss++;
        /*Miss-first search for an invalid position in the way*/
        for(unsigned long i=0; i<specs->asso; i++){
            if(cache[index].valid[i] == 0){
                cache[index].tag[i] = tag;
                cache[index].valid[i] = 1;
                cache[index].LRU[i] = 1;
                cout << "MISS, with index: ";
                print_bin_index(index,specs->index_size);
                if(specs->asso > 1){
                    cout << " WAY: " << i;
                    cout << " LRU: " << cache[index].LRU[i] ;
                }
                done = 1;
                break;
            }
        }
        if(!done && specs->asso > 1){
            /*LRU replacement algorithm*/
            min_lru = cache[index].LRU[0];
            max_lru = cache[index].LRU[0];
            for(unsigned long i=0; i<specs->asso; i++){
                if(cache[index].LRU[i] < min_lru ){
                    min_lru = cache[index].LRU[i];
                    way = i;
                }
                if(cache[index].LRU[i] > max_lru){
                    max_lru = cache[index].LRU[i];
                }
            }
            old_address=return_word(cache[index].tag[way],specs->tag_shift,
                    index,specs->block_offset);
			cout <<"MISS, Replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
            cout << " WAY: " << way;
            cout << " LRU: " << cache[index].LRU[way] ;
            cache[index].tag[way] = tag;
//            cache[index].LRU[way] = 1;
            cache[index].LRU[way] = max_lru+1;

        }
        /*Directed mapped cache access no need for LRU*/
        else if(!done && specs->asso == 1){
			old_address = return_word(cache[index].tag[0],specs->tag_shift,
                    index,specs->block_offset);
			cout <<"MISS, replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
		    cache[index].tag[0] = tag;
        }
    }
	cout << endl;
}

/*Returns the index of the first bit that is equal to 1 in the bitset*/
unsigned powerof2(bitset<32> bitset1)
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

void print_bin_index(unsigned long index, unsigned size)
{
    bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		cout << bits[i];
		if(i==0) break;
	}
}

void clear_contents(vector<entry> &cache, cache_char* specs, cache_prof *prof_info){
    for(unsigned i=0; i<specs->no_set; i++) {
        for(unsigned long j=0; j<specs->asso; j++){
            cache[i].valid[j] = 0;
            cache[i].tag[j] = 0; //not necessary
            if(specs->asso > 1){
                cache[i].LRU[j] = 0;
            }
        }
    }
    prof_info->check = 0;
    prof_info->hit = 0;
    prof_info->miss = 0;
    cout << "Flushed Cache and Reseted Statistics" << endl;
}

void display_contents(vector<entry> &cache, cache_char* specs)
{
    for(unsigned i=0; i<specs->no_set; i++) {
        for(unsigned long j=0; j<specs->asso; j++){
		bitset<32> bits = {cache[i].tag[j]};
		cout << "Index: ";
		print_bin_index(i,specs->index_size);
        cout << " Way: " << j;
		cout << " Valid: " << cache[i].valid[j];
        if(cache[i].valid[j]){
            cout << ", Tag: ";
            for(unsigned k=specs->tag_size-1; k>=0; k--){
                cout << bits[k] ;
                if (k==0) break;
            }
            if(specs->asso > 1){
                cout << " LRU: " << cache[i].LRU[j];
            }
            cout << " First Addr in Block: " << return_word(cache[i].tag[j],
                                       specs->tag_shift,i,specs->block_offset);
        }
		cout << endl;

        }
    }
}

unsigned long return_word(unsigned long tag, unsigned tag_shift,
        unsigned long index, unsigned block_offset)
{
    unsigned long retval;
	retval = tag << tag_shift;
	index = index << block_offset;
	retval = retval | index;
	return retval;
}

void print_results(cache_prof *prof_info)
{
    if(prof_info->check == 0 ) return ;
	float hitrate = 0.0;
	cout << "************* Cache Simulation Statistics *********"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",prof_info->check);
	printf ("*             Number of HITS : %18d *\n",prof_info->hit);
	printf ("*             Number of MISSES : %16d *\n",prof_info->miss);
	hitrate =100* (float(prof_info->hit) / float(prof_info->check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	cout << "***************************************************"<< endl;
}



int main(int argc, char* argv[])
{
/******************************** Variables declarations **********************/
	string filename,fname;
    unsigned long asso, val;
	unsigned long cache_size, block_size, memory_size, address;
	string str;
	ifstream infile,file;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
    cache_char cache_specs;
    int alnum_flag;
    string::size_type i;
    locale loc;
    size_t found;
    char *token,*endptr;

/*********************************** Initialization ***************************/
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
    i = 0;
    asso = 0;
    cache_size = 0;
    block_size = 0;
    memory_size = 0;
    word_size = 0 ;
    cout << "\nYAC Simulator, Copyright (C) 2018 Ioannis Vardas\n";
    cout << "Contact vardas@ics.forth.gr\n\n";
    /*Print info about GPL licence*
    cout << "This program is under ";
    cout << "the terms of the GNU General Public License v3\nFor more info see";
    cout << ": <https://www.gnu.org/licenses/>" << endl << endl;
    */
    if( argc < 2 ){

        while( ( memory_size = get_sizeof_memory() ) == 0 ){
            cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
        }

        while( ( word_size = get_sizeof_word() ) == 0 ){
            cout << "Bad Input: Word Size must be power of 2!\n" << endl;;
        }

        while( ( cache_size = get_sizeof_cache() ) == 0  ){
            cout << "Bad Input: Cache Size must be power of 2!\n" << endl;;
        }

        while( ( block_size = get_sizeof_cacheline() ) == 0 ){
            cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;;
        }

        while( ( asso = get_associativity() ) == 0 ){
            cout << "Bad Input: Associativity should be either 1,2,4,8 or 16"<<endl;
        }

    }
    else{
        cout << "Filename given: " << argv[1] << endl;
        infile.open(argv[1], ios::in);
        if (!infile){
            cout << "Error! File not found...\n";
            exit(-1);
        }
        while (getline(infile, str)){
            found = str.find("memsize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        cout << "Bad Input: memsize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_memory() ) == 0 ){
                            cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                        memory_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("memsize: bad input - strtol\n");
                        while( ( val = get_sizeof_memory() ) == 0 ){
                            cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: memsize found but no digits were found!\n");
                         while( ( val = get_sizeof_memory() ) == 0 ){
                            cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    cout << "Bad Input: memsize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_memory() ) == 0 ){
                        cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                    }
                }
                memory_size = val;
                continue;
            }
            found = str.find("wordsize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ) {
                        cout << "Bad Input: wordsize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_word() ) == 0 ){
                            cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                        word_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("wordsize: bad input - strtol\n");
                        while( ( val = get_sizeof_word() ) == 0 ){
                            cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: wordsize found but no digits were found!\n");
                        while( ( val = get_sizeof_word() ) == 0 ){
                            cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    cout << "Bad Input: wordsize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_word() ) == 0 ){
                        cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                    }
                }
                word_size = val;
                continue;
            }
            found = str.find("cachesize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        cout << "Bad Input: cachesize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_cache()) == 0 ){
                            cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                        cache_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("cachesize: bad input - strtol\n");
                        while( ( val = get_sizeof_cache() ) == 0 ){
                            cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: cachesize found but no digits were found!\n");
                        while( ( val = get_sizeof_cache() ) == 0  ){
                            cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    cout << "Bad Input: cachesize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_cache() ) == 0 ){
                        cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                    }
                }
                cache_size = val;
                continue;
            }
            found = str.find("linesize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        cout << "Bad Input: linesize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                            cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                        block_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("linesize: bad input - strtol\n");
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                                cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: linesize found but no digits were found!\n");
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                            cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    cout << "Bad Input: linesize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_cacheline() ) == 0 ){
                        cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                    }
                }
                block_size = val;
                continue;
            }
            found = str.find("asso");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        cout << "Bad Input: associativity found but no digits were found!" << endl;
                        while( ( val = get_associativity() ) == 0 ){
                            cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                        asso = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("associativity: bad input - strtol\n");
                        while( ( val = get_associativity() ) == 0 ){
                            cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: associativity found but no digits were found!\n");
                        while( ( val = get_associativity() ) == 0 ){
                            cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    cout << "Bad Input: associativity found but no digits were found!" << endl;
                    while( ( val = get_associativity() ) == 0 ){
                        cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                    }
                }
                asso = val;
                continue;
            }
            infile.close();
        }
        if(memory_size == 0){
            while( ( memory_size = get_sizeof_memory() ) == 0 ){
                cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
            }
        }
        if(word_size == 0 ){
            while( ( word_size = get_sizeof_word() ) == 0 ){
                cout << "Bad Input: Word Size must be power of 2!\n" << endl;
            }
        }
        if(cache_size == 0){
            while( ( cache_size = get_sizeof_cache()) == 0 ){
                cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
            }
        }
        if(block_size == 0){
            while( ( block_size = get_sizeof_cacheline() ) == 0 ){
                cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
            }
        }
        if(asso == 0 ){
            while( ( asso = get_associativity() ) == 0 ){
                cout << "Bad Input: Associativity must be power of 2!\n" << endl;
            }
        }
    }
    /* Input from user is complete and correct to this point */

    set_cache_specs(cache_size,block_size,memory_size,asso, &cache_specs);
    for(unsigned long i=0; i<asso; i++){
        init_entry.tag[i] = 0;
        init_entry.valid[i] = 0;
        init_entry.LRU[i] = 0;
    }
	for(unsigned long i=0; i<cache_specs.no_set; i++){
		cache.push_back(init_entry);
	}


	Fl_Double_Window win(0,0,442, 200, "Way 0");
	MyTable table(8, 5, 442, 200);
	win.end();
	win.resizable(table);
	Fl_Double_Window win2(454,0,442, 200, "Way 1");
	MyTable table2(8, 5, 442, 200);
	win2.end();
	win2.resizable(table2);
	win.show();
	win2.show();
	Fl::run();

	/*fl_double_window* win = new fl_double_window(862, 322, "fl_table spreadsheet");
	spreadsheet* table = new spreadsheet(10, 10, win->w() - 20, win->h() - 20);
	table->tooltip("use keyboard to navigate cells:\n"
		"arrow keys or tab/shift-tab");
	
	table->row_header(1);
	table->row_header_width(70);
	table->row_resize(1);
	table->rows(max_rows + 1);		
	table->row_height_all(25);
	
	table->col_header(1);
	table->col_header_height(25);
	table->col_resize(1);
	table->cols(max_cols + 1);		
	table->col_width_all(70);
	
	win->end();
	win->resizable(table);
	win->show();
	fl::run();*/


/***************************Simulation Starting********************************/
    cout << "Insert an address or a valid command, type \"help\" for a list of\
 available commands"<<endl;
	while (true){
        getline(cin,str);
        if(str.length() > 0 ){
            if(str.compare("help") == 0){
                printf("\texit:\t\t\tPrints the Statistics and exits YAC Simulator\n");
                printf("\tsource <filename>:\tRead addresses from a file\n");
                printf("\tdisplay:\t\tPrints the contents of the Cache and the\
 Statistics\n");
                printf("\tflush:\t\t\tClears the contents of the Cache and resets\
 statistics\n");
                printf("\thelp:\t\t\tDisplays this message\n");
                continue;
            }
            if(str.compare("exit") == 0 ) {
                print_results(&prof_info);
                cout << "Exit YAC Simulator!" << endl;
                break;
            }
            /*Display contents of cache*/
            if(str.compare("display") == 0 ){
				win.show();
				Fl::run();
                display_contents(cache,&cache_specs);
                cout << endl;
                print_results(&prof_info);
                cout << endl;
                cout << "Insert an address or a valid command, type \"help\" \
for a list of available commands"<<endl;
                continue;
            }
            if(str.compare("flush") == 0 ){
                clear_contents(cache,&cache_specs,&prof_info);
                cout << endl;
                cout << "Insert an address or a valid command, type \"help\" \
for a list of available commands"<<endl;
                continue;
            }
            /*Input from file*/
            if(str.length() > 6 ){
                string token1 = str.substr(0,6);
                string token2 = str.substr(7,string::npos);
                if(token1.compare("source") == 0 ){
                    fname.assign(token2);
                    if(fname.size() > MAX_FILENAME){
                        cout << "Filename exceeded max size\nExiting..." << endl;
                        exit(-1);
                    }
                    cout << "Filename given: " << fname << endl;
                    file.open(fname, ios::in);
                    if (!file){
                        cout << "Error! File not found...\n";
                        exit(-1);
                    }
                    while (getline(file, str)){
                        address = stoi(str);
                        cache_access(cache, address, &cache_specs,&prof_info);
                    }
                    file.close();
                    cout << endl;
                    cout << "Insert an address or a valid command, type \"help\" \
for a list of available commands"<<endl;
                    continue;
                }
                else{
                    alnum_flag = 1;
                }
            }
            /*Input single address*/
            alnum_flag = 0;
            while(i<str.length()) {
                if(isdigit(str[i])==false){
                    alnum_flag = 1;
                    break;
                }
                i++;
            }
            i = 0;
            if(alnum_flag == 1){
                cout << "Wrong command or Address not alphanumeric"<< endl;
                cout << endl;
                cout << "Insert an address or a valid command, type \"help\" \
for a list of available commands"<<endl;
                str.clear();
                alnum_flag = 0;
                continue;
            }
            address = std::stoi(str);
            cache_access(cache, address, &cache_specs,&prof_info);
            cout << endl;
            cout << "Insert an address or a valid command, type \"help\" \
for a list of available commands"<<endl;
        }
	}
	return 0;
}
