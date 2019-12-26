#include <iostream>
#include <string>
using namespace std;

int main(void){
    int i,j,k;
    int x_way_frame = 21;
    int y_way_frame = 60;
    int x_way_title_flame = x_way_frame;
    int x_index_frame = 8;
    int no_ways = 4;
    string Valid = " Valid |";
    string Data = " Data |";
    int tag_size = 8;
    string Tag = " Tag";
    string Index = "Index";
    int index_size = 6;
    int l_side, r_side;
    int no_sets = 4;
    if (tag_size > 5 ){
        int offset = tag_size - Tag.length() - 1;
        for(i=0; i<offset; i++){
            Tag.append(" ");
        }
    }
    Tag.append(" |");
    if ( index_size > 7 ){
        int offset = index_size - Index.length() - 1;
        for(i=0; i<offset; i++){
            Index.append(" ");
        }
    }
    Index.append(" |");
    x_index_frame = Index.length();
    x_way_frame = Valid.length()+Data.length()+Tag.length();
    if (x_way_frame % 2 == 0){
        l_side = (x_way_frame - 6 ) / 2;
        r_side = l_side;
    }
    else{
        l_side = 1+(x_way_frame - 6) / 2;
        r_side =(x_way_frame - 6 ) / 2;
    }
    for(k=0; k<no_ways; k++){
        if( k == 0 ){
            for(j=0; j<x_index_frame; j++){
                printf(" ");
            }
        }
        for( i=0; i < x_way_frame; i++){
            printf("-");
        }
    }
    printf("\n");
    for(j=0; j<x_index_frame - 1; j++){
        printf(" ");
    }
    printf("|");
    for(k=0; k<no_ways; k++){
        for(i=0; i<l_side; i++){
            printf(" ");
        }
        printf("Way %d",k);
        for(i=0; i<r_side; i++){
            printf(" ");
        }
        printf("|");
    }
    printf("\n");
    for(k=0; k<no_ways; k++){
        if ( k ==0 ){
            for(j=0; j<x_index_frame; j++){
                printf(" ");
            }
        }
        for(i=0; i < x_way_frame; i++){
            printf("-");
        }
    }
    printf("\n");
    cout << Index;
    for(k=0; k<no_ways; k++){
        cout << Valid;
        cout << Data;
        cout << Tag ;
    }
    printf("\n");
    for(i=0; i<no_ways; i++){
        if(i==0){
            for(j=0; j<x_index_frame+x_way_frame-1; j++){
                printf("-");
            }
        }
        else{
            for(j=0; j<x_way_frame; j++){
                printf("-");
            }
        }
    }
    printf("\n");
    for(i=0; i<2*no_sets; i++){
        for(k=0; k<no_ways; k++){
            int max;
            if(k == 0){
                max = x_index_frame+x_way_frame;
            }
            else{
                max = x_way_frame;
            }
            if( i%2 == 0){
                for(j=0; j<max-1; j++){
                    if (k ==0 ){
                        if(j == x_index_frame - 1 ){
                            printf("|");
                        }
                        else if( j == x_index_frame + 7 ){
                            printf("|");
                        }
                        else if( j == x_index_frame + 7 + Valid.length() - 1 ){
                            printf("|");
                        }
                        else{
                            printf(" ");
                            /*Print the index bits*/
                            if(j < x_index_frame - 1){

                            }
                        }

                    }
                    else{
                        if ( j == Valid.length() -1){
                            printf("|");
                        }
                        else if ( j == Valid.length()+ Data.length() -1){
                            printf("|");
                        }
                        else{
                            printf(" ");
                        }
                    }

                }
                printf("|");
            }
            else{
                for(j=0; j<max; j++){
                    printf("-");
                }
            }
        }
        printf("\n");
    }
    //printf("%d, %d, %d\n",Valid.length(),Data.length(),Tag.length());
    return 0;
}
